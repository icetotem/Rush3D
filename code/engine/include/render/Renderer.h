#ifndef Renderer_h__
#define Renderer_h__

#include "core/Core.h"
#include "render/RDefines.h"
#include "render/RenderQueue.h"
#include "Window.h"
#include <optional>
#include "RSurface.h"
#include "RShader.h"
#include "RUniform.h"

namespace rush
{

    class RTexture;

    struct FrameData
    {
        Matrix4 view;
        Matrix4 proj;
    };

    struct FrameBufferAttachment
    {
        String texture;
        TextureFormat format;
        //Vector2 scale = { 1.0f, 1.0f };
        Vector4 clearColor = Vector4(0.2f, 0.2f, 0.2f, 1.0f);
    };

    struct FrameBuffer
    {
        std::string lable;

        std::vector<FrameBufferAttachment> colorAttachment;
        std::optional<String> depthStencilTexture;
        std::optional<TextureFormat> depthStencilFormat;
        //std::optional<Vector2> scale;
        std::optional<float> clearDepth;
        std::optional<float> clearStencil;
    };

    struct FrameTexture
    {
        Ref<RTexture> texture;
        TextureFormat format;
        Vector2 scale = { 1.0f, 1.0f };
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
        TextureFormat GetFGTextureFormat(const StringView& name);
        Vector2 GetFGTextureScale(const StringView& name);

        const RBindGroup& GetFrameDataGroup() const { return m_FrameDataGroup; }

    protected:
        friend class Engine;

        static Ref<Renderer> Construct(uint32_t width, uint32_t height)
        {
            return std::shared_ptr<Renderer>(new Renderer(width, height));
        }

        Renderer(uint32_t width, uint32_t height);

        void CreateFullScreenQuad();

        void RegisterFGTexture(const StringView& name, TextureFormat format, Vector2 viewScale);

        void BeginDraw(Ref<RSurface> surface);
        void DrawScene(Ref<RenderQueue> renderQueue, const FrameBuffer& outputBuffers);
        void DrawQuad(Ref<RMaterial> material, const FrameBuffer& outputBuffers);
        void EndDraw();

    protected:
        wgpu::CommandEncoder encoder;
        wgpu::CommandBuffer commands;
        uint32_t m_Width;
        uint32_t m_Height;
        Ref<RGeometry> m_ScreenQuadGeo;
        Ref<RSurface> m_Surface;
        // frame graph resources
        HMap<String, FrameTexture> m_RenderTextures;
        Ref<RMaterial> m_FinalPassMat;

        // uniforms
        Ref<RBuffer> m_FrameDataBuffer;
        FrameData m_FrameData;
        RBindGroup m_FrameDataGroup;
    };

}

#endif // Renderer_h__
