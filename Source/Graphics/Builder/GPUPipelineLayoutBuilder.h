#pragma once

#include "Graphics/GPUResourceManager.h"
#include "Graphics/GPUResource/GPUPipelineLayout.h"

namespace Warp {
	namespace GPU {
		template<>
		class GPUResourceBuilder<GPUPipelineLayout> {
		public:
			using target_type = GPUPipelineLayout;
			using self_type = GPUResourceBuilder<target_type>;

			struct CreateInfo {
				MString name;
				VkPipelineLayoutCreateInfo ci_layout{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO
				};
			};

			GPUResourceBuilder(GPUResourceManager<target_type>* manager, const MString& name) : m_manager(manager) {
				create_info.name = name;
			}


			target_type* make(bool replace = false) {
				if (!create_info.name.empty() && m_manager->find_by_name(create_info.name) && !replace) {
					LOGE("[GPUResourceBuilder<{}>] \"{}\" is exist, change argument 'bool replace = true' to replace make.", typeid(target_type).name(), create_info.name);
					return nullptr;
				}

				VkResult res = VK_RESULT_MAX_ENUM;
				try {
					VkPipelineLayout layout{};

					create_info.ci_layout = {
						.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
						.pNext = nullptr,
						.flags = 0,
						.setLayoutCount = 0,
						.pSetLayouts = nullptr,
						.pushConstantRangeCount = 0,
						.pPushConstantRanges = nullptr
					};

					if (res = vkCreatePipelineLayout(GPUFactory::get_device(), &create_info.ci_layout, nullptr, &layout);
						res != VK_SUCCESS) {
						const char* code_desc = get_vk_result_string(res);
						LOGE("[GPUResourceBuilder<{}>] Name \"{}\" create failed, return code {} {}.", typeid(target_type).name(), create_info.name, code_desc, static_cast<int32_t>(res));
						return nullptr;
					}

					std::unique_ptr<GPUPipelineLayout> obj_ptr = std::make_unique<GPUPipelineLayout>(create_info.name);

					obj_ptr->m_layout = layout;

					const auto temp_ptr = obj_ptr.get();
					m_manager->add(std::move(obj_ptr));
					return temp_ptr;
				} catch (...) {
					const char* code_desc = get_vk_result_string(res);
					LOGE("[GPUResourceBuilder<{}>] Name \"{}\" create failed, return code {} {}.", typeid(target_type).name(), create_info.name, code_desc, static_cast<int32_t>(res));
					return nullptr;
				}
			}

		private:
			CreateInfo create_info{};
			GPUResourceManager<target_type>* m_manager{};
		};

	}
}