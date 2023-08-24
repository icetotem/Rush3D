#pragma once

#include "core/Core.h"
#include "render/RenderDefines.h"

namespace rush
{

    /// <summary>
    /// Shader
    /// </summary>
    class Shader
    {
    public:
        Shader();
        ~Shader();

    protected:
        friend class Renderer;

        WGPUShaderModuleImpl* m_ShaderModule = nullptr;
    };

}