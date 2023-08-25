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

    BindingInitializationHelper::BindingInitializationHelper(uint32_t binding, Ref<RUniformBuffer> buffer, uint64_t offset /*= 0*/, uint64_t size /*= kWholeSize*/)
    {
        this->m_Binding = binding;
        this->m_Buffer = buffer;
        this->m_Offset = offset;
        this->m_Size = size;
    }

    wgpu::BindGroupEntry GetAsBinding(const BindingInitializationHelper& helper)
    {
        wgpu::BindGroupEntry result = {};

        result.binding = helper.GetBinding();
        if (helper.GetTexture())
        {
            result.sampler = *helper.GetTexture()->GetSampler();
            result.textureView = *helper.GetTexture()->GetTextureView();
        }

        if (helper.GetBuffer())
            result.buffer = *helper.GetBuffer()->GetBuffer();

        result.offset = helper.GetOffset();
        result.size = helper.GetSize();

        return result;
    }

    static wgpu::BindGroup MakeBindGroup(
        const wgpu::Device& device,
        const wgpu::BindGroupLayout& layout,
        std::initializer_list<BindingInitializationHelper> entriesInitializer) 
    {
        std::vector<wgpu::BindGroupEntry> entries;
        for (const BindingInitializationHelper& helper : entriesInitializer) 
        {
            entries.push_back(GetAsBinding(helper));
        }

        wgpu::BindGroupDescriptor descriptor;
        descriptor.layout = layout;
        descriptor.entryCount = checked_cast<uint32_t>(entries.size());
        descriptor.entries = entries.data();

        return device.CreateBindGroup(&descriptor);
    }

    RBindGroup::RBindGroup(Ref<RContex> contex, Ref<BindingLayout> layout, std::initializer_list<BindingInitializationHelper> entriesInitializer, const char* lable)
    {
        m_BindGroup = CreateRef<wgpu::BindGroup>();
        *m_BindGroup = MakeBindGroup(contex->device, *layout->m_Layout, entriesInitializer);
    }

    RBindGroup::~RBindGroup()
    {
    }

    //////////////////////////////////////////////////////////////////////////

    RUniformBuffer::RUniformBuffer(Ref<RContex> contex, BufferUsage usage, uint64_t size, const char* lable)
    {
        m_Contex = contex;
        m_Buffer = CreateRef<wgpu::Buffer>();
        wgpu::BufferDescriptor descriptor;
        descriptor.size = size;

        descriptor.usage = g_BufferUsage[(int)usage] | wgpu::BufferUsage::CopyDst;
        *m_Buffer = contex->device.CreateBuffer(&descriptor);
    }

    RUniformBuffer::~RUniformBuffer()
    {

    }

    void RUniformBuffer::UpdateData(const void* data, uint64_t size, uint64_t offset /*= 0*/)
    {
        m_Contex->queue.WriteBuffer(*m_Buffer, offset, data, size);
    }


}