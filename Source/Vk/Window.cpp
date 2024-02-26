#include "Window.hpp"
#include "Context.hpp"

using namespace Warp;

Gpu::Window::Window(Context* context, const MString& title, uint32_t width, uint32_t height, uint32_t flags)
	: _title(title),  _context(context)
{
	_extent = {.width = width, .height = height};
	_window = SDL_CreateWindow(title.c_str(), width, height, flags | SDL_WINDOW_VULKAN);

	_create_or_recreate_swap_chain();
}

Gpu::Window::~Window()
{
	_swap_chain.reset();
	SDL_DestroyWindow(_window);
}

void Gpu::Window::set_title(const MString& title)
{
	SDL_SetWindowTitle(_window, title.c_str());
	_title = title;
}

void Gpu::Window::set_extent(const VkExtent2D& extent)
{
	_extent = extent;
	_create_or_recreate_swap_chain();
}

void Gpu::Window::set_extent(uint32_t width, uint32_t height)
{
	set_extent({ width, height });
}

void Gpu::Window::_create_or_recreate_swap_chain() {
	_swap_chain.reset();
	_swap_chain.reset(new SwapChain(this));
}
