#pragma once

#include "spdlog/spdlog.h"
#include "MIStl.h"
#include <format>

#define LOGGER_FORMAT "[%^%l%$] %v"

#ifndef ROOT_PATH_SIZE
#define ROOT_PATH_SIZE 0
#endif

#define __FILENAME__ (static_cast<const char*>(__FILE__) + ROOT_PATH_SIZE)

#define LOG(...) spdlog::info(std::format(__VA_ARGS__))
#define LOGW(...) spdlog::warn(std::format(__VA_ARGS__))
#define LOGE(...) spdlog::error(std::format(__VA_ARGS__))
#define LOGD(...) spdlog::debug(std::format(__VA_ARGS__))
#define LOGC(...) spdlog::critical(std::format(__VA_ARGS__))
#define LOGEL(...) spdlog::error("[{}:{}] {}", __FILENAME__, __LINE__, fmt::format(std::format(__VA_ARGS__)))

#define RUNTIME_ERROR(...) throw std::runtime_error(std::format("{}:{} ", __FILENAME__, __LINE__) + std::format(__VA_ARGS__))

#define VK_CHECK(X) if(X != VK_SUCCESS) { __debugbreak(); }