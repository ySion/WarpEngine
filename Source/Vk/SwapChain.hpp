#pragma once
#include "VkBase.hpp"

#include "Core/Inherit.hpp"

WARP_TYPE_NAME_2(Gpu, SwapChain)

namespace Warp::Gpu{

	class Window;
	class Device;
	class Instance;

	class SwapChain : public Inherit<SwapChain, Object> {
	public:
		SwapChain(Window* window);

		~SwapChain() override;

	private:
		VkSwapchainKHR _swap_chain{};
		VkSurfaceKHR _surface{};
		Window* _window{};
		uint32_t _image_count{};
	};
}