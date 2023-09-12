#ifndef RBatch_h__
#define RBatch_h__

#include "core/Core.h"
#include "render/RPipeline.h"
#include "render/RBuffer.h"
#include "render/RUniform.h"
#include "components/EcsSystem.h"

namespace rush
{

    struct Renderable
    {
        Ref<RPipeline> pipeline;
        Ref<RBindGroup> bindGroup;
        List<Ref<RVertexBuffer>> vertexBuffers;
        Ref<RIndexBuffer> indexBuffer;
        uint32_t instanceCount = 1;
    };

    class RenderableHub
    {
    public:
        ~RenderableHub() = default;

        Ref<Renderable> NewBatch();

        Entity camera;

    private:
        friend class RenderContex;
        List<Ref<Renderable>> m_Batches;
    };

}

#endif // RBatch_h__
