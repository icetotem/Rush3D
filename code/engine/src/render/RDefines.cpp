#include "stdafx.h"
#include "render/RDefines.h"
#include "dawn/webgpu_cpp.h"

namespace rush
{
    HMap<wgpu::FeatureName, std::string> g_Features;
    HMap<WGPUAdapterType, std::string> g_AdapterType;
    HMap<WGPUBackendType, std::string> g_BackendTypeName;

    static class WGPUMapping
    {
    public:
        WGPUMapping()
        {
            g_Features = {
                {wgpu::FeatureName::Undefined, "Undefined"},
                {wgpu::FeatureName::DepthClipControl, "DepthClipControl"},
                {wgpu::FeatureName::Depth32FloatStencil8, "Depth32FloatStencil8"},
                {wgpu::FeatureName::TimestampQuery, "TimestampQuery"},
                {wgpu::FeatureName::PipelineStatisticsQuery, "PipelineStatisticsQuery"},
                {wgpu::FeatureName::TextureCompressionBC, "TextureCompressionBC"},
                {wgpu::FeatureName::TextureCompressionETC2, "TextureCompressionETC2"},
                {wgpu::FeatureName::TextureCompressionASTC, "TextureCompressionASTC"},
                {wgpu::FeatureName::IndirectFirstInstance, "IndirectFirstInstance"},
                {wgpu::FeatureName::ShaderF16, "ShaderF16"},
                {wgpu::FeatureName::RG11B10UfloatRenderable, "RG11B10UfloatRenderable"},
                {wgpu::FeatureName::BGRA8UnormStorage, "BGRA8UnormStorage"},
                {wgpu::FeatureName::Float32Filterable, "Float32Filterable"},
                {wgpu::FeatureName::DawnInternalUsages, "DawnInternalUsages"},
                {wgpu::FeatureName::DawnMultiPlanarFormats, "DawnMultiPlanarFormats"},
                {wgpu::FeatureName::DawnNative, "DawnNative"},
                {wgpu::FeatureName::ChromiumExperimentalDp4a, "ChromiumExperimentalDp4a"},
                {wgpu::FeatureName::TimestampQueryInsidePasses, "TimestampQueryInsidePasses"},
                {wgpu::FeatureName::ImplicitDeviceSynchronization, "ImplicitDeviceSynchronization"},
                {wgpu::FeatureName::SurfaceCapabilities, "SurfaceCapabilities"},
                {wgpu::FeatureName::TransientAttachments, "TransientAttachments"},
                {wgpu::FeatureName::MSAARenderToSingleSampled, "MSAARenderToSingleSampled"},
                {wgpu::FeatureName::DualSourceBlending, "DualSourceBlending"},
                {wgpu::FeatureName::D3D11MultithreadProtected, "D3D11MultithreadProtected"},
                {wgpu::FeatureName::ANGLETextureSharing, "ANGLETextureSharing"},
                {wgpu::FeatureName::ChromiumExperimentalSubgroups, "ChromiumExperimentalSubgroups"},
                {wgpu::FeatureName::ChromiumExperimentalSubgroupUniformControlFlow, "ChromiumExperimentalSubgroupUniformControlFlow"},
                {wgpu::FeatureName::ChromiumExperimentalReadWriteStorageTexture, "ChromiumExperimentalReadWriteStorageTexture"},
                {wgpu::FeatureName::PixelLocalStorageCoherent, "PixelLocalStorageCoherent"},
                {wgpu::FeatureName::PixelLocalStorageNonCoherent, "PixelLocalStorageNonCoherent"},
                {wgpu::FeatureName::Norm16TextureFormats, "Norm16TextureFormats"},
                {wgpu::FeatureName::SharedTextureMemoryVkDedicatedAllocation, "SharedTextureMemoryVkDedicatedAllocation"},
                {wgpu::FeatureName::SharedTextureMemoryAHardwareBuffer, "SharedTextureMemoryAHardwareBuffer"},
                {wgpu::FeatureName::SharedTextureMemoryDmaBuf, "SharedTextureMemoryDmaBuf"},
                {wgpu::FeatureName::SharedTextureMemoryOpaqueFD, "SharedTextureMemoryOpaqueFD"},
                {wgpu::FeatureName::SharedTextureMemoryZirconHandle, "SharedTextureMemoryZirconHandle"},
                {wgpu::FeatureName::SharedTextureMemoryDXGISharedHandle, "SharedTextureMemoryDXGISharedHandle"},
                {wgpu::FeatureName::SharedTextureMemoryD3D11Texture2D, "SharedTextureMemoryD3D11Texture2D"},
                {wgpu::FeatureName::SharedTextureMemoryIOSurface, "SharedTextureMemoryIOSurface"},
                {wgpu::FeatureName::SharedTextureMemoryEGLImage, "SharedTextureMemoryEGLImage"},
                {wgpu::FeatureName::SharedFenceVkSemaphoreOpaqueFD, "SharedFenceVkSemaphoreOpaqueFD"},
                {wgpu::FeatureName::SharedFenceVkSemaphoreSyncFD, "SharedFenceVkSemaphoreSyncFD"},
                {wgpu::FeatureName::SharedFenceVkSemaphoreZirconHandle, "SharedFenceVkSemaphoreZirconHandle"},
                {wgpu::FeatureName::SharedFenceDXGISharedHandle, "SharedFenceDXGISharedHandle"},
                {wgpu::FeatureName::SharedFenceMTLSharedEvent, "SharedFenceMTLSharedEvent"}
            };

            g_AdapterType = {
                {WGPUAdapterType_DiscreteGPU, "DiscreteGPU"},
                {WGPUAdapterType_IntegratedGPU, "IntegratedGPU"},
                {WGPUAdapterType_CPU, "CPU"},
                {WGPUAdapterType_Unknown, "Unknown"},
            };

            g_BackendTypeName = {
                {WGPUBackendType_Null, "Null"},
                {WGPUBackendType_WebGPU, "WebGPU"},
                {WGPUBackendType_D3D11, "D3D11"},
                {WGPUBackendType_D3D12, "D3D12"},
                {WGPUBackendType_Metal, "Metal"},
                {WGPUBackendType_Vulkan, "Vulkan"},
                {WGPUBackendType_OpenGL, "OpenGL"},
                {WGPUBackendType_OpenGLES, "OpenGLES"},
            };
        }
    } g_GPUInfo;

    wgpu::BackendType g_WGPUBackendType[(int)RenderBackend::Count] =
    {
        wgpu::BackendType::D3D11,
        wgpu::BackendType::D3D12,
        wgpu::BackendType::Metal,
        wgpu::BackendType::Vulkan,
        wgpu::BackendType::OpenGLES,
        wgpu::BackendType::OpenGL,
        wgpu::BackendType::Null,
    };

    wgpu::FrontFace g_WGPUFrontFace[(int)FrontFace::Count] =
    {
        wgpu::FrontFace::CCW, 
        wgpu::FrontFace::CW
    };

    wgpu::CullMode g_WGPUCullMode[(int)CullMode::Count] =
    {
        wgpu::CullMode::None, 
        wgpu::CullMode::Front, 
        wgpu::CullMode::Back
    };

    wgpu::PrimitiveTopology g_WGPUPrimitiveTopology[(int)PrimitiveType::Count] =
    {
        wgpu::PrimitiveTopology::PointList, 
        wgpu::PrimitiveTopology::LineList, 
        wgpu::PrimitiveTopology::LineStrip, 
        wgpu::PrimitiveTopology::TriangleList, 
        wgpu::PrimitiveTopology::TriangleStrip
    };

    wgpu::BlendOperation g_WGPUBlendOperation[(int)BlendOperation::Count] =
    {
        wgpu::BlendOperation::Add, 
        wgpu::BlendOperation::Subtract, 
        wgpu::BlendOperation::ReverseSubtract, 
        wgpu::BlendOperation::Min, 
        wgpu::BlendOperation::Max
    };

    wgpu::BlendFactor g_WGPUBlendFactor[(int)BlendFactor::Count] =
    {
        wgpu::BlendFactor::Zero,
        wgpu::BlendFactor::One,
        wgpu::BlendFactor::Src,
        wgpu::BlendFactor::OneMinusSrc,
        wgpu::BlendFactor::SrcAlpha,
        wgpu::BlendFactor::OneMinusSrcAlpha,
        wgpu::BlendFactor::Dst,
        wgpu::BlendFactor::OneMinusDst,
        wgpu::BlendFactor::DstAlpha,
        wgpu::BlendFactor::OneMinusDstAlpha,
        wgpu::BlendFactor::SrcAlphaSaturated,
        wgpu::BlendFactor::Constant,
        wgpu::BlendFactor::OneMinusConstant
    };

    wgpu::TextureDimension g_TextureDimension[(int)TextureDimension::Count]
    {
        wgpu::TextureDimension::e1D, 
        wgpu::TextureDimension::e2D, 
        wgpu::TextureDimension::e3D
    };

    wgpu::BufferUsage g_BufferUsage[(int)BufferUsage::Count] =
    {
        wgpu::BufferUsage::None,
        wgpu::BufferUsage::MapRead,
        wgpu::BufferUsage::MapWrite,
        wgpu::BufferUsage::CopySrc,
        wgpu::BufferUsage::CopyDst,
        wgpu::BufferUsage::Index,
        wgpu::BufferUsage::Vertex,
        wgpu::BufferUsage::Uniform,
        wgpu::BufferUsage::Storage,
        wgpu::BufferUsage::Indirect,
        wgpu::BufferUsage::QueryResolve,
    };

    wgpu::BufferBindingType g_BufferBindingType[(int)BufferBindingType::Count] =
    {
        wgpu::BufferBindingType::Undefined,
        wgpu::BufferBindingType::Uniform,
        wgpu::BufferBindingType::Storage,
        wgpu::BufferBindingType::ReadOnlyStorage
    };

    wgpu::SamplerBindingType g_SamplerBindingType[(int)SamplerBindingType::Count] =
    {
        wgpu::SamplerBindingType::Undefined,
        wgpu::SamplerBindingType::Filtering,
        wgpu::SamplerBindingType::NonFiltering,
        wgpu::SamplerBindingType::Comparison
    };

    wgpu::TextureSampleType g_TextureSampleType[(int)TextureSampleType::Count] =
    {
        wgpu::TextureSampleType::Undefined,
        wgpu::TextureSampleType::Float,
        wgpu::TextureSampleType::UnfilterableFloat,
        wgpu::TextureSampleType::Depth,
        wgpu::TextureSampleType::Sint,
        wgpu::TextureSampleType::Uint
    };

    wgpu::TextureViewDimension g_TextureViewDimension[(int)TextureViewDimension::Count] =
    {
        wgpu::TextureViewDimension::Undefined,
        wgpu::TextureViewDimension::e1D,
        wgpu::TextureViewDimension::e2D,
        wgpu::TextureViewDimension::e2DArray,
        wgpu::TextureViewDimension::Cube,
        wgpu::TextureViewDimension::CubeArray,
        wgpu::TextureViewDimension::e3D
    };

    wgpu::CompareFunction g_WGPUCompareFunction[(int)DepthCompareFunction::Count] =
    {
        wgpu::CompareFunction::Never, 
        wgpu::CompareFunction::Less, 
        wgpu::CompareFunction::Equal, 
        wgpu::CompareFunction::LessEqual,
        wgpu::CompareFunction::Greater,
        wgpu::CompareFunction::NotEqual,
        wgpu::CompareFunction::GreaterEqual,
        wgpu::CompareFunction::Always
    };

    wgpu::ShaderStage g_WGPUShaderStage[(int)ShaderStage::Count] =
    {
        wgpu::ShaderStage::None,
        wgpu::ShaderStage::Vertex,
        wgpu::ShaderStage::Fragment,
        wgpu::ShaderStage::Compute
    };

    wgpu::VertexFormat g_WGPUVertexFormat[(int)VertexFormat::Count] =
    {
        wgpu::VertexFormat::Uint8x2,
        wgpu::VertexFormat::Uint8x4,
        wgpu::VertexFormat::Sint8x2,
        wgpu::VertexFormat::Sint8x4,
        wgpu::VertexFormat::Unorm8x2,
        wgpu::VertexFormat::Unorm8x4,
        wgpu::VertexFormat::Snorm8x2,
        wgpu::VertexFormat::Snorm8x4,
        wgpu::VertexFormat::Uint16x2,
        wgpu::VertexFormat::Uint16x4,
        wgpu::VertexFormat::Sint16x2,
        wgpu::VertexFormat::Sint16x4,
        wgpu::VertexFormat::Unorm16x2,
        wgpu::VertexFormat::Unorm16x4,
        wgpu::VertexFormat::Snorm16x2,
        wgpu::VertexFormat::Snorm16x4,
        wgpu::VertexFormat::Float16x2,
        wgpu::VertexFormat::Float16x4,
        wgpu::VertexFormat::Float32,
        wgpu::VertexFormat::Float32x2,
        wgpu::VertexFormat::Float32x3,
        wgpu::VertexFormat::Float32x4,
        wgpu::VertexFormat::Uint32,
        wgpu::VertexFormat::Uint32x2,
        wgpu::VertexFormat::Uint32x3,
        wgpu::VertexFormat::Uint32x4,
        wgpu::VertexFormat::Sint32,
        wgpu::VertexFormat::Sint32x2,
        wgpu::VertexFormat::Sint32x3,
        wgpu::VertexFormat::Sint32x4
    };

    wgpu::TextureFormat g_WGPUTextureFormat[(int)TextureFormat::Count] =
    {
        wgpu::TextureFormat::Undefined,
        wgpu::TextureFormat::R8Unorm,
        wgpu::TextureFormat::R8Snorm,
        wgpu::TextureFormat::R8Uint,
        wgpu::TextureFormat::R8Sint,
        wgpu::TextureFormat::R16Uint,
        wgpu::TextureFormat::R16Sint,
        wgpu::TextureFormat::R16Float,
        wgpu::TextureFormat::RG8Unorm,
        wgpu::TextureFormat::RG8Snorm,
        wgpu::TextureFormat::RG8Uint,
        wgpu::TextureFormat::RG8Sint,
        wgpu::TextureFormat::R32Float,
        wgpu::TextureFormat::R32Uint,
        wgpu::TextureFormat::R32Sint,
        wgpu::TextureFormat::RG16Uint,
        wgpu::TextureFormat::RG16Sint,
        wgpu::TextureFormat::RG16Float,
        wgpu::TextureFormat::RGBA8Unorm,
        wgpu::TextureFormat::RGBA8UnormSrgb,
        wgpu::TextureFormat::RGBA8Snorm,
        wgpu::TextureFormat::RGBA8Uint,
        wgpu::TextureFormat::RGBA8Sint,
        wgpu::TextureFormat::BGRA8Unorm,
        wgpu::TextureFormat::BGRA8UnormSrgb,
        wgpu::TextureFormat::RGB10A2Unorm,
        wgpu::TextureFormat::RG11B10Ufloat,
        wgpu::TextureFormat::RGB9E5Ufloat,
        wgpu::TextureFormat::RG32Float,
        wgpu::TextureFormat::RG32Uint,
        wgpu::TextureFormat::RG32Sint,
        wgpu::TextureFormat::RGBA16Uint,
        wgpu::TextureFormat::RGBA16Sint,
        wgpu::TextureFormat::RGBA16Float,
        wgpu::TextureFormat::RGBA32Float,
        wgpu::TextureFormat::RGBA32Uint,
        wgpu::TextureFormat::RGBA32Sint,
        wgpu::TextureFormat::Stencil8,
        wgpu::TextureFormat::Depth16Unorm,
        wgpu::TextureFormat::Depth24Plus,
        wgpu::TextureFormat::Depth24PlusStencil8,
        wgpu::TextureFormat::Depth32Float,
        wgpu::TextureFormat::Depth32FloatStencil8,
        wgpu::TextureFormat::BC1RGBAUnorm,
        wgpu::TextureFormat::BC1RGBAUnormSrgb,
        wgpu::TextureFormat::BC2RGBAUnorm,
        wgpu::TextureFormat::BC2RGBAUnormSrgb,
        wgpu::TextureFormat::BC3RGBAUnorm,
        wgpu::TextureFormat::BC3RGBAUnormSrgb,
        wgpu::TextureFormat::BC4RUnorm,
        wgpu::TextureFormat::BC4RSnorm,
        wgpu::TextureFormat::BC5RGUnorm,
        wgpu::TextureFormat::BC5RGSnorm,
        wgpu::TextureFormat::BC6HRGBUfloat,
        wgpu::TextureFormat::BC6HRGBFloat,
        wgpu::TextureFormat::BC7RGBAUnorm,
        wgpu::TextureFormat::BC7RGBAUnormSrgb,
        wgpu::TextureFormat::ETC2RGB8Unorm,
        wgpu::TextureFormat::ETC2RGB8UnormSrgb,
        wgpu::TextureFormat::ETC2RGB8A1Unorm,
        wgpu::TextureFormat::ETC2RGB8A1UnormSrgb,
        wgpu::TextureFormat::ETC2RGBA8Unorm,
        wgpu::TextureFormat::ETC2RGBA8UnormSrgb,
        wgpu::TextureFormat::EACR11Unorm,
        wgpu::TextureFormat::EACR11Snorm,
        wgpu::TextureFormat::EACRG11Unorm,
        wgpu::TextureFormat::EACRG11Snorm,
        wgpu::TextureFormat::ASTC4x4Unorm,
        wgpu::TextureFormat::ASTC4x4UnormSrgb,
        wgpu::TextureFormat::ASTC5x4Unorm,
        wgpu::TextureFormat::ASTC5x4UnormSrgb,
        wgpu::TextureFormat::ASTC5x5Unorm,
        wgpu::TextureFormat::ASTC5x5UnormSrgb,
        wgpu::TextureFormat::ASTC6x5Unorm,
        wgpu::TextureFormat::ASTC6x5UnormSrgb,
        wgpu::TextureFormat::ASTC6x6Unorm,
        wgpu::TextureFormat::ASTC6x6UnormSrgb,
        wgpu::TextureFormat::ASTC8x5Unorm,
        wgpu::TextureFormat::ASTC8x5UnormSrgb,
        wgpu::TextureFormat::ASTC8x6Unorm,
        wgpu::TextureFormat::ASTC8x6UnormSrgb,
        wgpu::TextureFormat::ASTC8x8Unorm,
        wgpu::TextureFormat::ASTC8x8UnormSrgb,
        wgpu::TextureFormat::ASTC10x5Unorm,
        wgpu::TextureFormat::ASTC10x5UnormSrgb,
        wgpu::TextureFormat::ASTC10x6Unorm,
        wgpu::TextureFormat::ASTC10x6UnormSrgb,
        wgpu::TextureFormat::ASTC10x8Unorm,
        wgpu::TextureFormat::ASTC10x8UnormSrgb,
        wgpu::TextureFormat::ASTC10x10Unorm,
        wgpu::TextureFormat::ASTC10x10UnormSrgb,
        wgpu::TextureFormat::ASTC12x10Unorm,
        wgpu::TextureFormat::ASTC12x10UnormSrgb,
        wgpu::TextureFormat::ASTC12x12Unorm,
        wgpu::TextureFormat::ASTC12x12UnormSrgb,
        wgpu::TextureFormat::R8BG8Biplanar420Unorm
    };
}