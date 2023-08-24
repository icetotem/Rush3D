#pragma once

#include "core/Core.h"
#include "render/RenderDefines.h"
#include "render/Layout.h"

namespace rush
{

    struct BlendState
    {
        BlendOperation OpColor = BlendOperation::Add;
        BlendFactor SrcColor = BlendFactor::SrcColor;
        BlendFactor DstColor = BlendFactor::DstColor;
        BlendOperation OpAlpha = BlendOperation::Add;
        BlendFactor SrcAlpha = BlendFactor::SrcAlpha;
        BlendFactor DstAlpha = BlendFactor::DstAlpha;
    };

    class Shader;

    /// <summary>
    /// PipelineDesc
    /// </summary>
    struct PipelineDesc
    {
        Ref<Shader> VS;
        Ref<Shader> FS;
        Ref<BindingLayout> BindLayout;
        List<VertexLayout> VLayouts;
        PrimitiveType Primitive = PrimitiveType::TriangleList;
        BlendState Blend;
        FrontFace Front = FrontFace::CCW;
        CullMode Cull = CullMode::Back;
        TextureFormat ColorFormat = TextureFormat::RGBA8Unorm;
        TextureFormat DepthFormat = TextureFormat::Depth24PlusStencil8;
        uint32_t WriteMask = ColorWriteMask::Write_All;
        bool WriteDepth = true;
        DepthCompareFunction DepthCompare = DepthCompareFunction::Less;
    };



    /// <summary>
    /// RPipeline
    /// </summary>
    class RenderPipeline
    {
    public:
        ~RenderPipeline();

    protected:
        friend class Renderer;

        RenderPipeline(Ref<RenderContex> contex, const PipelineDesc* desc, const char* lable = nullptr);

        static Ref<RenderPipeline> Construct(Ref<RenderContex> contex, const PipelineDesc* desc, const char* lable)
        {
            return std::shared_ptr<RenderPipeline>(new RenderPipeline(contex, desc, lable));
        }

        Ref<wgpu::RenderPipeline> m_Pipeline;
    };

}