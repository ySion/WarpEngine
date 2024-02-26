#pragma once

#include "GPUResourceBase.h"

#include "GPUFence.h"
#include "GPUSemaphore.h"

namespace Warp {

	namespace GPU {

		class GPUSwapChain final : public GPUResource {
		public:
			inline GPUSwapChain(const MString& name) : GPUResource("swap_chain", name) {}

			~GPUSwapChain() override {

				for (int i = 0; i < m_image.size(); i++) {
					vkDestroyImageView(GPUFactory::get_device(), m_image_view[i], nullptr);
				}

				vkDestroySwapchainKHR(GPUFactory::get_device(), m_swap_chain, nullptr);
				vkDestroySurfaceKHR(GPUFactory::get_instance(), m_surface, nullptr);
				SDL_DestroyWindow(m_window);
			}

			uint32_t acquire_next_image(GPUSemaphore* semaphore) {
				vkAcquireNextImageKHR(GPUFactory::get_device(), m_swap_chain, UINT64_MAX, semaphore->m_semaphore, nullptr, &current_acquired_image_index);
				return current_acquired_image_index;
			}

			uint32_t acquire_next_image(GPUSemaphore* semaphore, GPUFence* fence) {
				vkAcquireNextImageKHR(GPUFactory::get_device(), m_swap_chain, UINT64_MAX, semaphore->m_semaphore, fence->m_fence, &current_acquired_image_index);

				return current_acquired_image_index;
			}
			uint32_t acquire_next_image(GPUFence* fence) {
				vkAcquireNextImageKHR(GPUFactory::get_device(), m_swap_chain, UINT64_MAX, nullptr, fence->m_fence, &current_acquired_image_index);
				return current_acquired_image_index;
			}

			void present(const std::vector<GPUSemaphore*>& semaphores_wait_for = {}) {

				std::vector<VkSemaphore> res = semaphores_wait_for
					| std::ranges::views::transform([](const GPUSemaphore* ptr) {return ptr->m_semaphore; })
					| std::ranges::to<std::vector>();

				VkPresentInfoKHR info{
						.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
						.pNext = nullptr,
						.waitSemaphoreCount = (uint32_t)res.size(),
						.pWaitSemaphores = res.data(),
						.swapchainCount = 1,
						.pSwapchains = &m_swap_chain,
						.pImageIndices = &current_acquired_image_index
				};
				vkQueuePresentKHR(GPUFactory::get_queue(), &info);
			}

			VkSwapchainKHR m_swap_chain{};

			SDL_Window* m_window{};

			VkSurfaceKHR m_surface{};

			VkExtent2D m_extent{};

			MVector<VkImage> m_image{};
			MVector<VkImageView> m_image_view{};

			uint32_t m_img_count{};
			uint32_t current_acquired_image_index = 0;

			VkPresentModeKHR m_present_mode{};
			VkCompositeAlphaFlagBitsKHR m_composite_alpha_flags{};

			std::function<void(const GPUSwapChain*)> m_callback_on_resize{};
		};
	}
}
