#version 460 core

layout(location = 0) in vec2 v_TexCoord;

#include <Resources/FrameBlock.glsl>

layout(set = 2, binding = 0) uniform texture2D t_0;
layout(set = 2, binding = 1) uniform sampler s_0;

const uint Mode_Default = 0;
const uint Mode_LinearDepth = 1;
const uint Mode_RedChannel = 2;
const uint Mode_GreenChannel = 3;
const uint Mode_BlueChannel = 4;
const uint Mode_AlphaChannel = 5;
const uint Mode_ViewSpaceNormals = 6;

layout(set = 2, binding = 2) uniform Mode_t
{
    uint mode;
} Mode;

#define u_Mode Mode.mode

#include <Lib/Depth.glsl>

layout(location = 0) out vec4 FragColor;
void main() {
  const vec4 source = texture(sampler2D(t_0, s_0), v_TexCoord);

  switch (u_Mode) {
  case Mode_LinearDepth:
    FragColor.rgb = vec3(linearizeDepth(source.r) / u_Frame.camera.far);
    break;
  case Mode_RedChannel:
    FragColor.rgb = source.rrr;
    break;
  case Mode_GreenChannel:
    FragColor.rgb = source.ggg;
    break;
  case Mode_BlueChannel:
    FragColor.rgb = source.bbb;
    break;
  case Mode_AlphaChannel:
    FragColor.rgb = source.aaa;
    break;
  case Mode_ViewSpaceNormals:
    FragColor.rgb = (u_Frame.camera.view * vec4(source.rgb, 0.0)).xyz;
    break;

  case Mode_Default:
  default:
    FragColor.rgb = source.rgb;
    break;
  }
  FragColor.a = 1.0;
}
