#pragma once

#include "core/Core.h"
#include "render/RDefines.h"

namespace rush
{

    struct VertexAttribute
    {
        VertexFormat Format = VertexFormat::Float32;
        uint64_t Offset = 0;
        uint32_t ShaderLocation = 0;
    };

    struct VertexLayout
    {
        uint64_t Stride = 0;
        uint32_t AttributeCount = 0;
        VertexAttribute* Attributes = nullptr;
    };

    class BindingLayoutHelper
    {
    public:
        BindingLayoutHelper(uint32_t entryBinding,
            ShaderStage entryVisibility,
            BufferBindingType bufferType,
            bool bufferHasDynamicOffset = false,
            uint64_t bufferMinBindingSize = 0);

        BindingLayoutHelper(uint32_t entryBinding,
            ShaderStage entryVisibility,
            SamplerBindingType samplerType);

        BindingLayoutHelper(
            uint32_t entryBinding,
            ShaderStage entryVisibility,
            TextureSampleType textureSampleType,
            TextureViewDimension viewDimension = TextureViewDimension::e2D,
            bool textureMultisampled = false);

        Ref<wgpu::BindGroupLayoutEntry> GetEntry() const { return m_Entry; }

    private:
        Ref<wgpu::BindGroupLayoutEntry> m_Entry;
    };

    class BindingLayout
    {
    public:
        ~BindingLayout();

    private:
        friend class Renderer;
        friend class RPipeline;
        friend class RBindGroup;

        BindingLayout(Ref<RContex> contex, std::initializer_list<BindingLayoutHelper> entriesInitializer, const char* lable);

        static Ref<BindingLayout> Construct(Ref<RContex> contex, std::initializer_list<BindingLayoutHelper> entriesInitializer, const char* lable)
        {
            return std::shared_ptr<BindingLayout>(new BindingLayout(contex, entriesInitializer, lable));
        }

        Ref<wgpu::BindGroupLayout> m_Layout;
    };

}

