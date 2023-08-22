#include "stdafx.h"
#include "render/Uniform.h"
#include "dawn/webgpu.h"

namespace rush
{

    UniformBuffer::UniformBuffer()
    {
    }

    UniformBuffer::~UniformBuffer()
    {
        WGPU_RELEASE_RESOURCE(BindGroup, m_BindGroup);
        WGPU_RELEASE_RESOURCE(Buffer, m_Buffer);
    }
}