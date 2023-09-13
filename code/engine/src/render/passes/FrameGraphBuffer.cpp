#include "stdafx.h"
#include "render/passes/FrameGraphBuffer.h"
#include "render/passes/TransientResources.h"

namespace rush
{

    void FrameGraphBuffer::create(const Desc& desc, void* allocator) 
    {
        buffer = static_cast<TransientResources*>(allocator)->acquireBuffer(desc);
    }

    void FrameGraphBuffer::destroy(const Desc& desc, void* allocator)
    {
        static_cast<TransientResources*>(allocator)->releaseBuffer(desc, std::move(buffer));
    }

    std::string FrameGraphBuffer::toString(const Desc& desc) 
    {
        return fmt::format("FrameGraphBuffer size: {} bytes", desc.size);
    }

}