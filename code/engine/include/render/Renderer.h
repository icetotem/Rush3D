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
#include "RShader.h"

namespace rush
{

    struct FrameBufferAttachment
    {
        String texture;
        Vector4 clearColor = Vector4(0.2f, 0.2f, 0.2f, 1.0f);
    };

    struct FrameBuffer
    {
        std::string lable;

        std::vector<FrameBufferAttachment> colorAttachment;
        std::optional<String> depthStencilTexture;
        std::optional<float> clearDepth;
        std::optional<float> clearStencil;
    };

    struct FrameTexture
    {
        Ref<RTexture> texture;
        TextureFormat format;
        float widthScale = 1.0f;
        float heightScale = 1.0f;
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

        void Render(Ref<RenderQueue> renderQueue, Ref<RSurface> surface);

        void Resize(uint32_t width, uint32_t height);

        Ref<RTexture> GetFGTexture(const StringView& name);

    protected:
        friend class Engine;

        static Ref<Renderer> Construct(uint32_t width, uint32_t height)
        {
            return std::shared_ptr<Renderer>(new Renderer(width, height));
        }

        Renderer(uint32_t width, uint32_t height);

        void CreateFullScreenQuad();

        void RegisterFGTexture(const StringView& name, TextureFormat format, float widthScale, float heightScale);

        void BeginDraw(Ref<RSurface> surface);
        void DrawScene(Ref<RenderQueue> renderQueue, const FrameBuffer& outputBuffers);
        void DrawQuad(Ref<RMaterial> material, const FrameBuffer& outputBuffers);
        void EndDraw();

    protected:
        wgpu::CommandEncoder encoder;
        wgpu::CommandBuffer commands;
        uint32_t m_Width;
        uint32_t m_Height;
//         Ref<RShader> m_QuadFSFinal;
//         Ref<RShader> m_QuadVS;
//         Ref<RVertexBuffer> m_QuadVB;
        Ref<RGeometry> m_ScreenQuadGeo;
//         Ref<RPipeline> m_FinalPassPipeline;
//         Ref<RBindGroup> m_FinalPassBindGroup;
        Ref<RSurface> m_Surface;
        // frame graph resources
        HMap<String, FrameTexture> m_RenderTextures;

    };

}

#endif // Renderer_h__
