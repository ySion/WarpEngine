#pragma once

#include "MaterialParameter.hpp"

#include "Vk/Context.hpp"

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


	enum class ShaderType {
		None,
		Vertex,
		Fragment,
		Mesh,
		Task,
		Compute,
	};

	class MaterialSource : public Inherit<MaterialSource, Object> {
	public:
		MaterialSource(Context* ctx) : _ctx(ctx) {}

		~MaterialSource() override = default;

		bool compile();

		inline Context* get_context() const noexcept { return _ctx; }

		//inline ShaderModule* get_shader() const noexcept { return _shader.get(); }

		void add_parameter(const MaterialParameter& param) noexcept;

		void remove_parameter(std::string_view name, std::string_view identifier, MaterialParamterType type) noexcept;

		bool compile_from_shader_code(const MString& code_str) {

			
		}

	private:

		void instance_shader_code_template(const MString& code_str) {

			if(code_str.contains("main_vert")) {
				_source_codes.emplace_back( ShaderType::Vertex, code_str.replace_str("main_vert", "main"));
			}else if (code_str.contains("main_mesh")) {
				_source_codes.emplace_back(ShaderType::Mesh, code_str.replace_str("main_mesh", "main"));
			}
			
			if (code_str.contains("main_frag")) {
				_source_codes.emplace_back(ShaderType::Fragment, code_str.replace_str("main_mesh", "main"));
			}
		}

		Context* _ctx{};

		MMap<MString, uint8_t> _param_identifer_to_index{};
		MMap<MString, uint8_t> _param_name_to_index{};
		
		MVector<std::pair<ShaderType, MString>> _source_codes{};
		MVector<std::unique_ptr<ShaderModule>> _shaders{};

		std::unique_ptr<GraphicsPipeline> _pipeline;
	};
}