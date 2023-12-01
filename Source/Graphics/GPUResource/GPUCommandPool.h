#pragma once

#include "GPUResourceBase.h"
#include "GPUSemaphore.h"
#include "GPUFrameBuffer.h"
#include "GPUFence.h"
#include "GPURenderPass.h"

namespace Warp {

	namespace GPU {

		class GPUCommandPool;

		class GPUCommandBuffer {
		public:
			inline GPUCommandBuffer(VkCommandPool pool_ptr, VkCommandBuffer buffer, VkCommandBufferLevel level)
				: m_command_pool(pool_ptr), m_command_buffer(buffer), m_level(level) {}

			inline ~GPUCommandBuffer() { vkFreeCommandBuffers(GPUFactory::get_device(), m_command_pool, 1, &m_command_buffer); }

			GPUCommandBuffer(const GPUCommandBuffer&) = delete;

			GPUCommandBuffer(GPUCommandBuffer&&) = delete;

			GPUCommandBuffer& operator=(const GPUCommandBuffer&) = delete;

			GPUCommandBuffer& operator=(GPUCommandBuffer&&) = delete;

			void reset_command_buffer() const {
				vkResetCommandBuffer(m_command_buffer, 0);
			}

			VkResult begin_command_buffer(VkCommandBufferUsageFlags flags = 0) {
				const VkCommandBufferBeginInfo command_begin_info = {
					.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
					.pNext = nullptr,
					.flags = flags,
					.pInheritanceInfo = nullptr
				};
				return vkBeginCommandBuffer(m_command_buffer, &command_begin_info);
			}

			void end_command_buffer() const {
				vkEndCommandBuffer(m_command_buffer);
			}

			//要等待的信号量
			void submit_config_seamphores_wait_for(const std::vector<VkSemaphore>& seamphores, const std::vector<VkPipelineStageFlags>& stage_mask) {
				m_semaphore_wait_for = seamphores;
				m_submit_stage_mask = stage_mask;
				m_submit_info.waitSemaphoreCount = m_semaphore_wait_for.size();
				m_submit_info.pWaitSemaphores = m_semaphore_wait_for.data();
				m_submit_info.pWaitDstStageMask = m_submit_stage_mask.data();
			}

			//要等待的信号量
			void submit_config_seamphores_wait_for(const std::vector<GPUSemaphore*>& seamphores, const std::vector<VkPipelineStageFlags>& stage_mask) {
				m_semaphore_wait_for =
					seamphores | std::ranges::views::transform([](const GPUSemaphore* ptr) {return ptr->m_semaphore; }) | std::ranges::to<std::vector>();
				m_submit_stage_mask = stage_mask;
				m_submit_info.waitSemaphoreCount = m_semaphore_wait_for.size();
				m_submit_info.pWaitSemaphores = m_semaphore_wait_for.data();
				m_submit_info.pWaitDstStageMask = m_submit_stage_mask.data();
			}

			//完成后发送的信号量
			void submit_config_seamphores_signal_to(const std::vector<VkSemaphore>& seamphores) {
				m_semaphore_signal_to = seamphores;
				m_submit_info.signalSemaphoreCount = m_semaphore_signal_to.size();
				m_submit_info.pSignalSemaphores = m_semaphore_signal_to.data();
			}

			//完成后发送的信号量
			void submit_config_seamphores_signal_to(const std::vector<GPUSemaphore*>& seamphores) {
				m_semaphore_signal_to =
					seamphores | std::ranges::views::transform([](const GPUSemaphore* ptr) {return ptr->m_semaphore; }) | std::ranges::to<std::vector>();
				m_submit_info.signalSemaphoreCount = m_semaphore_signal_to.size();
				m_submit_info.pSignalSemaphores = m_semaphore_signal_to.data();
			}

			void submit(bool wait_idle = true) const {
				vkQueueSubmit(GPUFactory::get_queue(), 1, &m_submit_info, nullptr);
				if (wait_idle) {
					vkDeviceWaitIdle(GPUFactory::get_device());
				}
			}

			//执行完命令后触发一个信号
			void submit(VkFence fence_singal) const {
				vkQueueSubmit(GPUFactory::get_queue(), 1, &m_submit_info, fence_singal);
			}

			//执行完命令后触发一个信号
			void submit(GPUFence* fence_singal) const {
				vkQueueSubmit(GPUFactory::get_queue(), 1, &m_submit_info, fence_singal->m_fence);
			}

			void begin_render_pass(const GPURenderPass* render_pass, const GPUFrameBuffer* frame_buffer, const VkRect2D& render_area, std::span<VkClearValue> clear_values, VkSubpassContents subpass_contents = VK_SUBPASS_CONTENTS_INLINE) {

				VkRenderPassBeginInfo info{
					.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
					.pNext = nullptr,
					.renderPass = render_pass->m_render_pass,
					.framebuffer = frame_buffer->m_framebuffer,
					.renderArea = render_area,
					.clearValueCount = (uint32_t)clear_values.size(),
					.pClearValues = clear_values.data()
				};

				vkCmdBeginRenderPass(m_command_buffer, &info, subpass_contents);
			}

			void end_render_pass() const {
				vkCmdEndRenderPass(m_command_buffer);
			}

			void bind_graphics_pipeline(const GPUGraphicsPipeline* pipeline) const {
				vkCmdBindPipeline(m_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->m_pipeline);
			}

			void set_viewports(const std::span<VkViewport> viewport, uint32_t first_viewport = 0) const {
				vkCmdSetViewport(m_command_buffer, first_viewport, viewport.size(), viewport.data());
			}

			void set_scissors(const std::span<VkRect2D> scissors, uint32_t first_scissor = 0) const {
				vkCmdSetScissor(m_command_buffer, first_scissor, scissors.size(), scissors.data());
			}

			void draw(uint32_t vertex_count, uint32_t instance_count = 1, uint32_t first_vertex = 0, uint32_t first_instance = 0) const {
				vkCmdDraw(m_command_buffer, vertex_count, instance_count, first_vertex, first_instance);
			}

			void draw_indexed(uint32_t index_count, uint32_t instance_count = 1, uint32_t first_index = 0, uint32_t vertex_offset = 0, uint32_t first_instance = 0) const {
				vkCmdDrawIndexed(m_command_buffer, index_count, instance_count, first_index, vertex_offset, first_instance);
			}

			VkCommandPool m_command_pool{};
			VkCommandBuffer m_command_buffer{};
			VkCommandBufferLevel m_level{};
			VkSubmitInfo m_submit_info{};

		private:
			std::vector<VkSemaphore> m_semaphore_wait_for{};
			std::vector<VkSemaphore> m_semaphore_signal_to{};
			std::vector<VkPipelineStageFlags> m_submit_stage_mask{};
		};

		class GPUCommandPool final : public GPUResource {
		public:
			inline GPUCommandPool(const MString& name) : GPUResource("command_pool", name) {}

			inline ~GPUCommandPool() override {
				m_command_buffers.clear();
				vkDestroyCommandPool(GPUFactory::get_device(), m_command_pool, nullptr);
			}

			GPUCommandBuffer* make_command_buffer(VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
				const VkCommandBufferAllocateInfo allocate_info{
					.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
					.pNext = nullptr,
					.commandPool = m_command_pool,
					.level = level,
					.commandBufferCount = 1
				};

				VkResult res = VK_RESULT_MAX_ENUM;
				try {
					VkCommandBuffer cmdbuffer{};
					if (res = vkAllocateCommandBuffers(GPUFactory::get_device(), &allocate_info, &cmdbuffer);
						res != VK_SUCCESS || !cmdbuffer) {
						const char* code_desc = get_vk_result_string(res);
						LOGE("[GPUCommandPool] \"{}\" create a command buffer failed, return code {} {}.", get_name(), code_desc, static_cast<int32_t>(res));
						return nullptr;
					}

					auto obj_ptr = std::make_unique<GPUCommandBuffer>(m_command_pool, cmdbuffer, level);

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

					const auto temp_ptr = obj_ptr.get();
					m_command_buffers.emplace_back(std::move(obj_ptr));
					return temp_ptr;
				} catch (...) {
					const char* code_desc = get_vk_result_string(res);
					LOGE("[GPUCommandPool] \"{}\" create a command buffer failed, return code {} {}.", get_name(), code_desc, static_cast<int32_t>(res));
					return nullptr;
				}
			}

			void make_and_execute_single_time_command_buffer(const std::function<void(GPUCommandBuffer& command_buffer)>& call_back) {

				if (!call_back) { return; }

				VkResult res = VK_RESULT_MAX_ENUM;

				try {
					const VkCommandBufferAllocateInfo allocate_info{
						.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
						.pNext = nullptr,
						.commandPool = m_command_pool,
						.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
						.commandBufferCount = 1
					};

					VkCommandBuffer cmdbuffer{};

					if (res = vkAllocateCommandBuffers(GPUFactory::get_device(), &allocate_info, &cmdbuffer);
							res != VK_SUCCESS || !cmdbuffer) {
						const char* code_desc = get_vk_result_string(res);
						LOGE("[GPUCommandPool] \"{}\" create a single command buffer failed, return code {} {}.", get_name(), code_desc, static_cast<int32_t>(res));
						return;
					}

					const std::unique_ptr<GPUCommandBuffer> command_buffer_warper_ptr =
						std::make_unique<GPUCommandBuffer>(m_command_pool, cmdbuffer, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

					command_buffer_warper_ptr->begin_command_buffer(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

					call_back(*command_buffer_warper_ptr);

					command_buffer_warper_ptr->end_command_buffer();
					command_buffer_warper_ptr->submit();
				} catch (...) {
					const char* code_desc = get_vk_result_string(res);
					LOGE("[GPUCommandPool] \"{}\" create a single command buffer failed, return code {} {}.", get_name(), code_desc, static_cast<int32_t>(res));
					return;
				}
			}

			VkCommandPool m_command_pool{};

			uint32_t m_queue_family_index{};

			VkCommandPoolCreateFlags m_flag{};

			MVector<std::unique_ptr<GPUCommandBuffer>> m_command_buffers{};
		};
	}
}
