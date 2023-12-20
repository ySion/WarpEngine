#pragma once
#include <vector>

#include "Core/MIStl.h"
#include "glslang/Include/glslang_c_interface.h"
#include "glslang/Public/resource_limits_c.h"
#include "glslang/SPIRV/SpvBuilder.h"

#include "Core/Logger.hpp"

#include "sstream"
namespace Warp::Gpu {

	namespace GlslCompiler {

		MString compile_glsl_to_spirv(glslang_stage_t stage, const char* glsl_code,std::vector<uint8_t>& spirv);
	}
}
