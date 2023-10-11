#include "stdafx.h"
#include "render/RBindGroup.h"
#include "render/RDevice.h"

namespace rush
{

    void RBindGroup::AddBinding(uint32_t binding, ShaderStage visibility, Ref<RTexture> texture, TextureSampleType textureSampleType, TextureViewDimension viewDimension)
    {
        auto& bgLayoutEntry = bgLayoutEntris.emplace_back();
        bgLayoutEntry.binding = binding;
        bgLayoutEntry.visibility = visibility;
        bgLayoutEntry.texture.sampleType = textureSampleType;
        bgLayoutEntry.texture.viewDimension = viewDimension;

        auto& bgEntry = bgEntries.emplace_back();
        bgEntry.binding = binding;
        bgEntry.textureView = texture->GetTextureHandle().CreateView();

    }

    void RBindGroup::AddBinding(uint32_t binding, ShaderStage visibility, wgpu::Sampler sampler, SamplerBindingType samplerType)
    {
        auto& bgLayoutEntry = bgLayoutEntris.emplace_back();
        bgLayoutEntry.binding = binding;
        bgLayoutEntry.visibility = visibility;
        bgLayoutEntry.sampler.type = samplerType;

        auto& bgEntry = bgEntries.emplace_back();
        bgEntry.binding = binding;
        bgEntry.sampler = sampler;
    }

    void RBindGroup::AddBinding(uint32_t binding, ShaderStage visibility, Ref<RBuffer> buffer, wgpu::BufferBindingType bufferType)
    {
        auto& bgLayoutEntry = bgLayoutEntris.emplace_back();
        bgLayoutEntry.binding = binding;
        bgLayoutEntry.visibility = visibility;
        bgLayoutEntry.buffer.type = bufferType;

        auto& bgEntry = bgEntries.emplace_back();
        bgEntry.binding = binding;
        bgEntry.buffer = buffer->GetBufferHandle();
    }

    void RBindGroup::Create(const char* lable)
    {
        wgpu::BindGroupLayoutDescriptor layoutDesc;
        layoutDesc.label = lable;
        layoutDesc.entryCount = bgLayoutEntris.size();
        layoutDesc.entries = bgLayoutEntris.data();
        m_BindLayout = RDevice::instance().GetDevice().CreateBindGroupLayout(&layoutDesc);

        wgpu::BindGroupDescriptor descriptor;
        descriptor.label = lable;
        descriptor.layout = m_BindLayout;
        descriptor.entryCount = bgEntries.size();
        descriptor.entries = bgEntries.data();
        m_BindGroup = RDevice::instance().GetDevice().CreateBindGroup(&descriptor);

        bgLayoutEntris.clear();
        bgEntries.clear();
    }

}