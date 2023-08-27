#pragma once

#include "render/RDefines.h"
#include "render/RTexture.h"

namespace rush
{

    class RPass
    {
    public:
        ~RPass() = default;

        Ref<RTexture> GetColorTexture() const { return m_ColorTexture; }

        Ref<RTexture> GetDepthStencilTexture() const { return m_DepthStencilTexture; }

    private:
        RPass(Ref<RContex> renderContex, uint32_t width, uint32_t height, uint32_t msaa, TextureFormat colorFormat, TextureFormat depthStencilFormat, const Vector4& clearColor, float clearDepth, bool withDepth, const char* lable);

        static Ref<RPass> Construct(Ref<RContex> renderContex, uint32_t width, uint32_t height, uint32_t msaa, TextureFormat color, TextureFormat depthStencil, const Vector4& clearColor, float clearDepth, bool withDepth, const char* lable)
        {
            return std::shared_ptr<RPass>(new RPass(renderContex, width, height, msaa, color, depthStencil, clearColor, clearDepth, withDepth, lable));
        }

    private:
        friend class Renderer;

        uint32_t m_FrameBufferWidth;
        uint32_t m_FrameBufferHeight;

        Ref<RTexture> m_ColorTexture;
        Ref<RTexture> m_DepthStencilTexture;
        Ref<wgpu::RenderPassDescriptor> m_RenderPassDesc;
        Ref<wgpu::RenderPassColorAttachment> m_ColorAttachment;
        Ref<wgpu::RenderPassDepthStencilAttachment> m_DepthStencilAttachment;
    };

}

