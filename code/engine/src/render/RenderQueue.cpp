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
        hash_val(renderable.hashPipeline, geometry->GetLayoutHash(), material->GetHash());
        hash_val(renderable.hashInst, renderable.hashPipeline, (uint64_t*)geometry.get());
    }

    void RenderQueue::AddLight(Light* light)
    {
        m_Lights.push_back(light);
    }

    void RenderQueue::MergeAndSortBatch()
    {
        // merge
        for (const auto& renderable : m_Renderables)
        {
            auto& batch = m_BatchCache[renderable.hashInst];
            batch.renderable = renderable;
            batch.instanceCount++;
            batch.transforms.push_back(renderable.transform);
        }

        for (auto& batch : m_BatchCache)
        {
            m_BatchList.push_back(&batch.second);
        }

        // sort
        m_BatchList.sort([](const RenderBatch* a, const RenderBatch* b)->bool {
            auto ma = a->renderable.material;
            auto mb = b->renderable.material;
            if (ma->GetBindGroupHash() == mb->GetBindGroupHash()) {
                return a->renderable.hashPipeline < b->renderable.hashPipeline;
            }
            else {
                return ma->GetBindGroupHash() < mb->GetBindGroupHash();
            }
        });
    }

}