#version 450

layout(location = 0) out vec3 fragColor;

vec3 tri[] = {
    vec3(0.0, -0.5, 0.0),
    vec3(0.5, 0.5, 0.0),
    vec3(-0.2, 0.2, 0.0)
};
 
vec3 color[] = {
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0)
};

//layout(push_constant) uniform cts {
//	vec4 c1;
//	vec4 c2;
//	vec4 c3;
//	vec4 c4;
//}PushConstants;

void main(){
    gl_Position = vec4(tri[gl_VertexIndex] * 1.0f, 1.0);
    fragColor = color[gl_VertexIndex] * PushConstants.c1.xyz;
}