#include "stdafx.h"
#include "render/RenderPass.h"
#include <dawn/webgpu.h>
#include "RenderContex.h"

namespace rush
{
    extern WGPUTextureFormat g_WGPUTextureFormat[(int)TextureFormat::Count];

    //////////////////////////////////////////////////////////////////////////

    RenderPass::RenderPass(Ref<RenderContex> renderContex, const char* lable, uint32_t width, uint32_t height, TextureFormat color, TextureFormat depthStencil, const Vector4& clearColor, float clearDepth, bool withDepth)
    {

    }

    RenderPass::~RenderPass()
    {

    }

}
