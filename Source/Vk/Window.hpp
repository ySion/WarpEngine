#pragma once

#include "SDL_events.h"
#include "SwapChain.hpp"

#include "Core/Inherit.hpp"
#include "Core/observer_ptr.hpp"
#include "SDL3/SDL.h"

struct SDL_Window;

WARP_TYPE_NAME_2(Gpu, Window);

namespace Warp::Gpu {

	class Context;

	class Window : public Inherit<Window, Object> {
	public:
		Window(Context* context, const MString& title, uint32_t width, uint32_t height, uint32_t flags);

		~Window() override;

		inline Context* get_context() const { return _context; }

		inline const MString& get_title() const { return _title; }

		inline const VkExtent2D& get_extent() const { return _extent; }

		inline SwapChain* get_swap_chain() const { return _swap_chain.get(); }

		inline SDL_Window* get_sdl_window() const { return _window; }

		inline uint64_t get_window_id() const { return SDL_GetWindowID(_window); }

		void set_title(const MString& title);

		void set_extent(const VkExtent2D& extent);

		void set_extent(uint32_t width, uint32_t height);

	private:

		void _create_or_recreate_swap_chain();

		MString _title;

		VkExtent2D _extent{};

		SDL_Window* _window;

		Context* _context{};

		std::unique_ptr<SwapChain> _swap_chain{};
	};
}