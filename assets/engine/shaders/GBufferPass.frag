#version 460 core

// Use with Geometry.vert

#include "BasePassAttributes.glsl"

#include <Lib/Math.glsl>
#include <Lib/Color.glsl>
#include <Lib/NormalMapping.glsl>

#include <Material.glsl>
#include <User.glsl>

#if BLEND_MODE == BLEND_MODE_OPAQUE
// https://www.khronos.org/opengl/wiki/Early_Fragment_Test
layout(early_fragment_tests) in;
#endif

layout(set = 1, binding = 0) uniform MaterialFlags_t {
  int materialFlags;
} u_MaterialFlags;

#define u_MaterialFlags u_MaterialFlags.materialFlags

layout(location = 0) out vec4 GBuffer0; // .rgb = Normal
layout(location = 1) out vec4 GBuffer1; // .rgb = Albedo, .a = SpecularWeight
layout(location = 2) out vec4 GBuffer2; // .rgb = Emissive
layout(location = 3) out vec4 GBuffer3; // .r = Metallic, .g = Roughness, .b = AO, .a = ShadingModel/MaterialFlags

void main() {
  Material material = _initMaterial();
  _executeUserCode(material);

#if BLEND_MODE == BLEND_MODE_MASKED
  if (!material.visible) {
    discard;
    return;
  }
#endif

  GBuffer0.rgb = (normalize(material.normal) + 1) * 0.5;
  GBuffer1 = vec4(material.baseColor, material.specular);
  GBuffer2.rgb = material.emissiveColor;

  int encoded = bitfieldInsert(0, SHADING_MODEL, 0, 2);
  encoded = bitfieldInsert(encoded, u_MaterialFlags, 2, 6);

  GBuffer3 = vec4(clamp01(material.metallic), clamp01(material.roughness),
                  clamp01(material.ambientOcclusion), float(encoded) / 255.0);
}
