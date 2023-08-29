#ifndef RContex_h__
#define RContex_h__

#include <dawn/webgpu_cpp.h>
#include <dawn/native/DawnNative.h>

namespace rush
{

    struct RContex
    {
        dawn::native::Adapter adapter;
        wgpu::Device device;
        wgpu::SwapChain swapChain;
        wgpu::CommandEncoder encoder;
        wgpu::CommandBuffer commands;
        wgpu::Queue queue;
    };

}

#endif // RContex_h__
