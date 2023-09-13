#ifndef UploadFrameBlock_h__
#define UploadFrameBlock_h__

#include "render/fg/Fwd.hpp"
#include "components/Camera.h"
#include "render/RenderContext.h"

namespace rush
{

    struct FrameInfo 
    {
        float time;
        float deltaTime;
        Vector2 resolution;
        const Camera& camera;
        uint32_t features;
        uint32_t debugFlags;
    };

    void uploadFrameBlock(FrameGraph&, FrameGraphBlackboard&, const FrameInfo&);

}

#endif // UploadFrameBlock_h__
