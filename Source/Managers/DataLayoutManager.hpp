#pragma once

#include "Core/Core.hpp"
#include "Data/Data.hpp"

namespace Warp::Data {

	//DataLayout, named可以被定义成component, 而非named不行

	class DataLayoutManager : public Inherit<DataLayoutManager, Object> {
	public:
		DataLayoutManager() = default;
		~DataLayoutManager() override = default;

		DataLayoutManager(const DataLayoutManager&) = delete;
		DataLayoutManager(DataLayoutManager&&) = delete;

		DataLayoutManager& operator=(const DataLayoutManager&) = delete;
		DataLayoutManager& operator=(DataLayoutManager&&) = delete;

		DataLayout* create_layout(DataLayoutStructureDescriptor* struct_desc, std::string_view name = "") {
			if (!struct_desc) return nullptr;
			if (const auto ptr = create_layout_impl(struct_desc, name, _id_counter)) {
				_id_counter++;
				return ptr;
			}
			return nullptr;
		}

		DataLayout* create_layout_from_code(std::string_view code, std::string_view struct_name, std::string_view name = "") {
			if (const auto ptr = create_layout_from_code_impl(code, struct_name, name, _id_counter)) {
				_id_counter++;
				return ptr;
			}
			return nullptr;
		}

		DataLayout* get_layout(uint64_t id) {
			if (!_map_id_to_layouts.contains(id)) {
				MString msg = MString::format("DataLayoutManager::get_layout: layout with id {} does not exist, return null", id);
				warn(msg);
				return nullptr;
			}
			return _map_id_to_layouts[id].get();
		}

		DataLayout* get_layout(std::string_view name) {
			if (!_map_name_to_named_layouts.contains(name)) {
				MString msg = MString::format("DataLayoutManager::get_layout: layout with name {} does not exist, return null.", name);
				warn(msg);
				return nullptr;
			}
			const uint64_t id = _map_name_to_named_layouts[name];
			return _map_id_to_layouts[id].get();
		}

		std::optional<std::string_view> get_source_code(std::string_view name) {
			if (!_map_name_to_named_layouts.contains(name)) {
				MString msg = MString::format("DataLayoutManager::get_source_code: layout with name {} does not exist, return nullopt.", name);
				warn(msg);
				return std::nullopt;
			}
			const uint64_t id = _map_name_to_named_layouts[name];
			return _map_id_to_layouts[id]->object_get_value<std::string_view>("source_code");
		}

		std::optional<std::string_view> get_source_code(uint64_t id) {
			if (!_map_id_to_layouts.contains(id)) {
				MString msg = MString::format("DataLayoutManager::get_source_code: layout with id {} does not exist, return nullopt.", id);
				warn(msg);
				return std::nullopt;
			}
			return _map_id_to_layouts[id]->object_get_value<std::string_view>("source_code");
		}

		/*bool update_layout() {
			return true;
		}*/

		bool erase(uint64_t id) {
			if (!_map_id_to_layouts.contains(id)) {
				MString msg = MString::format("DataLayoutManager::erase: layout with id {} does not exist, return false.", id);
				warn(msg);
				return false;
			}
			_map_id_to_layouts.erase(id);
			return true;
		}

		bool erase(std::string_view name) {
			if (!_map_name_to_named_layouts.contains(name)) {
				MString msg = MString::format("DataLayoutManager::erase: layout with name {} does not exist, return false.", name);
				warn(msg);
				return false;
			}
			_map_name_to_named_layouts.erase(name);
			return true;
		}

		bool is_layout_exists(uint64_t id) const {
			return _map_id_to_layouts.contains(id);
		}

		bool is_layout_exists(std::string_view name) const {
			return _map_name_to_named_layouts.contains(name);
		}
	private:

		DataLayout* create_layout_impl(DataLayoutStructureDescriptor* struct_desc, std::string_view name, uint64_t id) {
			if (!name.empty()) {
				if (_map_name_to_named_layouts.contains(name)) {
					MString msg = MString::format("DataLayoutManager::create_named_layout: layout with name {} already exists.", name);
					error(msg);
					return nullptr;
				}
			}

			try {
				auto ptr = std::make_unique<DataLayout>(struct_desc, id);
				const auto ptr_temp = ptr.get();
				_map_id_to_layouts[id] = std::move(ptr);

				if (!name.empty()) {
					ptr_temp->object_set_value("name", name);
					_map_name_to_named_layouts[name] = id;
				}

				return ptr_temp;
			} catch (const Exception&) {
				MString msg = MString::format("DataLayoutManager::create_named_layout: create failed at name :{}");
				error(msg);
			}
			return nullptr;
		}

		DataLayout* create_layout_from_code_impl(std::string_view code, std::string_view struct_name, std::string_view name, uint64_t id) {
			DataLayoutStructureDescriptorPack desc_pack{};
			desc_pack.parse_and_replace_all(code);
			const auto struct_desc = desc_pack.find_struct(struct_name);
			if (!struct_desc) {
				MString msg = MString::format("DataLayoutManager::create_layout_from_code: struct with name {} does not exist.", struct_name);
				error(msg);
				return nullptr;
			}

			if (const auto ptr = create_layout(struct_desc, name)) {
				ptr->object_set_value("source_code", code);
			}
			return create_layout(struct_desc, name);
		}

	private:
		uint64_t _id_counter = 0;
		MMap<MString, uint64_t> _map_name_to_named_layouts{};
		MMap<uint64_t, std::unique_ptr<DataLayout>> _map_id_to_layouts{};

		friend class EngineContext;
	};



}