#ifndef RLayout_h__
#define RLayout_h__

#include "core/Core.h"
#include "render/RDefines.h"

namespace rush
{

    struct VertexAttribute
    {
        VertexFormat format = VertexFormat::Float32;
        uint64_t offset = 0;
        uint32_t shaderLocation = 0;
    };

    struct VertexLayout
    {
        uint64_t stride = 0;
        uint32_t attributeCount = 0;
        VertexAttribute* attributes = nullptr;
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
            TextureViewDimension viewDimension);

        Ref<wgpu::BindGroupLayoutEntry> GetEntry() const { return m_Entry; }

    private:
        Ref<wgpu::BindGroupLayoutEntry> m_Entry;
    };

    class BindingLayout
    {
    public:
        ~BindingLayout() = default;

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

#endif // RLayout_h__
