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
        ~RShader() = default;

        ShaderStage GetType() const { return m_ShaderType; }

    protected:
        friend class Renderer;
        friend class RPipeline;

        RShader(Ref<RContex> contex, ShaderStage type, const char* source, const char* lable);

        static Ref<RShader> Construct(Ref<RContex> contex, ShaderStage type, const char* source, const char* lable)
        {
            return std::shared_ptr<RShader>(new RShader(contex, type, source, lable));
        }

        ShaderStage m_ShaderType = ShaderStage::None;
        Ref<wgpu::ShaderModule> m_Module;
    };

}

#endif // RShader_h__
