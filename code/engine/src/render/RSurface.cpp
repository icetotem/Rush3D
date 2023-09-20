#include "stdafx.h"
#include "render/RSurface.h"
#include <dawn/dawn_proc.h>
#include "render/RDevice.h"

namespace rush
{

    std::unique_ptr<wgpu::ChainedStruct> SetupWindowAndGetSurfaceDescriptor(void* wndHandle, void* displayHandle)
    {
#ifdef RUSH_PLATFORM_WINDOWS
        std::unique_ptr<wgpu::SurfaceDescriptorFromWindowsHWND> desc =
            std::make_unique<wgpu::SurfaceDescriptorFromWindowsHWND>();
        desc->hwnd = wndHandle;
        desc->hinstance = GetModuleHandle(nullptr);
        return std::move(desc);
#elif defined(RUSH_PLATFORM_IOS)
        return nullptr;
#elif defined(RUSH_PLATFORM_MACOS)
        return nullptr;
#elif defined(RUSH_PLATFORM_LINUX)
        return nullptr;
#elif defined(RUSH_PLATFORM_ANDROID)
        return nullptr;
#endif
    }

    RSurface::RSurface(uint32_t width, uint32_t height, bool vsync, void* wndHandle, void* dispHandle)
    {
        // get dawn procs
        auto procs = dawn::native::GetProcs();
        dawnProcSetProcs(&procs);

        auto& rd = RDevice::instance();

        // create surface
        auto surfaceChainedDesc = SetupWindowAndGetSurfaceDescriptor(wndHandle, dispHandle);
        WGPUSurfaceDescriptor surfaceDesc;
        surfaceDesc.nextInChain = reinterpret_cast<WGPUChainedStruct*>(surfaceChainedDesc.get());
        m_Surface = procs.instanceCreateSurface(rd.GetDawnInstance().Get(), &surfaceDesc);

        m_Vsync = vsync;

        // create swap chain
        Resize(width, height);
    }

    void RSurface::Present()
    {
        RUSH_ASSERT(m_SwapChain);
        m_SwapChain.Present();
    }

    void RSurface::Resize(uint32_t width, uint32_t height)
    {
        auto& rd = RDevice::instance();

        // create swap chain
        m_Width = width;
        m_Height = height;
        m_Vsync = m_Vsync;
        wgpu::SwapChainDescriptor scDesc;
        scDesc.usage = wgpu::TextureUsage::RenderAttachment;
        scDesc.format = wgpu::TextureFormat::BGRA8Unorm; // TODO: get best format
        scDesc.width = m_Width;
        scDesc.height = m_Height;
        scDesc.presentMode = m_Vsync ? wgpu::PresentMode::Fifo : wgpu::PresentMode::Mailbox;
        m_SwapChain = rd.GetDevice().CreateSwapChain(m_Surface, &scDesc);
    }

}