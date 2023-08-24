#include "stdafx.h"
#include "render/Uniform.h"
#include "dawn/webgpu_cpp.h"
#include "render/Layout.h"
#include "RenderContex.h"
#include "render/RTexture.h"

namespace rush
{

    BindingInitializationHelper::BindingInitializationHelper(const BindingInitializationHelper&) =
        default;

    BindingInitializationHelper::BindingInitializationHelper(uint32_t binding, Ref<RTexture> texture)
    {
        this->binding = binding;
        this->texture = texture;
    }

    BindingInitializationHelper::BindingInitializationHelper(uint32_t binding, Ref<UniformBuffer> buffer, uint64_t offset /*= 0*/, uint64_t size /*= kWholeSize*/)
    {
        this->binding = binding;
        this->buffer = buffer;
        this->offset = offset;
        this->size = size;
    }

    BindingInitializationHelper::~BindingInitializationHelper() = default;

    wgpu::BindGroupEntry GetAsBinding(const BindingInitializationHelper& helper)
    {
        wgpu::BindGroupEntry result = {};

        result.binding = helper.binding;
        if (helper.texture)
        {
            result.sampler = *helper.texture->GetSampler();
            result.textureView = *helper.texture->GetTextureView();
        }

        if (helper.buffer)
            result.buffer = *helper.buffer->GetBuffer();

        result.offset = helper.offset;
        result.size = helper.size;

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

    BindGroup::BindGroup(Ref<RenderContex> contex, Ref<BindingLayout> layout, std::initializer_list<BindingInitializationHelper> entriesInitializer, const char* lable)
    {
        m_BindGroup = CreateRef<wgpu::BindGroup>();
        *m_BindGroup = MakeBindGroup(contex->device, *layout->m_Layout, entriesInitializer);
    }

    BindGroup::~BindGroup()
    {
    }

    //////////////////////////////////////////////////////////////////////////

    UniformBuffer::~UniformBuffer()
    {

    }

    void UniformBuffer::UpdateData(const void* data, uint64_t size, uint64_t offset /*= 0*/)
    {

    }

    UniformBuffer::UniformBuffer(Ref<RenderContex> contex, uint64_t size, const char* lable)
    {
        m_Buffer = CreateRef<wgpu::Buffer>();

    }

}