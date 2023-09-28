#version 450

// inputs
layout(location = 0) in vec2 v_texcoord0;

// outputs
layout(location = 0) out vec4 FragColor;

layout(set = 0, binding = 0) uniform  FrameData
{
    mat4 proj;
    mat4 view;
    mat4 viewProj;
} u_frameData;

layout(set = 1, binding = 0) uniform texture2D u_baseColor;
layout(set = 1, binding = 1) uniform sampler u_baseColorSampler;

void main()
{
    FragColor = texture(sampler2D(u_baseColor, u_baseColorSampler), v_texcoord0);
}