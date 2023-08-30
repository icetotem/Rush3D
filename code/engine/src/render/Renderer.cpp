#include "stdafx.h"

#include <dawn/dawn_proc.h>
#include <dawn/native/DawnNative.h>
#include <dawn/webgpu_cpp.h>

#include "render/Renderer.h"
#include "RContex.h"

namespace rush
{

    extern HMap<wgpu::FeatureName, std::string> g_Features;
    extern HMap<WGPUAdapterType, std::string> g_AdapterType;
    extern HMap<WGPUBackendType, std::string> g_BackendTypeName;
    extern wgpu::FrontFace g_WGPUFrontFace[(int)FrontFace::Count];
    extern wgpu::CullMode g_WGPUCullMode[(int)CullMode::Count];
    extern wgpu::PrimitiveTopology g_WGPUPrimitiveTopology[(int)PrimitiveType::Count];
    extern wgpu::BlendOperation g_WGPUBlendOperation[(int)BlendOperation::Count];
    extern wgpu::BlendFactor g_WGPUBlendFactor[(int)BlendFactor::Count];
    extern wgpu::TextureDimension g_TextureDimension[(int)TextureDimension::Count];
    extern wgpu::CompareFunction g_WGPUCompareFunction[(int)DepthCompareFunction::Count];
    extern wgpu::VertexFormat g_WGPUVertexFormat[(int)VertexFormat::Count];
    extern wgpu::TextureFormat g_WGPUTextureFormat[(int)TextureFormat::Count];
    extern wgpu::BackendType g_WGPUBackendType[(int)RenderBackend::Count];


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

    Renderer::Renderer(Ref<Window> window, const RendererDesc* rendererDesc)
        : m_Window(window)
    {
        if (g_DawnInstance == nullptr)
            g_DawnInstance = std::make_unique<dawn::native::Instance>();

        m_Width = m_Window->GetWidth();
        m_Height = m_Window->GetHeight();
        m_ClearColor = rendererDesc->clearColor;
        m_Contex = CreateRef<RContex>();

        CreateAdapter(rendererDesc);
        InitWGPU(rendererDesc);
        GatherCaps();
    }

    void Renderer::CreateAdapter(const RendererDesc* rendererDesc)
    {
        wgpu::RequestAdapterOptions options = {};
        options.backendType = g_WGPUBackendType[(int)rendererDesc->backend];
        auto adapters = g_DawnInstance->EnumerateAdapters(&options);
        LOG_INFO("Found {} adapters:", adapters.size());
        int index = 1;
        bool found = false;
        StringView adapterName;
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

            if (!found)
            {
                for (auto reqType : typePriority) 
                {
                    if (properties.adapterType == reqType)
                    {
                        found = true;
                        m_Contex->adapter = adapter;
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
            LOG_INFO("Select Adapter {}", adapterName);
        }
    }

    void Renderer::InitWGPU(const RendererDesc* rendererDesc)
    {
        // get dawn procs
        auto procs = dawn::native::GetProcs();
        dawnProcSetProcs(&procs);

        // setup toggles
        std::vector<const char*> enableToggleNames;
        std::vector<const char*> disabledToggleNames;
        for (const std::string& toggle : rendererDesc->enableToggles) 
        {
            enableToggleNames.push_back(toggle.c_str());
        }

        for (const std::string& toggle : rendererDesc->disableToggles) 
        {
            disabledToggleNames.push_back(toggle.c_str());
        }

        WGPUDawnTogglesDescriptor toggles;
        toggles.chain.sType = WGPUSType_DawnTogglesDescriptor;
        toggles.chain.next = nullptr;
        toggles.enabledToggles = enableToggleNames.data();
        toggles.enabledToggleCount = enableToggleNames.size();
        toggles.disabledToggles = disabledToggleNames.data();
        toggles.disabledToggleCount = disabledToggleNames.size();

        // create device
        WGPUDeviceDescriptor deviceDesc = {};
        deviceDesc.nextInChain = reinterpret_cast<WGPUChainedStruct*>(&toggles);
        auto device = m_Contex->adapter.CreateDevice(&deviceDesc);
        m_Contex->device = wgpu::Device::Acquire(device);

        // set device callbacks
        procs.deviceSetUncapturedErrorCallback(device, PrintDeviceError, nullptr);
        procs.deviceSetDeviceLostCallback(device, DeviceLostCallback, nullptr);
        procs.deviceSetLoggingCallback(device, DeviceLogCallback, nullptr);

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
        scDesc.presentMode = rendererDesc->vsync ? wgpu::PresentMode::Fifo : wgpu::PresentMode::Mailbox;
        m_Contex->swapChain = m_Contex->device.CreateSwapChain(surface, &scDesc);

        // get cmd queue
        m_Contex->queue = m_Contex->device.GetQueue();
    }

    void Renderer::GatherCaps()
    {
        // limits
        LOG_INFO("\n---------------Limits-----------------");
        WGPUSupportedLimits limits = {};
        if (m_Contex->adapter.GetLimits(&limits))
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

    Ref<RScreenQuad> Renderer::CreateScreenQuad(Ref<RShader> fs, Ref<RBindGroup> bindGroup)
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
            m_QuadVS = CreateShader(screen_quad_vs, ShaderStage::Vertex, "screen_quad_vs");
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

            m_QuadVB = CreateVertexBuffer(sizeof(float) * 2, sizeof(verts), "screen_quad_vb");
            m_QuadVB->UpdateData(verts, sizeof(verts));
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
        pipeDesc.writeMask = ColorWriteMask::Write_All;

        pipeDesc.bindLayout = bindGroup->GetBindingLayout();
        pipeDesc.primitiveType = PrimitiveType::TriangleStrip;
        pipeDesc.frontFace = FrontFace::CCW;
        pipeDesc.cullModel = CullMode::Back;

        sQuad->pipeline = CreatePipeline(pipeDesc, "screen_quad_pipeline");
        sQuad->bindGroup = bindGroup;
        return sQuad;
    }

    void Renderer::BeginDraw(const Vector4& viewport)
    {
        m_Viewport = viewport;
        dawn::native::InstanceProcessEvents(g_DawnInstance->Get());
        m_Contex->encoder = m_Contex->device.CreateCommandEncoder();
    }

    void Renderer::DrawOffScreenPass(Ref<RPass> renderPass, Ref<RContent> content)
    {
        wgpu::RenderPassEncoder pass = m_Contex->encoder.BeginRenderPass(renderPass->m_RenderPassDesc.get());
        pass.SetScissorRect(m_Viewport.x * m_Width, m_Viewport.y * m_Height, (m_Viewport.z - m_Viewport.x) * m_Width, (m_Viewport.w - m_Viewport.y) * m_Height);
        for (const auto batch : content->m_Batches)
        {
            pass.SetPipeline(*batch->pipeline->m_Pipeline);
            pass.SetBindGroup(0, *batch->uniforms->m_BindGroup);
            int vbIdx = 0;
            for (auto vb : batch->vertexBuffers)
            {
                pass.SetVertexBuffer(vbIdx, *vb->m_Buffer);
                ++vbIdx;
            }
            pass.SetIndexBuffer(*batch->indexBuffer->m_Buffer, batch->indexBuffer->Is32Bits() ? wgpu::IndexFormat::Uint32 : wgpu::IndexFormat::Uint16);
            pass.DrawIndexed(batch->indexBuffer->GetCount(), batch->instanceCount, 0, 0);
        }
        pass.End();
    }

    void Renderer::DrawOffScreenQuad(Ref<RPass> renderPass, Ref<RScreenQuad> sQuad)
    {
        wgpu::RenderPassEncoder pass = m_Contex->encoder.BeginRenderPass(renderPass->m_RenderPassDesc.get());
        pass.SetPipeline(*sQuad->pipeline->m_Pipeline);
        pass.SetBindGroup(0, *sQuad->bindGroup->m_BindGroup);
        pass.SetVertexBuffer(0, *m_QuadVB->m_Buffer);
        pass.Draw(4);
        pass.End();
    }

    void Renderer::DrawFinalScreenQuad(Ref<RScreenQuad> sQuad)
    {
        wgpu::TextureView backbufferView = m_Contex->swapChain.GetCurrentTextureView();
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

        wgpu::RenderPassEncoder pass = m_Contex->encoder.BeginRenderPass(&renderPassDesc);
        pass.SetPipeline(*sQuad->pipeline->m_Pipeline);
        pass.SetBindGroup(0, *sQuad->bindGroup->m_BindGroup);
        pass.SetVertexBuffer(0, *m_QuadVB->m_Buffer);
        pass.Draw(4);
        pass.End();
    }

    void Renderer::DrawFinalPass(Ref<RContent> content)
    {
        wgpu::TextureView backbufferView = m_Contex->swapChain.GetCurrentTextureView();
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

        wgpu::RenderPassEncoder pass = m_Contex->encoder.BeginRenderPass(&renderPassDesc);
        for (const auto batch : content->m_Batches)
        {
            pass.SetPipeline(*batch->pipeline->m_Pipeline);
            pass.SetBindGroup(0, *batch->uniforms->m_BindGroup);
            int vbIdx = 0;
            for (auto vb : batch->vertexBuffers)
            {
                pass.SetVertexBuffer(vbIdx, *vb->m_Buffer);
                ++vbIdx;
            }
            pass.SetIndexBuffer(*batch->indexBuffer->m_Buffer, batch->indexBuffer->Is32Bits() ? wgpu::IndexFormat::Uint32 : wgpu::IndexFormat::Uint16);
            pass.DrawIndexed(batch->indexBuffer->GetCount(), batch->instanceCount, 0, 0);
        }
        pass.End();
    }

    void Renderer::EndDraw()
    {
        wgpu::CommandBuffer commands = m_Contex->encoder.Finish();
        m_Contex->queue.Submit(1, &commands);
        m_Contex->swapChain.Present();
    }


}