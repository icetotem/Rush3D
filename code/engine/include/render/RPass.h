#pragma once

#include "render/RDefines.h"

namespace rush
{
    
    struct RContex;
    struct RenderPassContex;

    class RPass
    {
    public:
        ~RPass();

    private:
        RPass(Ref<RContex> renderContex, uint32_t width, uint32_t height, TextureFormat color, TextureFormat depthStencil, const Vector4& clearColor, float clearDepth, bool withDepth, const char* lable);

        static Ref<RPass> Construct(Ref<RContex> renderContex, uint32_t width, uint32_t height, TextureFormat color, TextureFormat depthStencil, const Vector4& clearColor, float clearDepth, bool withDepth, const char* lable)
        {
            return std::shared_ptr<RPass>(new RPass(renderContex, width, height, color, depthStencil, clearColor, clearDepth, withDepth, lable));
        }

    private:
        friend class Renderer;
        
        Ref<RenderPassContex> m_Contex;
    };

}

