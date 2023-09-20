#include "stdafx.h"
#include "render/RUniform.h"
#include "render/RDevice.h"

namespace rush
{

    //////////////////////////////////////////////////////////////////////////

    BindingInitializationHelper::BindingInitializationHelper(uint32_t binding, Ref<RTexture> texture)
    {
        this->m_Binding = binding;
        this->m_Texture = texture;
    }

    BindingInitializationHelper::BindingInitializationHelper(uint32_t binding, Ref<RSampler> sampler)
    {
        this->m_Binding = binding;
        this->m_Sampler = sampler;
    }

    BindingInitializationHelper::BindingInitializationHelper(uint32_t binding, Ref<RBuffer> buffer, uint64_t offset /*= 0*/, uint64_t size /*= kWholeSize*/)
    {
        this->m_Binding = binding;
        this->m_Buffer = buffer;
        this->m_Offset = offset;
        this->m_Size = size;
    }

    void BindingInitializationHelper::AddBinding(std::vector<wgpu::BindGroupEntry>& entries) const
    {
        wgpu::BindGroupEntry result = {};
        result.binding = GetBinding();
        if (GetSampler())
        {
            result.sampler = GetSampler()->GetSampler();
            entries.push_back(result);
        }
        else if (GetTexture())
        {
            result.textureView = GetTexture()->GetTexture().CreateView();
            entries.push_back(result);
        }
        else if (GetBuffer())
        {
            result.buffer = GetBuffer()->GetBuffer();
            result.offset = GetOffset();
            result.size = GetSize();
            entries.push_back(result);
        }
    }

    static wgpu::BindGroup MakeBindGroup(
        const wgpu::Device& device,
        const wgpu::BindGroupLayout& layout,
        std::initializer_list<BindingInitializationHelper> entriesInitializer) 
    {
        std::vector<wgpu::BindGroupEntry> entries;
        for (const BindingInitializationHelper& helper : entriesInitializer) 
        {
            helper.AddBinding(entries);
        }

        wgpu::BindGroupDescriptor descriptor;
        descriptor.layout = layout;
        descriptor.entryCount = checked_cast<uint32_t>(entries.size());
        descriptor.entries = entries.data();
        return device.CreateBindGroup(&descriptor);
    }

    RBindGroup::RBindGroup(Ref<BindingLayout> layout, std::initializer_list<BindingInitializationHelper> entriesInitializer, const char* lable)
    {
        m_BindGroup = MakeBindGroup(RDevice::instance().GetDevice(), layout->m_Layout, entriesInitializer);
        m_BindLayout = layout;
    }

    RBindGroup::~RBindGroup()
    {
    }


}