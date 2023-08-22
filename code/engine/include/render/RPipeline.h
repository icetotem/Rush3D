#pragma once

#include "core/Core.h"
#include "render/RenderDefines.h"
#include "render/RBuffer.h"

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
        List<VertexLayout> VLayouts;
        PrimitiveType Primitive = PrimitiveType::TriangleList;
        BlendState Blend;
        FrontFace Front = FrontFace::CCW;
        CullMode Cull = CullMode::Back;
        TextureFormat WriteFormat = TextureFormat::RGBA8Uint;
        uint32_t WriteMask = ColorWriteMask::Write_All;
        bool WriteDepth = true;
        TextureFormat DepthFormat = TextureFormat::Depth24Plus;
        DepthCompareFunction DepthCompare = DepthCompareFunction::Less;
    };



    /// <summary>
    /// RPipeline
    /// </summary>
    class RPipeline
    {
    public:
        RPipeline();
        ~RPipeline();

    protected:
        friend class Renderer;

        WGPURenderPipelineImpl* m_Pipeline = nullptr;
    };

}