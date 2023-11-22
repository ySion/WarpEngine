#include "GPUResourceBuilder.h"

#include "GPUResourceManager.h"
#include "mimalloc.h"
namespace Warp {
	namespace GPU {

		MVector<GPUCommandBuffer*> GPUResourceBuilder<GPUCommandBuffer>::make_multi(uint32_t count,
			bool anonymous_name) {

			MVector<GPUCommandBuffer*> buffers_res{ };

			if (count == 0) {
				return buffers_res;
			}

			MVector<VkCommandBuffer> vk_command_buffers{ count };
			create_info.m_vk_command_buffer_allocate_info.commandBufferCount = count;
			if (auto res = vkAllocateCommandBuffers(GPUFactory::get_device(), &create_info.m_vk_command_buffer_allocate_info, vk_command_buffers.data());
				res != VK_SUCCESS) {
				return buffers_res;
			}

			buffers_res.resize(count);

			if (anonymous_name || create_info.m_name.empty()) {
				create_info.m_name = "";
				for (size_t i = 0; i < count; i++) {
					auto obj_ptr = std::make_unique<GPUCommandBuffer>(create_info.m_name);
					obj_ptr->m_command_buffer = vk_command_buffers[i];
					obj_ptr->m_command_pool = create_info.m_vk_command_buffer_allocate_info.commandPool;
					obj_ptr->m_level = create_info.m_vk_command_buffer_allocate_info.level;
					obj_ptr->m_submit_info = {
						.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
						.pNext = nullptr,
						.waitSemaphoreCount = 0,
						.pWaitSemaphores = nullptr,
						.pWaitDstStageMask = nullptr,
						.commandBufferCount = 1,
						.pCommandBuffers = &obj_ptr->m_command_buffer,
						.signalSemaphoreCount = 0,
						.pSignalSemaphores = nullptr
					};
					buffers_res[i] = obj_ptr.get();
					m_manager->add(std::move(obj_ptr));
				}
			} else {
				MString origin_name = create_info.m_name;

				for (size_t i = 0; i < count; i++) {
					create_info.m_name = std::format("{}_{}", origin_name, i);
					auto obj_ptr = std::make_unique<GPUCommandBuffer>(create_info.m_name);
					obj_ptr->m_command_buffer = vk_command_buffers[i];
					obj_ptr->m_command_pool = create_info.m_vk_command_buffer_allocate_info.commandPool;
					obj_ptr->m_level = create_info.m_vk_command_buffer_allocate_info.level;
					obj_ptr->m_submit_info = {
						.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
						.pNext = nullptr,
						.waitSemaphoreCount = 0,
						.pWaitSemaphores = nullptr,
						.pWaitDstStageMask = nullptr,
						.commandBufferCount = 1,
						.pCommandBuffers = &obj_ptr->m_command_buffer,
						.signalSemaphoreCount = 0,
						.pSignalSemaphores = nullptr
					};

					buffers_res[i] = obj_ptr.get();
					m_manager->add(std::move(obj_ptr));
				}
			}

			return buffers_res;
		}

		void GPUResourceBuilder<GPUCommandBuffer>::make_and_execute_single_time_command_buffer(
			const std::function<void(GPUCommandBuffer& command_buffer)>& call_back) {
			if (create_info.m_vk_command_buffer_allocate_info.commandPool == VK_NULL_HANDLE) {
				return;
			}

			VkCommandBuffer vk_command_buffer{};

			create_info.m_vk_command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			create_info.m_vk_command_buffer_allocate_info.commandBufferCount = 1;

			if (auto res = vkAllocateCommandBuffers(GPUFactory::get_device(), &create_info.m_vk_command_buffer_allocate_info, &vk_command_buffer);
				res != VK_SUCCESS) {
				return;
			}

			const std::unique_ptr<GPUCommandBuffer> command_buffer_warper_ptr = std::make_unique<GPUCommandBuffer>("@single_time_command_buffer");

			command_buffer_warper_ptr->m_command_buffer = vk_command_buffer;
			command_buffer_warper_ptr->m_command_pool = create_info.m_vk_command_buffer_allocate_info.commandPool;
			command_buffer_warper_ptr->m_level = create_info.m_vk_command_buffer_allocate_info.level;
			command_buffer_warper_ptr->begin_command_buffer(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

			call_back(*command_buffer_warper_ptr);

			command_buffer_warper_ptr->end_command_buffer();
			command_buffer_warper_ptr->submit();
		}
	}
}
