#ifndef RSurface_h__
#define RSurface_h__

#include "render/RDefines.h"

namespace rush
{
    
    class RSurface
    {
    public:
        ~RSurface() = default;

        void Present();

        void Resize(uint32_t width, uint32_t height);

        wgpu::SwapChain& GetSwapChain() { return m_SwapChain; }

    private:
        static Ref<RSurface> Construct(uint32_t width, uint32_t height, bool vsync, void* wndHandle, void* dispHandle)
        {
            return std::shared_ptr<RSurface>(new RSurface(width, height, vsync, wndHandle, dispHandle));
        }

        RSurface(uint32_t width, uint32_t height, bool vsync, void* wndHandle, void* dispHandle);


    private:
        friend class Window;

        uint32_t m_Width = 0;
        uint32_t m_Height = 0;
        bool m_Vsync = true;
        wgpu::SwapChain m_SwapChain;
        WGPUSurface m_Surface;
    };

}

#endif // RSurface_h__
