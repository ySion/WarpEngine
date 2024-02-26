#pragma once

#include <bitset>

#include "Core/Core.hpp"
#include "Data/Data.hpp"
#include <ranges>

namespace Warp::DensifySystem {

	using ComponentID = uint64_t;
	using ArchetypeID = uint32_t;

	//有2种宏观component, 一种是static, 一种是dynamic
	//static 包含了type, static tag, 以及type和static tag组成的pair
	//dynamic component包含了使用EntityID作为tag的类型
	class DensifySystemManager;

	enum class ComponentType : uint8_t {
		Invaild,
		StaticData,
		StaticTag,
		Entity,
		Pair,
	};

	class ComponentLayout final {//: public Inherit<ComponentLayout, Object> {
	public:
		ComponentLayout(
			uint64_t idx,
			std::string_view name,
			Data::DataLayout* ptr,
			Data::DataLayout* ptr2,
			ComponentType whole_type,
			ComponentType pair_A_type = ComponentType::Invaild,
			ComponentType pair_B_type = ComponentType::Invaild)
			: _id(idx), _name(name), _layout(ptr), _layout_second(ptr2), _whole_type(whole_type), _pair_A_type(pair_A_type), _pair_B_type(pair_B_type) {}

		~ComponentLayout() = default;

		ComponentLayout(ComponentLayout&&) = delete;

		ComponentLayout& operator=(ComponentLayout&&) = delete;

		ComponentLayout(const ComponentLayout&) = delete;

		ComponentLayout& operator=(const ComponentLayout&) = delete;

		inline ComponentType get_component_type() const {
			return _whole_type;
		}

		inline ComponentType get_pair_A_type() const {
			return _pair_A_type;
		}

		inline ComponentType get_pair_B_type() const {
			return _pair_B_type;
		}

		inline uint64_t get_id() const {
			return _id;
		}

		inline auto get_name() const {
			return _name.get_text();
		}

		inline auto get_layout() const {
			return _layout;
		}

		inline auto get_layout_second() const {
			return _layout_second;
		}

		inline uint64_t get_pair_A_id() const {
			return _id >> 32;
		}

		inline uint64_t get_pair_B_id() const {
			return _id & (uint64_t)0x0000'0000'FFFF'FFFF;
		}

	private:
		uint64_t _id = {};
		Data::DataUnion _name{};
		Data::DataLayout* _layout{};
		Data::DataLayout* _layout_second{};
		ComponentType _whole_type = ComponentType::Invaild;
		ComponentType _pair_A_type = ComponentType::Invaild;
		ComponentType _pair_B_type = ComponentType::Invaild;
	};

	struct EntityHandle {
		uint32_t _index{};
		uint32_t _version{};
	};

	struct EntityData {
		uint32_t _version{};
		uint32_t _flag{};
	};

	class EntityVector {
	public:
		enum class CommandType : uint64_t {
			CommandStart,
			AddComponent,
			RemoveComponent,
		};

		static constexpr uint32_t InvaildID = std::numeric_limits<uint32_t>::max();
		static constexpr uint32_t EntityLimitCount = std::numeric_limits<uint32_t>::max() - WARP_MAX_COMPONENT;

		EntityVector(){
			_entity_datas.reserve(8192*4);
			_archetypeID_row.reserve(8192 * 4);
			_free_index.reserve(8192 * 4);
			_vec_command_record.reserve(8192 * 4);
		}

		EntityData* operator[](uint32_t index) {
			if (index >= _size) { return nullptr; }
			return &_entity_datas[index];
		}

		EntityData* operator[](EntityHandle const& handle) {
			return get_entity_if_alive(handle);
		}

		std::optional<EntityHandle> index_to_handle(uint32_t index) const {
			if (index >= _size) { return std::nullopt; }
			auto& ref = _entity_datas[index];
			if (!(ref._flag >> 31)) { return std::nullopt; }
			return EntityHandle{ index, ref._version };
		}

		EntityData* get_entity_if_alive(EntityHandle const& handle) {
			if (handle._index >= _size) { return nullptr; }
			auto& ref = _entity_datas[handle._index];
			if (ref._version != handle._version || !(ref._flag >> 31)) { return nullptr; }
			return &ref;
		}

		uint32_t size() const {
			return _size;
		}

		bool is_index_vaild(uint32_t idx) const {
			return idx < EntityLimitCount && idx < _size;
		}

		//39 ~ 31 flag is system, user is 0 ~ 28
		inline auto get_entity_flag(uint32_t h) const {
			return _entity_datas[h]._flag & 0x3FFF'FFFF;
		}

		//39 ~ 31 flag is system, user is 0 ~ 28
		inline bool get_entity_flag(uint32_t h, uint32_t pos) const {
			return (_entity_datas[h]._flag >> pos) & 1;
		}

		inline bool is_command_dirty(uint32_t h) const{
			// 29th bit is 1
			return (_entity_datas[h]._flag >> 29) & 1;
		}

		inline void set_command_dirty(uint32_t h) {
			_entity_datas[h]._flag |= (1 << 29);
		}

		inline void set_command_no_dirty(uint32_t h) {
			_entity_datas[h]._flag &= ~(1 << 29);
		}

		inline auto set_entity_flag(uint32_t h, uint32_t flag) {
			return _entity_datas[h]._flag = (flag & 0x3FFF'FFFF) | (_entity_datas[h]._flag & 0xC000'0000);
		}

		inline auto set_entity_flag_by_bit(uint32_t h, uint32_t pos, bool flag_value) {
			if(pos > 28) return _entity_datas[h]._flag;
			if (flag_value) {
				_entity_datas[h]._flag |= (1 << pos);
			} else {
				_entity_datas[h]._flag &= ~(1 << pos);
			}
			return _entity_datas[h]._flag;
		};

		inline bool is_entity_named(uint32_t idx) const{
			return _entity_datas[idx]._flag >> 30 == 3; //1100 = 12 = 0xC (vaild, and named)
		}

		EntityHandle new_entity(std::string_view name = "", uint32_t flag = 0) {
			uint32_t index;
			if (!_free_index.empty()) {
				index = _free_index.back();
				_free_index.pop_back();
			} else {
				try_adjust_space(1);
				index = _size;
				_size++;
			}

			if (index > EntityLimitCount) {
				error("EntityVector::new_entity: entity count limit reached, limit: {}", EntityLimitCount);
				return { InvaildID, 0 };
			}

			auto& handle_ref = _entity_datas[index];
			if (!name.empty()) {
				handle_ref._flag = 0xC000'0000 | (flag & 0x3FFF'FFFF);
			} else {
				handle_ref._flag = 0x8000'0000 | (flag & 0x3FFF'FFFF);
			}

			return { index, handle_ref._version };
		}

		MVector<EntityHandle> new_entities(uint32_t count, uint32_t flag, MVector<uint32_t>& entity_idx_in_idx_vec) {
			//WARP_BENCH(T0);
			uint32_t new_count_required;
			fetch_next_idx_vec_for_new_entities(count, entity_idx_in_idx_vec, new_count_required);
			//WARP_BENCH(T1);
			try_adjust_space(new_count_required);
			_size += new_count_required;
			//WARP_BENCH(T2);
			MVector<EntityHandle> handles{ count };
			//WARP_BENCH(T3);

			if(count < 25000) {
				for (uint32_t i = 0; i < count; i++) {
					auto& handle_ref = _entity_datas[i];
					handle_ref._flag = 0x8000'0000 | (flag & 0x3FFF'FFFF);
					handles[i] = { entity_idx_in_idx_vec[i], handle_ref._version };
				}
			}else {
				tbb::parallel_for(tbb::blocked_range<uint32_t>(0, count), [&](const tbb::blocked_range<uint32_t>& r) {
					for (uint32_t i = r.begin(); i < r.end(); i++) {
						auto& handle_ref = _entity_datas[i];
						handle_ref._flag = 0x8000'0000 | (flag & 0x3FFF'FFFF);
						handles[i] = { entity_idx_in_idx_vec[i], handle_ref._version };
					}
				}, tbb::auto_partitioner{});
			}

			//WARP_BENCH(T4);
			//WARP_BENCH_INFO("Fecth idxs", T0, T1);
			//WARP_BENCH_INFO("Adjust space", T1, T2);
			//WARP_BENCH_INFO("Make ret vec", T2, T3);
			//WARP_BENCH_INFO("Init entities", T3, T4);
			return handles;
		}

		//return false if entity not exist
		bool remove_entity(EntityHandle e) {
			auto& ref = _entity_datas[e._index];
			if (ref._version != e._version || !(ref._flag >> 31)) return false;

			ref._version++;
			ref._flag = 0;
			_free_index.push_back(e._index);

			return true;
		}

		//return failed count
		uint32_t remove_entities(std::span<EntityHandle> const es) {

			uint32_t failed_count = 0;
			for (uint32_t i = 0; i < es.size(); i++) {
				auto& ref = _entity_datas[i];
				if (ref._version != es[i]._version || !(ref._flag >> 31)) { failed_count++;  continue; }
				ref._version++;
				ref._flag = 0;
				_free_index.push_back(es[i]._index);
			}
			return failed_count;
		}

		inline auto& get_archetype_row_vec() {
			return _archetypeID_row;
		}

		inline auto& get_command_record_vec() {
			return _vec_command_record;
		}

	private:

		void fetch_next_idx_vec_for_new_entities(uint32_t count, MVector<uint32_t>& idx_vec, uint32_t& new_count_required) {
			idx_vec.resize(count);

			if (!_free_index.empty()) {
				if (count <= _free_index.size()) {
					std::copy(_free_index.end() - count, _free_index.end(), idx_vec.begin());
					_free_index.resize(_free_index.size() - count);
				} else {
					uint32_t free_count = _free_index.size_u32();
					std::ranges::copy(_free_index, idx_vec.begin());
					_free_index.clear();
					new_count_required = count - free_count;
					for (uint32_t i = 0; i < new_count_required; i++) {
						idx_vec[i + free_count] = _size;
						_size++;
					}
				}
			} else {
				new_count_required = count;
				for (uint32_t i = 0; i < count; i++) {
					idx_vec[i] = _size;
					_size++;
				}
			}
		}


		void try_adjust_space(uint32_t new_count) {
			_entity_datas.resize(_entity_datas.size() + new_count);
			_archetypeID_row.resize(_entity_datas.size() + new_count);
			_vec_command_record.resize(_entity_datas.size() + new_count, true);
		}

		MVector<uint32_t> _free_index{};
		MChunkVector<EntityData> _entity_datas{};
		MChunkVector<uint64_t> _archetypeID_row{};
		MChunkVector<MSet<std::pair<CommandType, ComponentID>>> _vec_command_record{};
		
		uint32_t _size{};
	};


	class ArchetypeData {
	public:
		ArchetypeData() = delete;
		~ArchetypeData() = default;
		ArchetypeData(ArchetypeData&&) = delete;
		ArchetypeData& operator=(ArchetypeData&&) = delete;
		ArchetypeData(const ArchetypeData&) = delete;
		ArchetypeData& operator=(const ArchetypeData&) = delete;

		ArchetypeData(const MVector<ComponentID>& com_ids, std::span<ComponentLayout*> layouts) {
			_datas.reserve(layouts.size());

			for (uint32_t i = 0, slot = 0; i < layouts.size(); i++, slot++) {
				auto const& it = *(layouts[i]);
				auto const layout_ptr = it.get_layout();

				_com_to_data_idx[com_ids[i]] = slot;
				_datas.emplace_back(layout_ptr == nullptr ? nullptr : new Data::DataLayoutUnorderedUniformChunkVector{ layout_ptr, 0});
				if (it.get_component_type() == ComponentType::Pair) {
					auto const layout_ptr2 = it.get_layout_second();
					_datas.emplace_back(layout_ptr2 == nullptr ? nullptr : new Data::DataLayoutUnorderedUniformChunkVector{ layout_ptr2, 0});
					slot++;
				}
			}
		}

		MVector<uint32_t>& get_entity_idx_vec() {
			return _entity_idx;
		}

		MVector<uint32_t>& get_free_row_vec() {
			return _free_idx;
		}

		auto& get_data() {
			return _datas;
		}

		void emplace_space(uint32_t need_count) {
			if (_free_idx.size() > need_count) {
				return;
			}

			uint32_t final_need_space = need_count - _free_idx.size_u32();
			_entity_idx.reserve(_entity_idx.size() + final_need_space);
			for (uint32_t i = 0; i < _datas.size(); i++) {
				if (_datas[i] != nullptr) {
					_datas[i]->push_back_empty(final_need_space);
				}
			}
		}

		MMap<ComponentID, uint32_t>& get_map_com_to_data_idx() {
			return _com_to_data_idx;
		}

		std::optional<uint32_t> get_com_data_idx(ComponentID com_id) {
			if (const auto v = _com_to_data_idx.find(com_id); v != _com_to_data_idx.end()) {
				return v->second;
			}
			return {};
		}

	private:

		MVector<uint32_t> _entity_idx{};
		MVector<uint32_t> _free_idx{};
		MMap<ComponentID, uint32_t> _com_to_data_idx{};
		MVector<std::unique_ptr<Data::DataLayoutUnorderedUniformChunkVector<8192*2, 4096>>> _datas{};
	};

	class ArchetypeNode;
	struct ArchetypeEdge {
		ArchetypeNode* _add;
		ArchetypeNode* _remove;
	};

	class ArchetypeNode {
	public:
		ArchetypeNode() = delete;
		~ArchetypeNode() = default;

		ArchetypeNode(ArchetypeNode&&) = delete;
		ArchetypeNode& operator=(ArchetypeNode&&) = delete;

		ArchetypeNode(const ArchetypeNode&) = delete;
		ArchetypeNode& operator=(const ArchetypeNode&) = delete;

		ArchetypeNode(ArchetypeID id, const MVector<ComponentID>& com_ids, std::span<ComponentLayout*> layouts)
			: _id(id), _components(com_ids), _data(com_ids, layouts) {}

		ArchetypeID _id;
		MVector<ComponentID> _components;
		ArchetypeData _data;
		MMap<ComponentID, ArchetypeEdge> _edges{};
	};


	class Collection {
	public:
		using CommandType = EntityVector::CommandType;

		Collection(DensifySystemManager* manager, std::string_view name);

		EntityHandle command_create_single_entity(std::string_view name = "", uint32_t flag = 0);

		bool command_remove_signle_entity(EntityHandle handle);

		MVector<EntityHandle> commnad_create_entities(uint32_t entity_count, uint32_t flag = 0);

		void command_record_add_entities_components_by_name(const MVector<std::string_view>& named_entities, const MVector<std::string_view>& coms_name);

		void command_record_add_entities_components_by_component_name(const MVector<EntityHandle>& entities_id, const MVector<std::string_view>& coms_name);

		void command_record_add_entities_components(const MVector<EntityHandle>& entities_id, const MVector<ComponentID>& coms_id);

		//void command_record_remove_entities_components_by_name(const MVector<std::string_view>& named_entities, const MVector<std::string_view>& coms_name);

		//void command_record_remove_entities_components_by_component_name(const MVector<EntityHandle>& entities_id, const MVector<std::string_view>& coms_name);

		void command_record_remove_entities_components(const MVector<EntityHandle>& entities_id, const MVector<ComponentID>& coms_id);

		void command_record_remove_entities_components_by_component_name(const MVector<EntityHandle>& entities_id, const MVector<std::string_view>& coms_name);

		//void command_remove_entities(const MVector<EntityHandle>& named_entities);

		//void command_remove_component(ComponentID id);

		//void command_update_component_data_layout(ComponentLayout, DataLayout);

		void submit_command();

		//bool command_remove_entities_components(std::span<uint32_t> entities_id, std::span<ComponentID> coms_id);

		//bool command_remove_entities(std::span<uint32_t> entities_id, std::span<ComponentID> coms_id);

		//bool command_remove_entities(std::span<uint32_t> entities_id, std::span<ComponentID> coms_id);

		bool is_entity_has_component(EntityHandle id, ComponentID com);

		std::optional<ComponentID> create_pair_component_dynamic(uint64_t A, EntityHandle B);

		std::optional<ComponentID> create_pair_component_dynamic(EntityHandle A, EntityHandle B);

		std::optional<ComponentID> create_pair_component_dynamic(std::string_view A, std::string_view B);

		bool is_entity_alived(EntityHandle id);

		bool is_component(uint64_t id) const;

		bool is_component_data(uint64_t id);

		bool is_component_tag(uint64_t id);

		bool is_component_pair(uint64_t id);

		bool is_static_component(uint64_t id) const;

		bool is_dynamic_component(uint64_t id) const;

		ComponentLayout* get_component_layout(uint64_t id);

		//bool update_component_from_static(ComponentID id, ComponentID static_component);

		//bool remove_pair_component_dynamic(ComponentID id);

		//bool remove_component();

		//bool remove_archetype_node(ArchetypeID id);

		EntityData* get_entity_data_if_alived(EntityHandle id) {
			return _entity_vec.get_entity_if_alive(id);
		}

		std::optional<std::string_view> get_entity_name(EntityHandle id) {
			return get_entity_name(id._index);
		}

		std::optional<std::string_view> get_entity_name(uint32_t id) {
			const auto find_name = _map_ComponentID_to_ComponentLayout.find(id);
			if (find_name != _map_ComponentID_to_ComponentLayout.end()) {
				return find_name->second->get_name();
			}
			return {};
		}

		std::optional<EntityHandle> get_named_entity_handle(std::string_view name) {
			if (const auto v = _map_ComponentName_to_ComponentID.find(name); v != _map_ComponentName_to_ComponentID.end()) {
				const auto res = _entity_vec.index_to_handle((uint32_t)v->second);
				if (res.has_value()) {
					return res.value();
				} else {
					return {};
				}
			}
			return {};
		}

		std::optional<EntityHandle> get_named_entity_handle_by_index(uint32_t index) const {
			const auto res = _entity_vec.index_to_handle(index);
			if (res.has_value()) {
				return res.value();
			} else {
				return {};
			}
		}


		//auto get_view_entities() const {
		//	return _entity_vec.get
		//}

		auto get_view_components_id() const {
			return std::ranges::views::keys(_map_ComponentID_to_ComponentLayout);
		}

		auto get_view_components_name() const {
			return std::ranges::views::keys(_map_ComponentName_to_ComponentID);
		}

		auto get_view_archetypes_id() const {
			return std::ranges::views::keys(_map_ArchetypeID_to_Archetype);
		}

		std::optional<MString> get_archetype_struct_desc(ArchetypeID id) {

			const auto res = get_archetype_node_if_exist(id);
			if (!res) {
				return {};
			}

			MString name = "[";
			for (uint32_t i = 0; i < res->_components.size(); i++) {
				name += get_component_layout(res->_components[i])->get_name();
				if (i != res->_components.size() - 1) {
					name += ", ";
				}
			}
			name += "]";

			return name;
		}
		std::optional<ComponentID> try_register_component_from_static(std::string_view static_component_name, bool auto_reference = true);


	private:
		inline uint64_t make_pair_component_id(uint64_t A, uint64_t B) const {
			return (A << 32) | (B & (uint64_t)(0x0000'0000'FFFF'FFFF));
		}


	private:

		bool is_entity_has_component(uint32_t id, ComponentID com);

		//Static Component Register or Get
		std::optional<ComponentID> try_register_component_from_static(ComponentID static_component, bool auto_reference = true);

		//Dynamic Component Create
		std::optional<ComponentID> create_pair_component_dynamic_StaticData_Entity(ComponentID A, EntityHandle B);

		std::optional<ComponentID> create_pair_component_dynamic_StaticTag_Entity(ComponentID A, EntityHandle B);

		std::optional<ComponentID> create_pair_component_dynamic_Entity_Entity(EntityHandle A, EntityHandle B);

		//Archetype Node Info Fetch
		ArchetypeNode* get_archetype_root_node();

		ArchetypeNode* get_archetype_node_if_exist(ArchetypeID id);

		ArchetypeNode* get_archetype_node_if_contain_component(ArchetypeID id, ComponentID com);

		//Archetype Node Walk Edge on Remove 
		ArchetypeNode* impl_get_or_create_archetype_node_single_edge_remove(ArchetypeNode* archtype_node, ComponentID remove_com);

		//Archetype Node Walk Edge on Add 
		ArchetypeNode* impl_get_or_create_archetype_node_single_edge_add(ArchetypeNode* archtype_node, ComponentID new_com);

		//Archetype Node Create
		ArchetypeNode* impl_get_or_create_archetype_node(const MVector< ComponentID>& coms);

		//Command Stage
		void submit_command_stage_entities_archetype_move();

		void command_record_modify_entities_components(const MVector<uint32_t>& entities_id, CommandType type, const MVector<ComponentID>& coms_id);

		void command_modify_preprocess();

	private:
		using Column = uint32_t;

		using Row = uint32_t;

		MString _name{};

		DensifySystemManager* _manager{};

		// ##==============================================##
		// ##============ Archetype Graph Part ============##
		// ##==============================================##

		//counter for Archetype Node id
		uint32_t _Archetype_ID_counter{};

		//Archetype Graph Node ID ->  Archetype Graph Node
		MMap<ArchetypeID, std::unique_ptr<ArchetypeNode>> _map_ArchetypeID_to_Archetype{};

		//Component List -> Archetype Graph Node ID
		MMap<MVector<ComponentID>, ArchetypeID> _map_ComponentList_to_ArchetypeID{};

		//Component ID -> (Archetype Graph Node ID, Column)
		MMap<ComponentID, MMap<ArchetypeID, Column>> _map_ComponentID_to_Position{};

		// ##==============================================##
		// ##============ Component Define Part ===========##
		// ##==============================================##

		// 有名entity可以被建模成component使用,但是仅限在pair中间

		//动态Component的Layout存储
		MMap<ComponentID, std::unique_ptr<ComponentLayout>> _map_ComponentID_to_Dynamic_ComponentLayout_Store{};

		//Component ID -> layout
		MMap<ComponentID, ComponentLayout*> _map_ComponentID_to_ComponentLayout{};

		//Component Name -> ID
		MMap<MString, ComponentID> _map_ComponentName_to_ComponentID{};

		// pair A -> set B
		MMap<ComponentID, MSet<ComponentID>> _map_pair_A_to_B{};

		// pair B -> set A
		MMap<ComponentID, MSet<ComponentID>> _map_pair_B_to_A{};

		// ##===============================================##
		// ##================ Entity Part ==================##
		// ##==============================================##

		EntityVector _entity_vec{};

		//Entity -> Archetype Graph Node ID
		//MMap<uint32_t, std::pair<ArchetypeID, Row>> _map_EntityID_to_ArchetypeID_Row{};
		
		// ##==============================================================##
		// ##================ inline command record Part ==================##
		// ##==============================================================##
		MVector<uint32_t> _vec_dirty_entities_idx{};

		MSet<uint64_t> _set_archetype_move_record{};

		//MMap<uint32_t, MSet<std::pair<CommandType, ComponentID>>> _map_command_record{};

		//old archetype id -> target archetype id,  <Entity Index, Row in old>
		MMap<std::pair<ArchetypeID, ArchetypeID>, MVector<std::pair<uint32_t, uint32_t>>> _map_commnad_old_ArchetypeID_to_target_ArchetypeID{};

		//记录哪些Archetype在执行命令后内部结构被修改了, 以通知之后的查询器缓存更新, 在每次执行命令前会清空
		MSet<ArchetypeID> _set_changed_ArchetypeID{};

		
	};

}
