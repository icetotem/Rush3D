#include "stdafx.h"
#include "render/RPipeline.h"
#include "dawn/webgpu_cpp.h"
#include "render/Shader.h"
#include "RenderContex.h"

namespace rush
{
    class ComboRenderPipelineDescriptor : public wgpu::RenderPipelineDescriptor {
    public:
        ComboRenderPipelineDescriptor();

        ComboRenderPipelineDescriptor(const ComboRenderPipelineDescriptor&) = delete;
        ComboRenderPipelineDescriptor& operator=(const ComboRenderPipelineDescriptor&) = delete;
        ComboRenderPipelineDescriptor(ComboRenderPipelineDescriptor&&) = delete;
        ComboRenderPipelineDescriptor& operator=(ComboRenderPipelineDescriptor&&) = delete;

        wgpu::DepthStencilState* EnableDepthStencil(
            wgpu::TextureFormat format = wgpu::TextureFormat::Depth24PlusStencil8);
        void DisableDepthStencil();

        std::array<wgpu::VertexBufferLayout, kMaxVertexBuffers> cBuffers;
        std::array<wgpu::VertexAttribute, kMaxVertexAttributes> cAttributes;
        std::array<wgpu::ColorTargetState, kMaxColorAttachments> cTargets;
        std::array<wgpu::BlendState, kMaxColorAttachments> cBlends;

        wgpu::FragmentState cFragment;
        wgpu::DepthStencilState cDepthStencil;
    };

    ComboRenderPipelineDescriptor::ComboRenderPipelineDescriptor() {
        wgpu::RenderPipelineDescriptor* descriptor = this;

        // Set defaults for the vertex state.
        {
            wgpu::VertexState* vertex = &descriptor->vertex;
            vertex->module = nullptr;
            vertex->entryPoint = "main";
            vertex->bufferCount = 0;

            // Fill the default values for vertexBuffers and vertexAttributes in buffers.
            for (uint32_t i = 0; i < kMaxVertexAttributes; ++i) {
                cAttributes[i].shaderLocation = 0;
                cAttributes[i].offset = 0;
                cAttributes[i].format = wgpu::VertexFormat::Float32;
            }
            for (uint32_t i = 0; i < kMaxVertexBuffers; ++i) {
                cBuffers[i].arrayStride = 0;
                cBuffers[i].stepMode = wgpu::VertexStepMode::Vertex;
                cBuffers[i].attributeCount = 0;
                cBuffers[i].attributes = nullptr;
            }
            // cBuffers[i].attributes points to somewhere in cAttributes.
            // cBuffers[0].attributes points to &cAttributes[0] by default. Assuming
            // cBuffers[0] has two attributes, then cBuffers[1].attributes should point to
            // &cAttributes[2]. Likewise, if cBuffers[1] has 3 attributes, then
            // cBuffers[2].attributes should point to &cAttributes[5].
            cBuffers[0].attributes = &cAttributes[0];
            vertex->buffers = &cBuffers[0];
        }

        // Set the defaults for the primitive state
        {
            wgpu::PrimitiveState* primitive = &descriptor->primitive;
            primitive->topology = wgpu::PrimitiveTopology::TriangleList;
            primitive->stripIndexFormat = wgpu::IndexFormat::Undefined;
            primitive->frontFace = wgpu::FrontFace::CCW;
            primitive->cullMode = wgpu::CullMode::None;
        }

        // Set the defaults for the depth-stencil state
        {
            wgpu::StencilFaceState stencilFace;
            stencilFace.compare = wgpu::CompareFunction::Always;
            stencilFace.failOp = wgpu::StencilOperation::Keep;
            stencilFace.depthFailOp = wgpu::StencilOperation::Keep;
            stencilFace.passOp = wgpu::StencilOperation::Keep;

            cDepthStencil.format = wgpu::TextureFormat::Depth24PlusStencil8;
            cDepthStencil.depthWriteEnabled = false;
            cDepthStencil.depthCompare = wgpu::CompareFunction::Always;
            cDepthStencil.stencilBack = stencilFace;
            cDepthStencil.stencilFront = stencilFace;
            cDepthStencil.stencilReadMask = 0xff;
            cDepthStencil.stencilWriteMask = 0xff;
            cDepthStencil.depthBias = 0;
            cDepthStencil.depthBiasSlopeScale = 0.0;
            cDepthStencil.depthBiasClamp = 0.0;
        }

        // Set the defaults for the multisample state
        {
            wgpu::MultisampleState* multisample = &descriptor->multisample;
            multisample->count = 1;
            multisample->mask = 0xFFFFFFFF;
            multisample->alphaToCoverageEnabled = false;
        }

        // Set the defaults for the fragment state
        {
            cFragment.module = nullptr;
            cFragment.entryPoint = "main";
            cFragment.targetCount = 1;
            cFragment.targets = &cTargets[0];
            descriptor->fragment = &cFragment;

            wgpu::BlendComponent blendComponent;
            blendComponent.srcFactor = wgpu::BlendFactor::One;
            blendComponent.dstFactor = wgpu::BlendFactor::Zero;
            blendComponent.operation = wgpu::BlendOperation::Add;

            for (uint32_t i = 0; i < kMaxColorAttachments; ++i) {
                cTargets[i].format = wgpu::TextureFormat::RGBA8Unorm;
                cTargets[i].blend = nullptr;
                cTargets[i].writeMask = wgpu::ColorWriteMask::All;

                cBlends[i].color = blendComponent;
                cBlends[i].alpha = blendComponent;
            }
        }
    }

    wgpu::DepthStencilState* ComboRenderPipelineDescriptor::EnableDepthStencil(
        wgpu::TextureFormat format) {
        this->depthStencil = &cDepthStencil;
        cDepthStencil.format = format;
        return &cDepthStencil;
    }

    void ComboRenderPipelineDescriptor::DisableDepthStencil() {
        this->depthStencil = nullptr;
    }

    //////////////////////////////////////////////////////////////////////////
    extern wgpu::VertexFormat g_WGPUVertexFormat[(int)VertexFormat::Count];
    extern wgpu::TextureFormat g_WGPUTextureFormat[(int)TextureFormat::Count];
    extern wgpu::CullMode g_WGPUCullMode[(int)CullMode::Count];
    extern wgpu::FrontFace g_WGPUFrontFace[(int)FrontFace::Count];
    extern wgpu::PrimitiveTopology g_WGPUPrimitiveTopology[(int)PrimitiveType::Count];
    extern wgpu::BlendFactor g_WGPUBlendFactor[(int)BlendFactor::Count];
    extern wgpu::BlendOperation g_WGPUBlendOperation[(int)BlendOperation::Count];

    static wgpu::PipelineLayout MakeBasicPipelineLayout(const wgpu::Device& device,
        const wgpu::BindGroupLayout* bindGroupLayout) 
    {
        wgpu::PipelineLayoutDescriptor descriptor;
        if (bindGroupLayout != nullptr) {
            descriptor.bindGroupLayoutCount = 1;
            descriptor.bindGroupLayouts = bindGroupLayout;
        }
        else {
            descriptor.bindGroupLayoutCount = 0;
            descriptor.bindGroupLayouts = nullptr;
        }
        return device.CreatePipelineLayout(&descriptor);
    }

    RenderPipeline::RenderPipeline(Ref<RenderContex> contex, const PipelineDesc* desc, const char* lable /*= nullptr*/)
    {
        m_Pipeline = CreateRef<wgpu::RenderPipeline>();

        ComboRenderPipelineDescriptor descriptor;
        descriptor.label = lable;

        descriptor.layout = MakeBasicPipelineLayout(contex->device, desc->BindLayout->m_Layout.get());

        // blend states
        descriptor.cBlends[0].color.srcFactor = g_WGPUBlendFactor[(int)desc->Blend.SrcColor];
        descriptor.cBlends[0].color.dstFactor = g_WGPUBlendFactor[(int)desc->Blend.DstColor];
        descriptor.cBlends[0].color.operation = g_WGPUBlendOperation[(int)desc->Blend.OpColor];
        descriptor.cBlends[0].alpha.srcFactor = g_WGPUBlendFactor[(int)desc->Blend.SrcAlpha];
        descriptor.cBlends[0].alpha.dstFactor = g_WGPUBlendFactor[(int)desc->Blend.DstAlpha];
        descriptor.cBlends[0].alpha.operation = g_WGPUBlendOperation[(int)desc->Blend.OpAlpha];

        descriptor.primitive.cullMode = g_WGPUCullMode[(int)desc->Cull];
        descriptor.primitive.frontFace = g_WGPUFrontFace[(int)desc->Front];
        descriptor.primitive.topology = g_WGPUPrimitiveTopology[(int)desc->Primitive];
        descriptor.primitive.stripIndexFormat = wgpu::IndexFormat::Undefined;

        descriptor.vertex.module = *desc->VS->m_Module;
        descriptor.vertex.bufferCount = desc->VLayouts.size();
        int i = 0;
        int attr = 0;
        for (const auto& layout : desc->VLayouts)
        {
            descriptor.cBuffers[i].arrayStride = layout.Stride;
            descriptor.cBuffers[i].attributeCount = layout.AttributeCount;
            descriptor.cBuffers[i].attributes = &descriptor.cAttributes[attr];
            ++i;

            for (uint32_t j = 0; j < layout.AttributeCount; ++j)
            {
                descriptor.cAttributes[attr].format = g_WGPUVertexFormat[(int)layout.Attributes[j].Format];
                descriptor.cAttributes[attr].offset = layout.Attributes[j].Offset;
                descriptor.cAttributes[attr].shaderLocation = layout.Attributes[j].ShaderLocation;
                ++attr;
            }
        }

        descriptor.cFragment.module = *desc->FS->m_Module;
        descriptor.cTargets[0].format = g_WGPUTextureFormat[(int)desc->ColorFormat];
        descriptor.cTargets[0].writeMask = (wgpu::ColorWriteMask)desc->WriteMask;

        if (desc->WriteDepth)
        {
            descriptor.EnableDepthStencil(g_WGPUTextureFormat[(int)desc->DepthFormat]);
        }

        *m_Pipeline = contex->device.CreateRenderPipeline(&descriptor);
    }

    RenderPipeline::~RenderPipeline()
    {

    }


}