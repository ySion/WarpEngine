#pragma once

#include "spdlog/spdlog.h"

namespace Warp {

	template<typename... Args>
	constexpr void debug(Args&&... args) {
		const MString str = MString::format(args...);
		spdlog::debug(str.c_str());
	}

	template<typename... Args>
	constexpr void info(Args&&... args) {
		const MString str = MString::format(args...);
		spdlog::info(str.c_str());
	}

	template<typename... Args>
	constexpr void warn(Args&&... args) {
		const MString str = MString::format(args...);
		spdlog::warn(str.c_str());
	}

	template<typename... Args>
	constexpr void error(Args&&... args) {
		const MString str = MString::format(args...);
		spdlog::error(str.c_str());
	}

	template<typename... Args>
	constexpr void fatal(Args&&... args) {
		const MString str = MString::format(args...);
		spdlog::critical(str.c_str());
	}
};