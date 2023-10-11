#ifndef _FRAME_BLOCK_GLSL_
#define _FRAME_BLOCK_GLSL_

struct Camera {
  mat4 projection;
  mat4 inversedProjection;
  mat4 view;
  mat4 inversedView;
  mat4 vp;
  float fov;
  float near, far;
  // Implicit padding, 4bytes
};

layout(set = 0, binding = 0, std140) uniform FrameBlock {
  float time;
  float deltaTime;
  uvec2 resolution;
  Camera camera;
  uint renderFeatures;
  uint debugFlags;
}
u_Frame;

#define u_proj u_Frame.camera.projection
#define u_view u_Frame.camera.view
#define u_invProj u_Frame.camera.inversedProjection
#define u_invView u_Frame.camera.inversedView
#define u_viewProj u_Frame.camera.vp

float getTime() { return u_Frame.time; }
float getDeltaTime() { return u_Frame.deltaTime; }

uvec2 getResolution() { return u_Frame.resolution; }
vec2 getTexelSize() { return 1.0 / vec2(u_Frame.resolution); }

vec3 getCameraPosition() { return u_Frame.camera.inversedView[3].xyz; }

const uint RenderFeature_Shadows = 1 << 0;
const uint RenderFeature_GI = 1 << 1;
const uint RenderFeature_IBL = 1 << 2;
const uint RenderFeature_SSAO = 1 << 3;

bool hasRenderFeatures(uint f) { return (u_Frame.renderFeatures & f) == f; }

const uint DebugFlag_RadianceOnly = 1 << 3;

bool hasDebugFlags(uint f) { return (u_Frame.debugFlags & f) == f; }

#endif
