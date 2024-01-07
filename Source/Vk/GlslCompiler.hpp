#pragma once
#include <vector>

#include "Core/Collection.hpp"
#include "glslang/Include/glslang_c_interface.h"
#include "glslang/Public/resource_limits_c.h"
#include "glslang/SPIRV/SpvBuilder.h"

#include "Core/Logger.hpp"

#include "sstream"
namespace Warp::Gpu {

	namespace GlslCompiler {

		MString compile_glsl_to_spirv(glslang_stage_t stage, const char* glsl_code, MVector<uint32_t>& spirv);
	}
}
