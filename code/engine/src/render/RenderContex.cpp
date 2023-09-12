#include "stdafx.h"

#include <dawn/dawn_proc.h>

#include "render/RenderContex.h"
#include "render/RShader.h"
#include "dawn/native/DawnNative.h"

namespace rush
{

    HMap<wgpu::FeatureName, std::string> g_Features;
    HMap<WGPUAdapterType, std::string> g_AdapterType;
    HMap<WGPUBackendType, std::string> g_BackendTypeName;

    static class WGPUMapping
    {
    public:
        WGPUMapping()
        {
            g_Features = {
                {wgpu::FeatureName::Undefined, "Undefined"},
                {wgpu::FeatureName::DepthClipControl, "DepthClipControl"},
                {wgpu::FeatureName::Depth32FloatStencil8, "Depth32FloatStencil8"},
                {wgpu::FeatureName::TimestampQuery, "TimestampQuery"},
                {wgpu::FeatureName::PipelineStatisticsQuery, "PipelineStatisticsQuery"},
                {wgpu::FeatureName::TextureCompressionBC, "TextureCompressionBC"},
                {wgpu::FeatureName::TextureCompressionETC2, "TextureCompressionETC2"},
                {wgpu::FeatureName::TextureCompressionASTC, "TextureCompressionASTC"},
                {wgpu::FeatureName::IndirectFirstInstance, "IndirectFirstInstance"},
                {wgpu::FeatureName::ShaderF16, "ShaderF16"},
                {wgpu::FeatureName::RG11B10UfloatRenderable, "RG11B10UfloatRenderable"},
                {wgpu::FeatureName::BGRA8UnormStorage, "BGRA8UnormStorage"},
                {wgpu::FeatureName::Float32Filterable, "Float32Filterable"},
                {wgpu::FeatureName::DawnInternalUsages, "DawnInternalUsages"},
                {wgpu::FeatureName::DawnMultiPlanarFormats, "DawnMultiPlanarFormats"},
                {wgpu::FeatureName::DawnNative, "DawnNative"},
                {wgpu::FeatureName::ChromiumExperimentalDp4a, "ChromiumExperimentalDp4a"},
                {wgpu::FeatureName::TimestampQueryInsidePasses, "TimestampQueryInsidePasses"},
                {wgpu::FeatureName::ImplicitDeviceSynchronization, "ImplicitDeviceSynchronization"},
                {wgpu::FeatureName::SurfaceCapabilities, "SurfaceCapabilities"},
                {wgpu::FeatureName::TransientAttachments, "TransientAttachments"},
                {wgpu::FeatureName::MSAARenderToSingleSampled, "MSAARenderToSingleSampled"},
                {wgpu::FeatureName::DualSourceBlending, "DualSourceBlending"},
                {wgpu::FeatureName::D3D11MultithreadProtected, "D3D11MultithreadProtected"},
                {wgpu::FeatureName::ANGLETextureSharing, "ANGLETextureSharing"},
                {wgpu::FeatureName::ChromiumExperimentalSubgroups, "ChromiumExperimentalSubgroups"},
                {wgpu::FeatureName::ChromiumExperimentalSubgroupUniformControlFlow, "ChromiumExperimentalSubgroupUniformControlFlow"},
                {wgpu::FeatureName::ChromiumExperimentalReadWriteStorageTexture, "ChromiumExperimentalReadWriteStorageTexture"},
                {wgpu::FeatureName::PixelLocalStorageCoherent, "PixelLocalStorageCoherent"},
                {wgpu::FeatureName::PixelLocalStorageNonCoherent, "PixelLocalStorageNonCoherent"},
                {wgpu::FeatureName::Norm16TextureFormats, "Norm16TextureFormats"},
                {wgpu::FeatureName::SharedTextureMemoryVkDedicatedAllocation, "SharedTextureMemoryVkDedicatedAllocation"},
                {wgpu::FeatureName::SharedTextureMemoryAHardwareBuffer, "SharedTextureMemoryAHardwareBuffer"},
                {wgpu::FeatureName::SharedTextureMemoryDmaBuf, "SharedTextureMemoryDmaBuf"},
                {wgpu::FeatureName::SharedTextureMemoryOpaqueFD, "SharedTextureMemoryOpaqueFD"},
                {wgpu::FeatureName::SharedTextureMemoryZirconHandle, "SharedTextureMemoryZirconHandle"},
                {wgpu::FeatureName::SharedTextureMemoryDXGISharedHandle, "SharedTextureMemoryDXGISharedHandle"},
                {wgpu::FeatureName::SharedTextureMemoryD3D11Texture2D, "SharedTextureMemoryD3D11Texture2D"},
                {wgpu::FeatureName::SharedTextureMemoryIOSurface, "SharedTextureMemoryIOSurface"},
                {wgpu::FeatureName::SharedTextureMemoryEGLImage, "SharedTextureMemoryEGLImage"},
                {wgpu::FeatureName::SharedFenceVkSemaphoreOpaqueFD, "SharedFenceVkSemaphoreOpaqueFD"},
                {wgpu::FeatureName::SharedFenceVkSemaphoreSyncFD, "SharedFenceVkSemaphoreSyncFD"},
                {wgpu::FeatureName::SharedFenceVkSemaphoreZirconHandle, "SharedFenceVkSemaphoreZirconHandle"},
                {wgpu::FeatureName::SharedFenceDXGISharedHandle, "SharedFenceDXGISharedHandle"},
                {wgpu::FeatureName::SharedFenceMTLSharedEvent, "SharedFenceMTLSharedEvent"}
            };

            g_AdapterType = {
                {WGPUAdapterType_DiscreteGPU, "DiscreteGPU"},
                {WGPUAdapterType_IntegratedGPU, "IntegratedGPU"},
                {WGPUAdapterType_CPU, "CPU"},
                {WGPUAdapterType_Unknown, "Unknown"},
            };

            g_BackendTypeName = {
                {WGPUBackendType_Null, "Null"},
                {WGPUBackendType_WebGPU, "WebGPU"},
                {WGPUBackendType_D3D11, "D3D11"},
                {WGPUBackendType_D3D12, "D3D12"},
                {WGPUBackendType_Metal, "Metal"},
                {WGPUBackendType_Vulkan, "Vulkan"},
                {WGPUBackendType_OpenGL, "OpenGL"},
                {WGPUBackendType_OpenGLES, "OpenGLES"},
            };
        }
    } g_GPUInfo;

    wgpu::Device RenderContex::device;
    wgpu::Queue RenderContex::queue;

    //////////////////////////////////////////////////////////////////////////

    void PrintDeviceError(WGPUErrorType errorType, const char* message, void*) 
    {
        const char* errorTypeName = "";
        switch (errorType) {
        case WGPUErrorType_Validation:
            errorTypeName = "Validation";
            break;
        case WGPUErrorType_OutOfMemory:
            errorTypeName = "Out of memory";
            break;
        case WGPUErrorType_Unknown:
            errorTypeName = "Unknown";
            break;
        case WGPUErrorType_DeviceLost:
            errorTypeName = "Device lost";
            break;
        default:
            RUSH_ASSERT("UNREACHABLE");
            return;
        }
        LOG_ERROR("{} error: {}", errorTypeName, message);
    }

    void DeviceLostCallback(WGPUDeviceLostReason reason, const char* message, void*) 
    {
        LOG_INFO("Device lost:  {}", message);
    }

    void DeviceLogCallback(WGPULoggingType type, const char* message, void*) 
    {
        LOG_ERROR("Device log:  {}", message);
    }

    std::unique_ptr<wgpu::ChainedStruct> SetupWindowAndGetSurfaceDescriptor(void* wndHandle, void* displayHandle) 
    {
#ifdef RUSH_PLATFORM_WINDOWS
        std::unique_ptr<wgpu::SurfaceDescriptorFromWindowsHWND> desc =
            std::make_unique<wgpu::SurfaceDescriptorFromWindowsHWND>();
        desc->hwnd = wndHandle;
        desc->hinstance = GetModuleHandle(nullptr);
        return std::move(desc);
#elif defined(DAWN_ENABLE_BACKEND_METAL)
        return SetupWindowAndGetSurfaceDescriptorCocoa(window);
#elif defined(DAWN_USE_WAYLAND) || defined(DAWN_USE_X11)
#if defined(GLFW_PLATFORM_WAYLAND) && defined(DAWN_USE_WAYLAND)
        if (glfwGetPlatform() == GLFW_PLATFORM_WAYLAND) {
            std::unique_ptr<wgpu::SurfaceDescriptorFromWaylandSurface> desc =
                std::make_unique<wgpu::SurfaceDescriptorFromWaylandSurface>();
            desc->display = displayHandle;
            desc->surface = wndHandle;
            return std::move(desc);
        }
        else  // NOLINT(readability/braces)
#endif
#if defined(DAWN_USE_X11)
        {
            std::unique_ptr<wgpu::SurfaceDescriptorFromXlibWindow> desc =
                std::make_unique<wgpu::SurfaceDescriptorFromXlibWindow>();
            desc->display = displayHandle;
            desc->window = wndHandle;
            return std::move(desc);
        }
#else
        {
            return nullptr;
        }
#endif
#else
        return nullptr;
#endif
    }

    //////////////////////////////////////////////////////////////////////////
    static std::unique_ptr<dawn::native::Instance> g_DawnInstance;
    static dawn::native::Adapter g_Adapter;

    void RenderContex::Init(BackendType backend)
    {
        if (g_DawnInstance != nullptr)
            return;

        g_DawnInstance = std::make_unique<dawn::native::Instance>();

        wgpu::RequestAdapterOptions options = {};
        options.backendType = backend;
        auto adapters = g_DawnInstance->EnumerateAdapters(&options);
        LOG_INFO("Found {} adapters:", adapters.size());
        int index = 1;
        bool found = false;
        StringView adapterName, backendName;
        std::vector<WGPUAdapterType> typePriority = std::vector<WGPUAdapterType>
        {
            WGPUAdapterType_DiscreteGPU,
            WGPUAdapterType_IntegratedGPU,
            WGPUAdapterType_CPU,
        };

        for (const auto& adapter : adapters)
        {
            WGPUAdapterProperties properties = {};
            adapter.GetProperties(&properties);

            LOG_INFO("{}.{}", index++, properties.name);
            LOG_INFO(" -Vendor: {}", properties.vendorName);
            LOG_INFO(" -Architecture: {}", properties.architecture);
            LOG_INFO(" -Driver: {}", properties.driverDescription);
            LOG_INFO(" -Adapter: {}", g_AdapterType[properties.adapterType]);
            LOG_INFO(" -Backend: {}", g_BackendTypeName[properties.backendType]);
            backendName = g_BackendTypeName[properties.backendType];
            if (!found)
            {
                for (auto reqType : typePriority)
                {
                    if (properties.adapterType == reqType)
                    {
                        found = true;
                        g_Adapter = adapter;
                        adapterName = properties.name;
                        break;
                    }
                }
            }
        }

        if (!found)
        {
            LOG_CRITICAL("Cannot find valid adapter");
        }
        else
        {
            LOG_INFO("Select Adapter {} {}", adapterName, backendName);
        }

        // get dawn procs
        auto procs = dawn::native::GetProcs();
        dawnProcSetProcs(&procs);

        // create device
        // request features when creating device
        WGPUDeviceDescriptor deviceDesc = {};
        WGPUFeatureName required_features[2] = 
        {
            // for texture compression
#if defined(RUSH_PLATFORM_WINDOWS)
            WGPUFeatureName_TextureCompressionBC,
#elif defined(RUSH_PLATFORM_ANDROID)
            WGPUFeatureName_TextureCompressionETC2,
#elif defined(RUSH_PLATFORM_MAC) || defined(RUSH_PLATFORM_IOS)
            WGPUFeatureName_TextureCompressionASTC,
#endif
            WGPUFeatureName_BGRA8UnormStorage,
        };
        deviceDesc.requiredFeatureCount = (uint32_t)ARRAY_SIZE(required_features);
        deviceDesc.requiredFeatures = required_features;
        auto device = g_Adapter.CreateDevice(&deviceDesc);
        RenderContex::device = wgpu::Device::Acquire(device);

        // set device callbacks
        procs.deviceSetUncapturedErrorCallback(device, PrintDeviceError, nullptr);
        procs.deviceSetDeviceLostCallback(device, DeviceLostCallback, nullptr);
        procs.deviceSetLoggingCallback(device, DeviceLogCallback, nullptr);

        // setup command queue
        RenderContex::queue = RenderContex::device.GetQueue();
    }

    void RenderContex::Shutdown()
    {
        RenderContex::queue = {};
        RenderContex::device = {};
    }

    RenderContex::RenderContex(Ref<Window> window, const RendererDesc& rendererDesc)
        : m_Window(window)
    {
        m_Width = m_Window->GetWidth();
        m_Height = m_Window->GetHeight();
        m_ClearColor = rendererDesc.clearColor;
        CreateSurface(rendererDesc);
        GatherCaps();
    }

    wgpu::TextureView CreateDefaultDepthStencilView(const wgpu::Device& device, uint32_t width, uint32_t height)
    {
        wgpu::TextureDescriptor descriptor;
        descriptor.dimension = wgpu::TextureDimension::e2D;
        descriptor.size.width = width;
        descriptor.size.height = height;
        descriptor.size.depthOrArrayLayers = 1;
        descriptor.sampleCount = 1;
        descriptor.format = wgpu::TextureFormat::Depth24PlusStencil8;
        descriptor.mipLevelCount = 1;
        descriptor.usage = wgpu::TextureUsage::RenderAttachment;
        auto depthStencilTexture = device.CreateTexture(&descriptor);
        return depthStencilTexture.CreateView();
    }

    void RenderContex::CreateSurface(const RendererDesc& rendererDesc)
    {
        // get dawn procs
        auto procs = dawn::native::GetProcs();
        dawnProcSetProcs(&procs);

        // create surface
        auto surfaceChainedDesc = SetupWindowAndGetSurfaceDescriptor(m_Window->GetNativeHandle(), m_Window->GetDisplay());
        WGPUSurfaceDescriptor surfaceDesc;
        surfaceDesc.nextInChain = reinterpret_cast<WGPUChainedStruct*>(surfaceChainedDesc.get());
        auto surface = procs.instanceCreateSurface(g_DawnInstance->Get(), &surfaceDesc);

        // create swap chain
        wgpu::SwapChainDescriptor scDesc;
        scDesc.usage = wgpu::TextureUsage::RenderAttachment;
        scDesc.format = wgpu::TextureFormat::BGRA8Unorm;
        scDesc.width = m_Width;
        scDesc.height = m_Height;
        scDesc.presentMode = rendererDesc.vsync ? wgpu::PresentMode::Fifo : wgpu::PresentMode::Mailbox;
        swapChain = RenderContex::device.CreateSwapChain(surface, &scDesc);
        depthStencilView = CreateDefaultDepthStencilView(RenderContex::device, m_Width, m_Height);
    }

    void RenderContex::GatherCaps()
    {
        // limits
        LOG_INFO("---------------Limits-----------------");
        WGPUSupportedLimits limits = {};
        if (g_Adapter.GetLimits(&limits))
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

        // features
        LOG_INFO("---------------Adapter Features-----------------");
        auto features = g_Adapter.GetSupportedFeatures();
        for (auto feature : features)
        {
            LOG_INFO("support: {}", feature);
        }
    }

    Ref<RScreenQuad> RenderContex::CreateScreenQuad(Ref<RShader> fs, Ref<RBindGroup> bindGroup)
    {
        Ref<RScreenQuad> sQuad = CreateRef<RScreenQuad>();
        if (m_QuadVS == nullptr)
        {
            const char screen_quad_vs[] = R"(
	            struct VertexIn {
		            @location(0) aPos : vec2<f32>
	            }
	            struct VertexOut {
		            @location(0) vUV  : vec2<f32>,
		            @builtin(position) Position : vec4<f32>
	            }
	            @vertex
	            fn main(input : VertexIn) -> VertexOut {
		            var output : VertexOut;
		            output.Position = vec4<f32>(input.aPos, 1.0, 1.0);
                    var uv = (input.aPos + 1.0) * 0.5;
		            output.vUV = vec2<f32>(uv.x, 1.0 - uv.y);
		            return output;
	            }
            )";
            m_QuadVS = CreateRef<RShader>(ShaderStage::Vertex, screen_quad_vs, "screen_quad_vs");
        }

        if (m_QuadVB == nullptr) 
        {
            float const verts[] =
            {
                -1.f,  1.f, // TL
                -1.f, -1.f, // BL
                 1.f,  1.f, // TR
                 1.f, -1.f, // BR
            };

            m_QuadVB = CreateRef<RVertexBuffer>(sizeof(float) * 2, sizeof(verts), verts, "screen_quad_vb");
        }

        PipelineDesc pipeDesc = {};
        pipeDesc.depthWrite = false;
        pipeDesc.depthTest = false;
        pipeDesc.colorFormat = TextureFormat::BGRA8Unorm;

        VertexAttribute vertAttrs[2];
        vertAttrs[0].format = VertexFormat::Float32x2;
        vertAttrs[0].offset = 0;
        vertAttrs[0].shaderLocation = 0;

        auto& vLayout = pipeDesc.vertexLayouts.emplace_back();
        vLayout.stride = sizeof(float) * 2;
        vLayout.attributes = &vertAttrs[0];
        vLayout.attributeCount = 1;

        pipeDesc.vs = m_QuadVS;
        pipeDesc.fs = fs;
        pipeDesc.writeMask = ColorWriteMask::All;

        pipeDesc.bindLayout = bindGroup->GetBindLayout();
        pipeDesc.primitiveType = PrimitiveTopology::TriangleStrip;
        pipeDesc.frontFace = FrontFace::CCW;
        pipeDesc.cullModel = CullMode::Back;

        sQuad->pipeline = CreateRef<RPipeline>(pipeDesc, "screen_quad_pipeline");
        sQuad->bindGroup = bindGroup;
        return sQuad;
    }

    void RenderContex::BeginDraw(const Vector4& viewport)
    {
        m_Viewport = viewport;
        dawn::native::InstanceProcessEvents(g_DawnInstance->Get());
        encoder = RenderContex::device.CreateCommandEncoder();
    }

    void RenderContex::DrawOffScreenPass(Ref<RPass> renderPass, Ref<RenderableHub> content)
    {
        wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&renderPass->m_RenderPassDesc);
        pass.SetScissorRect(m_Viewport.x * m_Width, m_Viewport.y * m_Height, (m_Viewport.z - m_Viewport.x) * m_Width, (m_Viewport.w - m_Viewport.y) * m_Height);
        for (const auto batch : content->m_Batches)
        {
            pass.SetPipeline(batch->pipeline->GetPipeline());
            pass.SetBindGroup(0, batch->bindGroup->GetBindGroup());
            int vbIdx = 0;
            for (auto vb : batch->vertexBuffers)
            {
                pass.SetVertexBuffer(vbIdx, vb->m_Buffer);
                ++vbIdx;
            }
            pass.SetIndexBuffer(batch->indexBuffer->m_Buffer, batch->indexBuffer->GetType());
            pass.DrawIndexed(batch->indexBuffer->GetIndexCount(), batch->instanceCount, 0, 0);
        }
        pass.End();
    }

    void RenderContex::DrawOffScreenQuad(Ref<RPass> renderPass, Ref<RScreenQuad> sQuad)
    {
        wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&renderPass->m_RenderPassDesc);
        pass.SetPipeline(sQuad->pipeline->GetPipeline());
        pass.SetBindGroup(0, sQuad->bindGroup->GetBindGroup());
        pass.SetVertexBuffer(0, m_QuadVB->m_Buffer);
        pass.Draw(4);
        pass.End();
    }

    void RenderContex::DrawFinalScreenQuad(Ref<RScreenQuad> sQuad)
    {
        wgpu::TextureView backbufferView = swapChain.GetCurrentTextureView();
        wgpu::RenderPassDescriptor renderPassDesc = {};
        wgpu::RenderPassColorAttachment attachment = {};
        attachment.view = backbufferView;
        attachment.resolveTarget = nullptr;
        attachment.loadOp = wgpu::LoadOp::Clear;
        attachment.storeOp = wgpu::StoreOp::Store;
        attachment.clearValue = { m_ClearColor.r, m_ClearColor.g, m_ClearColor.b, m_ClearColor.a };
        renderPassDesc.colorAttachmentCount = 1;
        renderPassDesc.colorAttachments = &attachment;
        renderPassDesc.depthStencilAttachment = nullptr;

        wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&renderPassDesc);
        pass.SetPipeline(sQuad->pipeline->GetPipeline());
        pass.SetBindGroup(0, sQuad->bindGroup->GetBindGroup());
        pass.SetVertexBuffer(0, m_QuadVB->m_Buffer);
        pass.Draw(4);
        pass.End();
    }

    void RenderContex::DrawFinalPass(Ref<RenderableHub> content)
    {
        wgpu::TextureView backbufferView = swapChain.GetCurrentTextureView();
        wgpu::RenderPassDescriptor renderPassDesc = {};
        wgpu::RenderPassColorAttachment attachment = {};
        attachment.view = backbufferView;
        attachment.resolveTarget = nullptr;
        attachment.loadOp = wgpu::LoadOp::Clear;
        attachment.storeOp = wgpu::StoreOp::Store;
        attachment.clearValue = { m_ClearColor.r, m_ClearColor.g, m_ClearColor.b, m_ClearColor.a };
        renderPassDesc.colorAttachmentCount = 1;
        renderPassDesc.colorAttachments = &attachment;

        wgpu::RenderPassDepthStencilAttachment depthStencilDesc;
        depthStencilDesc.view = depthStencilView;
        depthStencilDesc.depthReadOnly = false;
        depthStencilDesc.stencilReadOnly = false;
        depthStencilDesc.depthClearValue = 1.0f;
        depthStencilDesc.stencilClearValue = 0;
        depthStencilDesc.depthLoadOp = wgpu::LoadOp::Clear;
        depthStencilDesc.depthStoreOp = wgpu::StoreOp::Store;
        depthStencilDesc.stencilLoadOp = wgpu::LoadOp::Clear;
        depthStencilDesc.stencilStoreOp = wgpu::StoreOp::Store;
        renderPassDesc.depthStencilAttachment = &depthStencilDesc; 

        wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&renderPassDesc);
        for (const auto batch : content->m_Batches)
        {
            pass.SetPipeline(batch->pipeline->GetPipeline());
            pass.SetBindGroup(0, batch->bindGroup->GetBindGroup());
            int vbIdx = 0;
            for (auto vb : batch->vertexBuffers)
            {
                pass.SetVertexBuffer(vbIdx, vb->m_Buffer);
                ++vbIdx;
            }
            pass.SetIndexBuffer(batch->indexBuffer->m_Buffer, batch->indexBuffer->GetType());
            pass.DrawIndexed(batch->indexBuffer->GetIndexCount(), batch->instanceCount, 0, 0);
        }
        pass.End();
    }

    void RenderContex::EndDraw()
    {
        wgpu::CommandBuffer commands = encoder.Finish();
        RenderContex::queue.Submit(1, &commands);
    }

    void RenderContex::Present()
    {
        swapChain.Present();
    }

}