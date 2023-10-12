#ifndef RMaterial_h__
#define RMaterial_h__

#include "core/Core.h"
#include "Asset.h"
#include "render/RDefines.h"
#include "Renderer.h"
#include "RBuffer.h"

namespace rush
{

    class RGeometry;
    class RShader;
    class RBuffer;
    class RSampler;


    enum class MaterialType
    {
        Surface,
        PostProcess,
    };

    enum class ShadingModel
    {
        DefaultLit,
        Unlit,
    };

    enum class BindingType
    {
        Uniform,
        Storage,
        Texture,
        Sampler,
    };

    struct BindInfo
    {
        uint32_t binding;
        BindingType type;
        String name;
        std::optional<String> target;
        std::optional<String> path;
        std::optional<AddressMode> address;
        std::optional<FilterMode> mag;
        std::optional<FilterMode> min;
        std::optional<MipmapFilterMode> mip;
        std::optional<int> size;
    };

    class RMaterial : public Asset
    {
    public:
        virtual bool Load(const StringView& path) override;

        static const wgpu::RenderPipeline GetPipeline(Renderer* renderer, Ref<RGeometry> geometry, Ref<RMaterial> material, const FrameBuffer& outputBuffers);

        uint64_t GetHash() const { return m_Hash; }
        uint64_t GetBindGroupHash() const { return m_BindGroupHash; }
        uint64_t GetShaderHash() const { return m_ShaderHash; }
        uint64_t GetStateHash() const { return m_StateHash; }

        MaterialType GetType() const { return m_Type; }

        ShadingModel GetShadingModel() const { return m_ShadingModel; }

        Ref<RShader> GetVertexShader() const { return m_VertexShader; }

        Ref<RShader> GetFragmentShader() const { return m_FragmentShader; }

        BlendMode GetBlendMode() const { return blendMode; }

        const String& GetPath() const { return m_Path; }

        Ref<RBindGroup> GetBindGroup() const { return m_BindGroup; }

    public:
        void UpdateFrameData(const FrameData& data);

    protected:
        String m_Path;
        uint64_t m_Hash;
        uint64_t m_BindGroupHash;
        uint64_t m_ShaderHash;
        uint64_t m_StateHash;
        MaterialType m_Type = MaterialType::Surface;
        ShadingModel m_ShadingModel = ShadingModel::DefaultLit;
        BlendMode blendMode = BlendMode::Opaque;
        FrontFace frontFace = FrontFace::CW;
        CullMode cullMode = CullMode::Back;
        ColorWriteMask writeMask = ColorWriteMask::All;
        bool depthTest = true;
        bool depthWrite = true;
        CompareFunction depthCompare = CompareFunction::LessEqual;
        bool stencilTest = false;
        bool stencilWrite = false;
        BlendOperation opColor = BlendOperation::Add;
        BlendFactor srcColor = BlendFactor::Src;
        BlendFactor dstColor = BlendFactor::Dst;
        BlendOperation opAlpha = BlendOperation::Add;
        BlendFactor srcAlpha = BlendFactor::SrcAlpha;
        BlendFactor dstAlpha = BlendFactor::DstAlpha;
        Ref<RShader> m_VertexShader;
        Ref<RShader> m_FragmentShader;
        Ref<RBindGroup> m_BindGroup;
        DArray<BindInfo> m_BindInfos;
        static Map<uint64_t, wgpu::RenderPipeline> s_PipelineCache;
        Ref<RUniformBuffer> m_UniformBuffer;
    };

}

#endif // RMaterial_h__
