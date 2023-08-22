#pragma once

namespace rush
{

    enum class FrontFace
    {
        CCW,
        CW,
        Count
    };

    enum class CullMode
    {
        None,
        Front,
        Back,
        Count
    };

    enum class PrimitiveType
    {
        PointList,
        LineList,
        LineStrip,
        TriangleList,
        TriangleStrip,
        Count
    };

    enum class BlendOperation
    {
        Add,
        Sub,
        RevSub,
        Min,
        Max,
        Count
    };

    enum ColorWriteMask
    {
        Write_None = 0x00000000,
        Write_R = 0x00000001,
        Write_G = 0x00000002,
        Write_B = 0x00000004,
        Write_A = 0x00000008,
        Write_All = 0x0000000F,
    };

    enum class BlendFactor
    {
        Zero,
        One,
        SrcColor,
        InvSrcColor,    // 1 - src.rgb
        SrcAlpha,
        InvSrcAlpha,    // 1 - src.a
        DstColor,
        InvDstColor,    // 1 - dst.rgb
        DstAlpha,
        InvDstAlpha,    // 1 - dst.a
        AlphaSaturate,  // min(src.a, 1 - dst.a)
        Const,          // const.rgb
        InvConst,       // 1 - const.rgb
        Count
    };

    enum class VertexFormat
    {
        Uint8x2,
        Uint8x4,
        Sint8x2,
        Sint8x4,
        Unorm8x2,
        Unorm8x4,
        Snorm8x2,
        Snorm8x4,
        Uint16x2,
        Uint16x4,
        Sint16x2,
        Sint16x4,
        Unorm16x2,
        Unorm16x4,
        Snorm16x2,
        Snorm16x4,
        Float16x2,
        Float16x4,
        Float32,
        Float32x2,
        Float32x3,
        Float32x4,
        Uint32,
        Uint32x2,
        Uint32x3,
        Uint32x4,
        Sint32,
        Sint32x2,
        Sint32x3,
        Sint32x4,
        Count
    };

    enum class TextureFormat
    {
        Undefined,
        R8Unorm,
        R8Snorm,
        R8Uint,
        R8Sint,
        R16Uint,
        R16Sint,
        R16Float,
        RG8Unorm,
        RG8Snorm,
        RG8Uint,
        RG8Sint,
        R32Float,
        R32Uint,
        R32Sint,
        RG16Uint,
        RG16Sint,
        RG16Float,
        RGBA8Unorm,
        RGBA8UnormSrgb,
        RGBA8Snorm,
        RGBA8Uint,
        RGBA8Sint,
        BGRA8Unorm,
        BGRA8UnormSrgb,
        RGB10A2Unorm,
        RG11B10Ufloat,
        RGB9E5Ufloat,
        RG32Float,
        RG32Uint,
        RG32Sint,
        RGBA16Uint,
        RGBA16Sint,
        RGBA16Float,
        RGBA32Float,
        RGBA32Uint,
        RGBA32Sint,
        Stencil8,
        Depth16Unorm,
        Depth24Plus,
        Depth24PlusStencil8,
        Depth32Float,
        Depth32FloatStencil8,
        BC1RGBAUnorm,
        BC1RGBAUnormSrgb,
        BC2RGBAUnorm,
        BC2RGBAUnormSrgb,
        BC3RGBAUnorm,
        BC3RGBAUnormSrgb,
        BC4RUnorm,
        BC4RSnorm,
        BC5RGUnorm,
        BC5RGSnorm,
        BC6HRGBUfloat,
        BC6HRGBFloat,
        BC7RGBAUnorm,
        BC7RGBAUnormSrgb,
        ETC2RGB8Unorm,
        ETC2RGB8UnormSrgb,
        ETC2RGB8A1Unorm,
        ETC2RGB8A1UnormSrgb,
        ETC2RGBA8Unorm,
        ETC2RGBA8UnormSrgb,
        EACR11Unorm,
        EACR11Snorm,
        EACRG11Unorm,
        EACRG11Snorm,
        ASTC4x4Unorm,
        ASTC4x4UnormSrgb,
        ASTC5x4Unorm,
        ASTC5x4UnormSrgb,
        ASTC5x5Unorm,
        ASTC5x5UnormSrgb,
        ASTC6x5Unorm,
        ASTC6x5UnormSrgb,
        ASTC6x6Unorm,
        ASTC6x6UnormSrgb,
        ASTC8x5Unorm,
        ASTC8x5UnormSrgb,
        ASTC8x6Unorm,
        ASTC8x6UnormSrgb,
        ASTC8x8Unorm,
        ASTC8x8UnormSrgb,
        ASTC10x5Unorm,
        ASTC10x5UnormSrgb,
        ASTC10x6Unorm,
        ASTC10x6UnormSrgb,
        ASTC10x8Unorm,
        ASTC10x8UnormSrgb,
        ASTC10x10Unorm,
        ASTC10x10UnormSrgb,
        ASTC12x10Unorm,
        ASTC12x10UnormSrgb,
        ASTC12x12Unorm,
        ASTC12x12UnormSrgb,
        R8BG8Biplanar420Unorm,
        Count
    };

    enum class DepthCompareFunction
    {
        Never,
        Less,
        LessEqual,
        Greater,
        GreaterEqual,
        Equal,
        NotEqual,
        Always,
        Count
    };

    enum ShaderStage
    {
        None = 0x00000000,
        Vertex = 0x00000001,
        Fragment = 0x00000002,
        Compute = 0x00000004
    };

    enum class TextureDimension
    {
        Texture1D, 
        Texture2D,
        Texture3D,
        Count,
    };

    class RTexture;
    class RBuffer;
    class Shader;
    class UniformBuffer;
    struct PipelineDesc;
    class RPipeline;

}

struct WGPUAdapterImpl;
struct WGPUBindGroupImpl;
struct WGPUBindGroupLayoutImpl;
struct WGPUBufferImpl;
struct WGPUCommandBufferImpl;
struct WGPUCommandEncoderImpl;
struct WGPUComputePassEncoderImpl;
struct WGPUComputePipelineImpl;
struct WGPUDeviceImpl;
struct WGPUExternalTextureImpl;
struct WGPUInstanceImpl;
struct WGPUPipelineLayoutImpl;
struct WGPUQuerySetImpl;
struct WGPUQueueImpl;
struct WGPURenderBundleImpl;
struct WGPURenderBundleEncoderImpl;
struct WGPURenderPassEncoderImpl;
struct WGPURenderPipelineImpl;
struct WGPUSamplerImpl;
struct WGPUShaderModuleImpl;
struct WGPUSurfaceImpl;
struct WGPUSwapChainImpl;
struct WGPUTextureImpl;
struct WGPUTextureViewImpl;


#define WGPU_RELEASE_RESOURCE(Type, Name)                                      \
  if (Name) {                                                                  \
    wgpu##Type##Release(Name);                                                 \
    Name = NULL;                                                               \
  }