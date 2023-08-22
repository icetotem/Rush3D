#include "stdafx.h"
#include "render/RBuffer.h"
#include "dawn/webgpu.h"

namespace rush
{

    RBuffer::RBuffer()
    {
    }

    RBuffer::~RBuffer()
    {
        WGPU_RELEASE_RESOURCE(Buffer, m_Handle);
    }

}