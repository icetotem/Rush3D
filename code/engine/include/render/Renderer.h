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

    struct InstanceData
    {
        uint32_t transformOffset;
    };

    struct InstanceBindGroup
    {
        Ref<RBuffer> buffer;
        InstanceData data;
        Ref<RBindGroup> group;
    };

    struct LightData
    {
        Vector4 position = {0, 0, 0, 0};
        Vector4 direction = {0, 0, 0, 0};
        Vector4 color = {0, 0, 0, 0};
        uint32_t type = 0;
        float innerConeAngle = 0;
        float outerConeAngle = 0;
        float padding = 0;
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

    struct SceneGraphTexture
    {
        bool isRT = true;
        Ref<RTexture> texture;
        TextureFormat format;
        TextureViewDimension dim;
        Vector2 size = { 1.0f, 1.0f };
    };

#define MAX_NUM_CASCADES 4

    struct Cascades
    {
        Vector4 splitDepth;
        Matrix4 viewProjMatrices[MAX_NUM_CASCADES];
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
        TextureViewDimension GetFGTextureViewDim(const StringView& name);
        Ref<RBuffer> GetFGBuffer(const StringView& name);

        void GenAttachment(FrameBufferAttachment& inout, const StringView& name, const Vector4& clearColor = Vector4(0.2f, 0.2f, 0.2f, 1.0f))
        {
            inout.texture = String(name);
            inout.format = GetFGTextureFormat(name);
            inout.clearColor = clearColor;
        }

        const Ref<RBindGroup> GetFrameDataGroup() const { return m_FrameDataGroup; }
        const Ref<RBindGroup> GetTransformDataGroup() const { return m_TransformDataGroup; }
        const Ref<RBindGroup> GetInstanceDataGroup() const { return m_InstanceBindGroups[0].group; }

    protected:
        friend class Engine;

        static Ref<Renderer> Construct(uint32_t width, uint32_t height)
        {
            return std::shared_ptr<Renderer>(new Renderer(width, height));
        }

        Renderer(uint32_t width, uint32_t height);

        void CreateFullScreenQuad();

        void RegisterFGRenderTexture(const StringView& name, TextureFormat format, TextureViewDimension dim, Vector2 viewScale);
        void RegisterFGDynamicTexture(const StringView& name, TextureFormat format, TextureViewDimension dim, Vector2 size);
        void RegisterFGBuffer(const StringView& name, Ref<RBuffer> buffer);

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
        HMap<String, SceneGraphTexture> m_RenderTextures;
        Ref<RMaterial> m_ToneMappingPassMat;
        Ref<RMaterial> m_FinalPassMat;
        Ref<RMaterial> m_DeferredLightingPassMat;
        HMap<String, Ref<RBuffer>> m_FGBuffers;

        // FrameData
        Ref<RBuffer> m_FrameDataBuffer;
        FrameData m_FrameData;
        Ref<RBindGroup> m_FrameDataGroup;

        // TransformData
        Ref<RBuffer> m_TransformBuffer;
        Ref<RBuffer> m_NormalTransformBuffer;
        DArray<Matrix4> m_Transforms;
        DArray<Matrix4> m_NormalTransforms;
        Ref<RBindGroup> m_TransformDataGroup;

        // for shadow map
        Cascades m_Cascades;
        Ref<RBuffer> m_CascadesBuffer;

        // instance data
        DArray<InstanceBindGroup> m_InstanceBindGroups;

        // Light Data
        Ref<RBuffer> m_LightsCountBuffer;
        Ref<RBuffer> m_LightsBuffer;
        DArray<LightData> m_LightsData;
    };

}

#endif // Renderer_h__
