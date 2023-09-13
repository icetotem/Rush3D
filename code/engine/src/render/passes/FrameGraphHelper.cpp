#include "stdafx.h"
#include "render/fg/FrameGraph.hpp"
#include "render/passes/FrameGraphHelper.h"
#include "render/passes/FrameGraphTexture.h"
#include "render/passes/FrameGraphBuffer.h"
#include "render/RBuffer.h"

namespace rush
{

    FrameGraphResource importTexture(FrameGraph& fg, const std::string_view name, RTexture* texture) 
    {
        RUSH_ASSERT(texture && texture->IsValid());
        return fg.import<FrameGraphTexture>(
            name,
            {
              {texture->GetWidth(), texture->GetHeight()},
              texture->GetDepth(),
              texture->GetMips(),
              texture->GetDepth(),
              texture->GetFormat(),
            },
            { texture });
    }

    RTexture& getTexture(FrameGraphPassResources& resources, FrameGraphResource id)
    {
        return *resources.get<FrameGraphTexture>(id).texture;
    }

    FrameGraphResource importBuffer(FrameGraph& fg, const std::string_view name, RStorageBuffer* buffer)
    {
        RUSH_ASSERT(buffer && buffer->IsValid());
        return fg.import<FrameGraphBuffer>(name, { buffer->GetSize() }, { buffer });
    }

    RStorageBuffer& getBuffer(FrameGraphPassResources& resources, FrameGraphResource id)
    {
        return *resources.get<FrameGraphBuffer>(id).buffer;
    }

}