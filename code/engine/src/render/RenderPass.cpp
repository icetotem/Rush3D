#include "stdafx.h"
#include "render/RenderPass.h"
#include <dawn/webgpu.h>
#include "RenderContex.h"

namespace rush
{
    extern WGPUTextureFormat g_WGPUTextureFormat[(int)TextureFormat::Count];

    //////////////////////////////////////////////////////////////////////////

    RenderPass::RenderPass(Ref<RenderContex> renderContex, const char* lable, uint32_t width, uint32_t height, TextureFormat color, TextureFormat depthStencil, const Vector4& clearColor, float clearDepth, bool withDepth)
    {
        m_Contex = CreateRef<RenderPassContex>();
        m_Contex->render_pass.width = width;
        m_Contex->render_pass.height = height;

        WGPUSamplerDescriptor desc = {
            .label = lable,
                .addressModeU = WGPUAddressMode_ClampToEdge,
                .addressModeV = WGPUAddressMode_ClampToEdge,
                .addressModeW = WGPUAddressMode_ClampToEdge,
                .magFilter = WGPUFilterMode_Linear,
                .minFilter = WGPUFilterMode_Linear,
                .mipmapFilter = WGPUMipmapFilterMode_Linear,
                .lodMinClamp = 0.0f,
                .lodMaxClamp = 1.0f,
                .maxAnisotropy = 1,
        };

        m_Contex->render_pass.sampler = wgpuDeviceCreateSampler(renderContex->device, &desc);

        RUSH_ASSERT(m_Contex->render_pass.sampler != nullptr);

        // Create the texture extent
        WGPUExtent3D texture_extent = {
          .width = width,
          .height = height,
          .depthOrArrayLayers = 1,
        };

        // Color attachment
        {
            WGPUTextureDescriptor texture_desc = {
              .label = lable,
              .usage = WGPUTextureUsage_RenderAttachment | WGPUTextureUsage_TextureBinding,
              .dimension = WGPUTextureDimension_2D,
              .size = texture_extent,
              .format = g_WGPUTextureFormat[(int)color],
              .mipLevelCount = 1,
              .sampleCount = 1,
            };

            m_Contex->frame_buffer.color.texture = wgpuDeviceCreateTexture(renderContex->device, &texture_desc);
            RUSH_ASSERT(m_Contex->frame_buffer.color.texture != NULL);

            // Create the texture view
            WGPUTextureViewDescriptor texture_view_dec = {
                .label = lable,
                .format = texture_desc.format,
                .dimension = WGPUTextureViewDimension_2D,
                .baseMipLevel = 0,
                .mipLevelCount = 1,
                .baseArrayLayer = 0,
                .arrayLayerCount = 1,
            };

            m_Contex->frame_buffer.color.texture_view = wgpuTextureCreateView(m_Contex->frame_buffer.color.texture, &texture_view_dec);
            RUSH_ASSERT(m_Contex->frame_buffer.color.texture_view != NULL);
        }

        // Depth stencil attachment
        if (withDepth)
        {
            WGPUTextureDescriptor texture_desc = {
                .label = lable,
                .usage = WGPUTextureUsage_RenderAttachment | WGPUTextureUsage_CopySrc,
                .dimension = WGPUTextureDimension_2D,
                .size = texture_extent,
                .format = g_WGPUTextureFormat[(int)depthStencil],
                .mipLevelCount = 1,
                .sampleCount = 1,
            };
            m_Contex->frame_buffer.depth.texture = wgpuDeviceCreateTexture(renderContex->device, &texture_desc);
            RUSH_ASSERT(m_Contex->frame_buffer.depth.texture != NULL);

            // Create the texture view
            WGPUTextureViewDescriptor texture_view_dec = {
                .label = lable,
                .format = texture_desc.format,
                .dimension = WGPUTextureViewDimension_2D,
                .baseMipLevel = 0,
                .mipLevelCount = 1,
                .baseArrayLayer = 0,
                .arrayLayerCount = 1,
                .aspect = WGPUTextureAspect_All,
            };
            m_Contex->frame_buffer.depth.texture_view = wgpuTextureCreateView(m_Contex->frame_buffer.depth.texture, &texture_view_dec);
            RUSH_ASSERT(m_Contex->frame_buffer.depth.texture_view != NULL);
        }

        // Create a separate render pass for the offscreen rendering as it may differ
        // from the one used for scene rendering
        // Color attachment
        m_Contex->frame_buffer.render_pass_desc.color_attachment[0] = {
              .view = m_Contex->frame_buffer.color.texture_view,
              .loadOp = WGPULoadOp_Clear,
              .storeOp = WGPUStoreOp_Store,
              .clearValue = {
                .r = clearColor.r,
                .g = clearColor.g,
                .b = clearColor.b,
                .a = clearColor.a,
              },
        };

        if (withDepth)
        {
            // Depth stencil attachment
            m_Contex->frame_buffer.render_pass_desc.depth_stencil_attachment = {
                  .view = m_Contex->frame_buffer.depth.texture_view,
                  .depthLoadOp = WGPULoadOp_Clear,
                  .depthStoreOp = WGPUStoreOp_Store,
                  .depthClearValue = clearDepth,
                  .stencilLoadOp = WGPULoadOp_Clear,
                  .stencilStoreOp = WGPUStoreOp_Store,
                  .stencilClearValue = 0,
            };

            // Render pass descriptor
            m_Contex->frame_buffer.render_pass_desc.render_pass_descriptor = {
                  .label = "Render pass descriptor",
                  .colorAttachmentCount = 1,
                  .colorAttachments = m_Contex->frame_buffer.render_pass_desc.color_attachment,
                  .depthStencilAttachment
                  = &m_Contex->frame_buffer.render_pass_desc.depth_stencil_attachment,
            };
        }
    }

    RenderPass::~RenderPass()
    {
        WGPU_RELEASE_RESOURCE(Texture, m_Contex->frame_buffer.color.texture);
        WGPU_RELEASE_RESOURCE(Texture, m_Contex->frame_buffer.depth.texture);
        WGPU_RELEASE_RESOURCE(TextureView, m_Contex->frame_buffer.color.texture_view);
        WGPU_RELEASE_RESOURCE(TextureView, m_Contex->frame_buffer.depth.texture_view);
        WGPU_RELEASE_RESOURCE(Sampler, m_Contex->render_pass.sampler);
    }

}
