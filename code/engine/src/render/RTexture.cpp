#include "stdafx.h"

#include <dawn/webgpu_cpp.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "render/RTexture.h"
#include "RContex.h"
#include "BundleManager.h"

namespace rush
{
    extern wgpu::TextureFormat g_WGPUTextureFormat[(int)TextureFormat::Count];

    RSampler::RSampler(const char* lable)
    {
        wgpu::SamplerDescriptor desc = {};
        desc.label = lable;
        m_Sampler = CreateRef<wgpu::Sampler>(RContex::device.CreateSampler(&desc));
    }

    //////////////////////////////////////////////////////////////////////////

    static wgpu::Buffer CreateBufferFromData(const wgpu::Device& device,
        const void* data,
        uint64_t size,
        wgpu::BufferUsage usage) 
    {
        wgpu::BufferDescriptor descriptor;
        descriptor.size = size;
        descriptor.usage = usage | wgpu::BufferUsage::CopyDst;
        wgpu::Buffer buffer = device.CreateBuffer(&descriptor);
        device.GetQueue().WriteBuffer(buffer, 0, data, size);
        return buffer;
    }

    static wgpu::TextureDataLayout CreateTextureDataLayout(uint64_t offset,
        uint32_t bytesPerRow,
        uint32_t rowsPerImage)
    {
        wgpu::TextureDataLayout textureDataLayout;
        textureDataLayout.offset = offset;
        textureDataLayout.bytesPerRow = bytesPerRow;
        textureDataLayout.rowsPerImage = rowsPerImage;
        return textureDataLayout;
    }

    static wgpu::ImageCopyBuffer CreateImageCopyBuffer(wgpu::Buffer buffer,
        uint64_t offset,
        uint32_t bytesPerRow = wgpu::kCopyStrideUndefined,
        uint32_t rowsPerImage = wgpu::kCopyStrideUndefined)
    {
        wgpu::ImageCopyBuffer imageCopyBuffer = {};
        imageCopyBuffer.buffer = buffer;
        imageCopyBuffer.layout = CreateTextureDataLayout(offset, bytesPerRow, rowsPerImage);
        return imageCopyBuffer;
    }

    static wgpu::ImageCopyTexture CreateImageCopyTexture(wgpu::Texture texture,
        uint32_t mipLevel = 0,
        wgpu::Origin3D origin = { 0, 0, 0 },
        wgpu::TextureAspect aspect = wgpu::TextureAspect::All)
    {
        wgpu::ImageCopyTexture imageCopyTexture;
        imageCopyTexture.texture = texture;
        imageCopyTexture.mipLevel = mipLevel;
        imageCopyTexture.origin = origin;
        imageCopyTexture.aspect = aspect;
        return imageCopyTexture;
    }

    RTexture::RTexture(uint32_t width, uint32_t height, TextureFormat format, uint32_t mips, uint32_t depth/* = 1*/, const char* lable/* = nullptr*/)
    {
        m_Width = width;
        m_Height = height;
        m_Depth = depth;
        m_Mips = mips;
        m_Format = format;
        m_Dim = TextureDimension::Texture2D;
        wgpu::TextureDescriptor descriptor;
        descriptor.dimension = wgpu::TextureDimension::e2D;
        descriptor.size.width = m_Width;
        descriptor.size.height = m_Height;
        descriptor.size.depthOrArrayLayers = m_Depth;
        descriptor.sampleCount = 1;
        descriptor.format =  wgpu::TextureFormat::BGRA8Unorm;
        descriptor.mipLevelCount = m_Mips;
        descriptor.usage = wgpu::TextureUsage::CopyDst | wgpu::TextureUsage::TextureBinding;
        m_Texture = CreateRef<wgpu::Texture>(RContex::device.CreateTexture(&descriptor));
    }

    RTexture::RTexture()
    {
        
    }

    struct stb_image_load_result_t
    {
        int32_t image_width;
        int32_t image_height;
        int32_t channel_count;
        stbi_uc* pixel_data;
    };

    static stb_image_load_result_t stb_load_image_from_mem(const uint8_t* data, uint64_t size, bool flip_y)
    {
        static const uint8_t comp_map[5] = {
          0, //
          1, //
          2, //
          4, //
          4  //
        };
        static const uint32_t channels[5] = {
          STBI_default,    // only used for req_comp
          STBI_grey,       //
          STBI_grey_alpha, //
          STBI_rgb_alpha,  //
          STBI_rgb_alpha   //
        };

        int width = 0, height = 0;
        // Force loading 4 channel images to 3 channel by stb becasue Dawn doesn't
        // support 3 channel formats currently. The group is discussing on whether
        // webgpu shoud support 3 channel format.
        // https://github.com/gpuweb/gpuweb/issues/66#issuecomment-410021505
        int read_comps = 4;
        stbi_set_flip_vertically_on_load(flip_y);
        stbi_uc* pixel_data = stbi_load_from_memory(data, size,            //
            &width,              //
            &height,             //
            &read_comps,         //
            channels[read_comps] //
        );

        stb_image_load_result_t result;
        result.image_width = width;
        result.image_height = height;
        result.channel_count = comp_map[read_comps];
        result.pixel_data = pixel_data;
        return result;
    }

    /**
     * @brief Determines the number of mip levels needed for a full mip chain given
     * the width and height of texture level 0.
     *
     * @param {int} width width of texture level 0.
     * @param {int} height height of texture level 0.
     * @return {uint32_t} Ideal number of mip levels.
     */
    static uint32_t calculate_mip_level_count(int width, int height)
    {
        return (uint32_t)(floor((float)(log2(std::max(width, height))))) + 1;
    }

    typedef enum color_space_enum_t {
        COLOR_SPACE_UNDEFINED,
        COLOR_SPACE_SRGB,
        COLOR_SPACE_LINEAR,
    } color_space_enum_t;

    static WGPUTextureFormat linear_to_sgrb_format(WGPUTextureFormat format)
    {
        switch (format) {
        case WGPUTextureFormat_RGBA8Unorm:
            return WGPUTextureFormat_RGBA8UnormSrgb;
        case WGPUTextureFormat_BGRA8Unorm:
            return WGPUTextureFormat_BGRA8UnormSrgb;
        case WGPUTextureFormat_BC1RGBAUnorm:
            return WGPUTextureFormat_BC1RGBAUnormSrgb;
        case WGPUTextureFormat_BC2RGBAUnorm:
            return WGPUTextureFormat_BC2RGBAUnormSrgb;
        case WGPUTextureFormat_BC3RGBAUnorm:
            return WGPUTextureFormat_BC3RGBAUnormSrgb;
        case WGPUTextureFormat_BC7RGBAUnorm:
            return WGPUTextureFormat_BC7RGBAUnormSrgb;
        default:
            return format;
        }
    }

    static WGPUTextureFormat srgb_to_linear_format(WGPUTextureFormat format)
    {
        switch (format) {
        case WGPUTextureFormat_RGBA8UnormSrgb:
            return WGPUTextureFormat_RGBA8Unorm;
        case WGPUTextureFormat_BGRA8UnormSrgb:
            return WGPUTextureFormat_BGRA8Unorm;
        case WGPUTextureFormat_BC1RGBAUnormSrgb:
            return WGPUTextureFormat_BC1RGBAUnorm;
        case WGPUTextureFormat_BC2RGBAUnormSrgb:
            return WGPUTextureFormat_BC2RGBAUnorm;
        case WGPUTextureFormat_BC3RGBAUnormSrgb:
            return WGPUTextureFormat_BC3RGBAUnorm;
        case WGPUTextureFormat_BC7RGBAUnormSrgb:
            return WGPUTextureFormat_BC7RGBAUnorm;
        default:
            return format;
        }
    }

    static WGPUTextureFormat format_for_color_space(WGPUTextureFormat format,
        color_space_enum_t colorSpace)
    {
        switch (colorSpace) {
        case COLOR_SPACE_SRGB:
            return linear_to_sgrb_format(format);
        case COLOR_SPACE_LINEAR:
            return srgb_to_linear_format(format);
        default:
            return format;
        }
    }

    bool RTexture::Load(const StringView& path)
    {
        auto stream = BundleManager::instance().Get(path);
        if (stream->IsEmpty())
        {
            return false;
        }

        const bool flip_y = false;
        stb_image_load_result_t image_load_result = stb_load_image_from_mem(stream->GetData(), stream->GetSize(), flip_y);
        if (image_load_result.pixel_data == nullptr)
        {
            LOG_ERROR("Cannot load image from {}", path);
            return false;
        }

        const int width = image_load_result.image_width;
        const int height = image_load_result.image_height;
        const int channel_count = image_load_result.channel_count;
        const bool generate_mipmaps = true;// options ? options->generate_mipmaps : false;
        const uint32_t mip_level_count = generate_mipmaps ? calculate_mip_level_count(width, height) : 1u;

        m_Width = width;
        m_Height = height;
        m_Depth = 1;
        m_Mips = 1;
        m_Format = TextureFormat::RGBA8Unorm;//format_for_color_space();
        m_Dim = TextureDimension::Texture2D;
        wgpu::TextureDescriptor descriptor;
        descriptor.usage = wgpu::TextureUsage::CopyDst | wgpu::TextureUsage::TextureBinding;
        descriptor.dimension = wgpu::TextureDimension::e2D;
        descriptor.size.width = m_Width;
        descriptor.size.height = m_Height;
        descriptor.size.depthOrArrayLayers = m_Depth;
        descriptor.sampleCount = 1;
        descriptor.format = g_WGPUTextureFormat[(int)m_Format];
        descriptor.mipLevelCount = m_Mips;
        descriptor.usage = wgpu::TextureUsage::CopyDst | wgpu::TextureUsage::TextureBinding;
        m_Texture = CreateRef<wgpu::Texture>(RContex::device.CreateTexture(&descriptor));

        const uint64_t data_size = width * height * m_Depth * image_load_result.channel_count * sizeof(uint8_t);
        UpdateData(image_load_result.pixel_data, data_size);

        stbi_image_free(image_load_result.pixel_data);

        return true;
    }

    void RTexture::UpdateData(const void* data, uint64_t size)
    {
        wgpu::Buffer stagingBuffer = CreateBufferFromData(RContex::device, data, size, wgpu::BufferUsage::CopySrc);
        wgpu::ImageCopyBuffer imageCopyBuffer = CreateImageCopyBuffer(stagingBuffer, 0, size / m_Height);
        wgpu::ImageCopyTexture imageCopyTexture = CreateImageCopyTexture(*m_Texture, 0, {0, 0, 0});
        wgpu::Extent3D copySize = { m_Width, m_Height, m_Depth };
        wgpu::CommandEncoder encoder = RContex::device.CreateCommandEncoder();
        encoder.CopyBufferToTexture(&imageCopyBuffer, &imageCopyTexture, &copySize);
        wgpu::CommandBuffer copy = encoder.Finish();
        RContex::queue.Submit(1, &copy);
    }

}
