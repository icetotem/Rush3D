#include "stdafx.h"
#include "render/RenderQueue.h"

namespace rush
{

    Ref<Renderable> RenderQueue::NewBatch()
    {
        auto batch = CreateRef<Renderable>();
        m_Batches.push_back(batch);
        return batch;
    }

}