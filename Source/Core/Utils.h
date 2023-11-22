#pragma once
#include <format>
#include <vector>

#include "MIStl.h"

template<class ...ARGS>
constexpr void println(const std::string_view ctx, ARGS ...arg) {
	std::string temp = vformat(ctx, std::make_format_args(arg...));
	std::printf("%s\n", temp.c_str());
}


namespace Warp {

MString read_text_file(const MString& filename);

MVector<uint8_t> read_binary_file(const MString& filename, const uint32_t count);

}

