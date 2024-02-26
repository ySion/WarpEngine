#version 460 core

//nonuniformEXT
#extension GL_EXT_nonuniform_qualifier : require

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 space_normal;
layout (location = 2) in vec3 global_normal;
layout (location = 3) in vec3 view_normal;
layout (location = 4) in vec2 ouv;

layout(location = 5) flat in uint drawID;

layout (location = 0) out vec4 outColor;

layout(set = 0, binding = 0)uniform sampler2D global_textures[];
layout(set = 0, binding = 0)uniform usampler2D global_textures_uint[];
layout(set = 0, binding = 0)uniform sampler3D global_textures_3d[];
layout(set = 0, binding = 0)uniform usampler3D global_textures_3d_uint[];

layout(set = 1, binding = 0) buffer GlobalParameters {
	mat4 V;
	mat4 P;
} global_parameters[];

layout(set = 2, binding = 0) buffer MaterialParameters {
	float brightness_multi;
	float roughness_multi;
	float metallic_multi;
	uint sampler_index[16];
} material_parameters[];

layout(set = 3, binding = 0) buffer ObjectParameters {
	mat4 M;
} object_parameters[];


void main(){
	
	vec2 texCoord = view_normal.xy * 0.5 + 0.5;
	double t = 5*5;
	vec3 color = texture(global_textures[nonuniformEXT(drawID)], texCoord).rgb;

	outColor = vec4(color, 1.0);
}
