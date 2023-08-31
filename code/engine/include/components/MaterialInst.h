#ifndef MaterialInst_h__
#define MaterialInst_h__

#include "components/EcsSystem.h"
#include "render/RPipeline.h"
#include "render/RUniform.h"

namespace rush
{
    
    class MaterialInst : public Component
    {
    public:
        MaterialInst(Entity owner);
        ~MaterialInst();

        void SetAsset(const StringView& assetPath);

    protected:
        friend class RenderManager;
        Ref<RPipeline> m_Pipeline;
        Ref<RBindGroup> m_BindGroup;
        Ref<RUniformBuffer> m_UniformBuf;
    };
        
}

#endif // MaterialInst_h__
