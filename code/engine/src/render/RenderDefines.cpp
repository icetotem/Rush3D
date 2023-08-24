#include "stdafx.h"
#include "render/RenderDefines.h"
#include "dawn/webgpu.h"

namespace rush
{
    HMap<WGPUFeatureName, std::string> g_Features;
    HMap<WGPUAdapterType, std::string> g_AdapterType;
    HMap<WGPUBackendType, std::string> g_BackendType;

    static class WGPUMapping
    {
    public:
        WGPUMapping()
        {
            g_Features = {
                {WGPUFeatureName_Undefined, "Undefined"},
                {WGPUFeatureName_DepthClipControl, "DepthClipControl"},
                {WGPUFeatureName_Depth32FloatStencil8, "Depth32FloatStencil8"},
                {WGPUFeatureName_TimestampQuery, "TimestampQuery"},
                {WGPUFeatureName_PipelineStatisticsQuery, "PipelineStatisticsQuery"},
                {WGPUFeatureName_TextureCompressionBC, "TextureCompressionBC"},
                {WGPUFeatureName_TextureCompressionETC2, "TextureCompressionETC2"},
                {WGPUFeatureName_TextureCompressionASTC, "TextureCompressionASTC"},
                {WGPUFeatureName_IndirectFirstInstance, "IndirectFirstInstance"},
                {WGPUFeatureName_ShaderF16, "ShaderF16"},
                {WGPUFeatureName_RG11B10UfloatRenderable, "RG11B10UfloatRenderable"},
                {WGPUFeatureName_BGRA8UnormStorage, "BGRA8UnormStorage"},
                {WGPUFeatureName_Float32Filterable, "Float32Filterable"},
                {WGPUFeatureName_DawnInternalUsages, "DawnInternalUsages"},
                {WGPUFeatureName_DawnMultiPlanarFormats, "DawnMultiPlanarFormats"},
                {WGPUFeatureName_DawnNative, "DawnNative"},
                {WGPUFeatureName_ChromiumExperimentalDp4a, "ChromiumExperimentalDp4a"},
                {WGPUFeatureName_TimestampQueryInsidePasses, "TimestampQueryInsidePasses"},
                {WGPUFeatureName_ImplicitDeviceSynchronization, "ImplicitDeviceSynchronization"},
                {WGPUFeatureName_SurfaceCapabilities, "SurfaceCapabilities"},
                {WGPUFeatureName_TransientAttachments, "TransientAttachments"},
                {WGPUFeatureName_MSAARenderToSingleSampled, "MSAARenderToSingleSampled"},
                {WGPUFeatureName_DualSourceBlending, "DualSourceBlending"},
                {WGPUFeatureName_D3D11MultithreadProtected, "D3D11MultithreadProtected"},
                {WGPUFeatureName_ANGLETextureSharing, "ANGLETextureSharing"},
                {WGPUFeatureName_ChromiumExperimentalSubgroups, "ChromiumExperimentalSubgroups"},
                {WGPUFeatureName_ChromiumExperimentalSubgroupUniformControlFlow, "ChromiumExperimentalSubgroupUniformControlFlow"},
                {WGPUFeatureName_ChromiumExperimentalReadWriteStorageTexture, "ChromiumExperimentalReadWriteStorageTexture"},
                {WGPUFeatureName_PixelLocalStorageCoherent, "PixelLocalStorageCoherent"},
                {WGPUFeatureName_PixelLocalStorageNonCoherent, "PixelLocalStorageNonCoherent"},
                {WGPUFeatureName_Norm16TextureFormats, "Norm16TextureFormats"},
                {WGPUFeatureName_SharedTextureMemoryVkDedicatedAllocation, "SharedTextureMemoryVkDedicatedAllocation"},
                {WGPUFeatureName_SharedTextureMemoryAHardwareBuffer, "SharedTextureMemoryAHardwareBuffer"},
                {WGPUFeatureName_SharedTextureMemoryDmaBuf, "SharedTextureMemoryDmaBuf"},
                {WGPUFeatureName_SharedTextureMemoryOpaqueFD, "SharedTextureMemoryOpaqueFD"},
                {WGPUFeatureName_SharedTextureMemoryZirconHandle, "SharedTextureMemoryZirconHandle"},
                {WGPUFeatureName_SharedTextureMemoryDXGISharedHandle, "SharedTextureMemoryDXGISharedHandle"},
                {WGPUFeatureName_SharedTextureMemoryD3D11Texture2D, "SharedTextureMemoryD3D11Texture2D"},
                {WGPUFeatureName_SharedTextureMemoryIOSurface, "SharedTextureMemoryIOSurface"},
                {WGPUFeatureName_SharedTextureMemoryEGLImage, "SharedTextureMemoryEGLImage"},
                {WGPUFeatureName_SharedFenceVkSemaphoreOpaqueFD, "SharedFenceVkSemaphoreOpaqueFD"},
                {WGPUFeatureName_SharedFenceVkSemaphoreSyncFD, "SharedFenceVkSemaphoreSyncFD"},
                {WGPUFeatureName_SharedFenceVkSemaphoreZirconHandle, "SharedFenceVkSemaphoreZirconHandle"},
                {WGPUFeatureName_SharedFenceDXGISharedHandle, "SharedFenceDXGISharedHandle"},
                {WGPUFeatureName_SharedFenceMTLSharedEvent, "SharedFenceMTLSharedEvent"}
            };

            g_AdapterType = {
                {WGPUAdapterType_DiscreteGPU, "DiscreteGPU"},
                {WGPUAdapterType_IntegratedGPU, "IntegratedGPU"},
                {WGPUAdapterType_CPU, "CPU"},
                {WGPUAdapterType_Unknown, "Unknown"},
            };

            g_BackendType = {
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

    WGPUFrontFace g_WGPUFrontFace[(int)FrontFace::Count] =
    {
        WGPUFrontFace_CCW, 
        WGPUFrontFace_CW
    };

    WGPUCullMode g_WGPUCullMode[(int)CullMode::Count] =
    {
        WGPUCullMode_None, 
        WGPUCullMode_Front, 
        WGPUCullMode_Back
    };

    WGPUPrimitiveTopology g_WGPUPrimitiveTopology[(int)PrimitiveType::Count] =
    {
        WGPUPrimitiveTopology_PointList, 
        WGPUPrimitiveTopology_LineList, 
        WGPUPrimitiveTopology_LineStrip, 
        WGPUPrimitiveTopology_TriangleList, 
        WGPUPrimitiveTopology_TriangleStrip
    };

    WGPUBlendOperation g_WGPUBlendOperation[(int)BlendOperation::Count] =
    {
        WGPUBlendOperation_Add, 
        WGPUBlendOperation_Subtract, 
        WGPUBlendOperation_ReverseSubtract, 
        WGPUBlendOperation_Min, 
        WGPUBlendOperation_Max
    };

    WGPUBlendFactor g_WGPUBlendFactor[(int)BlendFactor::Count] =
    {
        WGPUBlendFactor_Zero,
        WGPUBlendFactor_One,
        WGPUBlendFactor_Src,
        WGPUBlendFactor_OneMinusSrc,
        WGPUBlendFactor_SrcAlpha,
        WGPUBlendFactor_OneMinusSrcAlpha,
        WGPUBlendFactor_Dst,
        WGPUBlendFactor_OneMinusDst,
        WGPUBlendFactor_DstAlpha,
        WGPUBlendFactor_OneMinusDstAlpha,
        WGPUBlendFactor_SrcAlphaSaturated,
        WGPUBlendFactor_Constant,
        WGPUBlendFactor_OneMinusConstant
    };

    WGPUTextureDimension g_TextureDimension[(int)TextureDimension::Count]
    {
        WGPUTextureDimension_1D, 
        WGPUTextureDimension_2D, 
        WGPUTextureDimension_3D
    };

    WGPUCompareFunction g_WGPUCompareFunction[(int)DepthCompareFunction::Count] =
    {
        WGPUCompareFunction_Never, 
        WGPUCompareFunction_Less, 
        WGPUCompareFunction_Equal, 
        WGPUCompareFunction_LessEqual,
        WGPUCompareFunction_Greater,
        WGPUCompareFunction_NotEqual,
        WGPUCompareFunction_GreaterEqual,
        WGPUCompareFunction_Always
    };

    WGPUVertexFormat g_WGPUVertexFormat[(int)VertexFormat::Count] =
    {
        WGPUVertexFormat_Uint8x2,
        WGPUVertexFormat_Uint8x4,
        WGPUVertexFormat_Sint8x2,
        WGPUVertexFormat_Sint8x4,
        WGPUVertexFormat_Unorm8x2,
        WGPUVertexFormat_Unorm8x4,
        WGPUVertexFormat_Snorm8x2,
        WGPUVertexFormat_Snorm8x4,
        WGPUVertexFormat_Uint16x2,
        WGPUVertexFormat_Uint16x4,
        WGPUVertexFormat_Sint16x2,
        WGPUVertexFormat_Sint16x4,
        WGPUVertexFormat_Unorm16x2,
        WGPUVertexFormat_Unorm16x4,
        WGPUVertexFormat_Snorm16x2,
        WGPUVertexFormat_Snorm16x4,
        WGPUVertexFormat_Float16x2,
        WGPUVertexFormat_Float16x4,
        WGPUVertexFormat_Float32,
        WGPUVertexFormat_Float32x2,
        WGPUVertexFormat_Float32x3,
        WGPUVertexFormat_Float32x4,
        WGPUVertexFormat_Uint32,
        WGPUVertexFormat_Uint32x2,
        WGPUVertexFormat_Uint32x3,
        WGPUVertexFormat_Uint32x4,
        WGPUVertexFormat_Sint32,
        WGPUVertexFormat_Sint32x2,
        WGPUVertexFormat_Sint32x3,
        WGPUVertexFormat_Sint32x4
    };

    WGPUTextureFormat g_WGPUTextureFormat[(int)TextureFormat::Count] =
    {
        WGPUTextureFormat_Undefined,
        WGPUTextureFormat_R8Unorm,
        WGPUTextureFormat_R8Snorm,
        WGPUTextureFormat_R8Uint,
        WGPUTextureFormat_R8Sint,
        WGPUTextureFormat_R16Uint,
        WGPUTextureFormat_R16Sint,
        WGPUTextureFormat_R16Float,
        WGPUTextureFormat_RG8Unorm,
        WGPUTextureFormat_RG8Snorm,
        WGPUTextureFormat_RG8Uint,
        WGPUTextureFormat_RG8Sint,
        WGPUTextureFormat_R32Float,
        WGPUTextureFormat_R32Uint,
        WGPUTextureFormat_R32Sint,
        WGPUTextureFormat_RG16Uint,
        WGPUTextureFormat_RG16Sint,
        WGPUTextureFormat_RG16Float,
        WGPUTextureFormat_RGBA8Unorm,
        WGPUTextureFormat_RGBA8UnormSrgb,
        WGPUTextureFormat_RGBA8Snorm,
        WGPUTextureFormat_RGBA8Uint,
        WGPUTextureFormat_RGBA8Sint,
        WGPUTextureFormat_BGRA8Unorm,
        WGPUTextureFormat_BGRA8UnormSrgb,
        WGPUTextureFormat_RGB10A2Unorm,
        WGPUTextureFormat_RG11B10Ufloat,
        WGPUTextureFormat_RGB9E5Ufloat,
        WGPUTextureFormat_RG32Float,
        WGPUTextureFormat_RG32Uint,
        WGPUTextureFormat_RG32Sint,
        WGPUTextureFormat_RGBA16Uint,
        WGPUTextureFormat_RGBA16Sint,
        WGPUTextureFormat_RGBA16Float,
        WGPUTextureFormat_RGBA32Float,
        WGPUTextureFormat_RGBA32Uint,
        WGPUTextureFormat_RGBA32Sint,
        WGPUTextureFormat_Stencil8,
        WGPUTextureFormat_Depth16Unorm,
        WGPUTextureFormat_Depth24Plus,
        WGPUTextureFormat_Depth24PlusStencil8,
        WGPUTextureFormat_Depth32Float,
        WGPUTextureFormat_Depth32FloatStencil8,
        WGPUTextureFormat_BC1RGBAUnorm,
        WGPUTextureFormat_BC1RGBAUnormSrgb,
        WGPUTextureFormat_BC2RGBAUnorm,
        WGPUTextureFormat_BC2RGBAUnormSrgb,
        WGPUTextureFormat_BC3RGBAUnorm,
        WGPUTextureFormat_BC3RGBAUnormSrgb,
        WGPUTextureFormat_BC4RUnorm,
        WGPUTextureFormat_BC4RSnorm,
        WGPUTextureFormat_BC5RGUnorm,
        WGPUTextureFormat_BC5RGSnorm,
        WGPUTextureFormat_BC6HRGBUfloat,
        WGPUTextureFormat_BC6HRGBFloat,
        WGPUTextureFormat_BC7RGBAUnorm,
        WGPUTextureFormat_BC7RGBAUnormSrgb,
        WGPUTextureFormat_ETC2RGB8Unorm,
        WGPUTextureFormat_ETC2RGB8UnormSrgb,
        WGPUTextureFormat_ETC2RGB8A1Unorm,
        WGPUTextureFormat_ETC2RGB8A1UnormSrgb,
        WGPUTextureFormat_ETC2RGBA8Unorm,
        WGPUTextureFormat_ETC2RGBA8UnormSrgb,
        WGPUTextureFormat_EACR11Unorm,
        WGPUTextureFormat_EACR11Snorm,
        WGPUTextureFormat_EACRG11Unorm,
        WGPUTextureFormat_EACRG11Snorm,
        WGPUTextureFormat_ASTC4x4Unorm,
        WGPUTextureFormat_ASTC4x4UnormSrgb,
        WGPUTextureFormat_ASTC5x4Unorm,
        WGPUTextureFormat_ASTC5x4UnormSrgb,
        WGPUTextureFormat_ASTC5x5Unorm,
        WGPUTextureFormat_ASTC5x5UnormSrgb,
        WGPUTextureFormat_ASTC6x5Unorm,
        WGPUTextureFormat_ASTC6x5UnormSrgb,
        WGPUTextureFormat_ASTC6x6Unorm,
        WGPUTextureFormat_ASTC6x6UnormSrgb,
        WGPUTextureFormat_ASTC8x5Unorm,
        WGPUTextureFormat_ASTC8x5UnormSrgb,
        WGPUTextureFormat_ASTC8x6Unorm,
        WGPUTextureFormat_ASTC8x6UnormSrgb,
        WGPUTextureFormat_ASTC8x8Unorm,
        WGPUTextureFormat_ASTC8x8UnormSrgb,
        WGPUTextureFormat_ASTC10x5Unorm,
        WGPUTextureFormat_ASTC10x5UnormSrgb,
        WGPUTextureFormat_ASTC10x6Unorm,
        WGPUTextureFormat_ASTC10x6UnormSrgb,
        WGPUTextureFormat_ASTC10x8Unorm,
        WGPUTextureFormat_ASTC10x8UnormSrgb,
        WGPUTextureFormat_ASTC10x10Unorm,
        WGPUTextureFormat_ASTC10x10UnormSrgb,
        WGPUTextureFormat_ASTC12x10Unorm,
        WGPUTextureFormat_ASTC12x10UnormSrgb,
        WGPUTextureFormat_ASTC12x12Unorm,
        WGPUTextureFormat_ASTC12x12UnormSrgb,
        WGPUTextureFormat_R8BG8Biplanar420Unorm
    };
}