#ifndef RBindGroup_h__
#define RBindGroup_h__

#include "core/Core.h"
#include "render/RDefines.h"
#include "render/RBuffer.h"
#include "render/RTexture.h"

namespace rush
{

    class RBindGroup
    {
    public:
        RBindGroup() = default;
        ~RBindGroup() = default;

        void AddBinding(uint32_t binding, ShaderStage visibility, Ref<RTexture> texture, TextureSampleType textureSampleType, TextureViewDimension viewDimension);
        void AddBinding(uint32_t binding, ShaderStage visibility, wgpu::Sampler sampler, SamplerBindingType samplerType);
        void AddBinding(uint32_t binding, ShaderStage visibility, Ref<RBuffer> buffer, wgpu::BufferBindingType bufferType);
        void Create(const char* lable = nullptr);

        const wgpu::BindGroupLayout& GetBindLayoutHandle() const { return m_BindLayout; }

        const wgpu::BindGroup& GetBindGroupHandle() const { return m_BindGroup; }

    protected:
        wgpu::BindGroupLayout m_BindLayout;
        wgpu::BindGroup m_BindGroup;

        DArray<wgpu::BindGroupLayoutEntry> bgLayoutEntris;
        DArray<wgpu::BindGroupEntry> bgEntries;
    };

}

#endif // RBindGroup_h__
