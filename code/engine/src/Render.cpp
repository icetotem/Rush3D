#include "stdafx.h"
#include "Render.h"
#include "webgpu.h"
#include "dawn/webgpu.h"

namespace rush
{
    
    WGPUFrontFace g_WGPUFrontFace[(int)FrontFace::FF_Count] =
    {
        WGPUFrontFace_CCW, WGPUFrontFace_CW
    };

    WGPUCullMode g_WGPUCullMode[(int)CullMode::CM_Count] =
    {
        WGPUCullMode_None, WGPUCullMode_Front, WGPUCullMode_Back
    };

    WGPUPrimitiveTopology g_WGPUPrimitiveTopology[(int)PrimitiveType::PT_Count] =
    {
        WGPUPrimitiveTopology_PointList, WGPUPrimitiveTopology_LineList, WGPUPrimitiveTopology_LineStrip, WGPUPrimitiveTopology_TriangleList, WGPUPrimitiveTopology_TriangleStrip
    };
    
    WGPUBlendOperation g_WGPUBlendOperation[(int)BlendOperation::BO_Count] =
    {
        WGPUBlendOperation_Add, WGPUBlendOperation_Subtract, WGPUBlendOperation_ReverseSubtract, WGPUBlendOperation_Min, WGPUBlendOperation_Max
    };

    WGPUBlendFactor g_WGPUBlendFactor[(int)BlendFactor::BF_Count] =
    {
        WGPUBlendFactor_Zero, 
        WGPUBlendFactor_One, 
        WGPUBlendFactor_Src, 
        WGPUBlendFactor_OneMinusSrc, 
        WGPUBlendFactor_SrcAlpha,
        WGPUBlendFactor_OneMinusSrcAlpha, 
        WGPUBlendFactor_Dst, 
        WGPUBlendFactor_OneMinusDst, 
        WGPUBlendFactor_DstAlpha, 
        WGPUBlendFactor_OneMinusDstAlpha, 
        WGPUBlendFactor_SrcAlphaSaturated, 
        WGPUBlendFactor_Constant,
        WGPUBlendFactor_OneMinusConstant
    };

    WGPUCompareFunction g_WGPUCompareFunction[(int)DepthCompareFunction::DC_Count] =
    {
        WGPUCompareFunction_Never, WGPUCompareFunction_Less, WGPUCompareFunction_Equal, WGPUCompareFunction_LessEqual, WGPUCompareFunction_Greater, WGPUCompareFunction_NotEqual, WGPUCompareFunction_GreaterEqual, WGPUCompareFunction_Always
    };

    WGPUVertexFormat g_WGPUVertexFormat[(int)VertexFormat::VF_Count] =
    {
        WGPUVertexFormat_Uint8x2,
        WGPUVertexFormat_Uint8x4,
        WGPUVertexFormat_Sint8x2,
        WGPUVertexFormat_Sint8x4,
        WGPUVertexFormat_Unorm8x2,
        WGPUVertexFormat_Unorm8x4,
        WGPUVertexFormat_Snorm8x2,
        WGPUVertexFormat_Snorm8x4,
        WGPUVertexFormat_Uint16x2,
        WGPUVertexFormat_Uint16x4,
        WGPUVertexFormat_Sint16x2,
        WGPUVertexFormat_Sint16x4,
        WGPUVertexFormat_Unorm16x2,
        WGPUVertexFormat_Unorm16x4,
        WGPUVertexFormat_Snorm16x2,
        WGPUVertexFormat_Snorm16x4,
        WGPUVertexFormat_Float16x2,
        WGPUVertexFormat_Float16x4,
        WGPUVertexFormat_Float32,
        WGPUVertexFormat_Float32x2,
        WGPUVertexFormat_Float32x3,
        WGPUVertexFormat_Float32x4,
        WGPUVertexFormat_Uint32,
        WGPUVertexFormat_Uint32x2,
        WGPUVertexFormat_Uint32x3,
        WGPUVertexFormat_Uint32x4,
        WGPUVertexFormat_Sint32,
        WGPUVertexFormat_Sint32x2,
        WGPUVertexFormat_Sint32x3,
        WGPUVertexFormat_Sint32x4
    };

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
        if (m_Impl->buffer)
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
        if (m_Impl->shaderModule)
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
        if (m_Impl->pipeline)
            wgpuRenderPipelineRelease(m_Impl->pipeline);
    }

    //////////////////////////////////////////////////////////////////////////

    struct UniformBuffer::WebGpuImpl
    {
        WGPUBindGroup bindGroup = nullptr;
        WGPUBuffer buffer = nullptr;
    };

    UniformBuffer::UniformBuffer()
    {
        m_Impl = CreateRef<WebGpuImpl>();
    }

    UniformBuffer::~UniformBuffer()
    {
        if (m_Impl->bindGroup)
            wgpuBindGroupRelease(m_Impl->bindGroup);
        if (m_Impl->buffer)
            wgpuBufferRelease(m_Impl->buffer);
    }

    //////////////////////////////////////////////////////////////////////////

    struct FrameBuffer::WebGpuImpl
    {
        WGPUTextureView textureView = nullptr;
    };

    FrameBuffer::FrameBuffer()
    {
        m_Impl = CreateRef<WebGpuImpl>();
    }

    FrameBuffer::~FrameBuffer()
    {
        if (m_Impl->textureView)
            wgpuTextureViewRelease(m_Impl->textureView);
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

    Ref<RBuffer> Renderer::CreateVertexBuffer(const void* data, size_t size, uint32_t stride)
    {
        WGPUBufferDescriptor desc = {};
        desc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex;
        desc.size = size;
        WGPUBuffer buffer = wgpuDeviceCreateBuffer(m_Impl->device, &desc);
        wgpuQueueWriteBuffer(m_Impl->queue, buffer, 0, data, size);
        Ref<RBuffer> bufferObj = CreateRef<RBuffer>();
        bufferObj->m_Impl->buffer = buffer;
        bufferObj->m_Size = size;
        bufferObj->m_Stride = stride;
        bufferObj->m_Count = size / stride;
        return bufferObj;
    }

    Ref<RBuffer> Renderer::CreateIndexBuffer(const void* data, size_t size, uint32_t stride)
    {
        WGPUBufferDescriptor desc = {};
        desc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Index;
        desc.size = size;
        WGPUBuffer buffer = wgpuDeviceCreateBuffer(m_Impl->device, &desc);
        wgpuQueueWriteBuffer(m_Impl->queue, buffer, 0, data, size);
        Ref<RBuffer> bufferObj = CreateRef<RBuffer>();
        bufferObj->m_Impl->buffer = buffer;
        bufferObj->m_Size = size;
        bufferObj->m_Stride = stride;
        bufferObj->m_Count = size / stride;
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
        WGPUVertexAttribute vertAttrs[32] = {};
        for (uint32_t i = 0; i < pipeDesc->VLayout.AttributeCount; ++i)
        {
            vertAttrs[i].format = g_WGPUVertexFormat[(int)pipeDesc->VLayout.Attributes[i].Format];
            vertAttrs[i].offset = pipeDesc->VLayout.Attributes[i].Offset;
            vertAttrs[i].shaderLocation = pipeDesc->VLayout.Attributes[i].ShaderLocation;
        }

        WGPUVertexBufferLayout vertexBufferLayout = {};
        vertexBufferLayout.arrayStride = pipeDesc->VLayout.Stride;
        vertexBufferLayout.attributeCount = pipeDesc->VLayout.AttributeCount;
        vertexBufferLayout.attributes = vertAttrs;

        // Fragment state
        WGPUBlendState blend = {};
        blend.color.operation = g_WGPUBlendOperation[(int)pipeDesc->Blend.OpColor];
        blend.color.srcFactor = g_WGPUBlendFactor[(int)pipeDesc->Blend.SrcColor]; 
        blend.color.dstFactor = g_WGPUBlendFactor[(int)pipeDesc->Blend.DstColor];
        blend.alpha.operation = g_WGPUBlendOperation[(int)pipeDesc->Blend.OpAlpha];
        blend.alpha.srcFactor = g_WGPUBlendFactor[(int)pipeDesc->Blend.SrcAlpha];
        blend.alpha.dstFactor = g_WGPUBlendFactor[(int)pipeDesc->Blend.DstAlpha];

        WGPUColorTargetState colorTarget = {};
        colorTarget.format = webgpu::getSwapChainFormat(m_Impl->device);
        colorTarget.blend = &blend;
        colorTarget.writeMask = pipeDesc->WriteMask;

        WGPUFragmentState fragment = {};
        fragment.module = pipeDesc->FS->m_Impl->shaderModule;
        fragment.entryPoint = "main";
        fragment.targetCount = 1;
        fragment.targets = &colorTarget;

        WGPURenderPipelineDescriptor desc = {};
        desc.fragment = &fragment;

        WGPUDepthStencilState depthStencil = {};
        depthStencil.depthWriteEnabled = pipeDesc->WriteDepth;

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

        desc.primitive.frontFace = g_WGPUFrontFace[(int)pipeDesc->Front];
        desc.primitive.cullMode = g_WGPUCullMode[(int)pipeDesc->Cull];
        desc.primitive.topology = g_WGPUPrimitiveTopology[(int)pipeDesc->Primitive];
        desc.primitive.stripIndexFormat = WGPUIndexFormat_Undefined;

        WGPURenderPipeline pipeline = wgpuDeviceCreateRenderPipeline(m_Impl->device, &desc);
        Ref<RPipeline> pipelineObj = CreateRef<RPipeline>();
        pipelineObj->m_Impl->pipeline = pipeline;

        wgpuBindGroupLayoutRelease(bindGroupLayout);
        wgpuPipelineLayoutRelease(pipelineLayout);

        return pipelineObj;
    }

    Ref<UniformBuffer> Renderer::CreateUniformBuffer(const void* data, size_t size, uint32_t shaderVisibility)
    {
        WGPUBufferBindingLayout buf = {};
        buf.type = WGPUBufferBindingType_Uniform;

        // bind group layout (used by both the pipeline layout and uniform bind group, released at the end of this function)
        WGPUBindGroupLayoutEntry bglEntry = {};
        bglEntry.binding = 0;
        bglEntry.visibility = shaderVisibility;
        bglEntry.buffer = buf;

        WGPUBindGroupLayoutDescriptor bglDesc = {};
        bglDesc.entryCount = 1;
        bglDesc.entries = &bglEntry;
        WGPUBindGroupLayout bindGroupLayout = wgpuDeviceCreateBindGroupLayout(m_Impl->device, &bglDesc);

        // pipeline layout (used by the render pipeline, released after its creation)
        WGPUPipelineLayoutDescriptor layoutDesc = {};
        layoutDesc.bindGroupLayoutCount = 1;
        layoutDesc.bindGroupLayouts = &bindGroupLayout;

        WGPUBufferDescriptor desc = {};
        desc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform;
        desc.size = size;
        WGPUBuffer buffer = wgpuDeviceCreateBuffer(m_Impl->device, &desc);
        wgpuQueueWriteBuffer(m_Impl->queue, buffer, 0, data, size);

        WGPUBindGroupEntry bgEntry = {};
        bgEntry.binding = 0;
        bgEntry.buffer = buffer;
        bgEntry.offset = 0;
        bgEntry.size = size;

        WGPUBindGroupDescriptor bgDesc = {};
        bgDesc.layout = bindGroupLayout;
        bgDesc.entryCount = 1;
        bgDesc.entries = &bgEntry;

        auto bindGroup = wgpuDeviceCreateBindGroup(m_Impl->device, &bgDesc);

        Ref<UniformBuffer> bindGroupObj = CreateRef<UniformBuffer>();
        bindGroupObj->m_Impl->bindGroup = bindGroup;
        bindGroupObj->m_Impl->buffer = buffer;

        wgpuBindGroupLayoutRelease(bindGroupLayout);

        return bindGroupObj;
    }

    void Renderer::WriteUniformBuffer(Ref<UniformBuffer> buffer, size_t offset, const void* data, size_t size)
    {
        wgpuQueueWriteBuffer(m_Impl->queue, buffer->m_Impl->buffer, offset, data, size);
    }

    void Renderer::RenderOnePass(Ref<RenderPass> renderPass, Ref<RenderContent> content)
    {
        WGPUTextureView backBufView = wgpuSwapChainGetCurrentTextureView(m_Impl->swapchain);			// create textureView

        WGPURenderPassColorAttachment colorDesc = {};
        colorDesc.view = backBufView;
        colorDesc.loadOp = WGPULoadOp_Clear;
        colorDesc.storeOp = WGPUStoreOp_Store;
#ifdef __EMSCRIPTEN__
        // Dawn has both clearValue/clearColor but only Color works; Emscripten only has Value
        colorDesc.clearValue.r = renderPass->m_ClearColor.r;
        colorDesc.clearValue.g = renderPass->m_ClearColor.g;
        colorDesc.clearValue.b = renderPass->m_ClearColor.b;
        colorDesc.clearValue.a = renderPass->m_ClearColor.a;
#else
        colorDesc.clearColor.r = renderPass->m_ClearColor.r;
        colorDesc.clearColor.g = renderPass->m_ClearColor.g;
        colorDesc.clearColor.b = renderPass->m_ClearColor.b;
        colorDesc.clearColor.a = renderPass->m_ClearColor.a;
#endif

        WGPURenderPassDescriptor renderPassDesc = {};
        renderPassDesc.colorAttachmentCount = 1;
        renderPassDesc.colorAttachments = &colorDesc;

        WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(m_Impl->device, nullptr);			// create encoder
        WGPURenderPassEncoder pass = wgpuCommandEncoderBeginRenderPass(encoder, &renderPassDesc);	// create pass

        for (const auto batch : content->m_Batches)
        {
            if (batch->Pipeline)
            {
                wgpuRenderPassEncoderSetPipeline(pass, batch->Pipeline->m_Impl->pipeline);

                if (batch->Uniforms)
                {
                    wgpuRenderPassEncoderSetBindGroup(pass, 0, batch->Uniforms->m_Impl->bindGroup, 0, 0);
                }

                int slot = 0;
                for (auto vb : batch->VBList)
                {
                    wgpuRenderPassEncoderSetVertexBuffer(pass, slot, vb->m_Impl->buffer, 0, WGPU_WHOLE_SIZE);
                    ++slot;
                }

                if (batch->IB)
                {
                    wgpuRenderPassEncoderSetIndexBuffer(pass, batch->IB->m_Impl->buffer,
                        (batch->IB->m_Stride == sizeof(uint16_t) ? WGPUIndexFormat_Uint16 : WGPUIndexFormat_Uint32), 0, WGPU_WHOLE_SIZE);
                    wgpuRenderPassEncoderDrawIndexed(pass, batch->IB->m_Count, batch->InstanceCount, batch->FirstIndex, batch->FirstVertex, 0);
                }
                else
                {
                    // todo  WGPUProcRenderBundleEncoderDraw

                }

            }
        }

        wgpuRenderPassEncoderEnd(pass);
        wgpuRenderPassEncoderRelease(pass);														// release pass
        WGPUCommandBuffer commands = wgpuCommandEncoderFinish(encoder, nullptr);				// create commands
        wgpuCommandEncoderRelease(encoder);														// release encoder

        wgpuQueueSubmit(m_Impl->queue, 1, &commands);
        wgpuCommandBufferRelease(commands);														// release commands

        wgpuTextureViewRelease(backBufView);													// release textureView
    }


    void Renderer::SwapBuffers()
    {
#ifndef __EMSCRIPTEN__
        /*
         * TODO: wgpuSwapChainPresent is unsupported in Emscripten, so what do we do?
         */
        wgpuSwapChainPresent(m_Impl->swapchain);
#endif
    }

}