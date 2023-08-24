#include "stdafx.h"
#include "render/Shader.h"
#include "dawn/webgpu_cpp.h"
#include "RenderContex.h"

namespace rush
{

    Shader::Shader(Ref<RenderContex> contex, ShaderStage type, const char* source, const char* lable)
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

    Shader::~Shader()
    {
    }

}