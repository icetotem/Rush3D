#ifndef RContex_h__
#define RContex_h__

#include <dawn/webgpu_cpp.h>
#include <dawn/native/DawnNative.h>

namespace rush
{

    struct RContex
    {
        static dawn::native::Adapter adapter;
        static wgpu::Device device;
        static wgpu::Queue queue;
        wgpu::TextureView depthStencilView;
        wgpu::SwapChain swapChain;
        wgpu::CommandEncoder encoder;
        wgpu::CommandBuffer commands;
    };

}

#endif // RContex_h__
