#pragma once
#include "Graphics/GPUResource.h"

namespace Warp::Render {


	//layout vec3 position
	//layout vec3 normal
	//layout vec3 tangent
	//layout vec3 color
	//layout vec3 pivot
	//layout vec2 uv
	//layout vec2 uv1
	//layout vec2 uv2
	//layout vec2 uv3



	//uniform sampler2D global_texture[]
	//buffer global_parameter[] { //内定
	//      ...
	//}
	//buffer material_parameter[] { //MaterialParameterDesc 定义大小和规格
	//      ...
	//}
	//buffer object_parameter[] {  //一般来说只有Model矩阵和一些用户自定义数据,比如perobject, 颜色
	//      ...
	//}
	struct MaterialParameterDesc
	{
		// ?
	};


	class MaterialSource {
	private:
		GPU::GPUResourceHandle<GPU::GPUShader> m_shader{};
		GPU::GPUResourceHandle<GPU::GPUGraphicsPipeline> m_pipeline{};
		GPU::GPUResourceHandle<GPU::GPUPipelineLayout> m_pipeline_layout{};
		MaterialParameterDesc* material_parameter_desc{};
		void* material_parameter_data{};
	};

	class MaterialInstance {
	private:
		MaterialSource* parent{};
		void* material_parameter_data{};
	};
}
