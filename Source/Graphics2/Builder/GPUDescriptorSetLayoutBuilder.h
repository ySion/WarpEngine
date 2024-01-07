#pragma once

#include "Graphics/GPUResourceManager.h"
#include "Graphics/GPUResource/GPUDescriptorSetLayout.h"

namespace Warp {
	namespace GPU {
		template<>
		class GPUResourceBuilder<GPUDescriptorSetLayout> {
		public:
			using target_type = GPUDescriptorSetLayout;
			using self_type = GPUResourceBuilder<target_type>;

			struct CreateInfo {
				MString name;
				VkDescriptorSetLayoutCreateInfo ci_layout{
					.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
					.pNext = nullptr,
					.flags = 0,
					.bindingCount = 0,
					.pBindings = nullptr
				};

				MVector<VkDescriptorSetLayoutBinding> bindings{};
				MVector<VkSampler> immutable_samplers{};
			};

			GPUResourceBuilder(GPUResourceManager<target_type>* manager, const MString& name) : m_manager(manager) {
				create_info.name = name;
			}

			self_type& add_descriptor(VkDescriptorType type, VkShaderStageFlags shader_stages, VkSampler immutable_sampler = nullptr) {
				VkDescriptorSetLayoutBinding bind{
					.binding = static_cast<uint32_t>(create_info.bindings.size()),
					.descriptorType = type,
					.descriptorCount = 1,
					.stageFlags = shader_stages,
					.pImmutableSamplers = nullptr
				};

				if(immutable_sampler != nullptr) {
					create_info.immutable_samplers.push_back(immutable_sampler);
					const VkSampler* sampler = &(create_info.immutable_samplers.back());
					bind.pImmutableSamplers = sampler;
				}

				create_info.bindings.push_back(bind);
				return *this;
			}

			GPUDescriptorSetLayout* make(bool replace = false) {

				if (!create_info.name.empty() && m_manager->find_by_name(create_info.name) && !replace) {
					LOGE("[GPUResourceBuilder<{}>] \"{}\" is exist, change argument 'bool replace = true' to replace make.", typeid(target_type).name(), create_info.name);
					return nullptr;
				}

				create_info.ci_layout.bindingCount = static_cast<uint32_t>(create_info.bindings.size());
				create_info.ci_layout.pBindings = create_info.bindings.data();

				VkResult res = VK_RESULT_MAX_ENUM;
				try {
					VkDescriptorSetLayout layout{};
					if (res = vkCreateDescriptorSetLayout(GPUFactory::get_device(), &create_info.ci_layout,nullptr, &layout);
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