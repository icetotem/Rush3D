#ifndef RLayout_h__
#define RLayout_h__

#include "core/Core.h"
#include "render/RDefines.h"

namespace rush
{

//     class BindingLayoutHelper_t
//     {
//     public:
//         BindingLayoutHelper_t(uint32_t entryBinding,
//             ShaderStage entryVisibility,
//             BufferBindingType bufferType,
//             bool bufferHasDynamicOffset = false,
//             uint64_t bufferMinBindingSize = 0);
// 
//         BindingLayoutHelper_t(uint32_t entryBinding,
//             ShaderStage entryVisibility,
//             SamplerBindingType samplerType);
// 
//         BindingLayoutHelper_t(
//             uint32_t entryBinding,
//             ShaderStage entryVisibility,
//             TextureSampleType textureSampleType,
//             TextureViewDimension viewDimension);
// 
//         const wgpu::BindGroupLayoutEntry& GetEntry() const { return m_Entry; }
// 
//     private:
//         wgpu::BindGroupLayoutEntry m_Entry;
//     };
// 
//     class BindingLayout
//     {
//     public:
//         BindingLayout(std::initializer_list<BindingLayoutHelper_t> entriesInitializer, const char* lable = nullptr);
// 
//         ~BindingLayout() = default;
// 
//         const wgpu::BindGroupLayout& GetLayout() const { return m_Layout; }
// 
//     private:
//         friend class Renderer;
//         friend class RPipeline;
//         friend class RBindGroup;
// 
//         wgpu::BindGroupLayout m_Layout;
//     };

}

#endif // RLayout_h__
