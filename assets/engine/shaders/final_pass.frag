#version 450

// inputs
layout(location = 0) in vec2 v_texcoord0;

layout(binding = 0) uniform texture2D u_texture;
layout(binding = 1) uniform sampler u_sampler;

// outputs
layout(location = 0) out vec4 FragColor;

void main()
{
    FragColor = texture(sampler2D(u_texture, u_sampler), v_texcoord0);
}