#ifndef Renderer_h__
#define Renderer_h__

#include "core/Core.h"
#include "render/RDefines.h"
#include "render/RBatch.h"
#include "Window.h"
#include "RPass.h"

namespace rush
{

    struct RendererDesc
    {
        bool vsync = true;
        bool ssao = false;
        bool bloom = false;
        bool hdr = false;
        bool gamma = false;
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

        static void Init(RenderBackend backend);
        static void Shutdown();

        uint32_t GetWidth() const { return m_Width; }
        uint32_t GetHeight() const { return m_Height; }

        Ref<RScreenQuad> CreateScreenQuad(Ref<RShader> fs, Ref<RBindGroup> bindGroup);

        void BeginDraw(const Vector4& viewport);
        void DrawOffScreenPass(Ref<RPass> renderPass, Ref<RContent> content);
        void DrawOffScreenQuad(Ref<RPass> renderPass, Ref<RScreenQuad> sQuad);
        void DrawFinalScreenQuad(Ref<RScreenQuad> sQuad);
        void DrawFinalPass(Ref<RContent> content);
        void EndDraw();

    protected:
        friend class Engine;

        static Ref<Renderer> Construct(Ref<Window> window, const RendererDesc& rendererDesc)
        {
            return std::shared_ptr<Renderer>(new Renderer(window, rendererDesc));
        }

        Renderer(Ref<Window> window, const RendererDesc& rendererDesc);

        void CreateSurface(const RendererDesc& rendererDesc);

        void GatherCaps();

        void Present();

    protected:
        friend class Window;
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

}

#endif // Renderer_h__
