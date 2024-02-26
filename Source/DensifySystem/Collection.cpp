#include "Collection.hpp"

#include <execution>

#include "Managers/DensifySystemManager.hpp"

using namespace Warp::DensifySystem;

Collection::Collection(DensifySystemManager* manager, std::string_view name) : _name(name), _manager(manager) { //创建一个初始节点
	_map_ArchetypeID_to_Archetype[0] = std::unique_ptr<ArchetypeNode>{ new ArchetypeNode{0, {}, {}} };
}

//Named Entity被建模成Component
EntityHandle Collection::command_create_single_entity(std::string_view name, uint32_t flag) {
	EntityHandle new_index;
	if (!name.empty()) {
		if (_map_ComponentName_to_ComponentID.contains(name) || _manager->contains_component(name)) {
			MString msg = MString::format("Collection::command_create_signle_entity: Entity can't be named as {} component, because this name is exist as a component, name was be reset to \"\".", name);
			error(msg);
			new_index = _entity_vec.new_entity("", flag);
		} else {
			new_index = _entity_vec.new_entity(name, flag);
			auto ptr = std::make_unique<ComponentLayout>(new_index._index, name, nullptr, nullptr, ComponentType::Entity);
			_map_ComponentID_to_ComponentLayout[new_index._index] = ptr.get();
			_map_ComponentID_to_Dynamic_ComponentLayout_Store[new_index._index] = std::move(ptr);
			_map_ComponentName_to_ComponentID[name] = new_index._index;
		}
	} else {
		new_index = _entity_vec.new_entity("", flag);
	}

	ArchetypeNode* root_node = get_archetype_root_node();
	auto& e_idx_vec = root_node->_data.get_entity_idx_vec();
	auto& e_free_idx_vec = root_node->_data.get_free_row_vec();
	root_node->_data.emplace_space(1);

	uint32_t target_row;
	if (!e_free_idx_vec.empty()) {
		target_row = e_free_idx_vec.back();
		e_free_idx_vec.pop_back();
		e_idx_vec[target_row] = new_index._index;
	} else {
		target_row = e_idx_vec.size_u32();
		e_idx_vec.push_back(new_index._index);
	}
	_entity_vec.get_archetype_row_vec()[new_index._index] = target_row;

	return new_index;
}

bool Collection::command_remove_signle_entity(EntityHandle handle)
{
	if(!is_entity_alived(handle)) {
		return false;
	}
	if(_entity_vec.is_entity_named(handle._index)) {
		std::string_view name = get_entity_name(handle).value();
		_map_ComponentName_to_ComponentID.erase(name);
		_map_ComponentID_to_ComponentLayout.erase(handle._index);
		_map_ComponentID_to_Dynamic_ComponentLayout_Store.erase(handle._index);
		
	}
	return _entity_vec.remove_entity(handle);
}


MVector<EntityHandle> Collection::commnad_create_entities(uint32_t entity_count, uint32_t flag) {
	//WARP_BENCH(T0);
	MVector<uint32_t> entity_idxs_in_vec{};
	MVector<EntityHandle> result = _entity_vec.new_entities(entity_count, flag, entity_idxs_in_vec);
	const auto evec_size = _entity_vec.size();
	//WARP_BENCH(T1);

	ArchetypeNode* root_node = get_archetype_root_node();
	auto& e_idx_vec = root_node->_data.get_entity_idx_vec();

	root_node->_data.emplace_space(result.size_u32());
	//WARP_BENCH(T2);
	
	auto& archetype_row_vec = _entity_vec.get_archetype_row_vec();

	for(uint32_t i = 0 ; i < result.size(); i++) {
		archetype_row_vec[entity_idxs_in_vec[i]] = entity_idxs_in_vec[i];
	}

	//WARP_BENCH(T3);
	//WARP_BENCH_INFO("Make Entities", T0, T1);
	//WARP_BENCH_INFO("Emplace Space", T1, T2);
	//WARP_BENCH_INFO("MoveTIme", T2, T3);
	return result;
}

void Collection::command_record_add_entities_components(const MVector<EntityHandle>& entities_id, const MVector<ComponentID>& coms_id) {

	for (uint32_t i = 0; i < coms_id.size_u32(); i++) {
		try_register_component_from_static(coms_id[i]);
		if (!_map_ComponentID_to_ComponentLayout.contains(coms_id[i])) {
			MString msg = MString::format("Collection::command_record_add_entities_components: Component ID {} not exist, create faild.", coms_id[i]);
			error(msg);
			return;
		}
	}

	MVector<uint32_t> vec_vailed_entities_id{};
	vec_vailed_entities_id.reserve(entities_id.size());

	for(uint32_t i = 0; i < entities_id.size(); i++) {
		if (!is_entity_alived(entities_id[i])) continue;
		vec_vailed_entities_id.push_back(entities_id[i]._index);
	}

	command_record_modify_entities_components(vec_vailed_entities_id, CommandType::AddComponent, coms_id);
}

void Collection::command_record_modify_entities_components(const MVector<uint32_t>& entities_id, CommandType type, const MVector<ComponentID>& coms_id) {
	
	// make command
	auto& ref_entity_cmd_vec = _entity_vec.get_command_record_vec();
	
	CommandType opposite_cmd = (type == CommandType::AddComponent ? CommandType::RemoveComponent : CommandType::AddComponent);

	const int entity_size = entities_id.size();

	for (uint32_t i = 0; i < entity_size; ++i) {
		const auto entity_idx = entities_id[i];
		if (!_entity_vec.is_command_dirty(entity_idx)) {
			_entity_vec.set_command_dirty(entity_idx);
			_vec_dirty_entities_idx.push_back(entity_idx);
		}
	}

	if (type == CommandType::AddComponent) {
		if(entity_size > 5000) { // 多线程
			tbb::parallel_for_each(entities_id.begin(), entities_id.end(), [&](uint32_t i) {
				const auto entity_idx = entities_id[i];
				auto& entity_command_record = ref_entity_cmd_vec[entity_idx];
				uint32_t coms_size = coms_id.size();
				for (uint32_t j = 0; j < coms_size; j++) {
					std::pair opposite_key = { opposite_cmd,  coms_id[j] };
					const auto found = entity_command_record.find(opposite_key);
					if (found != entity_command_record.end()) {	// 有相反命令, 那么就中和, 删除相反命令
						entity_command_record.erase(found);
					} else {					// 否则看看entity是否有这个component, 如果没有, 就加入添加命令
						if (!is_entity_has_component(entity_idx, coms_id[j]) && entity_idx != coms_id[j]) {
							entity_command_record.insert({ type, coms_id[j] });
						}
					}
				}
			});
		} else { // 单线程
			for (uint32_t i = 0; i < entity_size; ++i) {

				const auto entity_idx = entities_id[i];
				auto& entity_command_record = ref_entity_cmd_vec[entity_idx];

				uint32_t coms_size = coms_id.size();
				for (uint32_t j = 0; j < coms_size; j++) {

					std::pair opposite_key = { opposite_cmd,  coms_id[j] };
					const auto found = entity_command_record.find(opposite_key);

					if (found != entity_command_record.end()) {	// 有相反命令, 那么就中和, 删除相反命令
						entity_command_record.erase(found);
					} else {					// 否则看看entity是否有这个component, 如果没有, 就加入添加命令
						if (!is_entity_has_component(entities_id[i], coms_id[j]) && entities_id[i] != coms_id[j]) {
							entity_command_record.insert({ type, coms_id[j] });
						}
					}
				}
			}
		}
	} else if (type == CommandType::RemoveComponent) {

		if (entity_size > 5000) { // 多线程
			tbb::parallel_for_each(entities_id.begin(), entities_id.end(), [&](uint32_t i) {
				const auto entity_idx = entities_id[i];
				auto& entity_command_record = ref_entity_cmd_vec[entity_idx];
				uint32_t coms_size = coms_id.size();
				for (uint32_t j = 0; j < coms_size; j++) {
					std::pair opposite_key = { opposite_cmd,  coms_id[j] };
					const auto found = entity_command_record.find(opposite_key);
					if (found != entity_command_record.end()) {	// 有相反命令, 那么就中和, 删除相反命令
						entity_command_record.erase(found);
					} else {					// 否则看看entity是否有这个component, 如果有, 那么就加入删除命令
						if (is_entity_has_component(entities_id[i], coms_id[j])) {
							entity_command_record.insert({ type, coms_id[j] });
						}
					}
				}
			});
		} else { // 单线程

			for (uint32_t i = 0; i < entity_size; ++i) {
				const auto entity_idx = entities_id[i];

				auto& entity_command_record = ref_entity_cmd_vec[entity_idx];
				uint32_t coms_size = coms_id.size();
				for (uint32_t j = 0; j < coms_size; j++) {

					std::pair opposite_key = { opposite_cmd,  coms_id[j] };
					const auto found = entity_command_record.find(opposite_key);

					if (found != entity_command_record.end()) {  // 有相反命令, 那么就中和, 删除相反命令
						entity_command_record.erase(found);
					} else {				     // 否则看看entity是否有这个component, 如果有, 那么就加入删除命令
						if (is_entity_has_component(entities_id[i], coms_id[j])) {
							entity_command_record.insert({ type, coms_id[j] });
						}
					}
				}
			}
		}
	}
}

void Collection::command_modify_preprocess() {
	if(_vec_dirty_entities_idx.empty()) { return; }
	MVector<ComponentID> final_coms{};
	MSet<ComponentID> remove_coms{};

	auto& _EntityID_to_ArchetypeID_Row = _entity_vec.get_archetype_row_vec();
	auto& ref_entity_cmd_vec = _entity_vec.get_command_record_vec();

	for(auto idx : _vec_dirty_entities_idx) {
		auto& cmd_list = ref_entity_cmd_vec[idx];
		if (cmd_list.empty()) { continue; }

		const auto entity_id = idx;
		const auto temp_archetype_id_row = _EntityID_to_ArchetypeID_Row[entity_id];
		const uint32_t archetype_id = (uint32_t)(temp_archetype_id_row >> 32);
		const uint32_t row = (uint32_t)(temp_archetype_id_row  & 0x0000'0000'FFFF'FFFF);

		const auto now_archetype = _map_ArchetypeID_to_Archetype[archetype_id].get();
		const auto& coms_ref = now_archetype->_components;

		auto target_archetype = now_archetype;
		for(auto& j : cmd_list) {
			if (j.first == CommandType::AddComponent) {
				final_coms.push_back(j.second); // 读取, + 获取 + 读取
				target_archetype = impl_get_or_create_archetype_node_single_edge_add(target_archetype, j.second);
			} else {
				remove_coms.insert(j.second);
				target_archetype = impl_get_or_create_archetype_node_single_edge_remove(target_archetype, j.second);
			}
		}

		for (uint32_t j = 0; j < coms_ref.size(); j++) {
			if (!remove_coms.contains(coms_ref[j])) {
				final_coms.push_back(coms_ref[j]);
			}
		}

		std::pair key = { now_archetype->_id, target_archetype->_id };
		_set_archetype_move_record.insert((uint64_t)now_archetype->_id << 32 | (uint64_t)target_archetype->_id);

		//old ArchetypeID -> target ArchetypeID, <entity_id, row>
		auto& cmd_info = _map_commnad_old_ArchetypeID_to_target_ArchetypeID[key];
		cmd_info.emplace_back(entity_id, row);
#if defined WARP_DEBUG
		//Debug
		std::string_view entity_name = "";
		if (_map_ComponentID_to_ComponentLayout.contains(entity_id)) {
			entity_name = get_component_layout(entity_id)->get_name();
		}

		info("Command Entity Move Archetype: Entity {} \"{}\", do Archetype {}(Row : {}) -> {}.",
			entity_id,
			entity_name.empty() ? "<Anonymous>" : entity_name,
			get_archetype_struct_desc(key.first).value(),
			row,
			get_archetype_struct_desc(key.second).value());
#endif
		remove_coms.clear();
		final_coms.clear();
		cmd_list.clear(); //保留entity修改缓存, 防止以后需要重复分配内存, 如果之后的操作尝试对entity删除, 那么会被清空
	}
}

//和add很接近
void Collection::command_record_remove_entities_components(const MVector<EntityHandle>& entities_id, const MVector<ComponentID>& coms_id) {

	for (uint32_t i = 0; i < coms_id.size_u32(); i++) {
		if (!_map_ComponentID_to_ComponentLayout.contains(coms_id[i])) {
			MString msg = MString::format("Collection::command_record_remove_entities_components: Component ID {} not exist, create faild.", coms_id[i]);
			error(msg);
			return;
		}
	}

	MVector<uint32_t> vec_vailed_entities_id{};
	vec_vailed_entities_id.reserve(entities_id.size());
	_vec_dirty_entities_idx.reserve(entities_id.size());

	for (uint32_t i = 0; i < entities_id.size(); i++) {
		if (!is_entity_alived(entities_id[i])) continue;
		vec_vailed_entities_id.push_back(entities_id[i]._index);
	}

	command_record_modify_entities_components(vec_vailed_entities_id, CommandType::RemoveComponent, coms_id);
}

void Collection::command_record_remove_entities_components_by_component_name(const MVector<EntityHandle>& entities_id, const MVector<std::string_view>& coms_name) {
	MVector<ComponentID> vec_coms_id{ };
	vec_coms_id.reserve(coms_name.size());

	for (const auto& id : coms_name) {
		if (!_map_ComponentName_to_ComponentID.contains(id)) {
			MString msg = MString::format("Collection::command_record_remove_entities_components_by_component_name: Component {} not exist, create faild.", id);
			error(msg);
			return;
		}
		vec_coms_id.push_back(_map_ComponentName_to_ComponentID[id]);
	}

	MVector<uint32_t> vec_vailed_entities_id{};
	vec_vailed_entities_id.reserve(entities_id.size());
	_vec_dirty_entities_idx.reserve(entities_id.size());
	for (uint32_t i = 0; i < entities_id.size(); i++) {
		if (!is_entity_alived(entities_id[i])) continue;
		vec_vailed_entities_id.push_back(entities_id[i]._index);
	}

	command_record_modify_entities_components(vec_vailed_entities_id, CommandType::RemoveComponent, vec_coms_id);
}

void Collection::submit_command() {
	//stage: 顺序:
	//1 对entity进行删除, 可能某些entity是component, 那么这时候还会触发 remove component, 这时候会生成整个archetype的搬运命令
	//2 Modify Component
	WARP_BENCH(T1);
	command_modify_preprocess();
	WARP_BENCH(T2);
	submit_command_stage_entities_archetype_move();
	WARP_BENCH(T3);
	if(!_vec_dirty_entities_idx.empty()){
		for (uint32_t i = 0; i < _vec_dirty_entities_idx.size(); ++i) {
			const auto entity_idx = _vec_dirty_entities_idx[i];
			_entity_vec.set_command_no_dirty(entity_idx);
		}
		_vec_dirty_entities_idx.clear();
	}
	WARP_BENCH(T4);
	if(!_set_archetype_move_record.empty()){
		_set_archetype_move_record.clear();
		
	}

	WARP_BENCH_INFO("==Modify Cmd", T1, T2);
	WARP_BENCH_INFO("==Submit Cmd", T2, T3);
	WARP_BENCH_INFO("==Clear Set", T3, T4);
}


// add named entitied
void Collection::command_record_add_entities_components_by_name(const MVector<std::string_view>& named_entities, const MVector<std::string_view>& coms_name) {
	MVector<uint32_t> vec_entities_id{ };
	vec_entities_id.reserve(named_entities.size());
	MVector<ComponentID> vec_coms_id{ };
	vec_coms_id.reserve(coms_name.size());
	_vec_dirty_entities_idx.reserve(named_entities.size());

	for (const auto& id : named_entities) {
		auto res = get_named_entity_handle(id);
		if (!res.has_value()) {
			MString msg = MString::format("Collection::command_record_add_entities_components_by_component_name: Entity {} not exist, create faild.", id);
			error(msg);
			return;
		}
		vec_entities_id.push_back(res.value()._index);
	}

	for (const auto& id : coms_name) {
		try_register_component_from_static(id);
		if (!_map_ComponentName_to_ComponentID.contains(id)) {
			MString msg = MString::format("Collection::command_record_add_entities_components_by_component_name: Component {} not exist, create faild.", id);
			error(msg);
			return;
		}
		vec_coms_id.push_back(_map_ComponentName_to_ComponentID[id]);
	}


	command_record_modify_entities_components(vec_entities_id, CommandType::AddComponent, vec_coms_id);
}

void Collection::command_record_add_entities_components_by_component_name(const MVector<EntityHandle>& entities_id, const MVector<std::string_view>& coms_name)
{
	//WARP_BENCH(T1);
	MVector<ComponentID> vec_coms_id{ };
	vec_coms_id.reserve(coms_name.size());
	for (const auto& id : coms_name) {
		try_register_component_from_static(id);
		if (!_map_ComponentName_to_ComponentID.contains(id)) {
			MString msg = MString::format("Collection::command_record_add_entities_components_by_component_name: Component {} not exist, create faild.", id);
			error(msg);
			return;
		}
		vec_coms_id.push_back(_map_ComponentName_to_ComponentID[id]);
	}
	//WARP_BENCH(T2);
	MVector<uint32_t> vec_vailed_entities_id{};
	vec_vailed_entities_id.reserve(entities_id.size());
	_vec_dirty_entities_idx.reserve(entities_id.size());
	//WARP_BENCH(T3);

	for (uint32_t i = 0; i < entities_id.size(); i++) {
		if (!is_entity_alived(entities_id[i])) continue;
		vec_vailed_entities_id.push_back(entities_id[i]._index);
	}


	WARP_BENCH(T4);
	command_record_modify_entities_components(vec_vailed_entities_id, CommandType::AddComponent, vec_coms_id);
	//WARP_BENCH(T5);
	//WARP_BENCH_INFO("Pick Live Entities emplace", T2, T3);
	//WARP_BENCH_INFO("Pick Live Entities", T3, T4);
	//WARP_BENCH_INFO("modify", T4, T5);

}

void Collection::submit_command_stage_entities_archetype_move() {
	_set_changed_ArchetypeID.clear();
	if (_set_archetype_move_record.empty()) {
		return;
	}

	//old index -> target index, and size
	MVector<std::tuple<uint32_t, uint32_t, uint32_t>> component_idxmap_old_to_target{};
	component_idxmap_old_to_target.reserve(8);
	
	auto& EntityID_to_ArchetypeID_Row = _entity_vec.get_archetype_row_vec();
	for(auto& i : _set_archetype_move_record) {
		ArchetypeID old_id = i >> 32;
		ArchetypeID target_id = i & 0x0000'0000'FFFF'FFFF;

		ArchetypeNode* old_node = get_archetype_node_if_exist(old_id);
		ArchetypeNode* target_node = get_archetype_node_if_exist(target_id);

		ArchetypeData& old_data = old_node->_data;
		ArchetypeData& target_data = target_node->_data;

		auto& old_data_vec = old_data.get_data();
		auto& target_data_vec = target_data.get_data();

		auto& old_entity_idx_vec = old_data.get_entity_idx_vec();
		auto& target_entity_idx_vec = target_data.get_entity_idx_vec();

		auto& old_free_row_vec = old_data.get_free_row_vec();
		auto& traget_free_row_vec = target_data.get_free_row_vec();

		auto& entities_need_moved_vec = _map_commnad_old_ArchetypeID_to_target_ArchetypeID[{old_id, target_id}];

		target_data.emplace_space(entities_need_moved_vec.size_u32());

		//calc component relate idx in old and target

		auto& old_coms_mapper = old_data.get_map_com_to_data_idx();
		auto& target_coms_mapper = target_data.get_map_com_to_data_idx();

		for(auto& j : old_coms_mapper) {
			if(auto finder = target_coms_mapper.find(j.first); finder != target_coms_mapper.end()) {
				const auto ptr = old_data_vec[j.second].get();
				if(ptr != nullptr) {
					component_idxmap_old_to_target.push_back({ j.second, finder->second, ptr->element_size() });
				}
				if(get_component_layout(j.first)->get_component_type() == ComponentType::Pair) {
					const auto ptr2 = old_data_vec[j.second + 1].get();
					if (ptr2 != nullptr) {
						component_idxmap_old_to_target.push_back({ j.second + 1, finder->second + 1, ptr2->element_size() });
					}
				}
			}
		}
		
		for(uint32_t j = 0; j < entities_need_moved_vec.size(); j++) { //old move_to target

			uint32_t entity_idx = entities_need_moved_vec[j].first;

			uint32_t target_row;
			uint32_t old_row = entities_need_moved_vec[j].second;
			if(!traget_free_row_vec.empty()) {
				target_row = traget_free_row_vec.back();
				traget_free_row_vec.pop_back();
				target_entity_idx_vec[target_row] = entity_idx; //first is entity index, second is old row;
			} else {
				target_row = target_entity_idx_vec.size_u32();
				target_entity_idx_vec.push_back(entity_idx);
			}

			for (uint32_t k = 0; k < component_idxmap_old_to_target.size_u32(); k++) {
				const auto copy_size = std::get<2>(component_idxmap_old_to_target[k]);
				if(copy_size == 0) { continue; }
				const auto column_old = std::get<0>(component_idxmap_old_to_target[k]);
				const auto column_target = std::get<1>(component_idxmap_old_to_target[k]);
				memcpy(target_data_vec[column_target]->at(target_row), old_data_vec[column_old]->at(target_row), copy_size);
			}

			old_entity_idx_vec[old_row] = EntityVector::InvaildID;
			old_free_row_vec.push_back(old_row);
			EntityID_to_ArchetypeID_Row[entity_idx] = ((uint64_t)target_id << 32) | target_row;
		}
		_set_changed_ArchetypeID.insert(old_id);
		_set_changed_ArchetypeID.insert(target_id);

		component_idxmap_old_to_target.clear();
		entities_need_moved_vec.clear();
	}
	//_map_commnad_old_ArchetypeID_to_target_ArchetypeID.clear();
}

bool Collection::is_entity_has_component(EntityHandle id, ComponentID com) {
	if (is_entity_alived(id)) {
		return is_entity_has_component(id._index, com);
	}
	return false;
}

bool Warp::DensifySystem::Collection::is_entity_has_component(uint32_t id, ComponentID com) {
	auto& ref = _entity_vec.get_archetype_row_vec();
	if (const auto com_position = _map_ComponentID_to_Position.find(com); com_position != _map_ComponentID_to_Position.end()) {
		return com_position->second.contains(ref[id] >> 32);
	}
	return false;
}

std::optional<ComponentID> Collection::try_register_component_from_static(ComponentID static_component, bool auto_reference) {

	if (_map_ComponentID_to_ComponentLayout.contains(static_component)) {
		return static_component;
	}

	if (_manager->contains_component(static_component)) {

		const auto layout = _manager->get_component_layout(static_component);

		if (layout->get_component_type() == ComponentType::Pair) {
			const uint64_t A_ID = layout->get_pair_A_id();
			const uint64_t B_ID = layout->get_pair_B_id();

			if (!_map_ComponentID_to_ComponentLayout.contains(A_ID)) {
				if (auto_reference) {
					const auto res = try_register_component_from_static(A_ID, false);
					if (!res.has_value()) { return std::nullopt; }
				} else {
					std::string_view nameA = _manager->get_component_layout(A_ID)->get_name();
					MString msg = MString::format("Collection::try_register_component_from_static: create pair failed, A Component {} not in collection {}, please create it first or use 'auto_reference'.", nameA, _name);
					error(msg);
					return std::nullopt;
				}
			}
			if (!_map_ComponentID_to_ComponentLayout.contains(B_ID)) {
				if (auto_reference) {
					const auto res = try_register_component_from_static(B_ID, false);
					if (!res.has_value()) { return std::nullopt; }
				} else {
					std::string_view nameB = _manager->get_component_layout(B_ID)->get_name();
					MString msg = MString::format("Collection::try_register_component_from_static: create pair failed, B Component {} not in collection {}, please create it first or use 'auto_reference'.", nameB, _name);
					error(msg);
					return std::nullopt;
				}
			}

			if (!_map_pair_A_to_B.contains(A_ID)) { _map_pair_A_to_B[A_ID] = {}; }
			if (!_map_pair_B_to_A.contains(B_ID)) { _map_pair_B_to_A[B_ID] = {}; }

			_map_pair_A_to_B[A_ID].insert(B_ID);
			_map_pair_B_to_A[B_ID].insert(A_ID);
		}

		_map_ComponentID_to_ComponentLayout[layout->get_id()] = layout;
		_map_ComponentName_to_ComponentID[layout->get_name()] = layout->get_id();
		return layout->get_id();
	} else {
		MString msg = MString::format("Collection::try_register_component_from_static: Component {} not exist, create faild.", static_component);
		warn(msg);
	}

	return std::nullopt;
}

std::optional<ComponentID> Collection::try_register_component_from_static(std::string_view static_component_name, bool auto_reference) {

	if (_map_ComponentName_to_ComponentID.contains(static_component_name)) {
		return _map_ComponentName_to_ComponentID[static_component_name];
	}

	if (const auto id = _manager->find_component(static_component_name); id.has_value()) {

		const auto layout = _manager->get_component_layout(id.value());

		if (layout->get_component_type() == ComponentType::Pair) {
			const uint64_t A_ID = layout->get_pair_A_id();
			const uint64_t B_ID = layout->get_pair_B_id();

			if (!_map_ComponentID_to_ComponentLayout.contains(A_ID)) {
				if (auto_reference) {
					const auto res = try_register_component_from_static(A_ID, false);
					if (!res.has_value()) { return std::nullopt; }
				} else {
					std::string_view nameA = _manager->get_component_layout(A_ID)->get_name();
					MString msg = MString::format("Collection::try_register_component_from_static: create pair failed, A Component {} not in collection {}, please create it first or use 'auto_reference'.", nameA, _name);
					error(msg);
					return std::nullopt;
				}
			}
			if (!_map_ComponentID_to_ComponentLayout.contains(B_ID)) {
				if (auto_reference) {
					const auto res = try_register_component_from_static(B_ID, false);
					if (!res.has_value()) { return std::nullopt; }
				} else {
					std::string_view nameB = _manager->get_component_layout(B_ID)->get_name();
					MString msg = MString::format("Collection::try_register_component_from_static: create pair failed, B Component {} not in collection {}, please create it first or use 'auto_reference'.", nameB, _name);
					error(msg);
					return std::nullopt;
				}
			}

			if (!_map_pair_A_to_B.contains(A_ID)) { _map_pair_A_to_B[A_ID] = {}; }
			if (!_map_pair_B_to_A.contains(B_ID)) { _map_pair_B_to_A[B_ID] = {}; }

			_map_pair_A_to_B[A_ID].insert(B_ID);
			_map_pair_B_to_A[B_ID].insert(A_ID);
		}

		_map_ComponentID_to_ComponentLayout[layout->get_id()] = layout;
		_map_ComponentName_to_ComponentID[layout->get_name()] = layout->get_id();

		return layout->get_id();
	}else {
		MString msg = MString::format("Collection::try_register_component_from_static: Component {} not exist, create faild.", static_component_name);
		warn(msg);
	}

	return std::nullopt;
}

std::optional<ComponentID> Collection::create_pair_component_dynamic_StaticData_Entity(ComponentID A, EntityHandle B) {

	ComponentID new_com_id = make_pair_component_id(A, B._index);

	if (_map_ComponentID_to_Dynamic_ComponentLayout_Store.contains(new_com_id)) {
		return new_com_id;
	}
	auto layout_A = get_component_layout(A);
	std::string_view name_A = layout_A->get_name();
	std::string_view name_B = get_entity_name(B).value();

	MString new_pair_name = MString::format("({},{})", name_A, name_B);

	auto new_pair_layout_ptr = std::make_unique<ComponentLayout>(new_com_id, new_pair_name, layout_A->get_layout(), nullptr,
		ComponentType::Pair, ComponentType::StaticData, ComponentType::Entity);
	auto new_pair_layout_ptr_temp = new_pair_layout_ptr.get();

	_map_ComponentID_to_Dynamic_ComponentLayout_Store[new_com_id] = std::move(new_pair_layout_ptr);
	_map_ComponentID_to_ComponentLayout[new_com_id] = new_pair_layout_ptr_temp;
	_map_ComponentName_to_ComponentID[new_pair_name] = new_com_id;

	if (!_map_pair_A_to_B.contains(A)) { _map_pair_A_to_B[A] = {}; }
	_map_pair_A_to_B[A].insert(B._index);

	if (!_map_pair_B_to_A.contains(B._index)) { _map_pair_B_to_A[B._index] = {}; }
	_map_pair_B_to_A[B._index].insert(A);

	return new_com_id;
}

std::optional<ComponentID> Collection::create_pair_component_dynamic_StaticTag_Entity(ComponentID A, EntityHandle B) {
	return create_pair_component_dynamic_StaticData_Entity(A, B);
}

std::optional<ComponentID> Collection::create_pair_component_dynamic_Entity_Entity(EntityHandle A, EntityHandle B) {

	ComponentID new_com_id = make_pair_component_id(A._index, B._index);

	if (_map_ComponentID_to_Dynamic_ComponentLayout_Store.contains(new_com_id)) {
		return new_com_id;
	}

	std::string_view name_A = get_entity_name(A).value();
	std::string_view name_B = get_entity_name(B).value();

	MString new_pair_name = MString::format("({},{})", name_A, name_B);

	auto new_pair_layout_ptr = std::make_unique<ComponentLayout>(new_com_id, new_pair_name, nullptr, nullptr,
				ComponentType::Pair, ComponentType::Entity, ComponentType::Entity);
	auto new_pair_layout_ptr_temp = new_pair_layout_ptr.get();

	_map_ComponentID_to_Dynamic_ComponentLayout_Store[new_com_id] = std::move(new_pair_layout_ptr);
	_map_ComponentID_to_ComponentLayout[new_com_id] = new_pair_layout_ptr_temp;
	_map_ComponentName_to_ComponentID[new_pair_name] = new_com_id;

	if (!_map_pair_A_to_B.contains(A._index)) { _map_pair_A_to_B[A._index] = {}; }
	_map_pair_A_to_B[A._index].insert(B._index);

	if (!_map_pair_B_to_A.contains(B._index)) { _map_pair_B_to_A[B._index] = {}; }
	_map_pair_B_to_A[B._index].insert(A._index);

	return new_com_id;
}
std::optional<ComponentID> Collection::create_pair_component_dynamic(EntityHandle A, EntityHandle B) {

	EntityData* data_B_opt = get_entity_data_if_alived(B);
	if (!data_B_opt) {
		MString msg = MString::format("Collection::create_pair_component_dynamic: B {} is not a entity or not exist, create dynamic pair faild.", B._index);
		error(msg);
		return std::nullopt;
	}

	if (!_map_ComponentID_to_ComponentLayout.contains(B._index)) {
		MString msg = MString::format("Collection::create_pair_component_dynamic: B {} is not a component entity, create dynamic pair faild.", B._index);
		error(msg);
		return std::nullopt;
	}

	EntityData* data_A_opt = get_entity_data_if_alived((EntityHandle)A);
	if (!data_A_opt) {
		MString msg = MString::format("Collection::create_pair_component_dynamic: A {} is not a entity or not exist, A can be [StaticData, StaticTag, Entity], create dynamic pair faild.", A._index);
		error(msg);
		return std::nullopt;
	}

	if (!_map_ComponentID_to_ComponentLayout.contains(A._index)) {
		MString msg = MString::format("Collection::create_pair_component_dynamic: A {} is not a component entity, create dynamic pair faild.", A._index);
		error(msg);
		return std::nullopt;
	}

	return create_pair_component_dynamic_Entity_Entity(A, B);
}

std::optional<ComponentID> Collection::create_pair_component_dynamic(std::string_view A, std::string_view B) {
	try_register_component_from_static(A, true);
	try_register_component_from_static(B, true);

	if (!_map_ComponentName_to_ComponentID.contains(A)) {
		MString msg = MString::format("Collection::create_pair_component_dynamic: A {} is not a vailed component, create dynamic pair faild.", A);
		error(msg);
		return std::nullopt;
	}

	const auto id_A = _map_ComponentName_to_ComponentID[A];
	const auto layout_A = get_component_layout(_map_ComponentName_to_ComponentID[A]);

	if (!_map_ComponentName_to_ComponentID.contains(B)) {
		MString msg = MString::format("Collection::create_pair_component_dynamic: B {} is not a vailed component, create dynamic pair faild.", B);
		error(msg);
		return std::nullopt;
	}

	const auto id_B = _map_ComponentName_to_ComponentID[B];
	const auto layout_B = get_component_layout(_map_ComponentName_to_ComponentID[B]);

	if (layout_A->get_component_type() == ComponentType::StaticData && layout_B->get_component_type() == ComponentType::Entity) {
		const auto handle = _entity_vec.index_to_handle((uint32_t)id_B);
		if (!handle.has_value()) {
			MString msg = MString::format("Collection::create_pair_component_dynamic: B {} is not a vaild entity, create dynamic pair faild.", B);
			error(msg);
			return std::nullopt;
		}
		return create_pair_component_dynamic_StaticData_Entity(id_A, handle.value());
	}

	if (layout_A->get_component_type() == ComponentType::StaticTag && layout_B->get_component_type() == ComponentType::Entity) {
		const auto handle = _entity_vec.index_to_handle((uint32_t)id_B);
		if (!handle.has_value()) {
			MString msg = MString::format("Collection::create_pair_component_dynamic: B {} is not a vaild entity, create dynamic pair faild.", B);
			error(msg);
			return std::nullopt;
		}
		return create_pair_component_dynamic_StaticTag_Entity(id_A, handle.value());
	}

	if (layout_A->get_component_type() == ComponentType::Entity && layout_B->get_component_type() == ComponentType::Entity) {
		const auto handle_A = _entity_vec.index_to_handle((uint32_t)id_A);
		const auto handle_B = _entity_vec.index_to_handle((uint32_t)id_B);
		if (!handle_A.has_value()) {
			MString msg = MString::format("Collection::create_pair_component_dynamic: A {} is not a vaild entity, create dynamic pair faild.", A);
			error(msg);
			return std::nullopt;
		}

		if (!handle_B.has_value()) {
			MString msg = MString::format("Collection::create_pair_component_dynamic: B {} is not a vaild entity, create dynamic pair faild.", B);
			error(msg);
			return std::nullopt;
		}

		return create_pair_component_dynamic_Entity_Entity(handle_A.value(), handle_B.value());
	}

	return std::nullopt;
}

std::optional<ComponentID> Collection::create_pair_component_dynamic(uint64_t A, EntityHandle B) {

	try_register_component_from_static(A);

	if (is_component_data(A)) { return create_pair_component_dynamic_StaticData_Entity(A, B); }

	if (is_component_tag(A)) { return create_pair_component_dynamic_StaticTag_Entity(A, B); }

	if (is_component_pair(A)) {
		MString msg = MString::format("Collection::create_pair_component_dynamic: A {} is a pair component, create pair(pair, entity) is not allowed, create dynamic pair faild.", A);
		error(msg);
		return std::nullopt;
	}

	MString msg = MString::format("Collection::create_pair_component_dynamic: A {} is invaild, A can be [StaticData, StaticTag, Entity], create dynamic pair faild.", A);
	error(msg);
	return std::nullopt;
}


bool Collection::is_entity_alived(EntityHandle id) {
	return _entity_vec[id] != nullptr;
}

bool Collection::is_component(uint64_t id) const {
	return _map_ComponentID_to_ComponentLayout.contains(id);
}

bool Collection::is_component_data(uint64_t id) {
	if (!is_component(id)) return false;
	return _map_ComponentID_to_ComponentLayout[id]->get_component_type() == ComponentType::StaticData;
}

bool Collection::is_component_tag(uint64_t id) {
	if (!is_component(id)) return false;
	return _map_ComponentID_to_ComponentLayout[id]->get_component_type() == ComponentType::StaticTag;
}

bool Collection::is_component_pair(uint64_t id) {
	if (!is_component(id)) return false;
	return _map_ComponentID_to_ComponentLayout[id]->get_component_type() == ComponentType::Pair;
}

bool Collection::is_static_component(uint64_t id) const {
	return _manager->contains_component(id);
}

bool Collection::is_dynamic_component(uint64_t id) const {
	return _map_ComponentID_to_Dynamic_ComponentLayout_Store.contains(id);
}

ComponentLayout* Collection::get_component_layout(uint64_t id) {
	if (!is_component(id)) return nullptr;
	return _map_ComponentID_to_ComponentLayout[id];
}

ArchetypeNode* Collection::get_archetype_root_node() {
	return _map_ArchetypeID_to_Archetype[0].get();
}

ArchetypeNode* Collection::get_archetype_node_if_exist(ArchetypeID id) {
	if (auto it = _map_ArchetypeID_to_Archetype.find(id); it != _map_ArchetypeID_to_Archetype.end()) {
		return it->second.get();
	}
	return nullptr;
}

ArchetypeNode* Collection::get_archetype_node_if_contain_component(ArchetypeID id, ComponentID com) {
	if (const auto ptr = _map_ComponentID_to_Position.find(com); ptr != _map_ComponentID_to_Position.end()) {
		if (const auto it = ptr->second.find(id); it != ptr->second.end()) {
			return _map_ArchetypeID_to_Archetype[it->first].get();
		}
	}
	return nullptr;
}

ArchetypeNode* Collection::impl_get_or_create_archetype_node_single_edge_remove(ArchetypeNode* archtype_node, ComponentID remove_com) {
	if(archtype_node->_components.empty()) {
		return _map_ArchetypeID_to_Archetype[0].get();
	}

	if(const auto found = archtype_node->_edges.find(remove_com); found != archtype_node->_edges.end()) {
		if (found->second._remove) {
			return found->second._remove;
		}
	}

	MVector<ComponentID> remove_coms{};
	remove_coms.reserve(archtype_node->_components.size() - 1);
	for(uint32_t i = 0; i < archtype_node->_components.size(); i++){
		if(archtype_node->_components[i] != remove_com) {
			remove_coms.push_back(archtype_node->_components[i]);
		}
	}
	std::ranges::sort(remove_coms);

	ArchetypeNode* remove_archetype_node = impl_get_or_create_archetype_node(remove_coms);

	archtype_node->_edges[remove_com]._remove = remove_archetype_node;		// orgin -remove-> removed
	remove_archetype_node->_edges[remove_com]._add = archtype_node;		// removed -add-> orgin

	return remove_archetype_node;
}

//用于基于某个基础节点, 创建或者获取新的节点, 并且连通
//archtype_node和new_com都必须经过了检查, 是合法的 才能调用此函数
ArchetypeNode* Collection::impl_get_or_create_archetype_node_single_edge_add(ArchetypeNode* archtype_node, ComponentID new_com) {
	if (const auto found = archtype_node->_edges.find(new_com); found != archtype_node->_edges.end()) {
		if(found->second._add) {
			return found->second._add;
		}
	}

	MVector<ComponentID> new_coms (archtype_node->_components.size() + 1);

	std::ranges::copy(archtype_node->_components, new_coms.begin());
	new_coms.back() = new_com;
	std::ranges::sort(new_coms);

	ArchetypeNode* new_archetype_node = impl_get_or_create_archetype_node(new_coms);

	archtype_node->_edges[new_com]._add = new_archetype_node;    // old -add-> new
	new_archetype_node->_edges[new_com]._remove = archtype_node; // new -remove-> old

	return new_archetype_node;
}

//用于创建或者获取新的节点
//new_com必须经过检查
ArchetypeNode* Collection::impl_get_or_create_archetype_node(const MVector<ComponentID>& coms) {

	if (_map_ComponentList_to_ArchetypeID.contains(coms)) { //这个node是否存在, 存在直接返回
		ArchetypeID id = _map_ComponentList_to_ArchetypeID[coms];
		return _map_ArchetypeID_to_Archetype[id].get();
	}

	_Archetype_ID_counter++;

	MVector<ComponentLayout*> com_layouts{  };
	com_layouts.reserve(coms.size_u32());

	for (uint32_t i = 0, slot = 0; i < coms.size(); i++, slot++) {

		if (!_map_ComponentID_to_Position.contains(coms[i])) _map_ComponentID_to_Position[coms[i]] = {};
		_map_ComponentID_to_Position[coms[i]][_Archetype_ID_counter] = slot;

		com_layouts.push_back(get_component_layout(coms[i]));

		if (com_layouts.back()->get_component_type() == ComponentType::Pair) {
			slot++;
		}
	}

	auto next = std::make_unique<ArchetypeNode>(_Archetype_ID_counter, coms, com_layouts);
	const auto temp_ptr = next.get();

	_map_ArchetypeID_to_Archetype[_Archetype_ID_counter] = std::move(next);
	_map_ComponentList_to_ArchetypeID[coms] = _Archetype_ID_counter;

	return temp_ptr;
}