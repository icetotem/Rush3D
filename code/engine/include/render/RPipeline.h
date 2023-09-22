#ifndef RPipeline_h__
#define RPipeline_h__

#include "core/Core.h"
#include "render/RDefines.h"
#include "render/RLayout.h"
#include "render/RShader.h"

namespace rush
{

    struct BlendState
    {
        BlendOperation opColor = BlendOperation::Add;
        BlendFactor srcColor = BlendFactor::Src;
        BlendFactor dstColor = BlendFactor::Dst;
        BlendOperation opAlpha = BlendOperation::Add;
        BlendFactor srcAlpha = BlendFactor::SrcAlpha;
        BlendFactor dstAlpha = BlendFactor::DstAlpha;
    };

    struct VertexAttribute
    {
        VertexFormat format = VertexFormat::Float32;
        uint64_t offset = 0;
        uint32_t shaderLocation = 0;
    };

    struct VertexLayout
    {
        uint64_t stride = 0;
        uint32_t attributeCount = 0;
        VertexAttribute* attributes = nullptr;
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
        PrimitiveTopology primitiveType = PrimitiveTopology::TriangleList;
        FrontFace frontFace = FrontFace::CCW;
        CullMode cullModel = CullMode::Back;
        bool useBlend = false;
        BlendState blendStates;
        TextureFormat colorFormat = TextureFormat::BGRA8Unorm;
        ColorWriteMask writeMask = ColorWriteMask::All;
        TextureFormat depthStencilFormat = TextureFormat::Depth24PlusStencil8;
        bool depthTest = true;
        bool depthWrite = true;
        CompareFunction depthCompare = CompareFunction::LessEqual;
        bool stencilTest = false;
        bool stencilWrite = false;
    };


    /// <summary>
    /// RPipeline
    /// </summary>
    class RPipeline
    {
    public:
        RPipeline(const PipelineDesc& desc, const char* lable = nullptr);

        ~RPipeline() = default;

        const wgpu::RenderPipeline& GetPipeline() const { return m_Pipeline; }

    protected:
        friend class Renderer;

        wgpu::RenderPipeline m_Pipeline;
    };


    class RGeometry;
    class RMaterial;

    wgpu::RenderPipeline GetPipeline(Ref<RGeometry>, Ref<RMaterial> material);

}

#endif // RPipeline_h__
