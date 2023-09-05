#ifndef RMaterial_h__
#define RMaterial_h__

#include "core/Core.h"
#include "render/RPipeline.h"
#include "RUniform.h"
#include "Asset.h"

namespace rush
{

    class RMaterial : public Asset
    {
    public:
        virtual bool Load(const StringView& path) override;

    protected:
        friend class RenderManager;
        friend class RMaterialInst;

        Ref<RPipeline> m_Pipeline;
    };

    //////////////////////////////////////////////////////////////////////////

    class RMaterialInst : public Asset
    {
    public:
        virtual bool Load(const StringView& path) override;

        static Ref<RUniformBuffer> GetGlobalUniformBuffer() { return s_GlobalUniformBuffer; }

    protected:
        friend class RenderManager;

        Ref<RMaterial> m_Material;
        Ref<RUniformBuffer> m_UniformBuffer;
        static Ref<RUniformBuffer> s_GlobalUniformBuffer;
        Ref<RBindGroup> m_BindGroup;
    };

}

#endif // RMaterial_h__
