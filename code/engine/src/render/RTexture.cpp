#include "stdafx.h"

#include "render/RTexture.h"
#include "BundleManager.h"
#include "ImageCodec.h"
#include "render/RDevice.h"

namespace rush
{

    RSampler::RSampler(const char* lable)
    {
        wgpu::SamplerDescriptor desc = {};
        desc.label = lable;
        m_Sampler = RDevice::instance().GetDevice().CreateSampler(&desc);
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

    RTexture::RTexture(uint32_t width, uint32_t height, TextureFormat format, uint32_t mips, uint32_t depth/* = 1*/, TextureDimension dim, TextureUsage usage, const char* lable/* = nullptr*/)
    {
        wgpu::TextureDescriptor descriptor;
        descriptor.dimension = dim;
        descriptor.size.width = width;
        descriptor.size.height = height;
        descriptor.size.depthOrArrayLayers = depth;
        descriptor.sampleCount = 1;
        descriptor.format = format;
        descriptor.mipLevelCount = mips;
        descriptor.usage = wgpu::TextureUsage::CopyDst | usage;
        m_TextureHandle = RDevice::instance().GetDevice().CreateTexture(&descriptor);
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

        auto width = rawData.width;
        auto height = rawData.height;
        auto dim = TextureDimension::e2D;
        auto depth = 1;
        auto mips = 1;
        TextureFormat format;
        if (dim == TextureDimension::e2D)
        {
            if (compress)
            {
                format = TextureFormat::BC3RGBAUnorm;
            }
            else
            {
                format = TextureFormat::RGBA8Unorm;
            }
        }
        else
        {
            LOG_CRITICAL("Not yet implment");
            return false;
        }

        wgpu::TextureDescriptor descriptor;
        descriptor.usage = wgpu::TextureUsage::CopyDst | wgpu::TextureUsage::TextureBinding;
        descriptor.dimension = dim;
        descriptor.size.width = width;
        descriptor.size.height = height;
        descriptor.size.depthOrArrayLayers = depth;
        descriptor.sampleCount = 1;
        descriptor.format = format;
        descriptor.mipLevelCount = mips;
        descriptor.usage = wgpu::TextureUsage::CopyDst | wgpu::TextureUsage::TextureBinding;
        m_TextureHandle = RDevice::instance().GetDevice().CreateTexture(&descriptor);

        if (compress)
        {
            TextureInfo texInfo;
            ImageCodec::GetTextureInfo(&texInfo, width, height, depth, false, mips > 1, 1, format);
            int compressedBlockSize = texInfo.storageSize;
            uint8_t* compressData = new uint8_t[compressedBlockSize];
            memset(compressData, 0, compressedBlockSize);
            ImageCodec::EncodeDTX(format, compressData, rawData.data, width, height, depth, ImageQuality::Fastest);
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
        return (bool)m_TextureHandle;
    }

    void RTexture::Destroy()
    {
        m_TextureHandle = {};
    }

    void RTexture::UpdateData(const void* data, uint64_t size)
    {
        wgpu::Buffer stagingBuffer = CreateBufferFromData(RDevice::instance().GetDevice(), data, size, wgpu::BufferUsage::CopySrc);
        wgpu::ImageCopyBuffer imageCopyBuffer = CreateImageCopyBuffer(stagingBuffer, 0, GetWidth() * 4); // TODO: calculate bytesPerRow
        wgpu::ImageCopyTexture imageCopyTexture = CreateImageCopyTexture(m_TextureHandle, 0, {0, 0, 0});
        wgpu::Extent3D copySize = { GetWidth(), GetHeight(), GetDepth() };
        wgpu::CommandEncoder encoder = RDevice::instance().GetDevice().CreateCommandEncoder();
        encoder.CopyBufferToTexture(&imageCopyBuffer, &imageCopyTexture, &copySize);
        wgpu::CommandBuffer copy = encoder.Finish();
        RDevice::instance().GetCmdQueue().Submit(1, &copy);
    }

}
