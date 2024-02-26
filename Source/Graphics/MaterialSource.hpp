#pragma once

#include "MaterialParameter.hpp"
#include "Vk/ShaderModule.hpp"
#include "Vk/Context.hpp"
#include "Vk/GlslCompiler.hpp"


#include "Data/DataLayout.hpp"
#include "Data/DataLayoutStructureDescriptor.hpp"

WARP_TYPE_NAME_2(Gpu, MaterialSource);

namespace Warp::Gpu {

	/*
		Shader file:
		vertex, fragment(base), 默认用bindlees
				      set  binding
				    |	  |	  |
		#global_arg_begin   |  0  |   0   |
		mat4 V		    |	  |	  |
		mat4 P		    |	  |	  |
		#global_arg_end	    |	  |	  |
				    |	  |	  |
		#sampler_arg_begin  |  1  |   0   |
		samper2D s0         |	  |	  |
		#sampler_arg_end    |	  |	  |
				    |	  |	  |
		#material_arg_begin |  2  |   0	  |
		bool a       	    |	  |	  |
		int b       	    |	  |	  |
		float c       	    |	  |	  |
		double d      	    |	  |	  |
		color e       	    |	  |	  |
		vec2 f       	    |	  |	  |
		vec4 g       	    |	  |	  |
		mat4 i       	    |	  |	  |
		#material_arg_end   |	  |	  |
				    |	  |	  |
		#object_arg_begin   |  3  |   0	  |
		mat4 M		    |	  |	  |
		#object_arg_end	    |	  |	  |

		========================================
		#vert_arg_begin
		#vert_arg_end

		#frag_arg_begin
		#frag_arg_end

		#mesh_arg_begin
		#mesh_arg_end

		#task_arg_begin
		#task_arg_end

		#compute_arg_begin
		#compute_arg_end

		void main_vert();
		void main_frag();
		void main_mesh();
		void main_task();
		void main_compute();
	*/

	struct MaterialSourceCodeInfo {
		ShaderType _shader_type;
		MString _code;
	};
	//need a RenderPass

	class MaterialInstance;

	class MaterialSource : public Inherit<MaterialSource, Object> {
	public:
		MaterialSource(Context* ctx, RenderPass* renderpass, const MString& name) : _ctx(ctx) {
			if (!name.empty()) {
				object_set_value("name", name);
			}
		}

		~MaterialSource() override = default;

		inline Context* get_context() const noexcept { return _ctx; }

		//void add_parameter(const MaterialParameter& param) noexcept;

		//void remove_parameter(std::string_view name, std::string_view identifier, MaterialParamterType type) noexcept;

		/*bool parse_glsl_codes_from_shader_code(const MString& code_str) {
			return true;
		}*/

		void add_glsl_code(ShaderType type, const MString& code_str) {
			_source_codes.emplace_back(type, code_str);
		}

		void clear_code() {
			_source_codes.clear();
		}

		bool compile() {
			std::string_view name = object_is_value_exist("name") ? object_get_value<std::string_view>("name").value_or("") : "";
			MVector <std::unique_ptr<ShaderModule>> shader_modules{};

			for (auto& code : _source_codes) {

				const glslang_stage_t _type = static_cast<glslang_stage_t>(code._shader_type);

				MVector<uint32_t> spirv{};
				const MString error_msg = GlslCompiler::compile_glsl_to_spirv(_type, code._code, spirv);

				if (!error_msg.empty()) {
					error("MaterialSource Shader {},  shader type:{}, compile failed:\n{}", name, shader_type_to_string(code._shader_type), error_msg);
					return false;
				}

				auto shader_mod = _ctx->create_shader_module(code._shader_type, spirv);
				if (!shader_mod) { return false; }

				shader_modules.emplace_back(std::move(shader_mod));
			}

			_shaders = std::move(shader_modules);

			GraphicsPipelineState state{};
			state.set_input_assembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
			state.set_rasterization(VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE);
			state.set_depth_stencil(VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS_OR_EQUAL);

			for (auto& shader : _shaders) {
				state.add_shader_stage(static_cast<VkShaderStageFlagBits>(shader->type()), *shader);
			}

			
			const auto desc_layout_global_buffer = _ctx->create_descriptor_set_layout();
			desc_layout_global_buffer->add_binding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1).compile();

			const auto desc_layout_material_buffer = _ctx->create_descriptor_set_layout();
			desc_layout_material_buffer->add_binding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2048).compile();

			auto layout = _ctx->create_pipeline_layout();
			layout->add_set_layout(desc_layout_global_buffer.get());
			layout->add_set_layout(desc_layout_material_buffer.get());
			VkResult res =  layout->compile();

			if(res != VK_SUCCESS) {
				error("MaterialSource Shader {}, compile failed:\n{}", name);
				return false;
			}

			_layout_descriptor->compile();
			//_data_packet_layout = std::make_unique<Data::DataLayout>(_layout_descriptor.get());
			//_data_packet_layout->compile();
			return true;
		}

		inline Data::DataLayout* get_parameters_datapacket_layout() const {
			return _data_packet_layout.get();
		}

		bool add_parameters_info(MaterialParameterType type, std::string_view var_name, std::string_view var_desc = "") const {
			if (type == MaterialParameterType::Invaild) return false;
			if (!_layout_descriptor) return false;
			uint32_t type_advance_desc_data = 0;

			switch (type) {
			case MaterialParameterType::Color:
				type_advance_desc_data = (uint32_t)Data::DataPacketMemberAdvanceCustomData::Color;
				break;
			default: break;
			}

			_layout_descriptor->add_normal_member(static_cast<Data::DataPacketMemberType>(type), var_name, var_desc, {}, 1, type_advance_desc_data);
			return true;
		}

		void reset_parameters_info() {
			_layout_descriptor = std::make_unique<Data::DataLayoutStructureDescriptor>("", "material_source_parameter");
		}

	private:

		void instance_shader_code_template_pass_argument_generate(const MString& code_str) {
			/*if (code_str.contains("#vert_arg_begin")) {
				if (code_str.contains("#vert_arg_end")) {
					auto res = code_str.pick_string_segment("#vert_arg_begin", "#vert_arg_end");
					if (res) {
						auto& [str, segmenet] = res.value();
						auto lines = segmenet.split("\n");
						for (auto& line : lines) {
							line.trim();
							if (line.start_with("mat4")) {
								auto name = line.split(" ")[1];
								_param_name_to_index.emplace(name, _param_name_to_index.size());
							}
						}
					}
				} else {
					MString error_str = "vert_arg_begin not match vert_arg_end";
				}
			}*/
		}

		void instance_shader_code_template(const MString& code_str) {

			if (code_str.contains("main_vert")) {
				_source_codes.emplace_back(ShaderType::Vertex, code_str.replace_str("main_vert", "main"));
			} else if (code_str.contains("main_mesh")) {
				_source_codes.emplace_back(ShaderType::Mesh, code_str.replace_str("main_mesh", "main"));
			}

			if (code_str.contains("main_frag")) {
				_source_codes.emplace_back(ShaderType::Fragment, code_str.replace_str("main_mesh", "main"));
			}
		}

		MMap<MString, uint8_t> _param_identifer_to_index{};
		MMap<MString, uint8_t> _param_name_to_index{};

		MVector<MaterialSourceCodeInfo> _source_codes{};
		MVector<std::unique_ptr<ShaderModule>> _shaders{};

		std::unique_ptr<Data::DataLayoutStructureDescriptor> _layout_descriptor;
		std::unique_ptr<Data::DataLayout> _data_packet_layout{};

		std::unique_ptr<GraphicsPipeline> _pipeline{};
		std::unique_ptr<PipelineLayout> _layout{};

		Context* _ctx{};
		RenderPass* _renderpass{};
	};
}