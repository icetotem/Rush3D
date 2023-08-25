#include "stdafx.h"

#include <dawn/webgpu_cpp.h>

#include "render/RPass.h"
#include "RContex.h"

namespace rush
{
    extern WGPUTextureFormat g_WGPUTextureFormat[(int)TextureFormat::Count];

    //////////////////////////////////////////////////////////////////////////

    RPass::RPass(Ref<RContex> renderContex, uint32_t width, uint32_t height, TextureFormat color, TextureFormat depthStencil, const Vector4& clearColor, float clearDepth, bool withDepth, const char* lable)
    {

    }

    RPass::~RPass()
    {

    }

}
