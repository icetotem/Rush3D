#pragma once

#include "render/Renderer.h"
#include <dawn/webgpu.h>

namespace rush
{

    struct RenderContex
    {
        WGPUAdapter adapter = nullptr;
        WGPUDevice device = nullptr;
        WGPUQueue queue = nullptr;

        struct swapChain_t
        {
            WGPUSwapChain instance = nullptr;
            WGPUTextureFormat format;
            WGPUTextureView frameBuffer;
            WGPUPresentMode presentMode;
        } swapChain;

        WGPUCommandEncoder cmdEncoder = nullptr;
        WGPURenderPassEncoder rPassEncoder = nullptr;
        WGPUComputePassEncoder cPassEncoder = nullptr;

        struct depthStencil_t
        {
            WGPUTexture texture = nullptr;
            WGPUTextureView frameBuffer = nullptr;
            WGPURenderPassDepthStencilAttachment desc;
        } depthStencil;

        struct render_pass_t
        {
            WGPURenderPassColorAttachment color_attachments[1];
            WGPURenderPassDescriptor descriptor;
        } render_pass = { 0 };

        struct submitInfo_t
        {
            uint32_t cmdBufCount;
            WGPUCommandBuffer commandBuffers[CMD_BUFFER_SIZE];
        } submitInfo;

        WGPUSurface surface = nullptr;
    };

    struct RenderPassContex
    {
        struct frame_buffer_attachment_t
        {
            WGPUTexture texture;
            WGPUTextureView texture_view;
        } frame_buffer_attachment;

        struct frame_buffer_t
        {
            frame_buffer_attachment_t color;
            frame_buffer_attachment_t depth;
            struct render_pass_desc_t
            {
                WGPURenderPassColorAttachment color_attachment[1];
                WGPURenderPassDepthStencilAttachment depth_stencil_attachment;
                WGPURenderPassDescriptor render_pass_descriptor;
            } render_pass_desc;
        } frame_buffer;

        struct render_pass_t
        {
            uint32_t width, height;
            WGPUSampler sampler;
            frame_buffer_t frame_buffer;
        } render_pass;
    };

}