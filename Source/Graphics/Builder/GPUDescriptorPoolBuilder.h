#pragma once

#include "Graphics/GPUResourceManager.h"
#include "Graphics/GPUResource/GPUDescriptorPool.h"


namespace Warp {
	namespace GPU {
		template<>
		class GPUResourceBuilder<GPUDescriptorPool> {
		public:
			using target_type = GPUDescriptorPool;
			using self_type = GPUResourceBuilder<target_type>;

			struct CreateInfo {
				MString name;
				VkDescriptorPoolCreateInfo ci_pool{
					.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
					.pNext = nullptr,
					.flags = 0,
					.maxSets = 1,
					.poolSizeCount = 1,
					.pPoolSizes = nullptr
				};

				MVector<VkDescriptorPoolSize> pool_sizes{};
			};

			GPUResourceBuilder(GPUResourceManager<target_type>* manager, const MString& name) : m_manager(manager) {
				create_info.name = name;
			}

			self_type& set_configs(uint32_t max_sets, const VkDescriptorPoolCreateFlags& flags = 0) {
				create_info.ci_pool.maxSets = max_sets;
				create_info.ci_pool.flags = flags;
				return *this;
			}

			self_type& add_pool_size_info(const VkDescriptorType& type, uint32_t count) {
				const VkDescriptorPoolSize pool_size{
					.type = type,
					.descriptorCount = count
				};
				create_info.pool_sizes.push_back(pool_size);
				return *this;
			}

			target_type* make() {
				if (!create_info.name.empty() && m_manager->find_by_name(create_info.name)) {
					LOGE("[GPUResourceBuilder<{}>] \"{}\" is exist, this type can't be replaced make.", typeid(target_type).name(), create_info.name);
					return nullptr;
				}

				create_info.ci_pool.poolSizeCount = create_info.pool_sizes.size();
				create_info.ci_pool.pPoolSizes = create_info.pool_sizes.data();

				VkResult res = VK_RESULT_MAX_ENUM;
				try {
					VkDescriptorPool pool{};
					if (res = vkCreateDescriptorPool(GPUFactory::get_device(), &create_info.ci_pool, nullptr, &pool);
						res != VK_SUCCESS || !pool) {
						const char* code_desc = get_vk_result_string(res);
						LOGE("[GPUResourceBuilder<{}>] Name \"{}\" create failed, return code {} {}.", typeid(target_type).name(), create_info.name, code_desc, static_cast<int32_t>(res));
						return nullptr;
					}

					const auto obj = new target_type(create_info.name);

					obj->m_pool = pool;
					obj->m_max_sets = create_info.ci_pool.maxSets;
					obj->m_flags = create_info.ci_pool.flags;
					obj->m_pool_sizes = create_info.pool_sizes;

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