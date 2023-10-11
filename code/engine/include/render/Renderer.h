#ifndef Renderer_h__
#define Renderer_h__

#include <optional>
#include "core/Core.h"
#include "render/RDefines.h"
#include "render/RenderQueue.h"
#include "Window.h"
#include "RSurface.h"

namespace rush
{

    class RTexture;
    class RBindGroup;
    class RBuffer;

    struct CameraData
    {
        Matrix4 projection;
        Matrix4 inversedProjection;
        Matrix4 view;
        Matrix4 inversedView;
        Matrix4 vp;
        float fov;
        float _near;
        float _far;
        float padding;
        // Implicit padding, 4bytes
    };

    struct FrameData
    {
        float time;
        float deltaTime;
        IVector2 resolution;
        CameraData camera;
        uint32_t renderFeatures;
        uint32_t debugFlags;
        float padding[2];
    };

    struct DirectLightData
    {
        Vector3 direction;
        float pointLightCount;
        Vector4 color;
    };

    static constexpr uint32_t kMaxPointLights = 16u;

    struct PointLightData
    {
        Vector3 position;
        float radius;
        Vector3 direction;
        float angle;
        Vector4 color;
    };

    struct FrameBufferAttachment
    {
        String texture;
        TextureFormat format;
        Vector4 clearColor = Vector4(0.2f, 0.2f, 0.2f, 1.0f);
    };

    struct FrameBuffer
    {
        std::string lable;

        std::vector<FrameBufferAttachment> colorAttachment;
        std::optional<String> depthStencilTexture;
        std::optional<TextureFormat> depthStencilFormat;
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

        const Ref<RBindGroup> GetFrameDataGroup() const { return m_FrameDataGroup; }
        const Ref<RBindGroup> GetTransformDataGroup() const { return m_TransformDataGroup; }
        //const Ref<RBindGroup> GetLightDataGroup() const { return m_LightingDataGroup; }

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

        // FrameData
        Ref<RBuffer> m_FrameDataBuffer;
        FrameData m_FrameData;
        Ref<RBindGroup> m_FrameDataGroup;

        // TransformData
        Ref<RBuffer> m_TransformBuffer;
        DArray<Matrix4> m_Transforms;
        Ref<RBindGroup> m_TransformDataGroup;

        // Lighting Data
        Ref<RBuffer> m_DirectionalLightBuffer;
        Ref<RBuffer> m_PointLightsBuffer;
        DirectLightData m_DirectionalLightData;
        DArray<PointLightData> m_PointLightsData;
        Ref<RBindGroup> m_LightingDataGroup;
    };

}

#endif // Renderer_h__
