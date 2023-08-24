#pragma once

#include "render/Renderer.h"
#include <dawn/webgpu_cpp.h>
#include "dawn/native/DawnNative.h"

namespace rush
{

    struct RenderContex
    {
        dawn::native::Adapter adapter;
        wgpu::Device device;
        wgpu::SwapChain swapChain;
        wgpu::CommandEncoder encoder;
        wgpu::CommandBuffer commands;
        wgpu::TextureView depthStencilView;
        Ref<wgpu::Queue> queue;
    };

    struct RenderPassContex
    {

    };

}