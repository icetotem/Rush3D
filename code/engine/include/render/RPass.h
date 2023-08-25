#pragma once

#include "render/RDefines.h"

namespace rush
{

    class RPass
    {
    public:
        ~RPass() = default;

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

        Ref<wgpu::Texture> m_ColorTexture;
        //Ref<wgpu::TextureView> m_ColorView;
        Ref<wgpu::Texture> m_DepthStencilTexture;
        //Ref<wgpu::TextureView> m_DepthStencilView;
        Ref<wgpu::RenderPassDescriptor> m_RenderPassDesc;
        Ref<wgpu::RenderPassColorAttachment> m_ColorAttachment;
        Ref<wgpu::RenderPassDepthStencilAttachment> m_DepthStencilAttachment;
    };

}

