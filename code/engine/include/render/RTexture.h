#ifndef RTexture_h__
#define RTexture_h__

#include "core/Core.h"
#include "render/RDefines.h"
#include "render/RBuffer.h"
#include "Asset.h"

namespace rush
{

    class RSampler
    {
    public:
        friend class Renderer;
        friend class RPass;

        RSampler(const char* lable = nullptr);

        const wgpu::Sampler GetSampler() const { return m_Sampler; }

    private:
        wgpu::Sampler m_Sampler;
    };

    //////////////////////////////////////////////////////////////////////////

    class RTexture : public Asset
    {
    public:
        RTexture() = default;
        RTexture(uint32_t width, uint32_t height, TextureFormat format, uint32_t mips, uint32_t depth, TextureDimension dim = TextureDimension::e2D, TextureUsage usage = TextureUsage::TextureBinding, const char* lable = nullptr);
        ~RTexture() = default;

        bool Load(const StringView& path) override;
        bool IsValid() const;
        void Destroy();

        void UpdateData(const void* data, uint64_t size);

        const wgpu::Texture GetTextureHandle() const { return m_TextureHandle; }

        uint32_t GetWidth() const { return m_TextureHandle.GetWidth(); }
        uint32_t GetHeight() const { return m_TextureHandle.GetHeight(); }
        uint32_t GetDepth() const { return m_TextureHandle.GetDepthOrArrayLayers(); }
        uint32_t GetMips() const { return m_TextureHandle.GetMipLevelCount(); }
        TextureFormat GetFormat() const { return m_TextureHandle.GetFormat(); }
        TextureDimension GetDim() const { return m_TextureHandle.GetDimension(); }

    private:
        friend class Renderer;
        friend class RPass;
        wgpu::Texture m_TextureHandle;
    };

}

#endif // RTexture_h__
