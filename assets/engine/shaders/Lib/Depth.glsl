#ifndef _DEPTH_GLSL_
#define _DEPTH_GLSL_

// NOTE: Must match glClipControl
#define DEPTH_ZERO_TO_ONE 1

#if DEPTH_ZERO_TO_ONE
#  define NEAR_CLIP_PLANE 0.0
#else
#  define NEAR_CLIP_PLANE -1.0
#endif

#include <Lib/Math.glsl>
#include <Lib/SpaceTraversal.glsl>

// Returns depth in clip-space
float getDepth(texture2D depthMap, sampler samp, vec2 texCoord) {
  const float sampledDepth = texture(sampler2D(depthMap, samp), texCoord).r;
#if DEPTH_ZERO_TO_ONE
  return sampledDepth;
#else
  return sampledDepth * 2.0 - 1.0;
#endif
}

// Returns depth in clip-space
float fetchDepth(sampler2D depthMap, ivec2 coord) {
  const float sampledDepth = texelFetch(depthMap, coord, 0).r;
#if DEPTH_ZERO_TO_ONE
  return sampledDepth;
#else
  return sampledDepth * 2.0 - 1.0;
#endif
}

float linearizeDepth(float n, float f, float sampledDepth) {
#if DEPTH_ZERO_TO_ONE
  const float z = sampledDepth;
#else
  const float z = sampledDepth * 2.0 - 1.0;
#endif
  return n * f / (f + z * (n - f));
}

float linearizeDepth(float sampledDepth) {
  return linearizeDepth(u_Frame.camera.near, u_Frame.camera.far, sampledDepth);
}

vec3 viewPositionFromDepth(float z, vec2 texCoord) {
  // https://stackoverflow.com/questions/11277501/how-to-recover-view-space-position-given-view-space-depth-value-and-ndc-xy/46118945#46118945
#if ORIGIN_UPPER_LEFT
  texCoord.y = 1.0 - texCoord.y;
#endif
  return clipToView(vec4(texCoord * 2.0 - 1.0, z, 1.0));
}

#endif
