#include "stdafx.h"
#include "render/Shader.h"
#include "dawn/webgpu.h"

namespace rush
{

    Shader::Shader()
    {
    }

    Shader::~Shader()
    {
        WGPU_RELEASE_RESOURCE(ShaderModule, m_ShaderModule);
    }

}