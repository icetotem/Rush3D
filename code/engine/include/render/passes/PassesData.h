#ifndef FrameData_h__
#define FrameData_h__

#include "render/fg/FrameGraphResource.hpp"

namespace rush
{

    struct FrameData 
    {
        FrameGraphResource frameBlock;
    };

    struct SceneColorData 
    {
        FrameGraphResource hdr;
        FrameGraphResource ldr;
    };


}

#endif // FrameData_h__
