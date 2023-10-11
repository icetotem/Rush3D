#version 460 core

layout(location = 0) in vec2 v_TexCoord;

layout(set = 3, binding = 0) uniform sampler2D t_SceneColor;
layout(set = 3, binding = 1) uniform sampler2D t_Blur;
layout(set = 3, location = 2) uniform float u_Strength;

layout(location = 0) out vec4 FragColor;
void main() {
  const vec3 sceneColor = texture(t_SceneColor, v_TexCoord).rgb;
  const vec3 blurred = texture(t_Blur, v_TexCoord).rgb;
  FragColor.rgb = mix(sceneColor, blurred, u_Strength);
}
