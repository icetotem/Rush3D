#include "stdafx.h"
#include "render/RTexture.h"
#include "dawn/webgpu.h"

namespace rush
{

    RTexture::RTexture()
    {
    }

    RTexture::~RTexture()
    {
        WGPU_RELEASE_RESOURCE(Texture, m_Texture);
        WGPU_RELEASE_RESOURCE(TextureView, m_View);
        WGPU_RELEASE_RESOURCE(Sampler, m_Sampler);
    }

}
