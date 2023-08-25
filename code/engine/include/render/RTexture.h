#pragma once

#include "core/Core.h"
#include "render/RDefines.h"
#include "render/RBuffer.h"

namespace rush
{

    class RTexture
    {
    public:
        ~RTexture() = default;

        void UpdateData(const void* data, uint64_t size);

        Ref<wgpu::TextureView> GetTextureView() const { return m_TextureView; }

        Ref<wgpu::Sampler> GetSampler() const { return m_Sampler; }


    protected:
        friend class Renderer;

        RTexture(Ref<RContex> contex, uint32_t width, uint32_t height, TextureFormat format, uint32_t mips, uint32_t depth, const char* lable);

        static Ref<RTexture> Construct(Ref<RContex> contex, uint32_t width, uint32_t height, TextureFormat format, uint32_t mips, uint32_t depth, const char* lable)
        {
            return std::shared_ptr<RTexture>(new RTexture(contex, width, height, format, mips, depth, lable));
        }

        TextureFormat m_Format = TextureFormat::RGBA8Unorm;
        TextureDimension m_Dim = TextureDimension::Texture2D;
        uint32_t m_Width = 1;
        uint32_t m_Height = 1;
        uint32_t m_Depth = 1;
        uint32_t m_Mips = 1;
        Ref<wgpu::Texture> m_Texture;
        Ref<wgpu::TextureView> m_TextureView;
        Ref<wgpu::Sampler> m_Sampler;
        Ref<RContex> m_Contex;
    };

}