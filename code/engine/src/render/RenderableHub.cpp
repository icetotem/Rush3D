#include "stdafx.h"
#include "render/RenderableHub.h"

namespace rush
{

    Ref<Renderable> RenderableHub::NewBatch()
    {
        auto batch = CreateRef<Renderable>();
        m_Batches.push_back(batch);
        return batch;
    }

}