#include "SwapChain.hpp"

#include "Context.hpp"
#include "Device.hpp"
#include "Instance.hpp"
#include "Window.hpp"

#include "Core/Logger.hpp"
#include "Core/Exception.hpp"

#include "SDL3/SDL.h"
#include "SDL3/SDL_vulkan.h"
using namespace Warp::Gpu;

SwapChain::SwapChain(Window* window) : _window(window)
{
	auto win = window->get_sdl_window();
	auto ctx = window->get_context();
	if(!SDL_Vulkan_CreateSurface(win, *ctx->get_instance(), nullptr, &_surface)) {
		MString err_msg = MString::format("Failed to create surface: {}.", SDL_GetError());
		error(err_msg);
		throw Exception(err_msg, 0);
	}

	const auto extent = window->get_extent();

	const VkSwapchainCreateInfoKHR sp_ci{
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.pNext = nullptr,
		.flags = 0,
		.surface = _surface,
		.minImageCount = 3,
		.imageFormat = VK_FORMAT_R8G8B8A8_SRGB,
		.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
		.imageExtent = extent,
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

	if(VkResult res = vkCreateSwapchainKHR(*ctx->get_device(), &sp_ci, nullptr, &_swap_chain); res != VK_SUCCESS) {
		MString err_msg = MString::format("Failed to create swap chain: {}, {}.", (int)res, msg_map_VkResult(res));
		error(err_msg);
		throw Exception(err_msg, 0);
	}
}

SwapChain::~SwapChain() {
	vkDestroySwapchainKHR(*_window->get_context()->get_device(), _swap_chain, nullptr);
	vkDestroySurfaceKHR(*_window->get_context()->get_instance(), _surface, nullptr);
}
