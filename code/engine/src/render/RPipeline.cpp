#include "stdafx.h"

#include <dawn/webgpu_cpp.h>

#include "render/RPipeline.h"
#include "RContex.h"

namespace rush
{

    //////////////////////////////////////////////////////////////////////////

    extern wgpu::VertexFormat g_WGPUVertexFormat[(int)VertexFormat::Count];
    extern wgpu::TextureFormat g_WGPUTextureFormat[(int)TextureFormat::Count];
    extern wgpu::CullMode g_WGPUCullMode[(int)CullMode::Count];
    extern wgpu::FrontFace g_WGPUFrontFace[(int)FrontFace::Count];
    extern wgpu::PrimitiveTopology g_WGPUPrimitiveTopology[(int)PrimitiveType::Count];
    extern wgpu::BlendFactor g_WGPUBlendFactor[(int)BlendFactor::Count];
    extern wgpu::BlendOperation g_WGPUBlendOperation[(int)BlendOperation::Count];
    extern wgpu::CompareFunction g_WGPUCompareFunction[(int)DepthCompareFunction::Count];


    RPipeline::RPipeline(Ref<RContex> contex, const PipelineDesc& desc, const char* lable /*= nullptr*/)
    {
        wgpu::PipelineLayoutDescriptor layoutDesc = {};
        if (desc.bindLayout != nullptr) 
        {
            layoutDesc.bindGroupLayoutCount = 1;
            layoutDesc.bindGroupLayouts = desc.bindLayout->m_Layout.get();
        }
        else 
        {
            layoutDesc.bindGroupLayoutCount = 0;
            layoutDesc.bindGroupLayouts = nullptr;
        }

        wgpu::RenderPipelineDescriptor descriptor = {};
        descriptor.label = lable;
        descriptor.layout = contex->device.CreatePipelineLayout(&layoutDesc);

        // Setup vertex state.
        std::array<wgpu::VertexBufferLayout, kMaxVertexBuffers> cBuffers;
        std::array<wgpu::VertexAttribute, kMaxVertexAttributes> cAttributes;
        {
            wgpu::VertexState* vertex = &descriptor.vertex;
            vertex->module = *desc.vs->m_Module;
            vertex->entryPoint = "main";

            int i = 0;
            int attr = 0;
            for (const auto& layout : desc.vertexLayouts)
            {
                cBuffers[i].stepMode = wgpu::VertexStepMode::Vertex;
                cBuffers[i].arrayStride = layout.stride;
                cBuffers[i].attributeCount = layout.attributeCount;
                cBuffers[i].attributes = &cAttributes[attr];
                ++i;

                for (uint32_t j = 0; j < layout.attributeCount; ++j)
                {
                    cAttributes[attr].format = g_WGPUVertexFormat[(int)layout.attributes[j].format];
                    cAttributes[attr].offset = layout.attributes[j].offset;
                    cAttributes[attr].shaderLocation = layout.attributes[j].shaderLocation;
                    ++attr;
                }
            }

            vertex->bufferCount = desc.vertexLayouts.size();
            vertex->buffers = &cBuffers[0];
        }

        // Setup fragment state
        wgpu::FragmentState cFragment = {};
        wgpu::ColorTargetState cTargets = {};
        wgpu::BlendState cBlendState = {};
        {
            cFragment.module = *desc.fs->m_Module;
            cFragment.entryPoint = "main";
            cFragment.targetCount = 1;
            descriptor.fragment = &cFragment;         

            cFragment.targets = &cTargets;
            cTargets.format = g_WGPUTextureFormat[(int)desc.colorFormat];
            cTargets.writeMask = (wgpu::ColorWriteMask)desc.writeMask;

            if (desc.useBlend)
            {
                cTargets.blend = &cBlendState;
                cBlendState.color.srcFactor = g_WGPUBlendFactor[(int)desc.blendStates.srcColor];
                cBlendState.color.dstFactor = g_WGPUBlendFactor[(int)desc.blendStates.dstColor];
                cBlendState.color.operation = g_WGPUBlendOperation[(int)desc.blendStates.opColor];
                cBlendState.alpha.srcFactor = g_WGPUBlendFactor[(int)desc.blendStates.srcAlpha];
                cBlendState.alpha.dstFactor = g_WGPUBlendFactor[(int)desc.blendStates.dstAlpha];
                cBlendState.alpha.operation = g_WGPUBlendOperation[(int)desc.blendStates.opAlpha];
            }
        }

        // Set the defaults for the primitive state
        {
            wgpu::PrimitiveState* primitive = &descriptor.primitive;
            primitive->topology = g_WGPUPrimitiveTopology[(int)desc.primitiveType];
            primitive->frontFace = g_WGPUFrontFace[(int)desc.frontFace];
            primitive->cullMode = g_WGPUCullMode[(int)desc.cullModel];
            primitive->stripIndexFormat = wgpu::IndexFormat::Undefined;
        }

        // Set the defaults for the depth-stencil state
        wgpu::DepthStencilState cDepthStencil = {};
        if (desc.depthTest || desc.depthWrite || desc.stencilTest || desc.stencilWrite)
        {
            descriptor.depthStencil = &cDepthStencil;
            cDepthStencil.format = g_WGPUTextureFormat[(int)desc.depthStencilFormat];
            cDepthStencil.depthWriteEnabled = desc.depthWrite;
            if (desc.depthTest)
                cDepthStencil.depthCompare = g_WGPUCompareFunction[(int)desc.depthCompare];
            else
                cDepthStencil.depthCompare = wgpu::CompareFunction::Always;
            cDepthStencil.depthBias = 0;
            cDepthStencil.depthBiasSlopeScale = 0.0;
            cDepthStencil.depthBiasClamp = 0.0;

            wgpu::StencilFaceState stencilFace;
            stencilFace.compare = wgpu::CompareFunction::Always;
            stencilFace.failOp = wgpu::StencilOperation::Keep;
            stencilFace.depthFailOp = wgpu::StencilOperation::Keep;
            stencilFace.passOp = wgpu::StencilOperation::Keep;
            cDepthStencil.stencilBack = stencilFace;
            cDepthStencil.stencilFront = stencilFace;
            cDepthStencil.stencilReadMask = 0xff;
            cDepthStencil.stencilWriteMask = 0xff;
        }
        else
        {
            descriptor.depthStencil = nullptr;
        }

        // Set the multisample state
//         {
//             wgpu::MultisampleState* multisample = &descriptor.multisample;
//             multisample->count = 1;
//             multisample->mask = 0xFFFFFFFF;
//             multisample->alphaToCoverageEnabled = false;
//         }

        m_Pipeline = CreateRef<wgpu::RenderPipeline>(contex->device.CreateRenderPipeline(&descriptor));
    }

}