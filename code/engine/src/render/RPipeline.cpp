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


    RPipeline::RPipeline(Ref<RContex> contex, const PipelineDesc* desc, const char* lable /*= nullptr*/)
    {
        m_Pipeline = CreateRef<wgpu::RenderPipeline>();

        wgpu::PipelineLayoutDescriptor layoutDesc = {};
        if (desc->BindLayout != nullptr) 
        {
            layoutDesc.bindGroupLayoutCount = 1;
            layoutDesc.bindGroupLayouts = desc->BindLayout->m_Layout.get();
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
            vertex->module = *desc->VS->m_Module;
            vertex->entryPoint = "main";

            int i = 0;
            int attr = 0;
            for (const auto& layout : desc->VLayouts)
            {
                cBuffers[i].stepMode = wgpu::VertexStepMode::Vertex;
                cBuffers[i].arrayStride = layout.Stride;
                cBuffers[i].attributeCount = layout.AttributeCount;
                cBuffers[i].attributes = &cAttributes[attr];
                ++i;

                for (uint32_t j = 0; j < layout.AttributeCount; ++j)
                {
                    cAttributes[attr].format = g_WGPUVertexFormat[(int)layout.Attributes[j].Format];
                    cAttributes[attr].offset = layout.Attributes[j].Offset;
                    cAttributes[attr].shaderLocation = layout.Attributes[j].ShaderLocation;
                    ++attr;
                }
            }

            vertex->bufferCount = desc->VLayouts.size();
            vertex->buffers = &cBuffers[0];
        }

        // Setup fragment state
        wgpu::FragmentState cFragment = {};
        wgpu::ColorTargetState cTargets = {};
        wgpu::BlendState cBlendState = {};
        {
            cFragment.module = *desc->FS->m_Module;
            cFragment.entryPoint = "main";
            cFragment.targetCount = 1;
            descriptor.fragment = &cFragment;         

            cFragment.targets = &cTargets;
            cTargets.format = g_WGPUTextureFormat[(int)desc->ColorFormat];
            cTargets.blend = &cBlendState;
            cTargets.writeMask = (wgpu::ColorWriteMask)desc->WriteMask;

            cBlendState.color.srcFactor = g_WGPUBlendFactor[(int)desc->Blend.SrcColor];
            cBlendState.color.dstFactor = g_WGPUBlendFactor[(int)desc->Blend.DstColor];
            cBlendState.color.operation = g_WGPUBlendOperation[(int)desc->Blend.OpColor];
            cBlendState.alpha.srcFactor = g_WGPUBlendFactor[(int)desc->Blend.SrcAlpha];
            cBlendState.alpha.dstFactor = g_WGPUBlendFactor[(int)desc->Blend.DstAlpha];
            cBlendState.alpha.operation = g_WGPUBlendOperation[(int)desc->Blend.OpAlpha];
        }

        // Set the defaults for the primitive state
        {
            wgpu::PrimitiveState* primitive = &descriptor.primitive;
            primitive->topology = g_WGPUPrimitiveTopology[(int)desc->Primitive];
            primitive->frontFace = g_WGPUFrontFace[(int)desc->Front];
            primitive->cullMode = g_WGPUCullMode[(int)desc->Cull];
            primitive->stripIndexFormat = wgpu::IndexFormat::Undefined;
        }

        // Set the defaults for the depth-stencil state
        wgpu::DepthStencilState cDepthStencil = {};
        if (desc->DepthTest || desc->DepthWrite || desc->StencilTest || desc->StencilWrite)
        {
            descriptor.depthStencil = &cDepthStencil;
            cDepthStencil.format = g_WGPUTextureFormat[(int)desc->DepthFormat];
            cDepthStencil.depthWriteEnabled = desc->DepthWrite;
            cDepthStencil.depthCompare = g_WGPUCompareFunction[(int)desc->DepthCompare];
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

        // Set the defaults for the multisample state
        {
            wgpu::MultisampleState* multisample = &descriptor.multisample;
            multisample->count = 1;
            multisample->mask = 0xFFFFFFFF;
            multisample->alphaToCoverageEnabled = false;
        }

        *m_Pipeline = contex->device.CreateRenderPipeline(&descriptor);
    }

    RPipeline::~RPipeline()
    {

    }


}