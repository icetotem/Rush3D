#include "stdafx.h"
#include "render/RPipeline.h"
#include "dawn/webgpu.h"

namespace rush
{

    RPipeline::RPipeline()
    {
    }

    RPipeline::~RPipeline()
    {
        WGPU_RELEASE_RESOURCE(RenderPipeline, m_Pipeline);
    }

}