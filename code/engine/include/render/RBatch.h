#pragma once

#include "core/Core.h"

#include "render/RPipeline.h"
#include "render/RBuffer.h"
#include "render/RUniform.h"

namespace rush
{

    struct RBatch
    {
        Ref<RPipeline> Pipeline;
        Ref<RBindGroup> Uniforms;
        List<Ref<RVertexBuffer>> VBList;
        Ref<RIndexBuffer> IB;
        uint32_t InstanceCount = 1;
        uint32_t FirstIndex = 0;
        uint32_t FirstVertex = 0;
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