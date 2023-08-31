#ifndef RBatch_h__
#define RBatch_h__

#include "core/Core.h"
#include "render/RPipeline.h"
#include "render/RBuffer.h"
#include "render/RUniform.h"

namespace rush
{

    struct RBatch
    {
        Ref<RPipeline> pipeline;
        Ref<RBindGroup> bindGroup;
        List<Ref<RVertexBuffer>> vertexBuffers;
        Ref<RIndexBuffer> indexBuffer;
        uint32_t instanceCount = 1;
    };

    class RContent
    {
    public:
        ~RContent() = default;

        Ref<RBatch> NewBatch();

    private:
        friend class Renderer;
        List<Ref<RBatch>> m_Batches;
    };

}

#endif // RBatch_h__
