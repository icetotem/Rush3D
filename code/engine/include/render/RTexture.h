#ifndef RTexture_h__
#define RTexture_h__

#include "core/Core.h"
#include "render/RDefines.h"
#include "render/RBuffer.h"

namespace rush
{

    class RSampler
    {
    public:
        friend class Renderer;
        friend class RPass;

        RSampler(const char* lable = nullptr);

        Ref<wgpu::Sampler> GetSampler() const { return m_Sampler; }

    private:
        Ref<wgpu::Sampler> m_Sampler;
    };

    //////////////////////////////////////////////////////////////////////////

    class RTexture
    {
    public:
        ~RTexture() = default;

        RTexture(uint32_t width, uint32_t height, TextureFormat format, uint32_t mips, uint32_t depth, const char* lable = nullptr);

        void UpdateData(const void* data, uint64_t size);

        Ref<wgpu::Texture> GetTexture() const { return m_Texture; }

    private:
        friend class Renderer;
        friend class RPass;

        TextureFormat m_Format = TextureFormat::RGBA8Unorm;
        TextureDimension m_Dim = TextureDimension::Texture2D;
        uint32_t m_Width = 1;
        uint32_t m_Height = 1;
        uint32_t m_Depth = 1;
        uint32_t m_Mips = 1;
        Ref<wgpu::Texture> m_Texture;
    };

}

#endif // RTexture_h__
