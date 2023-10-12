#ifndef RBatch_h__
#define RBatch_h__

#include "core/Core.h"
#include "components/EcsSystem.h"

namespace rush
{
    class RMaterial;
    class RGeometry;
    class Light;

    struct Renderable
    {
        uint64_t hashPipeline = 0;
        uint64_t hashInst = 0;
        Ref<RGeometry> geometry;
        Ref<RMaterial> material;
        const Matrix4* transform = nullptr;
    };

    struct RenderBatch
    {
        Renderable renderable;
        uint32_t instanceCount = 0;
        DArray<const Matrix4*> transforms;
    };

    class RenderQueue
    {
    public:
        RenderQueue(Entity camera);
        ~RenderQueue() = default;

        void Add(Ref<RGeometry> geometry, Ref<RMaterial> material, const Matrix4* transform);

        void AddLight(Light* light);

        const List<const RenderBatch*>& GetBatches() const { return m_BatchList; }

        void MergeAndSortBatch();

        Entity GetCamera() const { return m_Camera; }

        List<Light*> GetLights() const { return m_Lights; }

    private:
        List<Renderable> m_Renderables;
        Map<uint64_t, RenderBatch> m_BatchCache;
        List<const RenderBatch*> m_BatchList;
        List<Light*> m_Lights;
        Entity m_Camera;
    };

}

#endif // RBatch_h__
