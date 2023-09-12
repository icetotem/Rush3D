#ifndef RPass_h__
#define RPass_h__

#include "render/RDefines.h"
#include "render/RTexture.h"

namespace rush
{

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

    class RPass
    {
    public:
        RPass(const RenderPassDesc& desc, const char* lable = nullptr);

        ~RPass() = default;

        Ref<RTexture> GetColorTexture() const { return m_ColorTexture; }

        Ref<RTexture> GetDepthStencilTexture() const { return m_DepthStencilTexture; }

    private:
        friend class RenderContex;

        uint32_t m_FrameBufferWidth;
        uint32_t m_FrameBufferHeight;

        Ref<RTexture> m_ColorTexture;
        Ref<RTexture> m_DepthStencilTexture;
        wgpu::RenderPassDescriptor m_RenderPassDesc;
        wgpu::RenderPassColorAttachment m_ColorAttachment;
        wgpu::RenderPassDepthStencilAttachment m_DepthStencilAttachment;
    };

}

#endif // RPass_h__
