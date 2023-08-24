#pragma once

#include "core/Core.h"

namespace rush
{

    class RPipeline;
    class UniformBuffer;
    class RBuffer;

    class RenderBatch
    {
    public:
        Ref<RPipeline> Pipeline;
        Ref<UniformBuffer> Uniforms;
        List<Ref<RBuffer>> VBList;
        Ref<RBuffer> IB;
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