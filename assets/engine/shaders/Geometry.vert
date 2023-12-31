#version 460 core

// VertexFormat.hpp

layout(location = 0) in vec3 a_Position;
#ifdef HAS_COLOR
layout(location = 1) in vec3 a_Color0;
#endif
#ifdef HAS_NORMAL
layout(location = 2) in vec3 a_Normal;
#  ifdef HAS_TANGENTS
layout(location = 3) in vec3 a_Tangent;
#  endif
#endif
#ifdef HAS_TEXCOORD0
layout(location = 4) in vec2 a_TexCoord0;
#endif
#ifdef HAS_TEXCOORD1
layout(location = 5) in vec2 a_TexCoord1;
#endif
#ifdef IS_SKINNED
layout(location = 6) in ivec4 a_Joints;
layout(location = 7) in vec4 a_Weights;
#endif

#include <Resources/FrameBlock.glsl>

layout(set = 2, binding = 0) readonly buffer Transforms {
  mat4 mtx[];
} u_Transforms;

layout(set = 2, binding = 1) readonly buffer NormalTransforms {
  mat4 mtx[];
} u_NormalTransforms;

layout(set = 3, binding = 0) uniform Instance {
  int transformOffset;
} u_Instance;

out gl_PerVertex { vec4 gl_Position; };

layout(location = 0) out VertexData {
  vec4 fragPos; // world-space
#ifdef HAS_NORMAL
#  ifdef HAS_TANGENTS
  mat3 TBN; // tangent-space -> world-space
#  else
  vec3 normal;
#  endif
#endif
#ifdef HAS_TEXCOORD0
  vec2 texCoord0;
#endif
#ifdef HAS_TEXCOORD1
  vec2 texCoord1;
#endif
#ifdef HAS_COLOR
  vec3 color;
#endif
}
vs_out;

void main() {
  mat4 modelMtx = u_Transforms.mtx[u_Instance.transformOffset + gl_InstanceIndex];
  vs_out.fragPos = modelMtx * vec4(a_Position, 1.0);

#ifdef HAS_NORMAL
  const mat3 normalMatrix = mat3(u_NormalTransforms.mtx[u_Instance.transformOffset + gl_InstanceIndex]);
  const vec3 N = normalize(normalMatrix * a_Normal);
  //const vec3 N = a_Normal;
#  ifdef HAS_TANGENTS
  vec3 T = normalize(normalMatrix * a_Tangent);
  T = normalize(T - dot(T, N) * N);
  vec3 binormal = cross(a_Tangent, N);
  vec3 B = normalize(normalMatrix * binormal);
  vs_out.TBN = mat3(T, B, N);
#  else
  vs_out.normal = N;
#  endif
#endif

#ifdef HAS_TEXCOORD0
  vs_out.texCoord0 = a_TexCoord0;
#endif
#ifdef HAS_TEXCOORD1
  vs_out.texCoord1 = a_TexCoord1;
#endif

#ifdef HAS_COLOR
  vs_out.color = a_Color0;
#endif

  gl_Position = u_viewProj * vs_out.fragPos;
}
