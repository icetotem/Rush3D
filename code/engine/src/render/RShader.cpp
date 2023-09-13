#include "stdafx.h"

#include "render/RShader.h"
#include "render/RenderContext.h"

namespace rush
{

    RShader::RShader(ShaderStage type, const char* source, const char* lable)
    {
        m_ShaderType = type;
        wgpu::ShaderModuleWGSLDescriptor wgslDesc;
        wgslDesc.code = source;
        wgpu::ShaderModuleDescriptor descriptor;
        descriptor.nextInChain = &wgslDesc;
        descriptor.label = lable;
        m_Module = CreateRef<wgpu::ShaderModule>(RenderContext::device.CreateShaderModule(&descriptor));
    }


}