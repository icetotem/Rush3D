#include "stdafx.h"

#include <dawn/webgpu_cpp.h>

#include "render/RUniform.h"
#include "RContex.h"

namespace rush
{
    extern wgpu::BufferUsage g_BufferUsage[(int)BufferUsage::Count];

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

    BindingInitializationHelper::BindingInitializationHelper(uint32_t binding, Ref<RUniformBuffer> buffer, uint64_t offset /*= 0*/, uint64_t size /*= kWholeSize*/)
    {
        this->m_Binding = binding;
        this->m_Buffer = buffer;
        this->m_Offset = offset;
        this->m_Size = size;
    }

    void AddBinding(const BindingInitializationHelper& helper, std::vector<wgpu::BindGroupEntry>& entries)
    {
        wgpu::BindGroupEntry result = {};
        result.binding = helper.GetBinding();
        if (helper.GetSampler())
        {
            result.sampler = *helper.GetSampler()->GetSampler();
            entries.push_back(result);
        }
        else if (helper.GetTexture())
        {
            result.textureView = helper.GetTexture()->GetTexture()->CreateView();
            entries.push_back(result);
        }
        else if (helper.GetBuffer())
        {
            result.buffer = *helper.GetBuffer()->GetBuffer();
            result.offset = helper.GetOffset();
            result.size = helper.GetSize();
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
            AddBinding(helper, entries);
        }

        wgpu::BindGroupDescriptor descriptor;
        descriptor.layout = layout;
        descriptor.entryCount = checked_cast<uint32_t>(entries.size());
        descriptor.entries = entries.data();
        return device.CreateBindGroup(&descriptor);
    }

    RBindGroup::RBindGroup(Ref<RContex> contex, Ref<BindingLayout> layout, std::initializer_list<BindingInitializationHelper> entriesInitializer, const char* lable)
    {
        m_BindGroup = CreateRef<wgpu::BindGroup>(MakeBindGroup(contex->device, *layout->m_Layout, entriesInitializer));
        m_BindLayout = layout;
    }

    RBindGroup::~RBindGroup()
    {
    }

    //////////////////////////////////////////////////////////////////////////

    RUniformBuffer::RUniformBuffer(Ref<RContex> contex, BufferUsage usage, uint64_t size, const char* lable)
    {
        m_Contex = contex;
        wgpu::BufferDescriptor descriptor;
        descriptor.size = size;
        descriptor.usage = g_BufferUsage[(int)usage] | wgpu::BufferUsage::CopyDst;
        m_Buffer = CreateRef<wgpu::Buffer>(contex->device.CreateBuffer(&descriptor));
    }

    void RUniformBuffer::UpdateData(const void* data, uint64_t size, uint64_t offset /*= 0*/)
    {
        m_Contex->queue.WriteBuffer(*m_Buffer, offset, data, size);
    }


}