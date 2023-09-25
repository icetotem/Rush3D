#include "stdafx.h"

#include "render/RShader.h"
#include "render/RDevice.h"
#include "BundleManager.h"

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
        m_Module = RDevice::instance().GetDevice().CreateShaderModule(&descriptor);
    }

    bool RShader::Load(const StringView& path)
    {
        if (path.find(".vert.") != path.npos)
        {
            m_ShaderType = ShaderStage::Vertex;
        }
        else if (path.find(".frag.") != path.npos)
        {
            m_ShaderType = ShaderStage::Fragment;
        }
        else if (path.find(".comp.") != path.npos)
        {
            m_ShaderType = ShaderStage::Compute;
        }

        auto stream = BundleManager::instance().Get(path);
        if (stream->IsEmpty())
        {
            LOG_ERROR("Cannot load shader {}", path.data());
            return false;
        }
        wgpu::ShaderModuleSPIRVDescriptor glslDesc;
        glslDesc.code = (const uint32_t*)stream->GetData();
        glslDesc.codeSize = stream->GetSize() / 2;
        wgpu::ShaderModuleDescriptor descriptor;
        descriptor.nextInChain = &glslDesc;
        descriptor.label = path.data();
        m_Module = RDevice::instance().GetDevice().CreateShaderModule(&descriptor);
        return true;
    }

}