#ifndef RUniform_h__
#define RUniform_h__

#include "core/Core.h"
#include "render/RDefines.h"
#include "render/RBuffer.h"
#include "render/RTexture.h"
#include "render/RLayout.h"

namespace rush
{

    class RUniformBuffer
    {
    public:
        RUniformBuffer(BufferUsage usage, uint64_t size, const char* lable = nullptr);

        ~RUniformBuffer() = default;

        void UpdateData(const void* data, uint64_t size, uint64_t offset = 0);

        Ref<wgpu::Buffer> GetBuffer() const { return m_Buffer; }

    protected:
        friend class Renderer;

        Ref<wgpu::Buffer> m_Buffer;
    };

    //////////////////////////////////////////////////////////////////////////

    class BindingInitializationHelper 
    {
    public:
        BindingInitializationHelper(uint32_t binding, Ref<RTexture> texture);

        BindingInitializationHelper(uint32_t binding, Ref<RSampler> sampler);

        BindingInitializationHelper(uint32_t binding,
            Ref<RUniformBuffer> buffer,
            uint64_t offset = 0,
            uint64_t size = kWholeSize);

        BindingInitializationHelper(const BindingInitializationHelper&) = default;
        ~BindingInitializationHelper() = default;

        uint32_t GetBinding() const { return m_Binding; }
        Ref<RTexture> GetTexture() const { return m_Texture; }
        Ref<RSampler> GetSampler() const { return m_Sampler; }
        Ref<RUniformBuffer> GetBuffer() const { return m_Buffer; }
        uint64_t GetSize() const { return m_Size; }
        uint64_t GetOffset() const { return m_Offset; }

    private:
        friend class RBindGroup;

        uint32_t m_Binding;
        Ref<RTexture> m_Texture;
        Ref<RSampler> m_Sampler;
        Ref<RUniformBuffer> m_Buffer;
        uint64_t m_Offset = 0;
        uint64_t m_Size = 0;
    };

    /// <summary>
    /// BindGroup
    /// </summary>
    class RBindGroup
    {
    public:
        RBindGroup(Ref<BindingLayout> layout, std::initializer_list<BindingInitializationHelper> entriesInitializer, const char* lable = nullptr);

        ~RBindGroup();

        Ref<BindingLayout> GetBindingLayout() const { return m_BindLayout; }

    protected:
        friend class Renderer;

        Ref<BindingLayout> m_BindLayout;
        Ref<wgpu::BindGroup> m_BindGroup;
    };

}

#endif // RUniform_h__
