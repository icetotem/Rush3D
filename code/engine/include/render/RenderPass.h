#pragma once

#include "render/RenderDefines.h"

namespace rush
{
    
    struct RenderContex;
    struct RenderPassContex;

    class RenderPass
    {
    public:
        ~RenderPass();

    private:
        RenderPass(Ref<RenderContex> renderContex, const char* lable, uint32_t width, uint32_t height, TextureFormat color, TextureFormat depthStencil, const Vector4& clearColor, float clearDepth, bool withDepth);

        static Ref<RenderPass> Construct(Ref<RenderContex> renderContex, const char* lable, uint32_t width, uint32_t height, TextureFormat color, TextureFormat depthStencil, const Vector4& clearColor, float clearDepth, bool withDepth)
        {
            return std::shared_ptr<RenderPass>(new RenderPass(renderContex, lable, width, height, color, depthStencil, clearColor, clearDepth, withDepth));
        }

    private:
        friend class Renderer;
        
        Ref<RenderPassContex> m_Contex;
    };

}

