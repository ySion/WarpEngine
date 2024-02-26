#pragma once

#include "Core/Inherit.hpp"
#include "Core/observer_ptr.hpp"
#include "MaterialSource.hpp"

namespace Warp::Gpu {

	class MaterialInstance : public Inherit<MaterialInstance, Object> {
	public:
		MaterialInstance(MaterialSource* material_source, std::string_view name) {
			_material_source = material_source;
			if (!_material_source) {
				MString msg = "MaterialInstance::MaterialInstance: parent material source is invaild";
				error(msg);
				throw Exception{ msg, 0 };
			}

			make_parm_memory();

		}

		MaterialInstance(MaterialInstance* parent_material_instance, std::string_view name) {
			_material_source = parent_material_instance->_material_source;
			_parent_material_instance = parent_material_instance;
			if(!_material_source) {
				MString msg = "MaterialInstance::MaterialInstance: parent material instance's source is invaild";
				error(msg);
				throw Exception{ msg, 0 };
			}

			if (!parent_material_instance) {
				MString msg = "MaterialInstance::MaterialInstance: parent material instance is invaild";
				error(msg);
				throw Exception{msg, 1};
			}

			make_parm_memory();
		}

		void make_parm_memory() {

			auto layout_ref = _material_source->get_parameters_datapacket_layout();
			auto param_need_size = layout_ref->get_data_segment_size();
			_param_cpu_data.resize(param_need_size);
			
			auto ctx = _material_source->get_context();
			_param_gpu_on_cpu_buffer = ctx->create_buffer(param_need_size, VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VmaAllocationCreateFlagBits::VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);
		}

	private:
		observer_ptr<MaterialSource> _material_source{};
		observer_ptr<MaterialInstance> _parent_material_instance{};
		Data::DataBuffer _param_cpu_data {};
		std::unique_ptr<Buffer> _param_gpu_on_cpu_buffer{};
	};
}
