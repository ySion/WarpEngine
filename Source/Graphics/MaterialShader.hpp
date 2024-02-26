#pragma once

#include "Core/Inherit.hpp"
#include "Vk/ShaderModule.hpp"

WARP_TYPE_NAME_2(Gpu, MaterialShader);

namespace Warp::Gpu {


	 /*
		 Shader file:
		 vertex, fragment(base), 默认用bindlees
		                       set binding
		 #material_param_start  0    0
		 bool a 'desc'
		 int b 'desc'
		 float c 'desc'
		 double d 'desc'
		 color e 'desc'
		 vec2 f 'desc'
		 vec4 g 'desc'
		 mat4 i 'desc'
		 texture2D j 'desc'
		 #material_param_end

		 void main_vert();

		 void main_frag();
	 */


	class MaterialShader : public Inherit<MaterialShader, Object> {
	public:
		MaterialShader() = default;
		~MaterialShader() override = default;

		bool compile_from_source_code(const MString& source) { return false; }

		//bool compile_from_shader_graph() { }
	private:
		MVector<ShaderModule> _shaders{};
	};

}