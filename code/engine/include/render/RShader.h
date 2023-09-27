#ifndef RShader_h__
#define RShader_h__

#include "core/Core.h"
#include "render/RDefines.h"
#include "Asset.h"

namespace rush
{

    /// <summary>
    /// Shader
    /// </summary>
    class RShader : public Asset
    {
    public:
        RShader() = default;
        RShader(const StringView& label);
        RShader(ShaderStage type, const char* source, const char* lable = nullptr);
        ~RShader() = default;

        virtual bool Load(const StringView& path) override;

        ShaderStage GetType() const { return m_ShaderType; }

        wgpu::ShaderModule GetModule() const { return m_Module; }

        uint64_t GetHash() const { return m_Hash; }

    protected:
        ShaderStage m_ShaderType = ShaderStage::None;
        wgpu::ShaderModule m_Module;
        uint64_t m_Hash;
        String m_Label;
    };

}

#endif // RShader_h__
