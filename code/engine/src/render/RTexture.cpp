#include "stdafx.h"

#include "render/RTexture.h"
#include "BundleManager.h"
#include "ImageCodec.h"
#include "render/RenderContext.h"

namespace rush
{

    RSampler::RSampler(const char* lable)
    {
        wgpu::SamplerDescriptor desc = {};
        desc.label = lable;
        m_Sampler = RenderContext::device.CreateSampler(&desc);
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

    //////////////////////////////////////////////////////////////////////////

    RTexture::RTexture(uint32_t width, uint32_t height, TextureFormat format, uint32_t mips, uint32_t depth/* = 1*/, TextureDimension dim, const char* lable/* = nullptr*/)
    {
        m_Width = width;
        m_Height = height;
        m_Depth = depth;
        m_Mips = mips;
        m_Format = format;
        m_Dim = dim;
        wgpu::TextureDescriptor descriptor;
        descriptor.dimension = m_Dim;
        descriptor.size.width = m_Width;
        descriptor.size.height = m_Height;
        descriptor.size.depthOrArrayLayers = m_Depth;
        descriptor.sampleCount = 1;
        descriptor.format = m_Format;
        descriptor.mipLevelCount = m_Mips;
        descriptor.usage = wgpu::TextureUsage::CopyDst | wgpu::TextureUsage::TextureBinding;
        m_Texture = RenderContext::device.CreateTexture(&descriptor);
    }

    bool RTexture::Load(const StringView& path)
    {
        auto stream = BundleManager::instance().Get(path);
        if (stream->IsEmpty())
        {
            LOG_ERROR("Texture path error {}", path);
            return false;
        }

        const bool compress = false;
        const bool flip_y = false;
        RawImageData rawData;
        String error;
        if (!ImageCodec::LoadImage2D(rawData, stream->GetData(), stream->GetSize(), flip_y, error))
        {
            LOG_ERROR("Cannot load image from {}, reason: {}", path, error);
            return false;
        }

        m_Width = rawData.width;
        m_Height = rawData.height;
        m_Dim = TextureDimension::e2D;
        m_Depth = 1;
        m_Mips = 1;

        if (m_Dim == TextureDimension::e2D)
        {
            if (compress)
            {
                m_Format = TextureFormat::BC3RGBAUnorm;
            }
            else
            {
                m_Format = TextureFormat::RGBA8Unorm;
            }
        }
        else
        {
            LOG_CRITICAL("Not yet implment");
            return false;
        }

        wgpu::TextureDescriptor descriptor;
        descriptor.usage = wgpu::TextureUsage::CopyDst | wgpu::TextureUsage::TextureBinding;
        descriptor.dimension = m_Dim;
        descriptor.size.width = m_Width;
        descriptor.size.height = m_Height;
        descriptor.size.depthOrArrayLayers = m_Depth;
        descriptor.sampleCount = 1;
        descriptor.format = m_Format;
        descriptor.mipLevelCount = m_Mips;
        descriptor.usage = wgpu::TextureUsage::CopyDst | wgpu::TextureUsage::TextureBinding;
        m_Texture = RenderContext::device.CreateTexture(&descriptor);

        if (compress)
        {
            TextureInfo texInfo;
            ImageCodec::GetTextureInfo(&texInfo, m_Width, m_Height, m_Depth, false, m_Mips > 1, 1, m_Format);
            int compressedBlockSize = texInfo.storageSize;
            uint8_t* compressData = new uint8_t[compressedBlockSize];
            memset(compressData, 0, compressedBlockSize);
            ImageCodec::EncodeDTX(m_Format, compressData, rawData.data, m_Width, m_Height, m_Depth, ImageQuality::Fastest);
            UpdateData(compressData, compressedBlockSize);
            delete[] compressData;
        }
        else
        {
            UpdateData(rawData.data, rawData.size);
        }

        return true;
    }

    bool RTexture::IsValid() const
    {
        return (bool)m_Texture;
    }

    void RTexture::Destroy()
    {
        m_Texture = {};
    }

    void RTexture::UpdateData(const void* data, uint64_t size)
    {
        wgpu::Buffer stagingBuffer = CreateBufferFromData(RenderContext::device, data, size, wgpu::BufferUsage::CopySrc);
        wgpu::ImageCopyBuffer imageCopyBuffer = CreateImageCopyBuffer(stagingBuffer, 0, m_Width * 4); // TODO: calculate bytesPerRow
        wgpu::ImageCopyTexture imageCopyTexture = CreateImageCopyTexture(m_Texture, 0, {0, 0, 0});
        wgpu::Extent3D copySize = { m_Width, m_Height, m_Depth };
        wgpu::CommandEncoder encoder = RenderContext::device.CreateCommandEncoder();
        encoder.CopyBufferToTexture(&imageCopyBuffer, &imageCopyTexture, &copySize);
        wgpu::CommandBuffer copy = encoder.Finish();
        RenderContext::queue.Submit(1, &copy);
    }

}
