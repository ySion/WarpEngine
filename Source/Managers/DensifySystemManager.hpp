#pragma once

#include "DensifySystemManager.hpp"

#include "Core/Core.hpp"
#include "Data/Data.hpp"

#include "DensifySystem/Collection.hpp"


namespace Warp::DensifySystem {

	//Pure Component:  pure component也就是非pair component

	class DensifySystemManager {
	public:
		//一共支持25W个component, 从uint32_t的顶端 - 25w开始分配
		
		DensifySystemManager(){
			_id_count = std::numeric_limits<uint32_t>::max() - WARP_MAX_COMPONENT;
		}
		~DensifySystemManager() = default;

		std::optional<uint64_t> create_component(std::string_view name, Data::DataLayout* base_layout = nullptr) {
			if(base_layout == nullptr){
				return create_tag_component(name);
			} else {
				return create_data_component(name, base_layout);
			}
		}

		std::optional<uint64_t> create_data_component(std::string_view name, Data::DataLayout* base_layout) {
			if (const auto res = create_component_impl(name, base_layout, _id_count); res.has_value()) {
				_id_count++;
				return res;
			}
			return {};
		}

		std::optional<uint64_t> create_tag_component(std::string_view name) {
			if (const auto res = create_tag_component_impl(name, _id_count); res.has_value()) {
				_id_count++;
				return res;
			}
			return {};
		}

		std::optional<uint64_t> create_pair_component(std::string_view A, std::string_view B) {
			return create_pair_component_impl(A, B);
		}

		std::optional<uint64_t> find_component(std::string_view name) const {
			const auto ptr = _map_component_name_to_component_id.find(name);
			if (ptr != _map_component_name_to_component_id.end()) {
				return ptr->second;
			}
			return {};
		}

		inline bool contains_component(uint64_t id) const {
			return _map_component_id_to_component_layout.contains(id);
		}

		inline bool contains_component(std::string_view name) const {
			return _map_component_name_to_component_id.contains(name);
		}

		inline bool contains_tag_component(uint64_t id) {
			if(!contains_component(id)) return false;
			return _map_component_id_to_component_layout[id]->get_component_type() == ComponentType::StaticTag;
		}

		inline bool continas_data_component(uint64_t id) {
			if (!contains_component(id)) return false;
			return _map_component_id_to_component_layout[id]->get_component_type() == ComponentType::StaticData;
		}

		inline bool contains_pure_component(uint64_t id) { //pure component, 也就是非pair的component
			if (!contains_component(id)) return false;
			return _map_component_id_to_component_layout[id]->get_component_type() != ComponentType::Pair;
		}

		static inline bool is_idx_in_pure_component_range(uint64_t id) {
			return (id >= (std::numeric_limits<uint32_t>::max() - WARP_MAX_COMPONENT)) && (id < std::numeric_limits<uint32_t>::max());
		}

		inline bool contains_pure_component(std::string_view name)  {
			if (const auto res = find_component(name); res.has_value()) {
				return _map_component_id_to_component_layout[res.value()]->get_component_type() != ComponentType::Pair;
			}
			return false;
		}

		bool contains_pair_component(uint64_t A, uint64_t B) const {
			const uint64_t id = make_pair_component_id(A, B);
			return _map_component_id_to_component_layout.contains(id);
		}

		bool contains_pair_component(uint64_t com) const {
			return _map_component_id_to_component_layout.contains(com);
		}

		const MSet<uint64_t>* get_pair_component_B_to_A_set(std::string_view B) {
			const auto id = find_component(B);
			if (!id) return {};
			if (!_map_pair_B_to_A.contains(id.value())) return nullptr;
			return &_map_pair_B_to_A[id.value()];
		}

		const MSet<uint64_t>* get_pair_component_A_to_B_set(std::string_view A) {
			const auto id = find_component(A);
			if (!id) return {};
			if (!_map_pair_A_to_B.contains(id.value())) return nullptr;
			return &_map_pair_A_to_B[id.value()];
		}

		const MSet<uint64_t>* get_pair_component_B_to_A_set(uint64_t B) {
			if (!contains_component(B)) return nullptr;
			if (!_map_pair_B_to_A.contains(B)) return nullptr;
			return &_map_pair_B_to_A[B];
		}

		const MSet<uint64_t>* get_pair_component_A_to_B_set(uint64_t A) {
			if (!contains_component(A)) return nullptr;
			if (!_map_pair_A_to_B.contains(A)) return nullptr;
			return &_map_pair_A_to_B[A];
		}

		ComponentLayout* get_component_layout(uint64_t id) {
			if (!contains_component(id)) return nullptr;
			return _map_component_id_to_component_layout[id].get();
		}

		Collection* create_collection(std::string_view collection_name) {
			if (_map_name_to_collection.contains(collection_name)) {
				MString msg = MString::format("DensifyComponentLayoutManager::create_collection: collection with name {} already exists.", collection_name);
				error(msg);
			}
			_map_name_to_collection[collection_name] = std::make_unique<Collection>(this, collection_name);
			return _map_name_to_collection[collection_name].get();
		}

		void remove_collection(std::string_view name) {
			if (_map_name_to_collection.contains(name)) {
				_map_name_to_deleted_collection[name] = std::move(_map_name_to_collection[name]);
				_map_name_to_collection.erase(name);
			}
		}

		void clean_collection_trashcan() {
			_map_name_to_deleted_collection.clear();
		}

	private:
		inline uint64_t make_pair_component_id(uint64_t A, uint64_t B) const {
			return (A << 32) | (B & (uint64_t)(0x0000'0000'FFFF'FFFF));
		}

		std::optional<uint64_t> create_pair_component_impl(std::string_view A, std::string_view B) {
			if(A == B) {
				MString msg = MString::format("DensifyComponentLayoutManager::create_pair_component: pair ({}, {}) failed, A and B are the same.", A, B);
				error(msg);
				return {};
			}

			const auto com_A = find_component(A);
			if (!com_A) {
				MString msg = MString::format("DensifyComponentLayoutManager::create_pair_component: component with name {} does not exist.", A);
				error(msg);
				return {};
			}

			const auto com_B = find_component(B);
			if (!com_B) {
				MString msg = MString::format("DensifyComponentLayoutManager::create_pair_component: component with name {} does not exist.", B);
				error(msg);
				return {};
			}
			
			if(!contains_pure_component(com_A.value())) {
				MString msg = MString::format("DensifyComponentLayoutManager::create_pair_component: component {} is not a pure component, create pair ({}, {}) failed.", A, A, B);
				error(msg);
				return {};
			}

			if (!contains_pure_component(com_B.value())) {
				MString msg = MString::format("DensifyComponentLayoutManager::create_pair_component: component {} is not a pure component, create pair ({}, {}) failed.", B, A, B);
				error(msg);
				return {};
			}

			uint64_t componentID = make_pair_component_id(com_A.value(), com_B.value());

			if(_map_component_id_to_component_layout.contains(componentID)) {
				MString msg = MString::format("DensifyComponentLayoutManager::create_pair_component: pair ({}, {}) already exists.", A, B);
				error(msg);
				return {};
			}

			MString pair_name = MString::format("({},{})", A, B);

			auto layout_A = get_component_layout(com_A.value());
			auto layout_B = get_component_layout(com_B.value());

			auto ptr = std::make_unique<ComponentLayout>(componentID, pair_name, layout_A->get_layout(), layout_B->get_layout(),
				ComponentType::Pair, layout_A->get_component_type(), layout_B->get_component_type());

			if (!ptr) {
				MString msg = MString::format("DensifyComponentLayoutManager::register_component: create {} failed.", pair_name);
				error(msg);
				return {};
			}
			_map_component_name_to_component_id[pair_name] = componentID;
			_map_component_id_to_component_layout[componentID] = std::move(ptr);

			if(!_map_pair_A_to_B.contains(com_A.value())) {
				_map_pair_A_to_B[com_A.value()] = { };
			}

			if (!_map_pair_B_to_A.contains(com_B.value())) {
				_map_pair_B_to_A[com_B.value()] = { };
			}

			_map_pair_A_to_B[com_A.value()].insert(com_B.value());
			_map_pair_B_to_A[com_B.value()].insert(com_A.value());

			return componentID;
		}

		std::optional<uint64_t> create_tag_component_impl(std::string_view component_name, uint64_t idx) {
			if (_map_component_name_to_component_id.contains(component_name)) {
				MString msg = MString::format("DensifyComponentLayoutManager::register_component: component with name {} already exists.", component_name);
				error(msg);
				return {};
			}

			if (!is_idx_in_pure_component_range(idx)) {
				MString msg = MString::format("DensifyComponentLayoutManager::register_component: component count over 250'000, create component name {} failed, idx {}.", component_name, idx);
				error(msg);
				return {};
			}

			auto ptr = std::make_unique<ComponentLayout>(idx, component_name, nullptr, nullptr, ComponentType::StaticTag);
			if (!ptr) {
				MString msg = MString::format("DensifyComponentLayoutManager::register_component: component {} create failed.", component_name);
				error(msg);
				return {};
			}

			_map_component_name_to_component_id[component_name] = idx;
			_map_component_id_to_component_layout[idx] = std::move(ptr);
			return idx;
		}

		std::optional<uint64_t> create_component_impl(std::string_view component_name, Data::DataLayout* base_layout, uint64_t idx) {
			if (const observer_ptr p{ base_layout }; !p.is_object_vaild()) {
				MString msg = "ComponentLayout::ComponentLayout(Data::DataLayout* ptr) ptr is nullptr or a invaild object";
				error(msg);
				return {};
			}

			if(!base_layout->is_pure())
			{
				MString msg = MString::format("DensifyComponentLayoutManager::register_component: base data layout is not a pure datalayout(no document, no ptr), component {} create failed.", component_name);
				error(msg);
				return {};
			}

			if (!base_layout->object_get_value<std::string_view>("name")) {
				MString msg = MString::format("DensifyComponentLayoutManager::register_component: base data layout is not a named datalayout, component {} create failed.", component_name);
				error(msg);
				return {};
			}

			if (_map_component_name_to_component_id.contains(component_name)) {
				MString msg = MString::format("DensifyComponentLayoutManager::register_component: component with name {} already exists.", component_name);
				error(msg);
				return {};
			}

			if (!is_idx_in_pure_component_range(idx)) {
				MString msg = MString::format("DensifyComponentLayoutManager::register_component: component count over 250'000, create component name {} failed, idx {}.", component_name, idx);
				error(msg);
				return {};
			}

			auto ptr = std::make_unique<ComponentLayout>(idx, component_name,  base_layout, nullptr, ComponentType::StaticData);
			if (!ptr) {
				MString msg = MString::format("DensifyComponentLayoutManager::register_component: component {} create failed.", component_name);
				error(msg);
				return {};
			}

			_map_component_name_to_component_id[component_name] = idx;
			_map_component_id_to_component_layout[idx] = std::move(ptr);
			return idx;
		}

		uint64_t _id_count = std::numeric_limits<uint32_t>::max() - WARP_MAX_COMPONENT;

		// ##=========== Static Component Define Part ===========

		//ComponentID -> Component Layout
		MMap<uint64_t, std::unique_ptr<ComponentLayout>> _map_component_id_to_component_layout{};

		//Component name -> Component ID  (Pure Component only, not for pair)
		MMap<MString, uint64_t> _map_component_name_to_component_id{};

		//Component ID to other Component ID in pair
		MMap<uint64_t, MSet<uint64_t>> _map_pair_A_to_B{};

		//Component ID to other Component ID in pair
		MMap<uint64_t, MSet<uint64_t>> _map_pair_B_to_A{};

		// ##=========== Collection Part ===========

		//Collection
		MMap<MString, std::unique_ptr<Collection>> _map_name_to_collection{};

		//Collection TrashCan
		MMap<MString, std::unique_ptr<Collection>> _map_name_to_deleted_collection{};
	};
}