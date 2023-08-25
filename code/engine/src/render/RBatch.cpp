#include "stdafx.h"

#include "render/RBatch.h"

namespace rush
{

    Ref<RBatch> RContent::NewBatch()
    {
        auto batch = CreateRef<RBatch>();
        m_Batches.push_back(batch);
        return batch;
    }

}