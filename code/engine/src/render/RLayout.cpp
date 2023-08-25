#include "stdafx.h"

#include <dawn/webgpu_cpp.h>

#include "render/RLayout.h"
#include "RContex.h"

namespace rush
{

    extern wgpu::BufferBindingType g_BufferBindingType[(int)BufferBindingType::Count];
    extern wgpu::SamplerBindingType g_SamplerBindingType[(int)SamplerBindingType::Count];
    extern wgpu::ShaderStage g_WGPUShaderStage[(int)ShaderStage::Count];
    extern wgpu::TextureSampleType g_TextureSampleType[(int)TextureSampleType::Count];
    extern wgpu::TextureViewDimension g_TextureViewDimension[(int)TextureViewDimension::Count];
        
    //////////////////////////////////////////////////////////////////////////

    BindingLayoutHelper::BindingLayoutHelper(uint32_t entryBinding, ShaderStage entryVisibility, BufferBindingType bufferType, bool bufferHasDynamicOffset /*= false*/, uint64_t bufferMinBindingSize /*= 0*/)
    {
        m_Entry = CreateRef<wgpu::BindGroupLayoutEntry>();
        m_Entry->binding = entryBinding;
        m_Entry->visibility = g_WGPUShaderStage[(int)entryVisibility];
        m_Entry->buffer.type = g_BufferBindingType[(int)bufferType];
        m_Entry->buffer.hasDynamicOffset = bufferHasDynamicOffset;
        m_Entry->buffer.minBindingSize = bufferMinBindingSize;
    }

    BindingLayoutHelper::BindingLayoutHelper(uint32_t entryBinding, ShaderStage entryVisibility, SamplerBindingType samplerType)
    {
        m_Entry = CreateRef<wgpu::BindGroupLayoutEntry>();
        m_Entry->binding = entryBinding;
        m_Entry->visibility = g_WGPUShaderStage[(int)entryVisibility];
        m_Entry->sampler.type = g_SamplerBindingType[(int)samplerType];
    }

    BindingLayoutHelper::BindingLayoutHelper(uint32_t entryBinding, ShaderStage entryVisibility, TextureSampleType textureSampleType, TextureViewDimension viewDimension /*= TextureViewDimension::e2D*/, bool textureMultisampled /*= false*/)
    {
        m_Entry = CreateRef<wgpu::BindGroupLayoutEntry>();
        m_Entry->binding = entryBinding;
        m_Entry->visibility = g_WGPUShaderStage[(int)entryVisibility];
        m_Entry->texture.sampleType = g_TextureSampleType[(int)textureSampleType];
        m_Entry->texture.viewDimension = g_TextureViewDimension[(int)viewDimension];
        m_Entry->texture.multisampled = textureMultisampled;
    }

    BindingLayout::~BindingLayout()
    {

    }

    BindingLayout::BindingLayout(Ref<RContex> contex, std::initializer_list<BindingLayoutHelper> entriesInitializer, const char* lable)
    {
        std::vector<wgpu::BindGroupLayoutEntry> entries;
        for (const BindingLayoutHelper& entry : entriesInitializer) 
        {
            entries.push_back(*entry.GetEntry());
        }

        wgpu::BindGroupLayoutDescriptor descriptor;
        descriptor.entryCount = entries.size();
        descriptor.entries = entries.data();
        descriptor.label = lable;
        m_Layout = CreateRef<wgpu::BindGroupLayout>();
        *m_Layout = contex->device.CreateBindGroupLayout(&descriptor);
    }

}
