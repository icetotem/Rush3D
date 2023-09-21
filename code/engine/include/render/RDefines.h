#ifndef RDefines_h__
#define RDefines_h__

#include <dawn/webgpu_cpp.h>
#include <dawn/webgpu_cpp_print.h>
#include <dawn/native/DawnNative.h>

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
    using TextureUsage = wgpu::TextureUsage;

}

#endif // RDefines_h__
