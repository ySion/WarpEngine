#include "GPUShaderCompiler.h"

#include <stdexcept>
#include <vector>
#include <string>
#include <format>

#include "GPUHelper.h"
#include "Core/Utils.h"

namespace Warp
{
	namespace GPU
	{
		namespace ShaderCompiler
		{

			MString compile_shader_from_file(const MString& file_name, const MString& name, EShLanguage lang,MVector<uint32_t>& spriv) {

				MString codes = read_text_file(file_name);
				if(codes.empty())
				{
					throw std::runtime_error("shader file not found.");
				}
				return compile_shader_from_string(name, codes, lang, spriv);
			}

			//res message;
			MString compile_shader_from_string(const MString& name, const MString& codes, EShLanguage lang, MVector<uint32_t>& spriv) {

				const char* p = codes.c_str();
				const char* fname = name.c_str();
				glslang::TShader shader{ lang };
				shader.setStringsWithLengthsAndNames(&p, nullptr, &fname, 1);
				shader.setEntryPoint("main");
				shader.setEnvInput(glslang::EShSourceGlsl, lang, glslang::EShClientVulkan, 460);
				shader.setEnvTarget(glslang::EShTargetCount, glslang::EShTargetSpv_1_6);
				shader.setEnvClient(glslang::EShClientVulkan, glslang::EShTargetVulkan_1_3);
				
				constexpr EShMessages mesages = static_cast<EShMessages>(EShMsgDefault | EShMsgVulkanRules | EShMsgSpvRules);
				if (!shader.parse(GetDefaultResources(), 460, false, mesages)) {
					return std::format("parse stage error:\n{}\n{}", shader.getInfoLog(), shader.getInfoDebugLog()).c_str();
				}

				glslang::TProgram program{};
				program.addShader(&shader);
				if (!program.link(mesages)) {
					return std::format("link stage error:\n{}\n{}", shader.getInfoLog(), shader.getInfoDebugLog()).c_str();
				}

				glslang::TIntermediate* inter = program.getIntermediate(lang);
				if (!inter) {
					return std::format("get intermediate stage error:\n{}\n{}", shader.getInfoLog(), shader.getInfoDebugLog()).c_str();
				}
				std::vector<uint32_t> spirv_temp{};
				glslang::GlslangToSpv(*inter, spirv_temp);
				spriv = MVector<uint32_t>(spirv_temp.begin(), spirv_temp.end());
				return {};
			}
		}
	}
}
