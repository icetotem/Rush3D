#ifndef RBatch_h__
#define RBatch_h__

#include "core/Core.h"
#include "components/EcsSystem.h"

namespace rush
{
    class RMaterial;
    class RGeometry;

    struct Renderable
    {
        Ref<RGeometry> geometry;
        Ref<RMaterial> material;
    };

    struct RenderBatch
    {
        Renderable renderable;
        uint32_t instanceCount = 1;
    };

    class RenderQueue
    {
    public:
        RenderQueue(Entity camera);
        ~RenderQueue() = default;

        void Add(Ref<RGeometry> geometry, Ref<RMaterial> material);

        List<RenderBatch> GetBatches() const { return m_Batches; }

        void MergeBatch();

        Entity GetCamera() const { return m_Camera; }

    private:
        List<Renderable> m_Renderables;
        List<RenderBatch> m_Batches;
        Entity m_Camera;
    };

}

#endif // RBatch_h__
