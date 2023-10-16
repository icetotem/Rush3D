#version 460 core

layout(location = 0) in vec2 v_TexCoord;

layout(set = 1, binding = 0) uniform texture2D t_0;
layout(set = 1, binding = 1) uniform sampler s_0;

const uint Tonemap_Clamp = 0;
const uint Tonemap_ACES = 1;
const uint Tonemap_Filmic = 2;
const uint Tonemap_Reinhard = 3;
const uint Tonemap_Uncharted = 4;

//layout(location = 0) uniform uint u_Tonemap = Tonemap_Clamp;

#include <Lib/Math.glsl>
#include <Lib/Color.glsl>
#include <Tonemapping/ACES.glsl>
#include <Tonemapping/Filmic.glsl>
#include <Tonemapping/Reinhard.glsl>
#include <Tonemapping/Uncharted.glsl>

layout(location = 0) out vec4 FragColor;
void main() {
  const vec3 color = texture(sampler2D(t_0, s_0), v_TexCoord).rgb;
/*
  switch (u_Tonemap) {
  case Tonemap_ACES:
    FragColor = ACES(color);
    break;
  case Tonemap_Filmic:
    FragColor = tonemapFilmic(color);
    break;
  case Tonemap_Reinhard:
    FragColor = reinhard2(color);
    break;
  case Tonemap_Uncharted:
    FragColor = tonemapUncharted2(color);
    break;

  case Tonemap_Clamp:
  default:
    FragColor = color;
    break;
  }
*/
    
  FragColor.rgb = ACES(color);
  FragColor.rgb = linearTosRGB(FragColor.rgb);
  FragColor.a = 1.0;
}
