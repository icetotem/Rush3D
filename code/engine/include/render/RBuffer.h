#pragma once

#include "core/Core.h"
#include "render/RenderDefines.h"

namespace rush
{

    struct VertexAttribute
    {
        VertexFormat Format = VertexFormat::Float32;
        uint64_t Offset = 0;
        uint32_t ShaderLocation = 0;
    };

    struct VertexLayout
    {
        uint64_t Stride = 0;
        uint32_t AttributeCount = 0;
        VertexAttribute* Attributes = nullptr;
    };


    /// <summary>
    /// RBuffer
    /// </summary>
    class RBuffer
    {
    public:
        RBuffer();
        ~RBuffer();

    protected:
        friend class Renderer;

        uint32_t m_Count;
        uint32_t m_Stride;
        uint32_t m_Size;

        WGPUBufferImpl* m_Handle = nullptr;
    };
}