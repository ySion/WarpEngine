#pragma once

#include "Graphics/GPUFactory.h"
#include "Graphics/GPUResourceBase.h"

#include "GPUSemaphore.h"
#include "GPURenderPass.h"
#include "GPUFrameBuffer.h"
#include "GPUFence.h"

namespace Warp {
	namespace GPU {

		class GPUCommandBuffer final : public GPUResource {
		public:
			inline GPUCommandBuffer(const MString& name) : GPUResource("commandbuffer", name) {}

			~GPUCommandBuffer() override {
				vkFreeCommandBuffers(GPUFactory::get_device(), m_command_pool, 1, &m_command_buffer);
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

			void set_viewports(const std::vector<VkViewport>& viewport, uint32_t first_viewport = 0) const {
				vkCmdSetViewport(m_command_buffer, first_viewport, (uint32_t)viewport.size(), viewport.data());
			}

			void set_scissors(const std::vector<VkRect2D>& scissors, uint32_t first_scissor = 0) const {
				vkCmdSetScissor(m_command_buffer, first_scissor, (uint32_t)scissors.size(), scissors.data());
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

	}
}