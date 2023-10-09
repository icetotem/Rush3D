#version 460 core

layout(location = 0) in FragData {
  vec3 cellCoord;
  vec3 N;
}
fs_in;

layout(binding = 2) uniform sampler3D t_AccumulatedSH_R;
layout(binding = 3) uniform sampler3D t_AccumulatedSH_G;
layout(binding = 4) uniform sampler3D t_AccumulatedSH_B;

layout(location = 2) uniform vec3 u_GridSize;
layout(location = 3) uniform float u_CellSize;

#include "LPV.glsl"

layout(location = 0) out vec4 FragColor;

void main() {
  const vec4 SH_intensity = SH_evaluate(normalize(fs_in.N));

  // clang-format off
  const SHcoeffs coeffs = {
    texture(t_AccumulatedSH_R, fs_in.cellCoord, 0),
    texture(t_AccumulatedSH_G, fs_in.cellCoord, 0),
    texture(t_AccumulatedSH_B, fs_in.cellCoord, 0)
  };
  // clang-format on
  const vec3 intensity =
    vec3(dot(SH_intensity, coeffs.red), dot(SH_intensity, coeffs.green),
         dot(SH_intensity, coeffs.blue));

  FragColor.rgb = intensity;
}
