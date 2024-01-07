#pragma once

#include "Graphics/GPUResourceManager.h"
#include "Graphics/GPUResource/GPUBuffer.h"

namespace Warp {
	namespace GPU {

		template<>
		class GPUResourceBuilder<GPUBuffer> {

		public:
			using target_type = GPUBuffer;
			using self_type = GPUResourceBuilder<target_type>;

			struct CreateInfo {
				MString name;
				VkBufferCreateInfo ci_buffer {
					.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
					.pNext = nullptr,
					.flags = 0,
					.size = 0,
					.usage = 0,
				};

				VmaAllocationCreateInfo ci_allocation {
					.flags = 0,
					.usage = VMA_MEMORY_USAGE_AUTO,
					.requiredFlags = 0,
					.preferredFlags = 0,
					.memoryTypeBits = 0,
					.pool = VK_NULL_HANDLE,
					.pUserData = nullptr,
					.priority = 0.0f,
				};
			};

			GPUResourceBuilder(GPUResourceManager<target_type>* manager, const MString& name) : m_manager(manager) {
				create_info.name = name;
			}

			self_type& set_buffer_config(VkDeviceSize size, VkBufferUsageFlags buffer_usage = 0, VkBufferCreateFlags create_flags = 0) {
				create_info.ci_buffer.size = size;
				create_info.ci_buffer.usage = buffer_usage;
				create_info.ci_buffer.flags = create_flags;
				return *this;
			}

			self_type& set_memory_config(VmaMemoryUsage usage, VmaAllocationCreateFlags flags, VkMemoryPropertyFlags required_flags = 0, VkMemoryPropertyFlags preferred_flags = 0) {
				create_info.ci_allocation.usage = usage;
				create_info.ci_allocation.flags = flags;
				create_info.ci_allocation.requiredFlags = required_flags;
				create_info.ci_allocation.preferredFlags = preferred_flags;
				return *this;
			}

			self_type& template_upload(VkDeviceSize size) {

				create_info.ci_buffer.size = size;
				create_info.ci_buffer.usage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
				create_info.ci_buffer.flags = 0;

				create_info.ci_allocation.usage = VMA_MEMORY_USAGE_AUTO;
				create_info.ci_allocation.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

				return *this;
			}

			self_type& template_readback(VkDeviceSize size) {

				create_info.ci_buffer.size = size;
				create_info.ci_buffer.usage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT;
				create_info.ci_buffer.flags = 0;

				create_info.ci_allocation.usage = VMA_MEMORY_USAGE_AUTO;
				create_info.ci_allocation.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT;

				return *this;
			}

			self_type& template_gpu_copy_src_dst(VkDeviceSize size) {

				create_info.ci_buffer.size = size;
				create_info.ci_buffer.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
				create_info.ci_buffer.flags = 0;

				create_info.ci_allocation.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
				create_info.ci_allocation.flags = 0;
				return *this;
			}

			self_type& template_on_device_storage_copy_src_dst(VkDeviceSize size) {

				create_info.ci_buffer.size = size;
				create_info.ci_buffer.usage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
				create_info.ci_buffer.flags = 0;

				create_info.ci_allocation.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
				create_info.ci_allocation.flags = 0;
				return *this;
			}

			self_type& template_on_device_vertex_copy_dst(VkDeviceSize size) {

				create_info.ci_buffer.size = size;
				create_info.ci_buffer.usage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT;
				create_info.ci_buffer.flags = 0;

				create_info.ci_allocation.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
				create_info.ci_allocation.flags = 0;
				return *this;
			}

			self_type& template_on_device_vertex_index_copy_dst(VkDeviceSize size) {

				create_info.ci_buffer.size = size;
				create_info.ci_buffer.usage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT;
				create_info.ci_buffer.flags = 0;

				create_info.ci_allocation.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
				create_info.ci_allocation.flags = 0;
				return *this;
			}

			//一般性能的uniform buffer，可以进行高频写入，但是它速度一般！它是线性写入的！请不要随机读写
			self_type& template_uniform_buffer(VkDeviceSize size) {

				create_info.ci_buffer.size = size;
				create_info.ci_buffer.usage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
				create_info.ci_buffer.flags = 0;

				create_info.ci_allocation.usage = VMA_MEMORY_USAGE_AUTO;
				create_info.ci_allocation.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
				return *this;
			}

			target_type* make(bool replace = false) {

				if (!create_info.name.empty() && m_manager->find_by_name(create_info.name) && !replace) {
					LOGE("[GPUResourceBuilder<{}>] \"{}\" is exist, change argument 'bool replace = true' to replace make.", typeid(target_type).name(), create_info.name);
					return nullptr;
				}

				VkResult res = VK_RESULT_MAX_ENUM;
				try {
					VkBuffer buffer{};
					VmaAllocation allocation{};
					if (res = vmaCreateBuffer(GPUFactory::get_vma(), &create_info.ci_buffer, &create_info.ci_allocation, &buffer, &allocation, nullptr);
						res != VK_SUCCESS || !buffer || !allocation) {
						const char* code_desc = get_vk_result_string(res);
						LOGE("[GPUResourceBuilder<{}>] Name \"{}\" create failed, return code {} {}.", typeid(target_type).name(), create_info.name, code_desc, static_cast<int32_t>(res));
						return nullptr;
					}

					const auto obj = new target_type(create_info.name);

					obj->m_buffer = buffer;
					obj->m_memory = allocation;
					obj->m_size = create_info.ci_buffer.size;
					obj->m_usage = create_info.ci_buffer.usage;

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