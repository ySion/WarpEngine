#version 460

#extension GL_ARB_separate_shader_objects : require
#extension GL_EXT_nonuniform_qualifier : require

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

layout(location = 0) out vec3 pos;
layout(location = 1) out vec3 space_normal;
layout(location = 2) out vec3 global_normal;
layout(location = 3) out vec3 view_normal;
layout(location = 4) out vec2 ouv;
layout(location = 5) flat out uint drawID;


layout(set = 0, binding = 0)uniform sampler2D global_textures[];
layout(set = 0, binding = 0)uniform usampler2D global_textures_uint[];
layout(set = 0, binding = 0)uniform sampler3D global_textures_3d[];
layout(set = 0, binding = 0)uniform usampler3D global_textures_3d_uint[];

layout(set = 1, binding = 0) buffer GlobalParameters {
	mat4 V;
	mat4 P;
} global_parameters[];

layout(set = 2, binding = 0) buffer MaterialParameters { //dynamic construction
	float brightness_multi;
	float roughness_multi;
	float metallic_multi;
} material_parameters[];

layout(set = 3, binding = 0) buffer ObjectParameters {
	uint sampler_index[16];
	mat4 M;
} object_parameters[];


layout(push_constant) uniform PushConstants {
	mat4 V;
	mat4 P;
}info;

void main(){
	mat4 MVP = global_parameters[nonuniformEXT(drawID)].P * global_parameters[nonuniformEXT(drawID)].V;
	mat4 MV = global_parameters[nonuniformEXT(drawID)].V;
	
	gl_Position = MVP * vec4(position, 1.0);

	space_normal = normal;
	global_normal = (vec4(normalize(normal), 0.0)).xyz;
	view_normal = normalize(MV * vec4(normalize(normal), 0.0f)).xyz;
	pos = gl_Position.xyz;
	drawID = gl_DrawID;
	ouv = uv;
}

