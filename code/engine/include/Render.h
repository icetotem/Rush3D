#pragma once
#include "Platform.h"

namespace rush
{


    /// <summary>
    /// 
    /// </summary>
    class RBuffer
    {
    public:
        RBuffer();
        ~RBuffer();

    protected:
        friend class Renderer;
        struct WebGpuImpl;
        Ref<WebGpuImpl> m_Impl;
    };

    /// <summary>
    /// 
    /// </summary>
    class Shader
    {
    public:
        Shader();
        ~Shader();

    protected:
        friend class Renderer;
        struct WebGpuImpl;
        Ref<WebGpuImpl> m_Impl;
    };

    /// <summary>
    /// RPipeline
    /// </summary>
    class RPipeline
    {
    public:
        RPipeline();
        ~RPipeline();

    protected:
        friend class Renderer;
        struct WebGpuImpl;
        Ref<WebGpuImpl> m_Impl;
    };

    struct PipelineDesc
    {
        Ref<Shader> VS;
        Ref<Shader> FS;

    };

    /// <summary>
    /// 
    /// </summary>
    class Renderer
    {
    public:
        Renderer(WindowHandle window);
        ~Renderer();

        Ref<Shader> CreateShader(const char* code, const char* debugName = nullptr);

        Ref<RBuffer> CreateVertexBuffer(const void* data, size_t stride);

        Ref<RBuffer> CreateIndexBuffer(const void* data, size_t stride);

        Ref<RPipeline> CreatePipeline(const PipelineDesc* pipeDesc);

        void RenderOneFrame();

    protected:
        struct WebGpuImpl;
        Ref<WebGpuImpl> m_Impl;
        WindowHandle m_WindowHandle;
    };

}