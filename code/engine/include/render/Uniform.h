#pragma once

#include "core/Core.h"
#include "render/RenderDefines.h"
#include "render/RBuffer.h"

namespace rush
{

    /// <summary>
    /// UniformBuffer
    /// </summary>
    class UniformBuffer
    {
    public:
        UniformBuffer();
        ~UniformBuffer();

    protected:
        friend class Renderer;

        WGPUBindGroupImpl* m_BindGroup = nullptr;
        WGPUBufferImpl* m_Buffer = nullptr;
    };

}