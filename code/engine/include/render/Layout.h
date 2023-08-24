#pragma once

#include "core/Core.h"
#include "RenderDefines.h"

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

    struct BindingLayoutHelper
    {
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

    
        Ref<wgpu::BindGroupLayoutEntry> m_Entry;
    };

    class BindingLayout
    {
    public:
        ~BindingLayout();

    private:
        friend class Renderer;
        friend class RenderPipeline;
        friend class BindGroup;

        BindingLayout(Ref<RenderContex> contex, std::initializer_list<BindingLayoutHelper> entriesInitializer);

        static Ref<BindingLayout> Construct(Ref<RenderContex> contex, std::initializer_list<BindingLayoutHelper> entriesInitializer)
        {
            return std::shared_ptr<BindingLayout>(new BindingLayout(contex, entriesInitializer));
        }

        Ref<wgpu::BindGroupLayout> m_Layout;
    };

}

