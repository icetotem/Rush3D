#include "stdafx.h"
#include "render/RenderQueue.h"
#include "render/RGeometry.h"
#include "render/RMaterial.h"

namespace rush
{

    RenderQueue::RenderQueue(Entity camera)
        : m_Camera(camera)
    {
    }

    void RenderQueue::Add(Ref<RGeometry> geometry, Ref<RMaterial> material, const Matrix4* transform)
    {
        auto& renderable = m_Renderables.emplace_back();
        renderable.geometry = geometry;
        renderable.material = material;
        renderable.transform = transform;
        hash_val(renderable.hash, geometry->GetLayoutHash(), material->GetHash());
    }

    void RenderQueue::AddLight(Light* light)
    {
        m_Lights.push_back(light);
    }

    void RenderQueue::MergeBatch()
    {
        for (const auto& renderable : m_Renderables)
        {
            auto& batch = m_Batches[renderable.hash];
            batch.renderable = renderable;
            batch.instanceCount++;
            batch.transforms.push_back(renderable.transform);
        }
    }

}