#pragma once

#include "core/Core.h"
#include "render/RenderDefines.h"
#include "render/RBuffer.h"

namespace rush
{

    struct BindingInitializationHelper 
    {
        BindingInitializationHelper(uint32_t binding, Ref<RTexture> texture);

        BindingInitializationHelper(uint32_t binding,
            Ref<UniformBuffer> buffer,
            uint64_t offset = 0,
            uint64_t size = kWholeSize);

        BindingInitializationHelper(const BindingInitializationHelper&);
        ~BindingInitializationHelper();

        uint32_t binding;
        Ref<RTexture> texture;
        Ref<UniformBuffer> buffer;
        uint64_t offset = 0;
        uint64_t size = 0;
    };

    /// <summary>
    /// BindGroup
    /// </summary>
    class BindGroup
    {
    public:
        ~BindGroup();

    protected:
        friend class Renderer;

        BindGroup(Ref<RenderContex> contex, Ref<BindingLayout> layout, std::initializer_list<BindingInitializationHelper> entriesInitializer, const char* lable);

        static Ref<BindGroup> Construct(Ref<RenderContex> contex, Ref<BindingLayout> layout, std::initializer_list<BindingInitializationHelper> entriesInitializer, const char* lable)
        {
            return std::shared_ptr<BindGroup>(new BindGroup(contex, layout, entriesInitializer, lable));
        }

        Ref<wgpu::BindGroup> m_BindGroup;
    };

    //////////////////////////////////////////////////////////////////////////

    class UniformBuffer
    {
    public:
        ~UniformBuffer();

        void UpdateData(const void* data, uint64_t size, uint64_t offset = 0);

        Ref<wgpu::Buffer> GetBuffer() const { return m_Buffer; }

    protected:
        friend class Renderer;

        UniformBuffer(Ref<RenderContex> contex, BufferUsage usage, uint64_t size, const char* lable);

        static Ref<UniformBuffer> Construct(Ref<RenderContex> contex, BufferUsage usage, uint64_t size, const char* lable)
        {
            return std::shared_ptr<UniformBuffer>(new UniformBuffer(contex, usage, size, lable));
        }

        Ref<wgpu::Buffer> m_Buffer;
        Ref<wgpu::Queue> m_Queue;
    };

}