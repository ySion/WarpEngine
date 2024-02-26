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
				MVector<VkPushConstantRange> push_constant_ranges{};
				MVector<VkDescriptorSetLayout> set_layouts{};
			};

			GPUResourceBuilder(GPUResourceManager<target_type>* manager, const MString& name) : m_manager(manager) {
				create_info.name = name;
			}

			self_type& add_set_layout(const VkDescriptorSetLayout& range) {
				create_info.set_layouts.push_back(range);
				return *this;
			}

			self_type& add_push_constant(const VkPushConstantRange& range) {
				create_info.push_constant_ranges.push_back(range);
				return *this;
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
						.setLayoutCount = static_cast<uint32_t>(create_info.set_layouts.size()),
						.pSetLayouts = create_info.set_layouts.data(),
						.pushConstantRangeCount = static_cast<uint32_t>(create_info.push_constant_ranges.size()),
						.pPushConstantRanges = create_info.push_constant_ranges.data()
					};

					if (res = vkCreatePipelineLayout(GPUFactory::get_device(), &create_info.ci_layout, nullptr, &layout);
						res != VK_SUCCESS || !layout) {
						const char* code_desc = get_vk_result_string(res);
						LOGE("[GPUResourceBuilder<{}>] Name \"{}\" create failed, return code {} {}.", typeid(target_type).name(), create_info.name, code_desc, static_cast<int32_t>(res));
						return nullptr;
					}

					const auto obj = new target_type(create_info.name);

					obj->m_layout = layout;

					m_manager->add(obj);
					return obj;
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