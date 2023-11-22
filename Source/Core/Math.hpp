#pragma once
#define GLM_FORCE_SWIZZLE
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <fmt/format.h>

// 让fmt可以接收gml vec的输出
template <typename Char, typename T, glm::length_t L> struct fmt::formatter<glm::vec<L, T>, Char> : fmt::formatter<T, Char> {
      template <typename FormatContext> auto format(const glm::vec<L, T>& vec, FormatContext& ctx) {
            fmt::format_to(ctx.out(), "({}", vec[0]);
            for (glm::length_t i = 1; i < L; i++) {
                  fmt::format_to(ctx.out(), ", {}", vec[i]);
            }
            return fmt::format_to(ctx.out(), ")");
      }
};
