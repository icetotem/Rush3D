#ifndef Renderer_h__
#define Renderer_h__

#include "core/Core.h"
#include "render/RDefines.h"
#include "render/RenderQueue.h"
#include "render/RTexture.h"
#include "render/RMaterial.h"
#include "Window.h"
#include <optional>
#include "RSurface.h"

namespace rush
{

    struct FrameBufferAttachment
    {
        RTexture* texture = nullptr;
        uint32_t mipLevel = 0;
        Vector4 clearColor;
    };

    struct FrameBufferInfo
    {
        std::string lable;
        uint32_t width = 0;
        uint32_t height = 0;
        std::vector<FrameBufferAttachment> colorAttachment;
        std::optional<FrameBufferAttachment> depthAttachment;
        std::optional<float> clearDepth;
    };

    /// <summary>
    /// Renderer
    /// </summary>
    class Renderer
    {
    public:
        virtual ~Renderer() = default;

        uint32_t GetWidth() const { return m_Width; }
        uint32_t GetHeight() const { return m_Height; }  

        void BeginDraw();
        void DrawScene(Ref<RenderQueue> renderQueue, const FrameBufferInfo& frameBuffer);
        void DrawQuad(const FrameBufferInfo& frameBuffer, Ref<RMaterialInst> material);
        void DrawSurface(RSurface& surface, const StringView& renderTexture);
        void EndDraw();

        void Resize(uint32_t width, uint32_t height);

        Ref<RTexture> GetRenderTexture(const StringView& name) const;

    protected:
        friend class Engine;

        static Ref<Renderer> Construct(uint32_t width, uint32_t height)
        {
            return std::shared_ptr<Renderer>(new Renderer(width, height));
        }

        Renderer(uint32_t width, uint32_t height);

        void CreateFullScreenQuad();


    protected:
        wgpu::CommandEncoder encoder;
        wgpu::CommandBuffer commands;
        uint32_t m_Width;
        uint32_t m_Height;
        Ref<RShader> m_QuadFSFinal;
        Ref<RShader> m_QuadVS;
        Ref<RVertexBuffer> m_QuadVB;
        Ref<RPipeline> m_FinalPassPipeline;
        Ref<RBindGroup> m_FinalPassBindGroup;
        HMap<String, Ref<RTexture>> m_RenderTextures;
    };

}

#endif // Renderer_h__
