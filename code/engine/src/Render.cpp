#include "stdafx.h"
#include "Render.h"
#include "WebGpuHelper.h"
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

    WGPUTextureFormat g_WGPUTextureFormat[(int)TextureFormat::TF_Count] =
    {
        WGPUTextureFormat_R8Unorm,
        WGPUTextureFormat_R8Snorm,
        WGPUTextureFormat_R8Uint,
        WGPUTextureFormat_R8Sint,
        WGPUTextureFormat_R16Uint,
        WGPUTextureFormat_R16Sint,
        WGPUTextureFormat_R16Float,
        WGPUTextureFormat_RG8Unorm,
        WGPUTextureFormat_RG8Snorm,
        WGPUTextureFormat_RG8Uint,
        WGPUTextureFormat_RG8Sint,
        WGPUTextureFormat_R32Float,
        WGPUTextureFormat_R32Uint,
        WGPUTextureFormat_R32Sint,
        WGPUTextureFormat_RG16Uint,
        WGPUTextureFormat_RG16Sint,
        WGPUTextureFormat_RG16Float,
        WGPUTextureFormat_RGBA8Unorm,
        WGPUTextureFormat_RGBA8UnormSrgb,
        WGPUTextureFormat_RGBA8Snorm,
        WGPUTextureFormat_RGBA8Uint,
        WGPUTextureFormat_RGBA8Sint,
        WGPUTextureFormat_BGRA8Unorm,
        WGPUTextureFormat_BGRA8UnormSrgb,
        WGPUTextureFormat_RGB10A2Unorm,
        WGPUTextureFormat_RG11B10Ufloat,
        WGPUTextureFormat_RGB9E5Ufloat,
        WGPUTextureFormat_RG32Float,
        WGPUTextureFormat_RG32Uint,
        WGPUTextureFormat_RG32Sint,
        WGPUTextureFormat_RGBA16Uint,
        WGPUTextureFormat_RGBA16Sint,
        WGPUTextureFormat_RGBA16Float,
        WGPUTextureFormat_RGBA32Float,
        WGPUTextureFormat_RGBA32Uint,
        WGPUTextureFormat_RGBA32Sint,
        WGPUTextureFormat_Stencil8,
        WGPUTextureFormat_Depth16Unorm,
        WGPUTextureFormat_Depth24Plus,
        WGPUTextureFormat_Depth24PlusStencil8,
        WGPUTextureFormat_Depth32Float,
        WGPUTextureFormat_Depth32FloatStencil8,
        WGPUTextureFormat_BC1RGBAUnorm,
        WGPUTextureFormat_BC1RGBAUnormSrgb,
        WGPUTextureFormat_BC2RGBAUnorm,
        WGPUTextureFormat_BC2RGBAUnormSrgb,
        WGPUTextureFormat_BC3RGBAUnorm,
        WGPUTextureFormat_BC3RGBAUnormSrgb,
        WGPUTextureFormat_BC4RUnorm,
        WGPUTextureFormat_BC4RSnorm,
        WGPUTextureFormat_BC5RGUnorm,
        WGPUTextureFormat_BC5RGSnorm,
        WGPUTextureFormat_BC6HRGBUfloat,
        WGPUTextureFormat_BC6HRGBFloat,
        WGPUTextureFormat_BC7RGBAUnorm,
        WGPUTextureFormat_BC7RGBAUnormSrgb,
        WGPUTextureFormat_ETC2RGB8Unorm,
        WGPUTextureFormat_ETC2RGB8UnormSrgb,
        WGPUTextureFormat_ETC2RGB8A1Unorm,
        WGPUTextureFormat_ETC2RGB8A1UnormSrgb,
        WGPUTextureFormat_ETC2RGBA8Unorm,
        WGPUTextureFormat_ETC2RGBA8UnormSrgb,
        WGPUTextureFormat_EACR11Unorm,
        WGPUTextureFormat_EACR11Snorm,
        WGPUTextureFormat_EACRG11Unorm,
        WGPUTextureFormat_EACRG11Snorm,
        WGPUTextureFormat_ASTC4x4Unorm,
        WGPUTextureFormat_ASTC4x4UnormSrgb,
        WGPUTextureFormat_ASTC5x4Unorm,
        WGPUTextureFormat_ASTC5x4UnormSrgb,
        WGPUTextureFormat_ASTC5x5Unorm,
        WGPUTextureFormat_ASTC5x5UnormSrgb,
        WGPUTextureFormat_ASTC6x5Unorm,
        WGPUTextureFormat_ASTC6x5UnormSrgb,
        WGPUTextureFormat_ASTC6x6Unorm,
        WGPUTextureFormat_ASTC6x6UnormSrgb,
        WGPUTextureFormat_ASTC8x5Unorm,
        WGPUTextureFormat_ASTC8x5UnormSrgb,
        WGPUTextureFormat_ASTC8x6Unorm,
        WGPUTextureFormat_ASTC8x6UnormSrgb,
        WGPUTextureFormat_ASTC8x8Unorm,
        WGPUTextureFormat_ASTC8x8UnormSrgb,
        WGPUTextureFormat_ASTC10x5Unorm,
        WGPUTextureFormat_ASTC10x5UnormSrgb,
        WGPUTextureFormat_ASTC10x6Unorm,
        WGPUTextureFormat_ASTC10x6UnormSrgb,
        WGPUTextureFormat_ASTC10x8Unorm,
        WGPUTextureFormat_ASTC10x8UnormSrgb,
        WGPUTextureFormat_ASTC10x10Unorm,
        WGPUTextureFormat_ASTC10x10UnormSrgb,
        WGPUTextureFormat_ASTC12x10Unorm,
        WGPUTextureFormat_ASTC12x10UnormSrgb,
        WGPUTextureFormat_ASTC12x12Unorm,
        WGPUTextureFormat_ASTC12x12UnormSrgb,
        WGPUTextureFormat_R8BG8Biplanar420Unorm,
        WGPUTextureFormat_Force32
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
    struct RTexture::WebGpuImpl
    {
        WGPUTexture texture = nullptr;
    };

    RTexture::RTexture()
    {
        m_Impl = CreateRef<WebGpuImpl>();
    }

    RTexture::~RTexture()
    {
        if (m_Impl->texture)
            wgpuTextureRelease(m_Impl->texture);
    }

    //////////////////////////////////////////////////////////////////////////
    struct RenderPass::WebGpuImpl
    {
        WGPUTextureView view;
    };

    RenderPass::RenderPass()
    {
        m_Impl = CreateRef<WebGpuImpl>();
    }

    RenderPass::~RenderPass()
    {
        if (m_Impl->view)
            wgpuTextureViewRelease(m_Impl->view);
    }

    //////////////////////////////////////////////////////////////////////////

    struct Renderer::WebGpuImpl
    {
        WGPUDevice device = nullptr;
        WGPUQueue queue = nullptr;
        WGPUSwapChain swapchain = nullptr;
    };

    Renderer::Renderer(WindowHandle window, const RendererDesc* rendererDesc)
        : m_WindowHandle(window)
    {
        m_Impl = CreateRef<WebGpuImpl>();
        m_Impl->device = webgpu::create(m_WindowHandle);
        m_Impl->queue = wgpuDeviceGetQueue(m_Impl->device);

        m_Msaa = rendererDesc->msaa;

        Platform::GetWindowSize(window, m_Width, m_Height);
        m_Impl->swapchain = webgpu::createSwapChain(m_Impl->device, m_Width, m_Height);
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

        RUSH_ASSERT(pipeDesc->VLayouts.size() <= 16);

        WGPUVertexAttribute vertAttrs[16][16] = {};
        WGPUVertexBufferLayout vertexBufferLayouts[16] = {};
        int bufferIndex = 0;
        for (const auto& vLayout : pipeDesc->VLayouts)
        {
            RUSH_ASSERT(vLayout.AttributeCount <= 16);

            // describe buffer layouts
            for (uint32_t i = 0; i < vLayout.AttributeCount; ++i)
            {
                vertAttrs[bufferIndex][i].format = g_WGPUVertexFormat[(int)vLayout.Attributes[i].Format];
                vertAttrs[bufferIndex][i].offset = vLayout.Attributes[i].Offset;
                vertAttrs[bufferIndex][i].shaderLocation = vLayout.Attributes[i].ShaderLocation;
            }

            vertexBufferLayouts[bufferIndex].arrayStride = vLayout.Stride;
            vertexBufferLayouts[bufferIndex].attributeCount = vLayout.AttributeCount;
            vertexBufferLayouts[bufferIndex].attributes = vertAttrs[bufferIndex];
            ++bufferIndex;
        }


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
        desc.vertex.bufferCount = pipeDesc->VLayouts.size();
        desc.vertex.buffers = vertexBufferLayouts;

        desc.multisample.count = m_Msaa;
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

    Ref<RenderPass> Renderer::CreateRenderPass(const RenderPassDesc* renderPassDesc)
    {
        WGPUTextureDescriptor textureDesc = {};
        if (renderPassDesc->Width == 0 || renderPassDesc->Height == 0)
        {
            textureDesc.size.width = m_Width;
            textureDesc.size.height = m_Height;
        }
        else
        {
            textureDesc.size.width = renderPassDesc->Width;
            textureDesc.size.height = renderPassDesc->Height;
        }

        textureDesc.size.depthOrArrayLayers = 1;
        textureDesc.sampleCount = m_Msaa;
        textureDesc.dimension = WGPUTextureDimension_2D;
        if (renderPassDesc->IsSwapBuffer)
        {
            textureDesc.format = WGPUTextureFormat_RGBA8Unorm;
        }
        else
        {
            textureDesc.format = g_WGPUTextureFormat[(int)renderPassDesc->Format];
        }
        textureDesc.usage = WGPUTextureUsage_RenderAttachment;
        textureDesc.mipLevelCount = 1;

        Ref<RTexture> textureObject = CreateRef<RTexture>();
        textureObject->m_Impl->texture = wgpuDeviceCreateTexture(m_Impl->device, &textureDesc);

        WGPUTextureViewDescriptor textureViewDesc = {};
        textureViewDesc.arrayLayerCount = 1;
        textureViewDesc.mipLevelCount = 1;
        WGPUTextureView view = wgpuTextureCreateView(textureObject->m_Impl->texture, &textureViewDesc);

        Ref<RenderPass> renderPassObj = CreateRef<RenderPass>();
        renderPassObj->m_RenderTarget = textureObject;
        renderPassObj->m_Impl->view = view;
        renderPassObj->ClearColor = renderPassDesc->ClearColor;
        renderPassObj->IsSwapBuffer = renderPassDesc->IsSwapBuffer;

        return renderPassObj;
    }

    void Renderer::RenderOnePass(Ref<RenderPass> renderPass, Ref<RenderContent> content)
    {
        WGPURenderPassColorAttachment colorDesc = {};
        colorDesc.view = renderPass->m_Impl->view;

        if (renderPass->IsSwapBuffer)
        {
            WGPUTextureView backBufView = wgpuSwapChainGetCurrentTextureView(m_Impl->swapchain);			// create textureView
            colorDesc.resolveTarget = backBufView;
        }

        colorDesc.loadOp = WGPULoadOp_Clear;
        colorDesc.storeOp = WGPUStoreOp_Store;
#ifdef __EMSCRIPTEN__
        // Dawn has both clearValue/clearColor but only Color works; Emscripten only has Value
        colorDesc.clearValue.r = renderPass->ClearColor.r;
        colorDesc.clearValue.g = renderPass->ClearColor.g;
        colorDesc.clearValue.b = renderPass->ClearColor.b;
        colorDesc.clearValue.a = renderPass->ClearColor.a;
#else
        colorDesc.clearColor.r = renderPass->ClearColor.r;
        colorDesc.clearColor.g = renderPass->ClearColor.g;
        colorDesc.clearColor.b = renderPass->ClearColor.b;
        colorDesc.clearColor.a = renderPass->ClearColor.a;
#endif

        WGPURenderPassDescriptor desc = {};
        desc.colorAttachmentCount = 1;
        desc.colorAttachments = &colorDesc;

        WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(m_Impl->device, nullptr);			// create encoder
        WGPURenderPassEncoder pass = wgpuCommandEncoderBeginRenderPass(encoder, &desc);	// create pass

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

        if (renderPass->IsSwapBuffer)
            wgpuTextureViewRelease(colorDesc.resolveTarget);
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