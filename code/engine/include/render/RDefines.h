#ifndef RDefines_h__
#define RDefines_h__


#ifdef RUSH_PLATFORM_WINDOWS
#define DAWN_ENABLE_BACKEND_D3D12
#elif (defined(RUSH_PLATFORM_LINUX) || defined(RUSH_PLATFORM_ANDROID))
#define DAWN_ENABLE_BACKEND_VULKAN
#elif (defined(RUSH_PLATFORM_MAC) || defined(RUSH_PLATFORM_IOS))
#define DAWN_ENABLE_BACKEND_METAL
#endif

#include <dawn/webgpu_cpp.h>

#define WGPU_RELEASE_RESOURCE(Type, Name)                                      \
  if (Name) {                                                                  \
    wgpu##Type##Release(Name);                                                 \
    Name = NULL;                                                               \
  }

namespace rush
{

    using TextureFormat = wgpu::TextureFormat;
    using TextureDimension = wgpu::TextureDimension;
    using ShaderStage = wgpu::ShaderStage;
    using BackendType = wgpu::BackendType;
    using BufferBindingType = wgpu::BufferBindingType;
    using SamplerBindingType = wgpu::SamplerBindingType;
    using TextureSampleType = wgpu::TextureSampleType;
    using TextureViewDimension = wgpu::TextureViewDimension;
    using BlendOperation = wgpu::BlendOperation;
    using BlendFactor = wgpu::BlendFactor;
    using VertexFormat = wgpu::VertexFormat;
    using PrimitiveTopology = wgpu::PrimitiveTopology;
    using FrontFace = wgpu::FrontFace;
    using CullMode = wgpu::CullMode;
    using ColorWriteMask = wgpu::ColorWriteMask;
    using CompareFunction = wgpu::CompareFunction;
    using IndexFormat = wgpu::IndexFormat;
    using AddressMode = wgpu::AddressMode;
    using FilterMode = wgpu::FilterMode;

}

#endif // RDefines_h__
