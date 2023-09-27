#include "stdafx.h"
#include "render/RLayout.h"
#include "render/RDevice.h"

namespace rush
{

    //////////////////////////////////////////////////////////////////////////

//     BindingLayoutHelper_t::BindingLayoutHelper_t(uint32_t entryBinding, ShaderStage entryVisibility, BufferBindingType bufferType, bool bufferHasDynamicOffset /*= false*/, uint64_t bufferMinBindingSize /*= 0*/)
//     {
//         m_Entry.binding = entryBinding;
//         m_Entry.visibility = entryVisibility;
//         m_Entry.buffer.type = bufferType;
//         m_Entry.buffer.hasDynamicOffset = bufferHasDynamicOffset;
//         m_Entry.buffer.minBindingSize = bufferMinBindingSize;
//     }
// 
//     BindingLayoutHelper_t::BindingLayoutHelper_t(uint32_t entryBinding, ShaderStage entryVisibility, SamplerBindingType samplerType)
//     {
//         m_Entry.binding = entryBinding;
//         m_Entry.visibility = entryVisibility;
//         m_Entry.sampler.type = samplerType;
//     }
// 
//     BindingLayoutHelper_t::BindingLayoutHelper_t(uint32_t entryBinding, ShaderStage entryVisibility, TextureSampleType textureSampleType, TextureViewDimension viewDimension /*= TextureViewDimension::e2D*/)
//     {
//         m_Entry.binding = entryBinding;
//         m_Entry.visibility = entryVisibility;
//         m_Entry.texture.sampleType = textureSampleType;
//         m_Entry.texture.viewDimension = viewDimension;
//     }
// 
//     BindingLayout::BindingLayout(std::initializer_list<BindingLayoutHelper_t> entriesInitializer, const char* lable)
//     {
//         std::vector<wgpu::BindGroupLayoutEntry> entries;
//         for (const BindingLayoutHelper_t& entry : entriesInitializer) 
//         {
//             entries.push_back(entry.GetEntry());
//         }
// 
//         wgpu::BindGroupLayoutDescriptor descriptor;
//         descriptor.entryCount = entries.size();
//         descriptor.entries = entries.data();
//         descriptor.label = lable;
//         m_Layout = RDevice::instance().GetDevice().CreateBindGroupLayout(&descriptor);
//     }

}
