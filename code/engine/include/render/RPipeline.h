#pragma once

#include "core/Core.h"
#include "render/RDefines.h"
#include "render/RLayout.h"
#include "render/RShader.h"

namespace rush
{

    struct BlendState
    {
        BlendOperation opColor = BlendOperation::Add;
        BlendFactor srcColor = BlendFactor::SrcColor;
        BlendFactor dstColor = BlendFactor::DstColor;
        BlendOperation opAlpha = BlendOperation::Add;
        BlendFactor srcAlpha = BlendFactor::SrcAlpha;
        BlendFactor dstAlpha = BlendFactor::DstAlpha;
    };

    /// <summary>
    /// PipelineDesc
    /// </summary>
    struct PipelineDesc
    {
        Ref<RShader> vs;
        Ref<RShader> fs;
        Ref<BindingLayout> bindLayout;
        List<VertexLayout> vertexLayouts;
        PrimitiveType primitiveType = PrimitiveType::TriangleList;
        FrontFace frontFace = FrontFace::CCW;
        CullMode cullModel = CullMode::Back;
        bool useBlend = false;
        BlendState blendStates;
        TextureFormat colorFormat = TextureFormat::BGRA8Unorm;
        uint32_t writeMask = ColorWriteMask::Write_All;
        TextureFormat depthStencilFormat = TextureFormat::Depth24PlusStencil8;
        bool depthTest = true;
        bool depthWrite = true;
        DepthCompareFunction depthCompare = DepthCompareFunction::LessEqual;
        bool stencilTest = false;
        bool stencilWrite = false;
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

        RPipeline(Ref<RContex> contex, const PipelineDesc& desc, const char* lable);

        static Ref<RPipeline> Construct(Ref<RContex> contex, const PipelineDesc& desc, const char* lable)
        {
            return std::shared_ptr<RPipeline>(new RPipeline(contex, desc, lable));
        }

        Ref<wgpu::RenderPipeline> m_Pipeline;
    };

}