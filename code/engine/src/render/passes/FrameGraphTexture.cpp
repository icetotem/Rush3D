#include "stdafx.h"
#include <strstream>
#include <dawn/webgpu_cpp_print.h>
#include "render/passes/FrameGraphTexture.h"
#include "render/passes/TransientResources.h"

namespace rush
{

    static std::string toString(IVector2 extent, uint32_t depth) 
    {
        return depth > 0 ? fmt::format("{}x{}x{}", extent.x, extent.y, depth) : fmt::format("{}x{}", extent.x, extent.y);
    }

    void FrameGraphTexture::create(const Desc& desc, void* allocator)
    {
        texture = static_cast<TransientResources*>(allocator)->acquireTexture(desc);
    }
    void FrameGraphTexture::destroy(const Desc& desc, void* allocator) 
    {
        static_cast<TransientResources*>(allocator)->releaseTexture(desc, texture);
    }

    std::string FrameGraphTexture::toString(const Desc& desc) 
    {
        std::strstream strFmt;
        strFmt << desc.format;
        return fmt::format("FrameGraphTexture {} [{}]", rush::toString(desc.extent, desc.depth), strFmt.str());
    }

}