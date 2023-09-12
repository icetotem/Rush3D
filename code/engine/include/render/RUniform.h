#ifndef RUniform_h__
#define RUniform_h__

#include "core/Core.h"
#include "render/RDefines.h"
#include "render/RBuffer.h"
#include "render/RTexture.h"
#include "render/RLayout.h"

namespace rush
{

    //////////////////////////////////////////////////////////////////////////

    class BindingInitializationHelper 
    {
    public:
        BindingInitializationHelper(uint32_t binding, Ref<RTexture> texture);

        BindingInitializationHelper(uint32_t binding, Ref<RSampler> sampler);

        BindingInitializationHelper(uint32_t binding,
            Ref<RBuffer> buffer,
            uint64_t offset = 0,
            uint64_t size = wgpu::kWholeSize);

        BindingInitializationHelper(const BindingInitializationHelper&) = default;
        ~BindingInitializationHelper() = default;

        void AddBinding(std::vector<wgpu::BindGroupEntry>& entries) const;

        uint32_t GetBinding() const { return m_Binding; }
        Ref<RTexture> GetTexture() const { return m_Texture; }
        Ref<RSampler> GetSampler() const { return m_Sampler; }
        Ref<RBuffer> GetBuffer() const { return m_Buffer; }
        uint64_t GetSize() const { return m_Size; }
        uint64_t GetOffset() const { return m_Offset; }

    private:
        friend class RBindGroup;

        uint32_t m_Binding;
        Ref<RTexture> m_Texture;
        Ref<RSampler> m_Sampler;
        Ref<RBuffer> m_Buffer;
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

        Ref<BindingLayout> GetBindLayout() const { return m_BindLayout; }

        const wgpu::BindGroup& GetBindGroup() const { return m_BindGroup; }

    protected:
        friend class RenderContex;

        Ref<BindingLayout> m_BindLayout;
        wgpu::BindGroup m_BindGroup;
    };

}

#endif // RUniform_h__
