#pragma once


#include <fstream>
#include <string>
#include <variant>
#include <any>
#include <optional>
#include <expected>
#include <tuple>
#include <algorithm>

#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include "parallel_hashmap/phmap.h"

namespace Warp {

using std::variant;
using std::any;
using std::expected;
using std::optional;
using std::unexpected;
using std::tuple;
using std::pair;
using std::make_tuple;
using std::make_pair;

using std::make_shared;
using std::make_unique;
using std::shared_ptr;
using std::unique_ptr;
using std::weak_ptr;

using phmap::flat_hash_set;
using phmap::flat_hash_map;
template <class T> using vector = std::vector<T>;

}; // namespace Warp

#include "WarpError.hpp"
#include "WarpString.hpp"

namespace Warp {
using string = String::SIMDString<64>;

} // namespace Warp

template <> struct fmt::formatter<Warp::String::SIMDString<64>> : formatter<const char*> {
      template <typename FormatContext> auto format(const Warp::String::SIMDString<64>& str, FormatContext& ctx) {
            return fmt::format_to(ctx.out(), "{}", str.c_str());
      }
};