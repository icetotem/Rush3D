#ifndef RShader_h__
#define RShader_h__

#include "core/Core.h"
#include "render/RDefines.h"

namespace rush
{

    /// <summary>
    /// Shader
    /// </summary>
    class RShader
    {
    public:
        RShader(ShaderStage type, const char* source, const char* lable = nullptr);

        ~RShader() = default;

        ShaderStage GetType() const { return m_ShaderType; }

    protected:
        friend class Renderer;
        friend class RPipeline;

        ShaderStage m_ShaderType = ShaderStage::None;
        Ref<wgpu::ShaderModule> m_Module;
    };

}

#endif // RShader_h__
