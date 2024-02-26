#include "GlslCompiler.hpp"

MString Warp::Gpu::GlslCompiler::compile_glsl_to_spirv(glslang_stage_t stage, std::string_view glsl_code, MVector<uint32_t>& spirv)
{

	glslang_initialize_process();

	const glslang_input_t input = {
		.language = GLSLANG_SOURCE_GLSL,
		.stage = stage,
		.client = GLSLANG_CLIENT_VULKAN,
		.client_version = GLSLANG_TARGET_VULKAN_1_3,
		.target_language = GLSLANG_TARGET_SPV,
		.target_language_version = GLSLANG_TARGET_SPV_1_6,
		.code = glsl_code.data(),
		.default_version = 460,
		.default_profile = GLSLANG_NO_PROFILE,
		.force_default_version_and_profile = false,
		.forward_compatible = false,
		.messages = GLSLANG_MSG_DEFAULT_BIT,
		.resource = glslang_default_resource(),
	};

	glslang_shader_t* shader = glslang_shader_create(&input);
	
	if(!glslang_shader_preprocess(shader, &input)) {
		MString string = MString::format("Failed to preprocess shader: {}.", glslang_shader_get_info_log(shader));
		error(string);
		glslang_shader_delete(shader);
		glslang_finalize_process();
		return string;
	}

	if(!glslang_shader_parse(shader, &input)) {
		MString string = MString::format("Failed to parse shader: {}.", glslang_shader_get_info_log(shader));
		error(string);
		glslang_shader_delete(shader);
		glslang_finalize_process();
		return string;
	}

	glslang_program_t* program = glslang_program_create();
	glslang_program_add_shader(program, shader);

	if(!glslang_program_link(program, GLSLANG_MSG_SPV_RULES_BIT | GLSLANG_MSG_VULKAN_RULES_BIT)) {
		MString string = MString::format("Failed to link shader: {}.", glslang_program_get_info_log(program));
		error(string);
		glslang_program_delete(program);
		glslang_shader_delete(shader);
		glslang_finalize_process();
		return string;
	}

	glslang_spv_options_t spv_options = {
			.generate_debug_info = false,
			.strip_debug_info = false ,
			.disable_optimizer = false,
			.optimize_size = true,
			.disassemble = false,
			.validate = true,
			.emit_nonsemantic_shader_debug_info = false,
			.emit_nonsemantic_shader_debug_source = false,
			.compile_only = false,
	};

	//glslang_program_SPIRV_generate_with_options(program, stage, &spv_options);
	glslang_program_SPIRV_generate(program, stage);

	spirv.resize(glslang_program_SPIRV_get_size(program));
	memcpy(spirv.data(), glslang_program_SPIRV_get_ptr(program), glslang_program_SPIRV_get_size(program) * sizeof(uint32_t));
	glslang_program_delete(program);
	glslang_shader_delete(shader);

	glslang_finalize_process();

	return {};
}
