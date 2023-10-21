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

    bool RShader::Compile(const StringView& inputPath, const StringView& outputPath, const List<String>& defines, const StringView& code, const StringView& uniforms)
    {
        auto relPath = String("../../") + String(outputPath);
        char cmd[1024];
        auto src = String("../../") + String(inputPath);
        auto inc = Path(src).parent_path().string();

        std::string templateCode = "#pragma USER_SAMPLERS\nvoid _executeUserCode(inout Material material) { \n#pragma USER_CODE\n}";

        str_replace(templateCode, "#pragma USER_CODE", code);
        str_replace(templateCode, "#pragma USER_SAMPLERS", uniforms);
        auto outPath = std::filesystem::absolute("../../assets/spv/user.glsl").string();
        std::remove(outPath.c_str());
        std::ofstream outputFile(outPath.c_str(), std::ios::out | std::ios::trunc);
        if (!outputFile.is_open()) {
            LOG_ERROR("Compile Shader {} failed", String(inputPath));
            return false;
        }

        outputFile << templateCode;
        outputFile.close();

        sprintf(cmd, "glslc \"%s\" -I \"%s\" -I \"%s\" -o \"%s\"", src.c_str(), inc.c_str(), "../../assets/spv", relPath.c_str());

        // macros
        for (const auto& define : defines)
        {
            auto macro = String("-D") + String(define);
            sprintf(cmd, "%s %s", cmd, macro.c_str());
        }

        int ret = std::system(cmd);
        std::remove(outPath.c_str());
        return ret == 0;
    }

}