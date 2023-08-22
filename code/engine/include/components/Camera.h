#pragma once

#include "core/Common.h"
#include "Render/Renderer.h"

namespace rush
{

    struct Camera
    {
        Ref<Renderer> Renderer;
    };

}