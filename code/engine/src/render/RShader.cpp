#include "stdafx.h"

#include <dawn/webgpu_cpp.h>

#include "render/RShader.h"
#include "RContex.h"

namespace rush
{

    RShader::RShader(Ref<RContex> contex, ShaderStage type, const char* source, const char* lable)
    {
        m_ShaderType = type;
        m_Module = CreateRef<wgpu::ShaderModule>();
        wgpu::ShaderModuleWGSLDescriptor wgslDesc;
        wgslDesc.code = source;
        wgpu::ShaderModuleDescriptor descriptor;
        descriptor.nextInChain = &wgslDesc;
        descriptor.label = lable;
        *m_Module = contex->device.CreateShaderModule(&descriptor);
    }

    RShader::~RShader()
    {
    }

}