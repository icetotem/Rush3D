#include "stdafx.h"

#include <dawn/webgpu_cpp.h>

#include "render/RTexture.h"
#include "RContex.h"

namespace rush
{
    extern wgpu::TextureFormat g_WGPUTextureFormat[(int)TextureFormat::Count];

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

    RTexture::RTexture(Ref<RContex> contex, uint32_t width, uint32_t height, TextureFormat format, uint32_t mips, uint32_t depth/* = 1*/, const char* lable/* = nullptr*/)
    {
        m_Contex = contex;
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


        m_Texture = CreateRef<wgpu::Texture>();
        *m_Texture = contex->device.CreateTexture(&descriptor);
        m_Sampler = CreateRef<wgpu::Sampler>();
        *m_Sampler = contex->device.CreateSampler();
        m_TextureView = CreateRef<wgpu::TextureView>();
        *m_TextureView = m_Texture->CreateView();
    }

    RTexture::~RTexture()
    {

    }

    void RTexture::UpdateData(const void* data, uint64_t size)
    {
        wgpu::Buffer stagingBuffer = CreateBufferFromData(
            m_Contex->device, data, size, wgpu::BufferUsage::CopySrc);

        wgpu::ImageCopyBuffer imageCopyBuffer = CreateImageCopyBuffer(stagingBuffer, 0, size);

        wgpu::ImageCopyTexture imageCopyTexture = CreateImageCopyTexture(*m_Texture, 0, {0, 0, 0});

        wgpu::Extent3D copySize = { m_Width, m_Height, m_Depth };

        wgpu::CommandEncoder encoder = m_Contex->device.CreateCommandEncoder();
        encoder.CopyBufferToTexture(&imageCopyBuffer, &imageCopyTexture, &copySize);

        wgpu::CommandBuffer copy = encoder.Finish();
        m_Contex->queue.Submit(1, &copy);
    }

}
