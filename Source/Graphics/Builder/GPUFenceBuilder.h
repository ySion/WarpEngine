#pragma once

#include "Graphics/GPUResourceManager.h"
#include "Graphics/GPUResource/GPUFence.h"


namespace Warp {

	namespace GPU {
		template<>
		class GPUResourceBuilder<GPUFence> {
		public:
			using target_type = GPUFence;
			using self_type = GPUResourceBuilder<target_type>;

			struct CreateInfo {
				MString name;
				VkFenceCreateInfo ci_fence{
					.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
					.pNext = nullptr,
					.flags = VK_FENCE_CREATE_SIGNALED_BIT
				};
			};

			GPUResourceBuilder(GPUResourceManager<target_type>* manager, const MString& name) : m_manager(manager) {
				create_info.name = name;
			}

			self_type& set_signaled_on_create(bool signaled_on_create = true) {
				if (signaled_on_create) {
					create_info.ci_fence.flags = VK_FENCE_CREATE_SIGNALED_BIT;
				} else {
					create_info.ci_fence.flags = 0;
				}
				return *this;
			}

			target_type* make() {

				if (!create_info.name.empty() && m_manager->find_by_name(create_info.name)) {
					LOGE("[GPUResourceBuilder<{}>] \"{}\" is exist, this type can't be replaced make.", typeid(target_type).name(), create_info.name);
					return nullptr;
				}

				VkResult res = VK_RESULT_MAX_ENUM;
				try {
					VkFence fence{};
					if (res = vkCreateFence(GPUFactory::get_device(), &create_info.ci_fence, nullptr, &fence);
						res != VK_SUCCESS || !fence) {
						const char* code_desc = get_vk_result_string(res);
						LOGE("[GPUResourceBuilder<{}>] Name \"{}\" create failed, return code {} {}.", typeid(target_type).name(), create_info.name, code_desc, static_cast<int32_t>(res));
						return nullptr;
					}

					const auto obj = new target_type(create_info.name);

					obj->m_fence = fence;

					m_manager->add(obj);
					return obj;
				} catch (...) {
					const char* code_desc = get_vk_result_string(res);
					LOGE("[GPUResourceBuilder<{}>] Name \"{}\" create failed, return code {} {}.", typeid(target_type).name(), create_info.name, code_desc, static_cast<int32_t>(res));
					return nullptr;
				}
			}

			MVector<target_type*> make_multi(uint32_t count) {
				MVector<target_type*> result;
				MString name = create_info.name;
				for (uint32_t i = 0; i < count; ++i) {
					if (!create_info.name.empty()) {
						create_info.name = to_MString(std::format("{}_{}", name, i));
					}
					if (auto obj = make()) {
						result.push_back(obj);
					}
				}
				return result;
			}

		private:
			CreateInfo create_info{};
			GPUResourceManager<target_type>* m_manager{};
		};

	}
}