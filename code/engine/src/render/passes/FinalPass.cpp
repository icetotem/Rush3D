#include "stdafx.h"
#include "render/passes/FinalPass.h"
#include "render/passes/FrameData.h"
#include "render/passes/FrameGraphTexture.h"
#include "render/RenderContext.h"

namespace rush
{

    FinalPass::FinalPass()
    {
//         ShaderCodeBuilder shaderCodeBuilder;
// 
//         auto program = m_renderContext.createGraphicsProgram(
//             shaderCodeBuilder.build("FullScreenTriangle.vert"),
//             shaderCodeBuilder.build("FinalPass.frag"));
// 
//         m_pipeline = GraphicsPipeline::Builder{}
//             .setShaderProgram(program)
//             .setDepthStencil({
//               .depthTest = false,
//               .depthWrite = false,
//                 })
//                 .setRasterizerState({
//                   .polygonMode = PolygonMode::Fill,
//                   .cullMode = CullMode::Back,
//                   .scissorTest = false,
//                     })
//                     .build();
    }

    FinalPass::~FinalPass() 
    { 
    }

    void FinalPass::compose(FrameGraph& fg, const FrameGraphBlackboard& blackboard, OutputMode outputMode)
    {
        const auto [frameBlock] = blackboard.get<FrameData>();

        enum Mode_ {
            Mode_Discard = -1,
            Mode_Default,
            Mode_LinearDepth,
            Mode_RedChannel,
            Mode_GreenChannel,
            Mode_BlueChannel,
            Mode_AlphaChannel,
            Mode_ViewSpaceNormals,
        } mode{Mode_Default};

        FrameGraphResource output{ (int)OutputMode::FinalImage };
//         switch (outputMode) {
//         case OutputMode::Depth:
//             output = blackboard.get<GBufferData>().depth;
//             mode = Mode_LinearDepth;
//             break;
//         case OutputMode::Emissive:
//             output = blackboard.get<GBufferData>().emissive;
//             break;
//         case OutputMode::BaseColor:
//             output = blackboard.get<GBufferData>().albedo;
//             break;
//         case OutputMode::Normal:
//             output = blackboard.get<GBufferData>().normal;
//             // mode = Mode_ViewSpaceNormals;
//             break;
//         case OutputMode::Metallic:
//             output = blackboard.get<GBufferData>().metallicRoughnessAO;
//             mode = Mode_RedChannel;
//             break;
//         case OutputMode::Roughness:
//             output = blackboard.get<GBufferData>().metallicRoughnessAO;
//             mode = Mode_GreenChannel;
//             break;
//         case OutputMode::AmbientOcclusion:
//             output = blackboard.get<GBufferData>().metallicRoughnessAO;
//             mode = Mode_BlueChannel;
//             break;
//         case OutputMode::SSAO:
//             output = blackboard.get<SSAOData>().ssao;
//             mode = Mode_RedChannel;
//             break;
//         case OutputMode::BrightColor:
//             output = blackboard.get<BrightColorData>().brightColor;
//             break;
//         case OutputMode::Reflections:
//             output = blackboard.get<ReflectionsData>().reflections;
//             break;
// 
//         case OutputMode::Accum:
//             if (auto oit = blackboard.try_get<WeightedBlendedData>(); oit)
//                 output = oit->accum;
//             break;
//         case OutputMode::Reveal:
//             if (auto oit = blackboard.try_get<WeightedBlendedData>(); oit) {
//                 output = oit->reveal;
//                 mode = Mode_RedChannel;
//             }
//             break;
// 
//         case OutputMode::LightHeatmap: {
//             const auto& lightCullingData = blackboard.get<LightCullingData>();
//             if (lightCullingData.debugMap.has_value())
//                 output = *lightCullingData.debugMap;
//         } break;
// 
//         case OutputMode::HDR:
//             output = blackboard.get<SceneColorData>().hdr;
//             break;
//         case OutputMode::FinalImage:
//             output = blackboard.get<SceneColorData>().ldr;
//             break;
// 
//         default:
//             assert(false);
//             break;
//         }

        if (output == -1) 
            mode = Mode_Discard;

        fg.addCallbackPass(
            "FinalComposition",
                [&](FrameGraph::Builder& builder, auto&) {
                builder.read(frameBlock);
                if (mode != Mode_Discard) 
                    builder.read(output);
                builder.setSideEffect();
            },
            [=](const auto&, FrameGraphPassResources& resources, void* ctx) {
//                 NAMED_DEBUG_MARKER("FinalComposition");
//                 TracyGpuZone("FinalComposition");

                auto& o = resources.getDescriptor<FrameGraphTexture>(output);
                const auto extent = resources.getDescriptor<FrameGraphTexture>(output).extent;
                auto& rc = *static_cast<RenderContext*>(ctx);
                //rc.beginRendering({ .extent = rc.getSwapchainSize() }, glm::vec4{0.0f});
                rc.BeginDraw(Vector4(0, 0, 1, 1));
                if (mode != Mode_Discard) {
//                     rc.setGraphicsPipeline(m_pipeline)
//                         .bindUniformBuffer(0, getBuffer(resources, frameBlock))
//                         .bindTexture(0, getTexture(resources, output))
//                         .setUniform1ui("u_Mode", mode)
//                         .drawFullScreenTriangle();
                }
                rc.EndDraw();
            });
    }

}