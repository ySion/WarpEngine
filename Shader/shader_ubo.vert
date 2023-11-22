#version 450


layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

layout(location = 0) out vec3 pos;
layout(location = 1) out vec3 space_normal;
layout(location = 2) out vec3 global_normal;
layout(location = 3) out vec3 view_normal;
layout(location = 4) out vec2 ouv;


layout(set = 0, binding = 0) uniform UBO {
	float time;
} ubo;

layout(set = 0, binding = 1) uniform CameraInfo {
	mat4 view;
	mat4 projection;
}camera_info;

layout(set = 0, binding = 2) uniform ModelInfo {
	mat4 model;
}model_info;


void main(){
	mat4 MVP = camera_info.projection * camera_info.view * model_info.model;
	mat4 MV = camera_info.view * model_info.model;
	mat4 M = model_info.model;
	
	gl_Position = MVP * vec4(position + normal * ubo.time, 1.0);
	//space_normal = normal;
	//global_normal = (M * vec4(normalize(normal), 0.0)).xyz;
	view_normal = normalize(MV * vec4(normalize(normal), 0.0f)).xyz;
	pos = gl_Position.xyz;
	ouv = uv;
}