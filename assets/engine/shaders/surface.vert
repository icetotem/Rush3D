#version 450

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texcoord0;

layout (location = 0) out vec2 v_texcoord0;

layout(binding = 0) uniform  FrameData
{
    mat4 proj;
    mat4 view;
} u_frameData;

#define u_proj u_frameData.proj
#define u_view u_frameData.view
   
void main()
{
    v_texcoord0 = a_texcoord0;
    gl_Position = /*u_proj * u_view **/ vec4(a_position, 1.0);
}