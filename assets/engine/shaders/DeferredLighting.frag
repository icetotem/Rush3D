#version 460 core
#extension GL_EXT_samplerless_texture_functions : enable

layout(location = 0) in vec2 v_TexCoord;

#include <Resources/FrameBlock.glsl>

//#define ENABLE_SHADOW
//#define ENABLE_GI
#define USE_PBR

layout(set = 1, binding = 0) uniform texture2D t_SceneDepth; 
layout(set = 1, binding = 1) uniform sampler s_d;
layout(set = 1, binding = 2) uniform texture2D t_GBuffer0; // .rgb = Normal
layout(set = 1, binding = 3) uniform sampler s_0;
layout(set = 1, binding = 4) uniform texture2D t_GBuffer1; // .rgb = Albedo, .a = SpecularWeight
layout(set = 1, binding = 5) uniform sampler s_1;
layout(set = 1, binding = 6) uniform texture2D t_GBuffer2; // .rgb = EmissiveColor
layout(set = 1, binding = 7) uniform sampler s_2;
layout(set = 1, binding = 8) uniform texture2D t_GBuffer3; // .r = Metallic, .g = Roughness, .b = AO, .a = ShadingModel/MaterialFlags
layout(set = 1, binding = 9) uniform sampler s_3;
layout(set = 1, binding = 10) uniform texture2D t_SSAO;
layout(set = 1, binding = 11) uniform sampler s_ssao;
layout(set = 1, binding = 12) uniform texture2D t_BRDF;
layout(set = 1, binding = 13) uniform sampler s_brdf;
layout(set = 1, binding = 14) uniform textureCube t_IrradianceMap;
layout(set = 1, binding = 15) uniform sampler s_irr;
layout(set = 1, binding = 16) uniform textureCube t_PrefilteredEnvMap;
layout(set = 1, binding = 17) uniform sampler s_prf;

#if TILED_LIGHTING
layout(set = 1, binding = 18, std430) restrict readonly buffer LightIndexList {
  // .x = indices for opaque geometry
  // .y = indices for translucent geometry
  uvec2 g_LightIndexList[];
};

// [startOffset, lightCount]
// .rg = opaque geometry
// .ba = translucent geometry
layout(set = 1, binding = 19, rgba32ui) restrict readonly uniform uimage2D i_LightGrid;
#endif

#if ENABLE_SHADOW
layout(set = 1, binding = 20) uniform texture2DArray t_CascadedShadowMaps;
layout(set = 1, binding = 21) uniform sampler s_shadow;
#endif

#if ENABLE_GI
layout(set = 1, binding = 22) uniform texture3D t_AccumulatedSH_R;
layout(set = 1, binding = 23) uniform sampler s_acc_r;
layout(set = 1, binding = 24) uniform texture3D t_AccumulatedSH_G;
layout(set = 1, binding = 25) uniform sampler s_acc_g;
layout(set = 1, binding = 26) uniform texture3D t_AccumulatedSH_B;
layout(set = 1, binding = 27) uniform sampler s_acc_b;
#endif

#include <Lib/Depth.glsl>

#include <Lib/Light.glsl>
_DECLARE_LIGHT_BUFFER(g_LightCount, g_LightBuffer);

#include <Lib/IBL_AmbientLighting.glsl>
#include <Lib/PBR_DirectLighting.glsl>

#ifdef ENABLE_SHADOW
#define SOFT_SHADOWS 1
#include <Lib/CSM.glsl>
#include "LPV.glsl"
#endif

layout(set = 1, binding = 28) uniform LightGrid_t {
  vec3 minCorner;
  float padding;
  vec3 gridSize;
  float cellSize;
} LightGrid;

#define u_MinCorner LightGrid.minCorner
#define u_GridSize LightGrid.gridSize
#define u_CellSize LightGrid.cellSize

#include <MaterialDefines.glsl>

layout(location = 0) out vec4 FragColor;

void main() {
  const float depth = getDepth(t_SceneDepth, s_d, v_TexCoord);
  if (depth >= 1.0) discard;

  const vec3 emissiveColor = texture(sampler2D(t_GBuffer2, s_2), v_TexCoord).rgb;

  vec4 temp = texture(sampler2D(t_GBuffer3, s_3), v_TexCoord);

  const int encoded = int(temp.a * 255.0);
  const int shadingModel = bitfieldExtract(encoded, 0, 2);
  if (shadingModel == SHADING_MODEL_UNLIT) {
    FragColor.rgb = emissiveColor;
    return;
  }
  const int materialFlags = bitfieldExtract(encoded, 2, 6);
  const bool receiveShadow =
    (materialFlags & MaterialFlag_ReceiveShadow) == MaterialFlag_ReceiveShadow;

  const float metallic = temp.r;
  const float roughness = temp.g;
  float ao = temp.b;
  if (hasRenderFeatures(RenderFeature_SSAO)) {
    ao = texture(sampler2D(t_SSAO, s_ssao), v_TexCoord).r;
  }

  temp = texture(sampler2D(t_GBuffer1, s_1), v_TexCoord);
  const vec3 albedo = temp.rgb;
  const float specularWeight = temp.a;

  //
  // Lighting calculation:
  //

  // NOTE: values in world-space
  // N = normal (from surface)
  // L = fragment to light direction
  // V = fragment to eye direction
  // H = halfway vector (between V and L)

  const vec3 fragPosViewSpace = viewPositionFromDepth(depth, v_TexCoord);
  const vec3 fragPosWorldSpace = (u_Frame.camera.inversedView * vec4(fragPosViewSpace, 1.0)).xyz;

  const vec3 N = normalize(texture(sampler2D(t_GBuffer0, s_0), v_TexCoord).rgb * 2.0 - 1.0);
  const vec3 V = normalize(getCameraPosition() - fragPosWorldSpace);
  const float NdotV = clamp01(dot(N, V));

  // Dielectrics: [0.02..0.05], Metals: [0.5..1.0]
  const float kMinRoughness = 0.04;
  vec3 F0 = vec3(kMinRoughness);
  const vec3 diffuseColor = mix(albedo * (1.0 - F0), vec3(0.0), metallic);
  F0 = mix(F0, albedo, metallic);

  const float alphaRoughness = roughness * roughness;

  //
  // Ambient lighting:
  //

  vec3 Lo_diffuse = vec3(0.0);
  vec3 Lo_specular = vec3(0.0);
#if ENABLE_IBL
  if (hasRenderFeatures(RenderFeature_IBL)) {
    // clang-format off
    const LightContribution ambientLighting = IBL_AmbientLighting(
      diffuseColor,
      F0,
      specularWeight,
      roughness,
      N,
      V,
      NdotV
    );
    // clang-format on
    Lo_diffuse = ambientLighting.diffuse * ao;
    Lo_specular = ambientLighting.specular * ao;
  }
#endif

#ifdef ENABLE_GI
  if (hasRenderFeatures(RenderFeature_GI)) {
    const vec3 cellCoords =
      (fragPosWorldSpace - u_MinCorner) / u_CellSize / u_GridSize;

    // clang-format off
    const SHcoeffs coeffs = {
      texture(sampler3D(t_AccumulatedSH_R, s_acc_r), cellCoords, 0),
      texture(sampler3D(t_AccumulatedSH_G, s_acc_g), cellCoords, 0),
      texture(sampler3D(t_AccumulatedSH_B, s_acc_b), cellCoords, 0)
    };
    // clang-format on

    const vec4 SH_intensity = SH_evaluate(-N);
    const vec3 LPV_intensity =
      vec3(dot(SH_intensity, coeffs.red), dot(SH_intensity, coeffs.green),
           dot(SH_intensity, coeffs.blue));

    const vec3 LPV_radiance =
      max(LPV_intensity * 4 / u_CellSize / u_CellSize, 0.0);

    Lo_diffuse += albedo * LPV_radiance * ao;

    if (hasDebugFlags(DebugFlag_RadianceOnly)) {
      FragColor.rgb = LPV_radiance;
      return;
    }
  }
#endif

  //
  // Direct lighting:
  //

#if TILED_LIGHTING
  const ivec2 tileId = ivec2(floor(gl_FragCoord.xy / TILE_SIZE));
  const uvec2 tileInfo = imageLoad(i_LightGrid, tileId).rg;
  const uint startOffset = tileInfo.x;
  const uint lightCount = tileInfo.y;

  for (uint i = 0; i < lightCount; ++i) {
    const uint lightIndex = g_LightIndexList[startOffset + i].x;
#else
  for (uint i = 0; i < g_LightCount.count; ++i) {
    const uint lightIndex = i;
#endif
    const Light light = g_LightBuffer.data[lightIndex];

    const vec3 fragToLight = light.type != LightType_Directional
                               ? light.position.xyz - fragPosWorldSpace
                               : -light.direction.xyz;

    const vec3 L = normalize(fragToLight);
    const vec3 H = normalize(V + L);

    const float NdotL = clamp01(dot(N, L));
    if (NdotL > 0.0 || NdotV > 0.0) {
      float visibility = 1.0;
#ifdef ENABLE_SHADOW
      if (hasRenderFeatures(RenderFeature_Shadows) && receiveShadow) {
        if (light.type == LightType_Directional) {
          const uint cascadeIndex = _selectCascadeIndex(fragPosViewSpace);
          visibility =
            _getDirLightVisibility(cascadeIndex, fragPosWorldSpace, NdotL);
        }

        if (visibility == 0.0) continue;
      }
#endif

#ifdef USE_PBR
      const vec3 radiance = _getLightIntensity(light, fragToLight) * NdotL * visibility;
      // clang-format off
      const LightContribution directLighting = PBR_DirectLighting(
        radiance,
        diffuseColor,
        F0,
        specularWeight,
        alphaRoughness,
        NdotV,
        NdotL,
        clamp01(dot(N, H)),
        clamp01(dot(V, H))
      );
      // clang-format on

      Lo_diffuse += directLighting.diffuse;
      Lo_specular += directLighting.specular;
#else
      const vec3 radiance = albedo * _getLightIntensity(light, fragToLight);
      Lo_diffuse += radiance * NdotL * visibility;
      Lo_specular += radiance * pow(clamp01(dot(N, H)), 64) * visibility;
#endif
    }
  }

  FragColor.rgb = Lo_diffuse + Lo_specular + emissiveColor;
  //FragColor.rgb += diffuseColor;
  //FragColor.rgb = fragPosWorldSpace;
  //FragColor.rgb = V;
  //FragColor.rgb = (N + 1) * 0.5;
  //FragColor.rgb = N;
}
