#ifndef FrameGraphHelper_h__
#define FrameGraphHelper_h__

#include "core/Core.h"
#include "render/fg/FrameGraphResource.hpp"
#include "render/RTexture.h"

namespace rush
{

    class FrameGraph;
    class FrameGraphPassResources;
    class RTexture;

    [[nodiscard]] FrameGraphResource importTexture(FrameGraph&, const StringView name, RTexture*);

    [[nodiscard]] RTexture& getTexture(FrameGraphPassResources&, FrameGraphResource id);

    class Buffer;

    [[nodiscard]] FrameGraphResource importBuffer(FrameGraph&, const std::string_view name, RStorageBuffer*);

    [[nodiscard]] RStorageBuffer& getBuffer(FrameGraphPassResources&, FrameGraphResource id);

}

#endif // FrameGraphHelper_h__
