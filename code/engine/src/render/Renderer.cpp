#include "stdafx.h"
#include "render/Renderer.h"
#include "render/RTexture.h"
#include "render/RBuffer.h"
#include "render/Shader.h"
#include "render/Uniform.h"
#include "render/RPipeline.h"
#include "Window.h"
#include <dawn/webgpu.h>
#include <dawn/dawn_proc.h>
#include <dawn/native/DawnNative.h>
#include <dawn/webgpu_cpp.h>
#include "RenderContex.h"
#include "render/RenderPass.h"

namespace rush
{
    extern HMap<WGPUFeatureName, std::string> g_Features;
    extern HMap<WGPUAdapterType, std::string> g_AdapterType;
    extern HMap<WGPUBackendType, std::string> g_BackendType;
    extern WGPUFrontFace g_WGPUFrontFace[(int)FrontFace::Count];
    extern WGPUCullMode g_WGPUCullMode[(int)CullMode::Count];
    extern WGPUPrimitiveTopology g_WGPUPrimitiveTopology[(int)PrimitiveType::Count];
    extern WGPUBlendOperation g_WGPUBlendOperation[(int)BlendOperation::Count];
    extern WGPUBlendFactor g_WGPUBlendFactor[(int)BlendFactor::Count];
    extern WGPUTextureDimension g_TextureDimension[(int)TextureDimension::Count];
    extern WGPUCompareFunction g_WGPUCompareFunction[(int)DepthCompareFunction::Count];
    extern WGPUVertexFormat g_WGPUVertexFormat[(int)VertexFormat::Count];
    extern WGPUTextureFormat g_WGPUTextureFormat[(int)TextureFormat::Count];

    static struct 
    {
        struct 
        {
            DawnProcTable procTable;
            std::unique_ptr<dawn::native::Instance> instance = nullptr;
        } dawn_native;

        struct 
        {
            dawn::native::Adapter handle;
            wgpu::BackendType backendType;
        } adapter;

    } gpuContext = {};

    static class WebGpuInitializer
    {
    public:
        WebGpuInitializer()
        {
            // Set up the native procs for the global proctable
            gpuContext.dawn_native.procTable = dawn::native::GetProcs();
            dawnProcSetProcs(&gpuContext.dawn_native.procTable);
            gpuContext.dawn_native.instance = std::make_unique<dawn::native::Instance>();
            gpuContext.dawn_native.instance->DiscoverDefaultAdapters();
            gpuContext.dawn_native.instance->EnableBackendValidation(true);
            gpuContext.dawn_native.instance->SetBackendValidationLevel(dawn::native::BackendValidationLevel::Full);

            // Dawn backend type.
            // Default to D3D12, Metal, Vulkan, OpenGL in that order as D3D12 and Metal
            // are the preferred on their respective platforms, and Vulkan is preferred to
            // OpenGL
            gpuContext.adapter.backendType =
#if defined(DAWN_ENABLE_BACKEND_D3D12)
                wgpu::BackendType::D3D12;
#elif defined(DAWN_ENABLE_BACKEND_METAL)
                wgpu::BackendType::Metal;
#elif defined(DAWN_ENABLE_BACKEND_VULKAN)
                wgpu::BackendType::Vulkan;
#elif defined(DAWN_ENABLE_BACKEND_OPENGL)
                wgpu::BackendType::OpenGL;
#else
                wgpu::backendType::Null;
#endif
                gpuContext.adapter.handle = nullptr;
        }

    protected:
        DawnProcTable procTable;
    } g_WebGpuInitializer;

    static WGPUAdapter RequestAdapter(WGPURequestAdapterOptions* options)
    {
        WGPUPowerPreference powerPreference
            = options ?
            (options->powerPreference == WGPUPowerPreference_HighPerformance ?
                WGPUPowerPreference_HighPerformance :
                WGPUPowerPreference_LowPower) :
            WGPUPowerPreference_LowPower;

        // Search available adapters for a good match, in the following priority
        // order
        std::vector<wgpu::AdapterType> typePriority;
        if (powerPreference == WGPUPowerPreference_LowPower) 
        {
            // low power
            typePriority = std::vector<wgpu::AdapterType>{
              wgpu::AdapterType::IntegratedGPU,
              wgpu::AdapterType::DiscreteGPU,
              wgpu::AdapterType::CPU,
            };
        }
        else if (powerPreference == WGPUPowerPreference_HighPerformance) 
        {
            // high performance
            typePriority = std::vector<wgpu::AdapterType>{
              wgpu::AdapterType::DiscreteGPU,
              wgpu::AdapterType::IntegratedGPU,
              wgpu::AdapterType::CPU,
            };
        }

        std::vector<dawn::native::Adapter> adapters = gpuContext.dawn_native.instance->EnumerateAdapters();
        for (auto reqType : typePriority) 
        {
            for (const dawn::native::Adapter& adapter : adapters) 
            {
                wgpu::AdapterProperties ap;
                adapter.GetProperties(&ap);
                if (ap.adapterType == reqType
                    && (reqType == wgpu::AdapterType::CPU || ap.backendType == gpuContext.adapter.backendType)) 
                {
                    gpuContext.adapter.handle = adapter;
                    return gpuContext.adapter.handle.Get();
                }
            }
        }

        return nullptr;
    }

    static void wgpu_error_callback(WGPUErrorType error_type, char const* message, void* userdata)
    {
        const char* error_type_name = "";
        switch (error_type) {
        case WGPUErrorType_Validation:
            error_type_name = "Validation";
            break;
        case WGPUErrorType_OutOfMemory:
            error_type_name = "Out of memory";
            break;
        case WGPUErrorType_Unknown:
            error_type_name = "Unknown";
            break;
        case WGPUErrorType_DeviceLost:
            error_type_name = "Device lost";
            break;
        default:
            return;
        }

        LOG_ERROR("WGPU Error(%d) %s: %s", (int)error_type, error_type_name, message);
    }

    static std::unique_ptr<wgpu::ChainedStruct> SurfaceDescriptor(void* display, void* window)
    {
#if defined(RUSH_PLATFORM_WINDOWS)
        std::unique_ptr<wgpu::SurfaceDescriptorFromWindowsHWND> desc
            = std::make_unique<wgpu::SurfaceDescriptorFromWindowsHWND>();
        desc->hwnd = window;
        desc->hinstance = GetModuleHandle(nullptr);
        return std::move(desc);
#elif defined(RUSH_PLATFORM_LINUX) // X11
        std::unique_ptr<wgpu::SurfaceDescriptorFromXlibWindow> desc
            = std::make_unique<wgpu::SurfaceDescriptorFromXlibWindow>();
        desc->display = display;
        desc->window = *((uint32_t*)window);
        return std::move(desc);
#endif

        return nullptr;
    }

    static void setup_render_pass(Ref<RenderContex> wgpu_context, const Vector4& clearColor, float clearDepth)
    {
        // Color attachment
        wgpu_context->render_pass.color_attachments[0] = {
            .view = NULL, /* Assigned later */
            .loadOp = WGPULoadOp_Clear,
            .storeOp = WGPUStoreOp_Store,
            .clearValue = {
              .r = clearColor.r,
              .g = clearColor.g,
              .b = clearColor.b,
              .a = clearColor.a,
            },
        };

        // Render pass descriptor
        wgpu_context->render_pass.descriptor = {
          .label = "Render pass descriptor",
          .colorAttachmentCount = 1,
          .colorAttachments = wgpu_context->render_pass.color_attachments,
          .depthStencilAttachment = NULL,
        };

    }

    //////////////////////////////////////////////////////////////////////////

    Renderer::Renderer(Ref<Window> window, const RendererDesc* rendererDesc)
        : m_Window(window)
    {
        m_Msaa = rendererDesc->msaa;
        m_Width = m_Window->GetWidth();
        m_Height = m_Window->GetHeight();

        m_Contex = CreateRef<RenderContex>();
        m_Contex->swapChain.presentMode = (rendererDesc->vsync ? WGPUPresentMode_Fifo : WGPUPresentMode_Mailbox);

        CreateAdapter();
        CreateDevice();
        CreateQueue();
        GetInformation();
        CreateSurface();
        CreateSwapChain();

        setup_render_pass(m_Contex, Vector4(0.5f), 1.0f);
    }

    Renderer::~Renderer()
    {
        WGPU_RELEASE_RESOURCE(TextureView, m_Contex->depthStencil.frameBuffer);
        WGPU_RELEASE_RESOURCE(Texture, m_Contex->depthStencil.texture);
        WGPU_RELEASE_RESOURCE(SwapChain, m_Contex->swapChain.instance);
        WGPU_RELEASE_RESOURCE(Queue, m_Contex->queue);
        WGPU_RELEASE_RESOURCE(Device, m_Contex->device);
    }


    void Renderer::CreateAdapter()
    {
        WGPURequestAdapterOptions adapterOptions;
        adapterOptions.powerPreference = WGPUPowerPreference_HighPerformance;
        m_Contex->adapter = RequestAdapter(&adapterOptions);

        if (m_Contex->adapter == nullptr)
        {
            LOG_ERROR("WGPU create adapter failed");
        }
    }

    void Renderer::CreateDevice()
    {
        WGPUFeatureName required_features[2] = 
        {
            WGPUFeatureName_TextureCompressionBC,
            WGPUFeatureName_BGRA8UnormStorage,
        };

        WGPUDeviceDescriptor deviceDescriptor = {
          .requiredFeatureCount = (uint32_t)ARRAY_SIZE(required_features),
          .requiredFeatures = required_features,
        };

        m_Contex->device = wgpuAdapterCreateDevice(m_Contex->adapter, &deviceDescriptor);

        if (m_Contex->device == nullptr)
        {
            LOG_ERROR("WGPU create device failed");
        }

        wgpuDeviceSetUncapturedErrorCallback(m_Contex->device, &wgpu_error_callback, (void*)m_Contex.get());
    }

    void Renderer::CreateQueue()
    {
        m_Contex->queue = wgpuDeviceGetQueue(m_Contex->device);
    }

    void Renderer::CreateSurface()
    {
        std::unique_ptr<wgpu::ChainedStruct> sd = SurfaceDescriptor(m_Window->GetDisplay(), m_Window->GetNativeHandle());
        wgpu::SurfaceDescriptor descriptor;
        descriptor.nextInChain = sd.get();
        wgpu::Surface surface = wgpu::Instance(gpuContext.dawn_native.instance->Get())
            .CreateSurface(&descriptor);
        if (surface) 
        {
            WGPUSurface surf = surface.Get();
            wgpuSurfaceReference(surf);
            m_Contex->surface = surf;
        }
        else
        {
            LOG_ERROR("WGPU create surface failed");
        }
    }

    void Renderer::CreateSwapChain()
    {
        /* Create the swap chain */
        WGPUSwapChainDescriptor swap_chain_descriptor = {
          .usage = WGPUTextureUsage_RenderAttachment,
          .format = WGPUTextureFormat_BGRA8Unorm,
          .width = m_Width,
          .height = m_Height,
          .presentMode = m_Contex->swapChain.presentMode,
        };

        if (m_Contex->swapChain.instance) {
            wgpuSwapChainRelease(m_Contex->swapChain.instance);
        }
        m_Contex->swapChain.instance = wgpuDeviceCreateSwapChain(
            m_Contex->device, m_Contex->surface,
            &swap_chain_descriptor);

        if (m_Contex->swapChain.instance == nullptr)
        {
            LOG_ERROR("WGPU create swapchain failed");
        }

        /* Find a suitable depth format */
        m_Contex->swapChain.format = swap_chain_descriptor.format;
    }

    void Renderer::GetInformation()
    {
        // enumerate properties
        WGPUAdapterProperties properties = {};
        wgpuAdapterGetProperties(m_Contex->adapter, &properties);
        LOG_INFO("GPU Vendor: {}", properties.vendorName);
        LOG_INFO("GPU Architecture: {}", properties.architecture);
        LOG_INFO("GPU Name: {}", properties.name);
        LOG_INFO("GPU Driver: {}", properties.driverDescription);
        LOG_INFO("Adapter Type: {}", g_AdapterType[properties.adapterType]);
        LOG_INFO("Backend Type: {}", g_BackendType[properties.backendType]);

        // enumerate features
        WGPUFeatureName features[100];
        size_t featureCount = wgpuAdapterEnumerateFeatures(m_Contex->adapter, features);
        for (int i = 0; i < featureCount; ++i)
        {
            auto feature = features[i];
            LOG_INFO("Support Feature: {}", g_Features[feature]);
        }

        // limits
        LOG_INFO("\n---------------Limits-----------------");
        WGPUSupportedLimits limits = {};
        if (wgpuAdapterGetLimits(m_Contex->adapter, &limits)) // ??? always false
        {
            memcpy(&m_Caps, &limits.limits, sizeof(WGPULimits));
            LOG_INFO("maxTextureDimension1D: {}", limits.limits.maxTextureDimension1D);
            LOG_INFO("maxTextureDimension2D: {}", limits.limits.maxTextureDimension2D);
            LOG_INFO("maxTextureDimension3D: {}", limits.limits.maxTextureDimension3D);
            LOG_INFO("maxTextureArrayLayers: {}", limits.limits.maxTextureArrayLayers);
            LOG_INFO("maxBindGroups: {}", limits.limits.maxBindGroups);
            LOG_INFO("maxBindGroupsPlusVertexBuffers: {}", limits.limits.maxBindGroupsPlusVertexBuffers);
            LOG_INFO("maxBindingsPerBindGroup: {}", limits.limits.maxBindingsPerBindGroup);
            LOG_INFO("maxDynamicUniformBuffersPerPipelineLayout: {}", limits.limits.maxDynamicUniformBuffersPerPipelineLayout);
            LOG_INFO("maxDynamicStorageBuffersPerPipelineLayout: {}", limits.limits.maxDynamicStorageBuffersPerPipelineLayout);
            LOG_INFO("maxSampledTexturesPerShaderStage: {}", limits.limits.maxSampledTexturesPerShaderStage);
            LOG_INFO("maxSamplersPerShaderStage: {}", limits.limits.maxSamplersPerShaderStage);
            LOG_INFO("maxStorageBuffersPerShaderStage: {}", limits.limits.maxStorageBuffersPerShaderStage);
            LOG_INFO("maxStorageTexturesPerShaderStage: {}", limits.limits.maxStorageTexturesPerShaderStage);
            LOG_INFO("maxUniformBuffersPerShaderStage: {}", limits.limits.maxUniformBuffersPerShaderStage);
            LOG_INFO("maxUniformBufferBindingSize: {}", limits.limits.maxUniformBufferBindingSize);
            LOG_INFO("maxStorageBufferBindingSize: {}", limits.limits.maxStorageBufferBindingSize);
            LOG_INFO("minUniformBufferOffsetAlignment: {}", limits.limits.minUniformBufferOffsetAlignment);
            LOG_INFO("minStorageBufferOffsetAlignment: {}", limits.limits.minStorageBufferOffsetAlignment);
            LOG_INFO("maxVertexBuffers: {}", limits.limits.maxVertexBuffers);
            LOG_INFO("maxBufferSize: {}", limits.limits.maxBufferSize);
            LOG_INFO("maxVertexAttributes: {}", limits.limits.maxVertexAttributes);
            LOG_INFO("maxVertexBufferArrayStride: {}", limits.limits.maxVertexBufferArrayStride);
            LOG_INFO("maxVertexBufferArrayStride: {}", limits.limits.maxVertexBufferArrayStride);
            LOG_INFO("maxInterStageShaderComponents: {}", limits.limits.maxInterStageShaderComponents);
            LOG_INFO("maxInterStageShaderVariables: {}", limits.limits.maxInterStageShaderVariables);
            LOG_INFO("maxColorAttachments: {}", limits.limits.maxColorAttachments);
            LOG_INFO("maxColorAttachmentBytesPerSample: {}", limits.limits.maxColorAttachmentBytesPerSample);
            LOG_INFO("maxComputeWorkgroupStorageSize: {}", limits.limits.maxComputeWorkgroupStorageSize);
            LOG_INFO("maxComputeInvocationsPerWorkgroup: {}", limits.limits.maxComputeInvocationsPerWorkgroup);
            LOG_INFO("maxComputeWorkgroupSizeX: {}", limits.limits.maxComputeWorkgroupSizeX);
            LOG_INFO("maxComputeWorkgroupSizeY: {}", limits.limits.maxComputeWorkgroupSizeY);
            LOG_INFO("maxComputeWorkgroupSizeZ: {}", limits.limits.maxComputeWorkgroupSizeZ);
            LOG_INFO("maxComputeWorkgroupsPerDimension: {}", limits.limits.maxComputeWorkgroupsPerDimension);
        }
        LOG_INFO("\n");
    }

    static void wgpu_compilation_info_callback(WGPUCompilationInfoRequestStatus status,
            WGPUCompilationInfo const* compilationInfo,
            void* userdata)
    {
        if (status == WGPUCompilationInfoRequestStatus_Error) 
        {
            for (uint32_t m = 0; m < compilationInfo->messageCount; ++m) 
            {
                WGPUCompilationMessage message = compilationInfo->messages[m];
                LOG_ERROR("lineNum: %u, linePos: %u, Error: %s", message.lineNum,
                    message.linePos, message.message);
            }
        }
    }

    Ref<Shader> Renderer::CreateShader(const char* code, const char* debugName /*= nullptr*/)
    {
        WGPUShaderModuleWGSLDescriptor shader_module_wgsl_desc = 
        {
            .chain = 
            {
                .sType = WGPUSType_ShaderModuleWGSLDescriptor,
            },
            .code = code,
        };

        WGPUShaderModuleDescriptor shader_module_desc = 
        {
            .nextInChain = &shader_module_wgsl_desc.chain,
            .label = debugName,
        };

        WGPUShaderModule shaderModule = wgpuDeviceCreateShaderModule(m_Contex->device, &shader_module_desc);
        wgpuShaderModuleGetCompilationInfo(shaderModule, wgpu_compilation_info_callback, nullptr);

        auto shader = CreateRef<Shader>();
        shader->m_ShaderModule = shaderModule;
        return shader;
    }

    Ref<RBuffer> Renderer::CreateVertexBuffer(const void* data, size_t size, uint32_t stride)
    {
        WGPUBufferDescriptor desc = {};
        desc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex;
        desc.size = size;
        WGPUBuffer buffer = wgpuDeviceCreateBuffer(m_Contex->device, &desc);
        wgpuQueueWriteBuffer(m_Contex->queue, buffer, 0, data, size);
        Ref<RBuffer> bufferObj = CreateRef<RBuffer>();
        bufferObj->m_Handle = buffer;
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
        WGPUBuffer buffer = wgpuDeviceCreateBuffer(m_Contex->device, &desc);
        wgpuQueueWriteBuffer(m_Contex->queue, buffer, 0, data, size);
        Ref<RBuffer> bufferObj = CreateRef<RBuffer>();
        bufferObj->m_Handle = buffer;
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
        WGPUBindGroupLayout bindGroupLayout = wgpuDeviceCreateBindGroupLayout(m_Contex->device, &bglDesc);

        // pipeline layout (used by the render pipeline, released after its creation)
        WGPUPipelineLayoutDescriptor layoutDesc = {};
        layoutDesc.bindGroupLayoutCount = 1;
        layoutDesc.bindGroupLayouts = &bindGroupLayout;
        WGPUPipelineLayout pipelineLayout = wgpuDeviceCreatePipelineLayout(m_Contex->device, &layoutDesc);

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
        //colorTarget.format = webgpu::getSwapChainFormat(device);
        colorTarget.blend = &blend;
        colorTarget.writeMask = pipeDesc->WriteMask;

        WGPUFragmentState fragment = {};
        fragment.module = pipeDesc->FS->m_ShaderModule;
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

        desc.vertex.module = pipeDesc->VS->m_ShaderModule;
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

        WGPURenderPipeline pipeline = wgpuDeviceCreateRenderPipeline(m_Contex->device, &desc);
        Ref<RPipeline> pipelineObj = CreateRef<RPipeline>();
        pipelineObj->m_Pipeline = pipeline;

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
        WGPUBindGroupLayout bindGroupLayout = wgpuDeviceCreateBindGroupLayout(m_Contex->device, &bglDesc);

        // pipeline layout (used by the render pipeline, released after its creation)
        WGPUPipelineLayoutDescriptor layoutDesc = {};
        layoutDesc.bindGroupLayoutCount = 1;
        layoutDesc.bindGroupLayouts = &bindGroupLayout;

        WGPUBufferDescriptor desc = {};
        desc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform;
        desc.size = size;
        WGPUBuffer buffer = wgpuDeviceCreateBuffer(m_Contex->device, &desc);
        wgpuQueueWriteBuffer(m_Contex->queue, buffer, 0, data, size);

        WGPUBindGroupEntry bgEntry = {};
        bgEntry.binding = 0;
        bgEntry.buffer = buffer;
        bgEntry.offset = 0;
        bgEntry.size = size;

        WGPUBindGroupDescriptor bgDesc = {};
        bgDesc.layout = bindGroupLayout;
        bgDesc.entryCount = 1;
        bgDesc.entries = &bgEntry;

        auto bindGroup = wgpuDeviceCreateBindGroup(m_Contex->device, &bgDesc);

        Ref<UniformBuffer> bindGroupObj = CreateRef<UniformBuffer>();
        bindGroupObj->m_BindGroup = bindGroup;
        bindGroupObj->m_Buffer = buffer;

        wgpuBindGroupLayoutRelease(bindGroupLayout);

        return bindGroupObj;
    }

    void Renderer::WriteUniformBuffer(Ref<UniformBuffer> buffer, size_t offset, const void* data, size_t size)
    {
        wgpuQueueWriteBuffer(m_Contex->queue, buffer->m_Buffer, offset, data, size);
    }

    Ref<RenderPass> Renderer::CreateRenderPass(const RenderPassDesc* desc)
    {
        return RenderPass::Construct(m_Contex, desc->lable, desc->width, desc->height, desc->color, desc->depthStencil, desc->clearColor, desc->clearDepth, desc->withDepth);
    }

    void Renderer::BeginDraw()
    {
        // get current texture view
        m_Contex->swapChain.frameBuffer = wgpuSwapChainGetCurrentTextureView(m_Contex->swapChain.instance);
        RUSH_ASSERT(m_Contex->swapChain.frameBuffer);

        // Create command encoder
        m_Contex->cmdEncoder = wgpuDeviceCreateCommandEncoder(m_Contex->device, nullptr);
    }

    void Renderer::DrawOfflinePass(Ref<RenderPass> renderPass, Ref<RenderContent> content)
    {
        // Create render pass encoder for encoding drawing commands
        m_Contex->rPassEncoder = wgpuCommandEncoderBeginRenderPass(
            m_Contex->cmdEncoder,
            &renderPass->m_Contex->frame_buffer.render_pass_desc.render_pass_descriptor);

        // Set viewport
        wgpuRenderPassEncoderSetViewport(m_Contex->rPassEncoder, 0.0f, 0.0f,
            (float)renderPass->m_Contex->render_pass.width,
            (float)renderPass->m_Contex->render_pass.height, 0.0f, 1.0f);

        // Set scissor rectangle
        wgpuRenderPassEncoderSetScissorRect(m_Contex->rPassEncoder, 0u, 0u,
            renderPass->m_Contex->render_pass.width,
            renderPass->m_Contex->render_pass.height);

//         // 3D Scene
//         wgpuRenderPassEncoderSetPipeline(m_Contex->rPassEncoder,
//             pipelines.glow_pass);
//         wgpuRenderPassEncoderSetBindGroup(m_Contex->rPassEncoder, 0,
//             bind_groups.scene, 0, 0);
//         wgpu_gltf_model_draw(models.ufo_glow,
//             (wgpu_gltf_model_render_options_t) {
//             0
//         });

        // End render pass
        wgpuRenderPassEncoderEnd(m_Contex->rPassEncoder);
        WGPU_RELEASE_RESOURCE(RenderPassEncoder, m_Contex->rPassEncoder)
    }

    void Renderer::DrawFinalPass(Ref<RenderContent> content)
    {
        // Set target frame buffer
        m_Contex->render_pass.color_attachments[0].view = m_Contex->swapChain.frameBuffer;

        // Create render pass encoder for encoding drawing commands
        m_Contex->rPassEncoder = wgpuCommandEncoderBeginRenderPass(
            m_Contex->cmdEncoder, &m_Contex->render_pass.descriptor);

        // Set viewport
        wgpuRenderPassEncoderSetViewport(
            m_Contex->rPassEncoder, 0.0f, 0.0f, (float)m_Width, (float)m_Height, 0.0f, 1.0f);

        // Set scissor rectangle
        wgpuRenderPassEncoderSetScissorRect(
            m_Contex->rPassEncoder, 0u, 0u, m_Width, m_Height);

        // todo render 3D Scene

        // End render pass
        wgpuRenderPassEncoderEnd(m_Contex->rPassEncoder);
        WGPU_RELEASE_RESOURCE(RenderPassEncoder, m_Contex->rPassEncoder)
    }

    void Renderer::EndDraw()
    {
        // draw screen pass


        // Command buffer to be submitted to the queue
        m_Contex->submitInfo.cmdBufCount = 1;
        m_Contex->submitInfo.commandBuffers[0] = wgpuCommandEncoderFinish(m_Contex->cmdEncoder, nullptr);
        WGPU_RELEASE_RESOURCE(CommandEncoder, m_Contex->cmdEncoder);

        RUSH_ASSERT(m_Contex->submitInfo.commandBuffers != NULL)

        wgpuQueueSubmit(m_Contex->queue, m_Contex->submitInfo.cmdBufCount, m_Contex->submitInfo.commandBuffers);

        for (uint32_t i = 0; i < m_Contex->submitInfo.cmdBufCount; ++i)
        {
            WGPU_RELEASE_RESOURCE(CommandBuffer, m_Contex->submitInfo.commandBuffers[i])
        }

        wgpuSwapChainPresent(m_Contex->swapChain.instance);
        WGPU_RELEASE_RESOURCE(TextureView, m_Contex->swapChain.frameBuffer);
    }


}