#pragma once

#include "core/Core.h"
#include "render/RenderDefines.h"
#include "window.h"

namespace rush
{

    /// <summary>
    /// RenderPassDesc
    /// </summary>
    struct RenderPassDesc
    {
        std::string DebugName;
        TextureFormat Format = TextureFormat::RGBA8Snorm;
        Vector4 ClearColor = Vector4(0.2f, 0.2f, 0.2f, 1.0f);
        bool IsSwapBuffer = false;
        int Width = 0;
        int Height = 0;
    };

    /// <summary>
    /// RenderPass
    /// </summary>
    class RenderPass
    {
    public:
        RenderPass();
        ~RenderPass();

        Ref<RTexture> GetRenderTarget() const { return m_RenderTarget; }

    protected:
        Ref<RTexture> m_RenderTarget;
        friend class Renderer;
        struct WebGpuImpl;
        Ref<WebGpuImpl> m_Impl;
        bool IsSwapBuffer = false;
        Vector4 ClearColor;
    };

    struct RendererDesc
    {
        uint32_t msaa = 4;
        bool ssao = false;
        bool bloom = false;
        bool hdr = false;
        bool gamma = false;
    };

    struct RenderCaps
    {
        uint32_t maxTextureDimension1D;
        uint32_t maxTextureDimension2D;
        uint32_t maxTextureDimension3D;
        uint32_t maxTextureArrayLayers;
        uint32_t maxBindGroups;
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
        uint32_t maxComputeWorkgroupStorageSize;
        uint32_t maxComputeInvocationsPerWorkgroup;
        uint32_t maxComputeWorkgroupSizeX;
        uint32_t maxComputeWorkgroupSizeY;
        uint32_t maxComputeWorkgroupSizeZ;
        uint32_t maxComputeWorkgroupsPerDimension;
    };    

    class RenderBatch
    {
    public:
        Ref<RPipeline> Pipeline;
        Ref<UniformBuffer> Uniforms;
        List<Ref<RBuffer>> VBList;
        Ref<RBuffer> IB;
        uint32_t InstanceCount = 1;
        uint32_t FirstIndex = 0;
        uint32_t FirstVertex = 0;
    };

    class RenderContent
    {
    public:

        friend class Renderer;
        List<Ref<RenderBatch>> m_Batches;
    };

    class Window;

    /// <summary>
    /// Renderer
    /// </summary>
    class Renderer
    {
    public:
        Renderer(Ref<Window> window, const RendererDesc* rendererDesc);
        ~Renderer();

        Ref<Shader> CreateShader(const char* code, const char* debugName = nullptr);

        Ref<RBuffer> CreateVertexBuffer(const void* data, size_t size, uint32_t stride);

        Ref<RBuffer> CreateIndexBuffer(const void* data, size_t size, uint32_t stride);

        Ref<UniformBuffer> CreateUniformBuffer(const void* data, size_t size, uint32_t shaderVisibility);

        void WriteUniformBuffer(Ref<UniformBuffer> buffer, size_t offset, const void* data, size_t size);

        Ref<RPipeline> CreatePipeline(const PipelineDesc* pipeDesc);

        //Ref<RTexture> CreateTexture();

        Ref<RenderPass> CreateRenderPass(const RenderPassDesc* renderPassDesc);

        void RenderOnePass(Ref<RenderPass> renderPass, Ref<RenderContent> content);

        void SwapBuffers();

    protected:
        struct WebGpuImpl;
        Ref<WebGpuImpl> m_Impl;

        RenderCaps m_Caps;
        WindowHandle m_WindowHandle;
        uint32_t m_Width;
        uint32_t m_Height;
        uint32_t m_Msaa;

        Ref<Window> m_Window;
    };


}