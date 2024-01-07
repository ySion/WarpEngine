#pragma once
#include <string>
#include <vector>

#include <glslang/Public/ResourceLimits.h>
#include <glslang/SPIRV/GlslangToSpv.h>

#include "Core/MIStl.h"

namespace Warp
{
	namespace GPU
	{
		namespace ShaderCompiler
		{
			MString compile_shader_from_file(const MString& file_name, const MString& name, EShLanguage lang, MVector<uint32_t>& spriv);
			MString compile_shader_from_string(const MString& name, const MString& codes, EShLanguage lang, MVector<uint32_t>& spriv);
		}
	}

}
