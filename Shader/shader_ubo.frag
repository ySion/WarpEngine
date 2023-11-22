#version 460

layout(set = 0, binding = 0) uniform UniformBuffer{
	float time;
} ubo;

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 space_normal;
layout (location = 2) in vec3 global_normal;
layout (location = 3) in vec3 view_normal;
layout (location = 4) in vec2 ouv;

layout (location = 0) out vec4 outColor;

layout(set= 0, binding = 3) uniform sampler2D tex;
 
void main(){
	

	vec2 texCoord = view_normal.xy * 0.5 + 0.5;
	vec3 color = texture(tex, texCoord).rgb;

	outColor = vec4(color, 1.0);
}