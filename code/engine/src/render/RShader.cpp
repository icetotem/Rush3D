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

    RShader::RShader(const StringView& label)
    {
        m_Label = label;
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
        glslDesc.codeSize = stream->GetSize() / sizeof(uint32_t);
        wgpu::ShaderModuleDescriptor descriptor;
        descriptor.nextInChain = &glslDesc;
        descriptor.label = m_Label.c_str();
        m_Module = RDevice::instance().GetDevice().CreateShaderModule(&descriptor);

        // xxx.vert.hashxxxxx.spv
        auto strHash = Path(path).stem().string();
        DArray<String> strArray;
        splitStr(strHash, ".", strArray);
        RUSH_ASSERT(strArray.size() == 3);
        m_Hash = std::stoull(strArray[2]);

        if (!m_Module)
        {   
            LOG_ERROR("Compile Spv failed {}", String(path));
            return false;
        }

        return true;
    }

}