#pragma once

#include "Core/Inherit.hpp"
#include "Core/Visitor.hpp"
#include "SDL3/SDL.h"

WARP_TYPE_NAME_2(Event, WindowEvent)

namespace Warp::Gpu { class Window; };

namespace Warp::Event
{
	class WindowInputBloackBoard : public Inherit<WindowInputBloackBoard, Object> {
	public:
		WindowInputBloackBoard();

		

		
	private:

		SDL_Event event{};
	}; 
}