#version 450

// inputs
layout(location = 0) in vec2 v_texcoord0;

layout(binding = 0) uniform sampler2D s_input;

// outputs
layout(location = 0) out vec4 FragColor;

void main()
{
    FragColor = texture(s_input, v_texcoord0);
}