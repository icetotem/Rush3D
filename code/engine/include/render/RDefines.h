#ifndef RDefines_h__
#define RDefines_h__

namespace rush
{

    enum class RenderBackend
    {
        D3D11,
        D3D12,
        Metal,
        Vulkan,
        OpenGLES,
        OpenGL,
        Null,
        Count,
    };

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

    enum class ShaderStage
    {
        None,
        Vertex,
        Fragment,
        Compute,
        Count
    };

    enum class TextureDimension
    {
        Texture1D, 
        Texture2D,
        Texture3D,
        Count,
    };

    enum class BufferUsage : uint32_t
    {
        None,
        MapRead,
        MapWrite,
        CopySrc,
        CopyDst,
        Index,
        Vertex,
        Uniform,
        Storage,
        Indirect,
        QueryResolve,
        Count,
    };

    enum class BufferBindingType
    {
        Undefined,
        Uniform,
        Storage,
        ReadOnlyStorage,
        Count
    };

    enum class SamplerBindingType
    {
        Undefined,
        Filtering,
        NonFiltering,
        Comparison,
        Count
    };

    enum class TextureSampleType
    {
        Undefined,
        Float,
        UnfilterableFloat,
        Depth,
        Sint,
        Uint,
        Count
    };

    enum class TextureViewDimension 
    {
        Undefined,
        e1D,
        e2D,
        e2DArray,
        Cube,
        CubeArray,
        e3D,
        Count
    };

    static constexpr uint32_t kMaxBindGroups = 4u;
    static constexpr uint32_t kMaxBindingsPerBindGroup = 1000u;
    static constexpr uint8_t kMaxVertexAttributes = 16u;
    static constexpr uint8_t kMaxVertexBuffers = 8u;
    static constexpr uint32_t kMaxVertexBufferArrayStride = 2048u;
    static constexpr uint32_t kMaxBindGroupsPlusVertexBuffers = 24u;
    static constexpr uint32_t kNumStages = 3;
    static constexpr uint8_t kMaxColorAttachments = 8u;
    static constexpr uint32_t kTextureBytesPerRowAlignment = 256u;
    static constexpr uint32_t kQueryResolveAlignment = 256u;
    static constexpr uint32_t kMaxInterStageShaderComponents = 60u;
    static constexpr uint32_t kMaxInterStageShaderVariables = 16u;
    static constexpr uint64_t kAssumedMaxBufferSize =
        0x80000000u;  // Use 2 GB when the limit is unavailable

    // Per stage maximum limits used to optimized Dawn internals.
    static constexpr uint32_t kMaxSampledTexturesPerShaderStage = 16;
    static constexpr uint32_t kMaxSamplersPerShaderStage = 16;
    static constexpr uint32_t kMaxStorageBuffersPerShaderStage = 8;
    static constexpr uint32_t kMaxStorageTexturesPerShaderStage = 8;
    static constexpr uint32_t kMaxUniformBuffersPerShaderStage = 12;

    // Indirect command sizes
    static constexpr uint64_t kDispatchIndirectSize = 3 * sizeof(uint32_t);
    static constexpr uint64_t kDrawIndirectSize = 4 * sizeof(uint32_t);
    static constexpr uint64_t kDrawIndexedIndirectSize = 5 * sizeof(uint32_t);

    // Non spec defined constants.
    static constexpr float kLodMin = 0.0;
    static constexpr float kLodMax = 1000.0;

    // Offset alignment for CopyB2B. Strictly speaking this alignment is required only
    // on macOS, but we decide to do it on all platforms.
    static constexpr uint64_t kCopyBufferToBufferOffsetAlignment = 4u;

    // Metal has a maximum size of 32Kb for a counter set buffer. Each query is 8 bytes.
    // So, the maximum nymber of queries is 32Kb / 8.
    static constexpr uint32_t kMaxQueryCount = 4096;

    // An external texture occupies multiple binding slots. These are the per-external-texture bindings
    // needed.
    static constexpr uint8_t kSampledTexturesPerExternalTexture = 4u;
    static constexpr uint8_t kSamplersPerExternalTexture = 1u;
    static constexpr uint8_t kUniformsPerExternalTexture = 1u;

    // Wire buffer alignments.
    static constexpr size_t kWireBufferAlignment = 8u;


    static constexpr uint32_t kArrayLayerCountUndefined = 0xffffffffUL;
    static constexpr uint32_t kCopyStrideUndefined = 0xffffffffUL;
    static constexpr uint32_t kLimitU32Undefined = 0xffffffffUL;
    static constexpr uint64_t kLimitU64Undefined = 0xffffffffffffffffULL;
    static constexpr uint32_t kMipLevelCountUndefined = 0xffffffffUL;
    static constexpr size_t kWholeMapSize = SIZE_MAX;
    static constexpr uint64_t kWholeSize = 0xffffffffffffffffULL;

    struct RContex;

}

namespace wgpu
{
    class RenderPipeline;
    class Texture;
    class TextureView;
    class Sampler;
    class Buffer;
    class ComputePipeline;
    class BindGroup;
    class Queue;
    class ShaderModule;
    class BindGroupLayout;
    struct BindGroupLayoutEntry;
    struct RenderPassDescriptor;
    struct RenderPassColorAttachment;
    struct RenderPassDepthStencilAttachment;
}


#ifdef RUSH_PLATFORM_WINDOWS
#define DAWN_ENABLE_BACKEND_D3D12
#elif (defined(RUSH_PLATFORM_LINUX) || defined(RUSH_PLATFORM_ANDROID))
#define DAWN_ENABLE_BACKEND_VULKAN
#elif (defined(RUSH_PLATFORM_MAC) || defined(RUSH_PLATFORM_IOS))
#define DAWN_ENABLE_BACKEND_METAL
#endif


#define WGPU_RELEASE_RESOURCE(Type, Name)                                      \
  if (Name) {                                                                  \
    wgpu##Type##Release(Name);                                                 \
    Name = NULL;                                                               \
  }

#endif // RDefines_h__
