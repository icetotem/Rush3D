#pragma once

#include "core/Core.h"

#include "render/RPipeline.h"
#include "render/RBuffer.h"
#include "render/RUniform.h"

namespace rush
{

    struct RBatch
    {
        Ref<RPipeline> pipeline;
        Ref<RBindGroup> uniforms;
        List<Ref<RVertexBuffer>> vertexBufferList;
        Ref<RIndexBuffer> indexBuffer;
        uint32_t instanceCount = 1;
        uint32_t firstIndex = 0;
        uint32_t firstVertex = 0;
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