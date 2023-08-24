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
        ~Shader();

        ShaderStage GetType() const { return m_ShaderType; }

    protected:
        friend class Renderer;
        friend class RenderPipeline;

        Shader(Ref<RenderContex> contex, ShaderStage type, const char* source, const char* lable);

        static Ref<Shader> Construct(Ref<RenderContex> contex, ShaderStage type, const char* source, const char* lable)
        {
            return std::shared_ptr<Shader>(new Shader(contex, type, source, lable));
        }

        ShaderStage m_ShaderType = ShaderStage::None;
        Ref<wgpu::ShaderModule> m_Module;
    };

}