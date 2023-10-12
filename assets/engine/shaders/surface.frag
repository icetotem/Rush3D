#version 450

#define kMaxPointLights 16

// inputs
layout(location = 0) in vec2 v_texcoord0;

// outputs
layout(location = 0) out vec4 FragColor;

layout(set = 0, binding = 0) uniform FrameData
{
    mat4 proj;
    mat4 view;
    mat4 viewProj;
} u_frameData;

layout(set = 1, binding = 0) uniform DirectLightData
{
    vec3 direction;
    float pointLightCount;
    vec4 color;
} u_directLightData;

struct PointLightData
{
    vec3 position;
    float radius;
    vec3 direction;
    float angle;
    vec4 color;
};

layout(set = 1, binding = 1) uniform PointLightDataWrap
{
    PointLightData data[kMaxPointLights];
} pointLightData;

#define u_pointLightData pointLightData.data

layout(set = 1, binding = 0) uniform texture2D u_baseColor;
layout(set = 1, binding = 1) uniform sampler u_baseColorSampler;

void main()
{
    vec4 color = texture(sampler2D(u_baseColor, u_baseColorSampler), v_texcoord0);
    vec3 lightColor = u_directLightData.color.rgb * u_directLightData.color.a;
    for (int i = 0; i < int(u_directLightData.pointLightCount); ++i)
    {
        lightColor += u_pointLightData[i].color.rgb * u_pointLightData[i].color.a;
    }
    FragColor = vec4(lightColor + color.rgb, color.a);
}