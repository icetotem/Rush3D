#ifndef RMaterial_h__
#define RMaterial_h__

#include "core/Core.h"
#include "Asset.h"
#include "render/RDefines.h"

namespace rush
{

    class RMaterial : public Asset
    {
    public:
        virtual bool Load(const StringView& path) override;


    private:
        friend class Renderer;
        const wgpu::BindGroup GetBindGroup() const;
        const wgpu::RenderPipeline GetPipeline() const;

    protected:
        friend struct std::hash<rush::RMaterial>;
        CullMode cullModel = CullMode::Back;
        BlendMode blendMode = BlendMode::Opaque;
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
