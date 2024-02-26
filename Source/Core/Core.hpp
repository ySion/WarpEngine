#pragma once
//#define WARP_DEBUG
#include "Object.hpp"
#include "Inherit.hpp"
#include "Logger.hpp"
#include "Exception.hpp"
#include "Collection.hpp"
#include "Allocator.hpp"
#include "observer_ptr.hpp"
#include "SDL3/SDL.h"
#include "tbb/tbb.h"

#include <chrono>

#define WARP_MAX_COMPONENT 250000 // COMPONENT 数量

#define WARP_BENCH(A) \
		auto _WARP_BENCH_##A = std::chrono::steady_clock::now();

// * 1000.0 / SDL_GetPerformanceFrequency();

#define WARP_BENCH_INFO(_WARP_BENCH_Content, A, B)													\
	{																\
		double _WARP_BENCH_##A_##B = std::chrono::duration<double, std::milli>(_WARP_BENCH_##B - _WARP_BENCH_##A).count();	\
		info("SPeedBench \"{}\" : {:10} ms.", _WARP_BENCH_Content, _WARP_BENCH_##A_##B);							\
	}
		
		