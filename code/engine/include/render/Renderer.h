#pragma once

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

        Ref<RShader> CreateShader(const char* code, ShaderStage type, const char* lable = nullptr);

        Ref<RVertexBuffer> CreateVertexBuffer(uint32_t stride, uint64_t size, const char* lable = nullptr);

        Ref<RIndexBuffer> CreateIndexBuffer(uint64_t count, bool use32bits, const char* lable = nullptr);

        Ref<BindingLayout> CreateBindingLayout(std::initializer_list<BindingLayoutHelper> entriesInitializer, const char* lable = nullptr);

        Ref<RBindGroup> CreateBindGroup(Ref<BindingLayout> layout, std::initializer_list<BindingInitializationHelper> entriesInitializer, const char* lable = nullptr);

        Ref<RUniformBuffer> CreateUniformBuffer(uint64_t size, BufferUsage usage, const char* lable = nullptr);

        Ref<RPipeline> CreatePipeline(const PipelineDesc* pipeDesc, const char* lable = nullptr);

        Ref<RSampler> CreateSampler(const char* lable = nullptr);

        Ref<RPass> CreateRenderPass(const RenderPassDesc* desc, const char* lable = nullptr);

        Ref<RScreenQuad> CreateScreenQuad(Ref<RShader> fs, Ref<RBindGroup> bindGroup);

        void BeginDraw();

        void DrawOffScreenPass(Ref<RPass> renderPass, Ref<RContent> content);
        void DrawOffScreenQuad(Ref<RPass> renderPass, Ref<RScreenQuad> sQuad);
        void DrawFinalScreenQuad(Ref<RScreenQuad> sQuad);

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

        RenderCaps m_Caps;
        uint32_t m_Width;
        uint32_t m_Height;
        Vector4 m_ClearColor;
        Ref<Window> m_Window;

        Ref<RShader> m_QuadVS;
        Ref<RVertexBuffer> m_QuadVB;
    };


}