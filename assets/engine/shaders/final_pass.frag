#version 460 core

#include "BasePassAttributes.glsl"

#include <Lib/Math.glsl>
#include <Lib/Color.glsl>
#include <Lib/NormalMapping.glsl>

#include <Material.glsl>
#include <User.glsl>

// inputs
layout(location = 0) in vec2 v_texcoord0;

// uniforms
//layout(set = 0, binding = 0) uniform texture2D u_texture;
//layout(set = 0, binding = 1) uniform sampler u_sampler;

// outputs
layout(location = 0) out vec4 FragColor;

void main()
{
    FragColor = texture(sampler2D(u_texture, u_sampler), v_texcoord0);
}