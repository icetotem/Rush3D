#include "stdafx.h"

#include <dawn/webgpu_cpp.h>

#include "render/RPass.h"
#include "RContex.h"

namespace rush
{
    extern wgpu::TextureFormat g_WGPUTextureFormat[(int)TextureFormat::Count];

    //////////////////////////////////////////////////////////////////////////

    RPass::RPass(const RenderPassDesc& desc, const char* lable)
    {
        m_FrameBufferWidth = desc.width;
        m_FrameBufferHeight = desc.height;

        wgpu::TextureDescriptor colorTexDesc;
        colorTexDesc.dimension = wgpu::TextureDimension::e2D;
        colorTexDesc.size.width = m_FrameBufferWidth;
        colorTexDesc.size.height = m_FrameBufferHeight;
        colorTexDesc.size.depthOrArrayLayers = 1;
        colorTexDesc.format = g_WGPUTextureFormat[(int)desc.colorFormat];
        colorTexDesc.mipLevelCount = 1;
        colorTexDesc.usage = wgpu::TextureUsage::RenderAttachment | wgpu::TextureUsage::TextureBinding;
        m_ColorTexture = CreateRef<RTexture>(m_FrameBufferWidth, m_FrameBufferHeight, desc.colorFormat, 1, 1, lable);
        m_ColorTexture->m_Texture = CreateRef<wgpu::Texture>(RContex::device.CreateTexture(&colorTexDesc));

        m_RenderPassDesc = CreateRef<wgpu::RenderPassDescriptor>();
        m_ColorAttachment = CreateRef<wgpu::RenderPassColorAttachment>();
        m_ColorAttachment->view = m_ColorTexture->GetTexture()->CreateView();
        m_ColorAttachment->resolveTarget = nullptr;
        m_ColorAttachment->loadOp = wgpu::LoadOp::Clear;
        m_ColorAttachment->storeOp = wgpu::StoreOp::Store;
        m_ColorAttachment->clearValue = { desc.clearColor.r, desc.clearColor.g, desc.clearColor.b, desc.clearColor.a };
        m_RenderPassDesc->colorAttachmentCount = 1;
        m_RenderPassDesc->colorAttachments = m_ColorAttachment.get();

        if (desc.useDepthStencil)
        {
            wgpu::TextureDescriptor depthStencilTexDesc;
            depthStencilTexDesc.dimension = wgpu::TextureDimension::e2D;
            depthStencilTexDesc.size.width = m_FrameBufferWidth;
            depthStencilTexDesc.size.height = m_FrameBufferHeight;
            depthStencilTexDesc.size.depthOrArrayLayers = 1;
            depthStencilTexDesc.format = g_WGPUTextureFormat[(int)desc.depthStencilFormat];
            depthStencilTexDesc.mipLevelCount = 1;
            depthStencilTexDesc.usage = wgpu::TextureUsage::RenderAttachment | wgpu::TextureUsage::TextureBinding;

            m_DepthStencilTexture = CreateRef<RTexture>(m_FrameBufferWidth, m_FrameBufferHeight, desc.depthStencilFormat, 1, 1, lable);
            m_DepthStencilTexture->m_Texture = CreateRef<wgpu::Texture>(RContex::device.CreateTexture(&depthStencilTexDesc));

            m_DepthStencilAttachment = CreateRef<wgpu::RenderPassDepthStencilAttachment>();
            m_DepthStencilAttachment->view = m_DepthStencilTexture->GetTexture()->CreateView();
            m_DepthStencilAttachment->depthReadOnly = false;
            m_DepthStencilAttachment->stencilReadOnly = false;
            m_DepthStencilAttachment->depthClearValue = desc.clearDepth;
            m_DepthStencilAttachment->stencilClearValue = 0;
            m_DepthStencilAttachment->depthLoadOp = wgpu::LoadOp::Clear;
            m_DepthStencilAttachment->depthStoreOp = wgpu::StoreOp::Store;
            m_DepthStencilAttachment->stencilLoadOp = wgpu::LoadOp::Clear;
            m_DepthStencilAttachment->stencilStoreOp = wgpu::StoreOp::Store;
            m_RenderPassDesc->depthStencilAttachment = m_DepthStencilAttachment.get();
        }
    }

}
