#pragma once

#include "Graphics/GPUResourceManager.h"
#include "Graphics/GPUResource/GPUCommandPool.h"

namespace Warp {

	namespace GPU {

		template<>
		class GPUResourceBuilder<GPUCommandPool> {

		public:
			using target_type = GPUCommandPool;
			using self_type = GPUResourceBuilder<target_type>;

			struct CreateInfo {
				MString name;
				VkCommandPoolCreateInfo ci_command_pool{
					.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
					.pNext = nullptr,
					.flags = 0,
					.queueFamilyIndex = 0,
				};
			};

			GPUResourceBuilder(GPUResourceManager<target_type>* manager, const MString& name) : m_manager(manager) {
				create_info.name = name;
			}

			self_type& set_configs(VkCommandPoolCreateFlags flags) {
				create_info.ci_command_pool.flags = flags;
				return *this;
			}

			target_type* make(bool replace = false) {

				if (!create_info.name.empty() && m_manager->find_by_name(create_info.name) && !replace) {
					LOGE("[GPUResourceBuilder<{}>] \"{}\" is exist, change argument 'bool replace = true' to replace make.", typeid(target_type).name(), create_info.name);
					return nullptr;
				}

				VkResult res = VK_RESULT_MAX_ENUM;
				try {
					VkCommandPool cmdpool{};
					if (res = vkCreateCommandPool(GPUFactory::get_device(), &create_info.ci_command_pool, nullptr, &cmdpool);
						res != VK_SUCCESS || !cmdpool) {
						const char* code_desc = get_vk_result_string(res);
						LOGE("[GPUResourceBuilder<{}>] Name \"{}\" create failed, return code {} {}.", typeid(target_type).name(), create_info.name, code_desc, static_cast<int32_t>(res));
						return nullptr;
					}

					const auto obj = new target_type(create_info.name);
					obj->m_command_pool = cmdpool;
					obj->m_flag = create_info.ci_command_pool.flags;
					obj->m_queue_family_index = create_info.ci_command_pool.queueFamilyIndex;

					m_manager->add(obj);
					return obj;

				} catch (...) {
					const char* code_desc = get_vk_result_string(res);
					LOGE("[GPUResourceBuilder<{}>] Name \"{}\" create failed, return code {} {}.", typeid(target_type).name(), create_info.name, code_desc, static_cast<int32_t>(res));
					return nullptr;
				}
			}

		private:
			CreateInfo create_info;
			GPUResourceManager<target_type>* m_manager;
		};
	}
}
