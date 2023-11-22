#include "SDL3/SDL.h"
#include "SDL3/SDL_vulkan.h"

#include "Core/log.hpp"
#include "Render/context.hpp"
#include "Render/render_window.hpp"
#include "Render/pipeline.hpp"
#include "Render/image.hpp"
#include "Render/framebuffer.hpp"


#include "windows-helper.hpp"

#include "spirv_cross/spirv_glsl.hpp"

#include <functional>

#include "Render/command_pool.hpp"

#include "volk.h"


int main(int argc, char** argv) {

	wp::Context::prepare();

	unsigned int extensionCount = 0;
	SDL_Vulkan_GetInstanceExtensions(&extensionCount, nullptr);
	std::vector<const char*> extensionNames(extensionCount);
	SDL_Vulkan_GetInstanceExtensions(&extensionCount, extensionNames.data());

	for (auto& name : extensionNames) {
		LOG("extension : {}", name);
	}

	SDL_Window* window =
		window = SDL_CreateWindow("hello world", 800, 600, SDL_WINDOW_VULKAN | SDL_WINDOW_HIDDEN);


	wp::Context::function_create_surface_ = [&](VkInstance instance)-> VkSurfaceKHR {
		VkSurfaceKHR surface = nullptr;
		SDL_Vulkan_CreateSurface(window, instance, &surface);
		return surface;
		};

	try {
		wp::Context::init();
		SDL_DestroyWindow(window);
	} catch (const std::exception& e) {
		LOGC("exception : {}", e.what());
		return 0;
	}

	wp::RenderWindow render_window(1024, 768, "hello world");
	{
		wp::GraphicsPipeline pipeline{};

		//wp::Image image(VkExtent3D(512, 512, 1), VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT);
		//image.make_or_replace_image_view();

		wp::CommandPool cmd_pool{};
		cmd_pool.emplace_command_buffer();

		uint32_t imageIndex;
		vkAcquireNextImageKHR(wp::Context::get_instance().m_device, render_window.get_swap_chain()->m_swap_chain, UINT64_MAX, nullptr, nullptr, &imageIndex);


		wp::FrameBuffer frame_buffer(pipeline.m_render_pass, { &(render_window.get_swap_chain()->m_render_targets[imageIndex]) });



		VkCommandBufferBeginInfo begin_info{};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		VkRenderPassBeginInfo render_pass_begin_info{};
		render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		render_pass_begin_info.renderPass = pipeline.m_render_pass;
		render_pass_begin_info.framebuffer = frame_buffer.m_frame_buffer;
		render_pass_begin_info.renderArea.extent = { render_window.get_swap_chain()->m_extent.width, render_window.get_swap_chain()->m_extent.height };
		render_pass_begin_info.renderArea.offset = { 0, 0 };

		VkClearValue clearColor[1] = {};
		clearColor[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };

		render_pass_begin_info.clearValueCount = 1;
		render_pass_begin_info.pClearValues = clearColor;

		auto cmdbuf = cmd_pool.get_command_buffers()[0];
		vkBeginCommandBuffer(cmdbuf, &begin_info);

		vkCmdBindPipeline(cmdbuf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.m_pipeline);
		vkCmdBeginRenderPass(cmdbuf, &render_pass_begin_info, VkSubpassContents::VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.height = render_window.get_swap_chain()->m_extent.height;
		viewport.width = render_window.get_swap_chain()->m_extent.width;
		viewport.x = 0;
		viewport.y = 0;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor{};
		scissor.extent = { render_window.get_swap_chain()->m_extent.width, render_window.get_swap_chain()->m_extent.height };
		scissor.offset = { 0, 0 };

		vkCmdSetViewport(cmdbuf, 0, 1, &viewport);
		vkCmdSetScissor(cmdbuf, 0, 1, &scissor);

		vkCmdDraw(cmdbuf, 3, 1, 0, 0);

		vkCmdEndRenderPass(cmdbuf);

		if (vkEndCommandBuffer(cmdbuf) != VK_SUCCESS) {
			spdlog::error("结束渲染通道失败");
		}


		


		VkSubmitInfo submitInfo{};
		submitInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pCommandBuffers = &cmdbuf;
		submitInfo.commandBufferCount = 1;

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.pImageIndices = &imageIndex;

		VkSwapchainKHR swapchains[] = { render_window.get_swap_chain()->m_swap_chain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapchains;


		
		VkResult res = vkQueuePresentKHR(wp::Context::get_instance().m_present_queue, &presentInfo);


		SDL_Event event;
		bool      shouldClose = false;
		while (!shouldClose) {
			while (SDL_PollEvent(&event)) {
				if (event.type == SDL_EVENT_QUIT) {
					shouldClose = true;
				}
			}
		}
	}

	render_window.kill();

	wp::Context::quit();

	return 0;
}
