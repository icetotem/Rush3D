#ifndef Renderer_h__
#define Renderer_h__

#include "core/Core.h"
#include "render/RDefines.h"
#include "render/RLayout.h"
#include "render/RUniform.h"
#include "render/RShader.h"
#include "render/RPipeline.h"
#include "render/RPass.h"
#include "render/RBatch.h"
#include "Window.h"


namespace rush
{

    struct RendererDesc
    {
        RenderBackend backend = RenderBackend::D3D12;
        bool vsync = true;
        bool ssao = false;
        bool bloom = false;
        bool hdr = false;
        bool gamma = false;
        std::vector<std::string> enableToggles;
        std::vector<std::string> disableToggles;
        Vector4 clearColor = Vector4(0.2f, 0.2f, 0.2f, 1.0f);
    };

    struct RenderCaps
    {
        uint32_t maxTextureDimension1D;
        uint32_t maxTextureDimension2D;
        uint32_t maxTextureDimension3D;
        uint32_t maxTextureArrayLayers;
        uint32_t maxBindGroups;
        uint32_t maxBindGroupsPlusVertexBuffers;
        uint32_t maxBindingsPerBindGroup;
        uint32_t maxDynamicUniformBuffersPerPipelineLayout;
        uint32_t maxDynamicStorageBuffersPerPipelineLayout;
        uint32_t maxSampledTexturesPerShaderStage;
        uint32_t maxSamplersPerShaderStage;
        uint32_t maxStorageBuffersPerShaderStage;
        uint32_t maxStorageTexturesPerShaderStage;
        uint32_t maxUniformBuffersPerShaderStage;
        uint64_t maxUniformBufferBindingSize;
        uint64_t maxStorageBufferBindingSize;
        uint32_t minUniformBufferOffsetAlignment;
        uint32_t minStorageBufferOffsetAlignment;
        uint32_t maxVertexBuffers;
        uint64_t maxBufferSize;
        uint32_t maxVertexAttributes;
        uint32_t maxVertexBufferArrayStride;
        uint32_t maxInterStageShaderComponents;
        uint32_t maxInterStageShaderVariables;
        uint32_t maxColorAttachments;
        uint32_t maxColorAttachmentBytesPerSample;
        uint32_t maxComputeWorkgroupStorageSize;
        uint32_t maxComputeInvocationsPerWorkgroup;
        uint32_t maxComputeWorkgroupSizeX;
        uint32_t maxComputeWorkgroupSizeY;
        uint32_t maxComputeWorkgroupSizeZ;
        uint32_t maxComputeWorkgroupsPerDimension;
    };    

    struct RenderPassDesc
    {
        uint32_t width = 128;
        uint32_t height = 128;
        TextureFormat colorFormat = TextureFormat::BGRA8Unorm;
        TextureFormat depthStencilFormat = TextureFormat::Depth24PlusStencil8;
        Vector4 clearColor = Vector4(0.2f, 0.2f, 0.2f, 1.0f);
        float clearDepth = 1.0f;
        bool useDepthStencil = true;
    };

    struct RScreenQuad
    {
        Ref<RPipeline> pipeline;
        Ref<RBindGroup> bindGroup;
    };

    /// <summary>
    /// Renderer
    /// </summary>
    class Renderer
    {
    public:
        ~Renderer() = default;

        uint32_t GetWidth() const { return m_Width; }
        uint32_t GetHeight() const { return m_Height; }

        Ref<RShader> CreateShader(const char* code, ShaderStage type, const char* lable = nullptr);

        Ref<RVertexBuffer> CreateVertexBuffer(uint32_t stride, uint64_t size, const char* lable = nullptr);

        Ref<RIndexBuffer> CreateIndexBuffer(uint64_t count, bool use32bits, const char* lable = nullptr);

        Ref<BindingLayout> CreateBindingLayout(std::initializer_list<BindingLayoutHelper> entriesInitializer, const char* lable = nullptr);

        Ref<RBindGroup> CreateBindGroup(Ref<BindingLayout> layout, std::initializer_list<BindingInitializationHelper> entriesInitializer, const char* lable = nullptr);

        Ref<RUniformBuffer> CreateUniformBuffer(uint64_t size, BufferUsage usage, const char* lable = nullptr);

        Ref<RPipeline> CreatePipeline(const PipelineDesc& pipeDesc, const char* lable = nullptr);

        Ref<RSampler> CreateSampler(const char* lable = nullptr);

        Ref<RPass> CreateRenderPass(const RenderPassDesc& desc, const char* lable = nullptr);

        Ref<RScreenQuad> CreateScreenQuad(Ref<RShader> fs, Ref<RBindGroup> bindGroup);

        void BeginDraw(const Vector4& viewport);

        void DrawOffScreenPass(Ref<RPass> renderPass, Ref<RContent> content);
        void DrawOffScreenQuad(Ref<RPass> renderPass, Ref<RScreenQuad> sQuad);
        void DrawFinalScreenQuad(Ref<RScreenQuad> sQuad);
        void DrawFinalPass(Ref<RContent> content);

        void EndDraw();

    protected:
        friend class Engine;

        static Ref<Renderer> Construct(Ref<Window> window, const RendererDesc* rendererDesc)
        {
            return std::shared_ptr<Renderer>(new Renderer(window, rendererDesc));
        }

        Renderer(Ref<Window> window, const RendererDesc* rendererDesc);

        void CreateAdapter(const RendererDesc* rendererDesc);
        void InitWGPU(const RendererDesc* rendererDesc);

        void GatherCaps();


    protected:
        Ref<RContex> m_Contex;
        Vector4 m_Viewport = { 0.0f, 0.0f, 1.0f, 1.0f };
        RenderCaps m_Caps;
        uint32_t m_Width;
        uint32_t m_Height;
        Vector4 m_ClearColor;
        Ref<Window> m_Window;

        Ref<RShader> m_QuadVS;
        Ref<RVertexBuffer> m_QuadVB;
    };

    inline Ref<RShader> Renderer::CreateShader(const char* code, ShaderStage type, const char* lable/* = nullptr*/)
    {
        return RShader::Construct(m_Contex, type, code, lable);
    }

    inline Ref<RVertexBuffer> Renderer::CreateVertexBuffer(uint32_t stride, uint64_t size, const char* lable/* = nullptr*/)
    {
        return RVertexBuffer::Construct(m_Contex, stride, size, lable);
    }

    inline Ref<RIndexBuffer> Renderer::CreateIndexBuffer(uint64_t count, bool use32bits, const char* lable/* = nullptr*/)
    {
        return RIndexBuffer::Construct(m_Contex, count, use32bits, lable);
    }

    inline Ref<BindingLayout> Renderer::CreateBindingLayout(std::initializer_list<BindingLayoutHelper> entriesInitializer, const char* lable/* = nullptr*/)
    {
        return BindingLayout::Construct(m_Contex, entriesInitializer, lable);
    }

    inline Ref<RPipeline> Renderer::CreatePipeline(const PipelineDesc& pipeDesc, const char* lable/* = nullptr*/)
    {
        return RPipeline::Construct(m_Contex, pipeDesc, lable);
    }

    inline Ref<RSampler> Renderer::CreateSampler(const char* lable /*= nullptr*/)
    {
        return RSampler::Construct(m_Contex, lable);
    }

    inline Ref<RBindGroup> Renderer::CreateBindGroup(Ref<BindingLayout> layout, std::initializer_list<BindingInitializationHelper> entriesInitializer, const char* lable/* = nullptr*/)
    {
        return RBindGroup::Construct(m_Contex, layout, entriesInitializer, lable);
    }

    inline Ref<RUniformBuffer> Renderer::CreateUniformBuffer(uint64_t size, BufferUsage usage, const char* lable /*= nullptr*/)
    {
        return RUniformBuffer::Construct(m_Contex, usage, size, lable);
    }

    inline Ref<RPass> Renderer::CreateRenderPass(const RenderPassDesc& desc, const char* lable/* = nullptr*/)
    {
        return RPass::Construct(m_Contex, desc.width, desc.height, desc.colorFormat, desc.depthStencilFormat, desc.clearColor, desc.clearDepth, desc.useDepthStencil, lable);
    }

}

#endif // Renderer_h__
