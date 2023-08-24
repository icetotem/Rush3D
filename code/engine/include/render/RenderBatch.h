#pragma once

#include "core/Core.h"

namespace rush
{

    class RenderPipeline;
    class BindGroup;
    class RVertexBuffer;

    class RenderBatch
    {
    public:
        Ref<RenderPipeline> Pipeline;
        Ref<BindGroup> Uniforms;
        List<Ref<RVertexBuffer>> VBList;
        Ref<RIndexBuffer> IB;
        uint32_t InstanceCount = 1;
        uint32_t FirstIndex = 0;
        uint32_t FirstVertex = 0;
    };

    class RenderContent
    {
    public:

        friend class Renderer;
        List<Ref<RenderBatch>> m_Batches;
    };

}