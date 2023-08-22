#pragma once

#include "core/Core.h"
#include "render/RenderDefines.h"
#include "render/RBuffer.h"

namespace rush
{

    class RTexture
    {
    public:
        RTexture();
        ~RTexture();

    protected:
        friend class Renderer;

        TextureFormat m_Format = TextureFormat::RGBA8Uint;
        TextureDimension m_Dim = TextureDimension::Texture2D;
        uint32_t m_Width = 0;
        uint32_t m_Height = 0;
        uint32_t m_Depth = 0;
        uint32_t m_Mips = 0;
        WGPUTextureImpl* m_Texture = nullptr;
        WGPUTextureViewImpl* m_View = nullptr;
        WGPUSamplerImpl* m_Sampler = nullptr;
    };

}