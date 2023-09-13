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
        friend class RenderContext;
        friend class RPass;

        RSampler(const char* lable = nullptr);

        const wgpu::Sampler& GetSampler() const { return m_Sampler; }

    private:
        wgpu::Sampler m_Sampler;
    };

    //////////////////////////////////////////////////////////////////////////

    class RTexture : public Asset
    {
    public:
        RTexture() = default;
        RTexture(uint32_t width, uint32_t height, TextureFormat format, uint32_t mips, uint32_t depth, TextureDimension dim = TextureDimension::e2D, const char* lable = nullptr);
        ~RTexture() = default;

        bool Load(const StringView& path) override;
        bool IsValid() const;
        void Destroy();

        void UpdateData(const void* data, uint64_t size);

        const wgpu::Texture& GetTexture() const { return m_Texture; }

        uint32_t GetWidth() const { return m_Width; }
        uint32_t GetHeight() const { return m_Height; }
        uint32_t GetDepth() const { return m_Depth; }
        uint32_t GetMips() const { return m_Mips; }
        TextureFormat GetFormat() const { return m_Format; }

    private:
        friend class RenderContext;
        friend class RPass;

        TextureFormat m_Format = TextureFormat::RGBA8Unorm;
        TextureDimension m_Dim = TextureDimension::e2D;
        uint32_t m_Width = 1;
        uint32_t m_Height = 1;
        uint32_t m_Depth = 1;
        uint32_t m_Mips = 1;
        wgpu::Texture m_Texture;
    };

}

#endif // RTexture_h__
