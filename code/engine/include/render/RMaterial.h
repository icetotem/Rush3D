#ifndef RMaterial_h__
#define RMaterial_h__

#include "core/Core.h"
#include "Asset.h"
#include "render/RDefines.h"

namespace rush
{

    class RGeometry;
    class RShader;

    class RMaterial : public Asset
    {
    public:
        virtual bool Load(const StringView& path) override;

        const wgpu::BindGroup& GetBindGroup() const { return m_BindGroup; }

        static const wgpu::RenderPipeline GetPipeline(Ref<RGeometry> geometry, Ref<RMaterial> material);

        uint64_t GetHash() const { return m_Hash; }

    protected:
        uint64_t m_Hash;
        BlendMode blendMode = BlendMode::Opaque;
        CullMode cullModel = CullMode::Back;
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
        wgpu::BindGroup m_BindGroup;
        static Map<uint64_t, wgpu::RenderPipeline> s_PipelineCache;
    };

    //////////////////////////////////////////////////////////////////////////

//     class RMaterialInst : public Asset
//     {
//     public:
//         virtual bool Load(const StringView& path) override;
// 
//         static Ref<RUniformBuffer> GetGlobalUniformBuffer() { return s_GlobalUniformBuffer; }
// 
//         Ref<RMaterial> GetMaterial() const { return m_Material; }
// 
//         const wgpu::BindGroup& GetBindGroup() const { return m_BindGroup->GetBindGroup(); }
// 
//     protected:
//         friend class SceneManager;
// 
//         Ref<RMaterial> m_Material;
//         Ref<RUniformBuffer> m_UniformBuffer;
//         static Ref<RUniformBuffer> s_GlobalUniformBuffer;
//         Ref<RBindGroup> m_BindGroup;
//     };

}

#endif // RMaterial_h__
