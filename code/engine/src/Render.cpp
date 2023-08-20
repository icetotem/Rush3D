#include "stdafx.h"
#include "Render.h"
#include "webgpu.h"
#include "dawn/webgpu.h"

namespace rush
{
    //////////////////////////////////////////////////////////////////////////

    struct RBuffer::WebGpuImpl
    {
        WGPUBuffer buffer = nullptr;
    };

    RBuffer::RBuffer()
    {
        m_Impl = CreateRef<WebGpuImpl>();
    }

    RBuffer::~RBuffer()
    {
        wgpuBufferRelease(m_Impl->buffer);
    }

    //////////////////////////////////////////////////////////////////////////

    struct Shader::WebGpuImpl
    {
        WGPUShaderModule shaderModule = nullptr;
    };

    Shader::Shader()
    {
        m_Impl = CreateRef<WebGpuImpl>();
    }

    Shader::~Shader()
    {
        wgpuShaderModuleRelease(m_Impl->shaderModule);
    }

    //////////////////////////////////////////////////////////////////////////
    struct RPipeline::WebGpuImpl
    {
        WGPURenderPipeline pipeline = nullptr;
    };

    RPipeline::RPipeline()
    {
        m_Impl = CreateRef<WebGpuImpl>();
    }

    RPipeline::~RPipeline()
    {
        wgpuRenderPipelineRelease(m_Impl->pipeline);
    }

    //////////////////////////////////////////////////////////////////////////

    struct Renderer::WebGpuImpl
    {
        WGPUDevice device = nullptr;
        WGPUQueue queue = nullptr;
        WGPUSwapChain swapchain = nullptr;
    };

    Renderer::Renderer(WindowHandle window)
        : m_WindowHandle(window)
    {
        m_Impl = CreateRef<WebGpuImpl>();
        m_Impl->device = webgpu::create(m_WindowHandle);
        m_Impl->queue = wgpuDeviceGetQueue(m_Impl->device);
        m_Impl->swapchain = webgpu::createSwapChain(m_Impl->device);
    }

    Renderer::~Renderer()
    {
        wgpuSwapChainRelease(m_Impl->swapchain);
        wgpuQueueRelease(m_Impl->queue);
        wgpuDeviceRelease(m_Impl->device);
    }

    Ref<Shader> Renderer::CreateShader(const char* code, const char* debugName /*= nullptr*/)
    {
        WGPUShaderModuleWGSLDescriptor wgsl = {};
        wgsl.chain.sType = WGPUSType_ShaderModuleWGSLDescriptor;
        wgsl.source = code;
        WGPUShaderModuleDescriptor desc = {};
        desc.nextInChain = reinterpret_cast<WGPUChainedStruct*>(&wgsl);
        desc.label = debugName;
        auto shader = CreateRef<Shader>();
        shader->m_Impl->shaderModule = wgpuDeviceCreateShaderModule(m_Impl->device, &desc);
        return shader;
    }

    Ref<RBuffer> Renderer::CreateVertexBuffer(const void* data, size_t stride)
    {
        WGPUBufferDescriptor desc = {};
        desc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex;
        desc.size = stride;
        WGPUBuffer buffer = wgpuDeviceCreateBuffer(m_Impl->device, &desc);
        wgpuQueueWriteBuffer(m_Impl->queue, buffer, 0, data, stride);
        Ref<RBuffer> bufferObj = CreateRef<RBuffer>();
        bufferObj->m_Impl->buffer;
        return bufferObj;
    }

    Ref<RBuffer> Renderer::CreateIndexBuffer(const void* data, size_t stride)
    {
        WGPUBufferDescriptor desc = {};
        desc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Index;
        desc.size = stride;
        WGPUBuffer buffer = wgpuDeviceCreateBuffer(m_Impl->device, &desc);
        wgpuQueueWriteBuffer(m_Impl->queue, buffer, 0, data, stride);
        Ref<RBuffer> bufferObj = CreateRef<RBuffer>();
        bufferObj->m_Impl->buffer = buffer;
        return bufferObj;
    }

    Ref<RPipeline> Renderer::CreatePipeline(const PipelineDesc* pipeDesc)
    {
        WGPUBufferBindingLayout buf = {};
        buf.type = WGPUBufferBindingType_Uniform;

        // bind group layout (used by both the pipeline layout and uniform bind group, released at the end of this function)
        WGPUBindGroupLayoutEntry bglEntry = {};
        bglEntry.binding = 0;
        bglEntry.visibility = WGPUShaderStage_Vertex;
        bglEntry.buffer = buf;

        WGPUBindGroupLayoutDescriptor bglDesc = {};
        bglDesc.entryCount = 1;
        bglDesc.entries = &bglEntry;
        WGPUBindGroupLayout bindGroupLayout = wgpuDeviceCreateBindGroupLayout(m_Impl->device, &bglDesc);

        // pipeline layout (used by the render pipeline, released after its creation)
        WGPUPipelineLayoutDescriptor layoutDesc = {};
        layoutDesc.bindGroupLayoutCount = 1;
        layoutDesc.bindGroupLayouts = &bindGroupLayout;
        WGPUPipelineLayout pipelineLayout = wgpuDeviceCreatePipelineLayout(m_Impl->device, &layoutDesc);

        // describe buffer layouts
        WGPUVertexAttribute vertAttrs[2] = {};
        vertAttrs[0].format = WGPUVertexFormat_Float32x2;
        vertAttrs[0].offset = 0;
        vertAttrs[0].shaderLocation = 0;
        vertAttrs[1].format = WGPUVertexFormat_Float32x3;
        vertAttrs[1].offset = 2 * sizeof(float);
        vertAttrs[1].shaderLocation = 1;
        WGPUVertexBufferLayout vertexBufferLayout = {};
        vertexBufferLayout.arrayStride = 5 * sizeof(float);
        vertexBufferLayout.attributeCount = 2;
        vertexBufferLayout.attributes = vertAttrs;

        // Fragment state
        WGPUBlendState blend = {};
        blend.color.operation = WGPUBlendOperation_Add;
        blend.color.srcFactor = WGPUBlendFactor_One;
        blend.color.dstFactor = WGPUBlendFactor_One;
        blend.alpha.operation = WGPUBlendOperation_Add;
        blend.alpha.srcFactor = WGPUBlendFactor_One;
        blend.alpha.dstFactor = WGPUBlendFactor_One;

        WGPUColorTargetState colorTarget = {};
        colorTarget.format = webgpu::getSwapChainFormat(m_Impl->device);
        colorTarget.blend = &blend;
        colorTarget.writeMask = WGPUColorWriteMask_All;

        WGPUFragmentState fragment = {};
        fragment.module = pipeDesc->FS->m_Impl->shaderModule;
        fragment.entryPoint = "main";
        fragment.targetCount = 1;
        fragment.targets = &colorTarget;

        WGPURenderPipelineDescriptor desc = {};
        desc.fragment = &fragment;

        // Other state
        desc.layout = pipelineLayout;
        desc.depthStencil = nullptr;

        desc.vertex.module = pipeDesc->VS->m_Impl->shaderModule;
        desc.vertex.entryPoint = "main";
        desc.vertex.bufferCount = 1;//0;
        desc.vertex.buffers = &vertexBufferLayout;

        desc.multisample.count = 1;
        desc.multisample.mask = 0xFFFFFFFF;
        desc.multisample.alphaToCoverageEnabled = false;

        desc.primitive.frontFace = WGPUFrontFace_CCW;
        desc.primitive.cullMode = WGPUCullMode_None;
        desc.primitive.topology = WGPUPrimitiveTopology_TriangleList;
        desc.primitive.stripIndexFormat = WGPUIndexFormat_Undefined;

        WGPURenderPipeline pipeline = wgpuDeviceCreateRenderPipeline(m_Impl->device, &desc);
        Ref<RPipeline> pipelineObj = CreateRef<RPipeline>();
        pipelineObj->m_Impl->pipeline = pipeline;
        return pipelineObj;
    }

    void Renderer::RenderOneFrame()
    {

#ifndef __EMSCRIPTEN__
        /*
         * TODO: wgpuSwapChainPresent is unsupported in Emscripten, so what do we do?
         */
        wgpuSwapChainPresent(m_Impl->swapchain);
#endif
    }


}