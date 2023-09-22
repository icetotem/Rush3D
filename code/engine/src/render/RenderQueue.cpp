#include "stdafx.h"
#include "render/RenderQueue.h"

namespace rush
{

    RenderQueue::RenderQueue(Entity camera)
        : m_Camera(camera)
    {
    }

    void RenderQueue::Add(Ref<RGeometry> geometry, Ref<RMaterial> material)
    {
        auto& renderable = m_Renderables.emplace_back();
        renderable.geometry = geometry;
        renderable.material = material;
    }

    void RenderQueue::MergeBatch()
    {
        for (const auto& renderable : m_Renderables)
        {
            auto& batch = m_Batches.emplace_back();
            batch.renderable = renderable;
            batch.instanceCount = 1;
        }
    }

}