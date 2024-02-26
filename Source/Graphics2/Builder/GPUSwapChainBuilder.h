#pragma once

#include "Graphics/GPUResourceManager.h"
#include "Graphics/GPUResource/GPUBuffer.h"
#include "Graphics/GPUResource/GPUSwapChain.h"

namespace Warp {
	namespace GPU {

		template<>
		class GPUResourceBuilder<GPUSwapChain> {

		public:
			using target_type = GPUSwapChain;
			using self_type = GPUResourceBuilder<target_type>;

			struct CreateInfo {
				MString name{};
				MString window_title{};
				SDL_WindowFlags window_flag{};
				VkSwapchainCreateInfoKHR ci_swapchain{
					.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
					.pNext = nullptr,
					.flags = 0,
					.surface = nullptr,
					.minImageCount = 3,
					.imageFormat = VK_FORMAT_B8G8R8A8_SRGB,
					.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
					.imageExtent = {512,512},
					.imageArrayLayers = 1,
					.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
					.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
					.queueFamilyIndexCount = 0,
					.pQueueFamilyIndices = nullptr,
					.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
					.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
					.presentMode = VK_PRESENT_MODE_MAILBOX_KHR,
					.clipped = true,
					.oldSwapchain = nullptr
				};
			};

			GPUResourceBuilder(GPUResourceManager<target_type>* manager, const MString& name) : m_manager(manager) {
				create_info.name = name;
			}

			self_type& set_configs(VkExtent2D extent, const MString& title, uint32_t flags = SDL_WINDOW_NOT_FOCUSABLE, VkPresentModeKHR present_mode = VK_PRESENT_MODE_MAILBOX_KHR, VkCompositeAlphaFlagBitsKHR composite_alpha_flags = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR) {
				create_info.ci_swapchain.imageExtent = extent;
				create_info.ci_swapchain.presentMode = present_mode;
				create_info.window_title = title;
				create_info.window_flag = static_cast<SDL_WindowFlags>(flags);
				return *this;
			}

			void make_resize_only(target_type* old_swapchain) const {
				if (old_swapchain == nullptr) return;

				vkDeviceWaitIdle(GPUFactory::get_device());

				//Destroy all old resources
				for (uint32_t i = 0; i < old_swapchain->m_img_count; i++) {
					vkDestroyImageView(GPUFactory::get_device(), old_swapchain->m_image_view[i], nullptr);
				}

				vkDestroySwapchainKHR(GPUFactory::get_device(), old_swapchain->m_swap_chain, nullptr);
				vkDestroySurfaceKHR(GPUFactory::get_instance(), old_swapchain->m_surface, nullptr);

				int w = 0, h = 0;
				SDL_GetWindowSize(old_swapchain->m_window, &w, &h);

				VkSurfaceKHR surface = nullptr;
				VkSwapchainKHR swapchain = nullptr;

				//Create new swapchain
				if(!SDL_Vulkan_CreateSurface(old_swapchain->m_window, GPUFactory::get_instance(), nullptr, &surface)){
					LOGE("[GPUResourceBuilder<{}>] Name \"{}\"  Failed to Recreate Swapchain surface, error: {}.", typeid(target_type).name(), old_swapchain->get_name(), SDL_GetError());
					throw std::runtime_error("Recreate SwapChain failed, application crashed.");
				}

				if(surface == VK_NULL_HANDLE) {
					LOGE("[GPUResourceBuilder<{}>] Name \"{}\"  Failed to Recreate Swapchain, Surface create function called successfully, but Surface is nullptr.", typeid(target_type).name(), old_swapchain->get_name());
					throw std::runtime_error("Recreate SwapChain failed, application crashed.");
				}

				const VkSwapchainCreateInfoKHR ci_swapchain {
					.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
					.pNext = nullptr,
					.flags = 0,
					.surface = surface,
					.minImageCount = 3,
					.imageFormat = VK_FORMAT_B8G8R8A8_SRGB,
					.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
					.imageExtent = {static_cast<uint32_t>(w),static_cast<uint32_t>(h)},
					.imageArrayLayers = 1,
					.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
					.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
					.queueFamilyIndexCount = 0,
					.pQueueFamilyIndices = nullptr,
					.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
					.compositeAlpha = old_swapchain->m_composite_alpha_flags,
					.presentMode = old_swapchain->m_present_mode,
					.clipped = true,
					.oldSwapchain = nullptr
				};

				VkResult res = VK_RESULT_MAX_ENUM;
				try {
					if (res = vkCreateSwapchainKHR(GPUFactory::get_device(), &ci_swapchain, nullptr, &swapchain);
						res != VK_SUCCESS || !swapchain) {
						const char* code_desc = get_vk_result_string(res);
						LOGC("[GPUResourceBuilder<{}>] Name \"{}\"  Recreate SwapChain failed, application crashed, error code:{} {}.", typeid(target_type).name(), old_swapchain->get_name(), code_desc, static_cast<int32_t>(res));
						throw std::runtime_error("Recreate SwapChain failed, application crashed.");
					}

					uint32_t image_count = 0;
					vkGetSwapchainImagesKHR(GPUFactory::get_device(), swapchain, &image_count, nullptr);
					MVector<VkImage> vk_images{ image_count };
					MVector<VkImageView> vk_image_views{ image_count };
					vkGetSwapchainImagesKHR(GPUFactory::get_device(), swapchain, &image_count, vk_images.data());
					for (int i = 0; i < image_count; ++i) {
						VkImageViewCreateInfo image_view_create_info{
							.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
							.pNext = nullptr,
							.flags = 0,
							.image = vk_images[i],
							.viewType = VK_IMAGE_VIEW_TYPE_2D,
							.format = VK_FORMAT_B8G8R8A8_SRGB,
							.components = { VK_COMPONENT_SWIZZLE_IDENTITY },
							.subresourceRange = {
								.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
								.baseMipLevel = 0,
								.levelCount = 1,
								.baseArrayLayer = 0,
								.layerCount = 1
							}
						};
						vkCreateImageView(GPUFactory::get_device(), &image_view_create_info, nullptr, &vk_image_views[i]);
					}

					old_swapchain->m_surface = surface;
					old_swapchain->m_swap_chain = swapchain;
					old_swapchain->m_extent = { static_cast<uint32_t>(w),static_cast<uint32_t>(h) };

					old_swapchain->m_img_count = image_count;
					old_swapchain->m_image = std::move(vk_images);
					old_swapchain->m_image_view = std::move(vk_image_views);
					
				} catch (...) {
					LOGC("[GPUResourceBuilder<{}>] Name \"{}\" Recreate SwapChain failed, application crashed, error code: {}", typeid(target_type).name(), old_swapchain->get_name(), static_cast<int32_t>(res));
					throw std::runtime_error("Recreate SwapChain failed, application crashed.");
				}

				if (old_swapchain->m_callback_on_resize) {
					old_swapchain->m_callback_on_resize(old_swapchain);
				}
			}

			target_type* make(bool replace = false) {

				if (!create_info.name.empty() && m_manager->find_by_name(create_info.name) && !replace) {
					LOGE("[GPUResourceBuilder<{}>] \"{}\" is exist, change argument 'bool replace = true' to replace make.", typeid(target_type).name(), create_info.name);
					return nullptr;
				}

				SDL_Window* window = nullptr;
				try {
					window = SDL_CreateWindow(create_info.window_title.c_str(),
						create_info.ci_swapchain.imageExtent.width,
						create_info.ci_swapchain.imageExtent.height,
						create_info.window_flag | SDL_WINDOW_VULKAN);

					if (!window) {
						LOGE("[GPUResourceBuilder<{}>] Name \"{}\" Failed to create SDL_Window, error: {}.", typeid(target_type).name(), create_info.name, SDL_GetError());
						return nullptr;
					}

					SDL_SetWindowMinimumSize(window, 128, 72);
					
				} catch (...) {
					LOGE("[GPUResourceBuilder<{}>] Name \"{}\" Failed to create SDL_Window, error: {}.", typeid(target_type).name(), create_info.name, SDL_GetError());
					return nullptr;
				}

				VkSurfaceKHR surface = nullptr;

				try {
					if (!SDL_Vulkan_CreateSurface(window, GPUFactory::get_instance(), nullptr, &surface)) {
						LOGE("[GPUResourceBuilder<{}>] Name \"{}\" Failed to create Surface, error: {}.", typeid(target_type).name(), create_info.name, SDL_GetError());
						return nullptr;
					}

					if (!surface) {
						LOGE("[GPUResourceBuilder<{}>] Name \"{}\" Failed to create Surface, create function called successfully, but Surface is nullptr.", typeid(target_type).name(), create_info.name);
						return nullptr;
					}

					create_info.ci_swapchain.surface = surface;

				} catch (...) {
					LOGE("[GPUResourceBuilder<{}>] Name \"{}\" Failed to create Surface, error: {}.", typeid(target_type).name(), create_info.name, SDL_GetError());
					return nullptr;
				}

				VkSwapchainKHR swapchain = nullptr;
				VkResult res = VK_RESULT_MAX_ENUM;
				try {

					if (res = vkCreateSwapchainKHR(GPUFactory::get_device(), &create_info.ci_swapchain, nullptr, &swapchain);
						res != VK_SUCCESS) {
						const char* code_desc = get_vk_result_string(res);
						LOGE("[GPUResourceBuilder<{}>] Name \"{}\" create failed, return code {} {}.", typeid(target_type).name(), create_info.name, code_desc, static_cast<int32_t>(res));
						return nullptr;
					}

					//Get images and create views
					uint32_t image_count = 0;
					vkGetSwapchainImagesKHR(GPUFactory::get_device(), swapchain, &image_count, nullptr);
					MVector<VkImage> vk_images{ image_count };
					MVector<VkImageView> vk_image_views{ image_count };
					vkGetSwapchainImagesKHR(GPUFactory::get_device(), swapchain, &image_count, vk_images.data());

					for (int i = 0; i < image_count; ++i) {
						VkImageViewCreateInfo image_view_create_info{
							.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
							.pNext = nullptr,
							.flags = 0,
							.image = vk_images[i],
							.viewType = VK_IMAGE_VIEW_TYPE_2D,
							.format = VK_FORMAT_B8G8R8A8_SRGB,
							.components = { VK_COMPONENT_SWIZZLE_IDENTITY },
							.subresourceRange = {
								.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
								.baseMipLevel = 0,
								.levelCount = 1,
								.baseArrayLayer = 0,
								.layerCount = 1
							}
						};
						vkCreateImageView(GPUFactory::get_device(), &image_view_create_info, nullptr, &vk_image_views[i]);
					}

					const auto obj = new target_type(create_info.name);

					obj->m_window = window;
					obj->m_surface = surface;
					obj->m_swap_chain = swapchain;
					obj->m_extent = create_info.ci_swapchain.imageExtent;

					obj->m_present_mode = create_info.ci_swapchain.presentMode;
					obj->m_composite_alpha_flags = create_info.ci_swapchain.compositeAlpha;

					obj->m_img_count = image_count;
					obj->m_image = std::move(vk_images);
					obj->m_image_view = std::move(vk_image_views);

					//Add
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