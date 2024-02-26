#pragma once
#include <vector>

#include "Core/Collection.hpp"
#include "glslang/Include/glslang_c_interface.h"
#include "glslang/Public/resource_limits_c.h"
#include "Core/Logger.hpp"


namespace Warp::Gpu {

	namespace GlslCompiler {

		MString compile_glsl_to_spirv(glslang_stage_t stage, std::string_view glsl_code, MVector<uint32_t>& spirv);
	}
}
