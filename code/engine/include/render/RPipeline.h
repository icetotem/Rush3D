#pragma once

#include "core/Core.h"
#include "render/RDefines.h"
#include "render/RLayout.h"
#include "render/RShader.h"

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

    /// <summary>
    /// PipelineDesc
    /// </summary>
    struct PipelineDesc
    {
        Ref<RShader> VS;
        Ref<RShader> FS;
        Ref<BindingLayout> BindLayout;
        List<VertexLayout> VLayouts;
        PrimitiveType Primitive = PrimitiveType::TriangleList;
        BlendState Blend;
        FrontFace Front = FrontFace::CCW;
        CullMode Cull = CullMode::Back;
        TextureFormat ColorFormat = TextureFormat::BGRA8Unorm;
        TextureFormat DepthFormat = TextureFormat::Depth24PlusStencil8;
        uint32_t WriteMask = ColorWriteMask::Write_All;
        bool DepthTest = true;
        bool DepthWrite = true;
        DepthCompareFunction DepthCompare = DepthCompareFunction::LessEqual;
        bool StencilTest = false;
        bool StencilWrite = false;
    };



    /// <summary>
    /// RPipeline
    /// </summary>
    class RPipeline
    {
    public:
        ~RPipeline() = default;

    protected:
        friend class Renderer;

        RPipeline(Ref<RContex> contex, const PipelineDesc* desc, const char* lable);

        static Ref<RPipeline> Construct(Ref<RContex> contex, const PipelineDesc* desc, const char* lable)
        {
            return std::shared_ptr<RPipeline>(new RPipeline(contex, desc, lable));
        }

        Ref<wgpu::RenderPipeline> m_Pipeline;
    };

}