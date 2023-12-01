#pragma once

#include "Graphics/GPUResourceManager.h"
#include "Graphics/GPUResource/GPUSemaphore.h"

namespace Warp {
	namespace GPU {

		template<>
		class GPUResourceBuilder<GPUSemaphore> {
		public:
			using target_type = GPUSemaphore;
			using self_type = GPUResourceBuilder<target_type>;

			struct CreateInfo {
				MString name;
				VkSemaphoreCreateInfo ci_semaphore{
					.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
					.pNext = nullptr,
					.flags = 0
				};
			};

			GPUResourceBuilder(GPUResourceManager<target_type>* manager, const MString& name) : m_manager(manager) {
				create_info.name = name;
			}

			target_type* make() {
				if (!create_info.name.empty() && m_manager->find_by_name(create_info.name)) {
					LOGE("[GPUResourceBuilder<{}>] \"{}\" is exist, this type can't be replaced make.", typeid(target_type).name(), create_info.name);
					return nullptr;
				}

				VkResult res = VK_RESULT_MAX_ENUM;
				try {
					VkSemaphore semaphore{};

					if (res = vkCreateSemaphore(GPUFactory::get_device(), &create_info.ci_semaphore, nullptr, &semaphore);
						VK_SUCCESS != res || !semaphore) {
						const char* code_desc = get_vk_result_string(res);
						LOGE("[GPUResourceBuilder<{}>] Name \"{}\" create failed, return code {} {}.", typeid(target_type).name(), create_info.name, code_desc, static_cast<int32_t>(res));
						return nullptr;
					}

					const auto obj = new target_type(create_info.name);

					obj->m_semaphore = semaphore;

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
