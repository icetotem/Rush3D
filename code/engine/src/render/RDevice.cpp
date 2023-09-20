#include "stdafx.h"
#include "render/RDevice.h"
#include <dawn/dawn_proc.h>

namespace rush
{
    
    RDevice* RDevice::s_Inst = nullptr;

    RDevice::RDevice()
    {
        s_Inst = this;
    }

    bool RDevice::Create(BackendType backend)
    {
        if (m_Device)
        {
            LOG_ERROR("Already created a render device");
            return false;
        }

        wgpu::RequestAdapterOptions options = {};
        options.backendType = backend;
        auto adapters = m_DawnInstance.EnumerateAdapters(&options); // 枚举所有支持backend(API)的显卡
        LOG_INFO("Found {} adapters:", adapters.size());
        int index = 1;
        bool found = false;
        String adapterName;
        std::vector<WGPUAdapterType> typePriority = std::vector<WGPUAdapterType> // 按此顺序优先选择合适显卡
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
            LOG_INFO(" -Adapter: {}", properties.adapterType);
            LOG_INFO(" -Backend: {}", properties.backendType);
            if (!found)
            {
                for (auto reqType : typePriority)
                {
                    if (properties.adapterType == reqType)
                    {
                        found = true;
                        m_Adapter = adapter;
                        adapterName = properties.name;
                        break;
                    }
                }
            }
        }

        if (found)
        {
            std::ostringstream ss;
            ss << backend;
            LOG_INFO("Select Adapter: {}, Backend: {}", adapterName, ss.str());
        }
        else
        {
            LOG_CRITICAL("Cannot find valid adapter");
            return false;
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
        auto device = m_Adapter.CreateDevice(&deviceDesc);
        m_Device = wgpu::Device::Acquire(device);

        GatherCaps();

        // set device callbacks
        procs.deviceSetUncapturedErrorCallback(device, [](WGPUErrorType errorType, const char* message, void*) {
            LOG_ERROR("Dawn UncapturedError: type = {} message = {}", errorType, message);
        }, nullptr);

        procs.deviceSetDeviceLostCallback(device, [](WGPUDeviceLostReason reason, const char* message, void*) {
            LOG_INFO("Dawn Device Lost: reason = {} message = {}", reason, message);
        }, nullptr);

        procs.deviceSetLoggingCallback(device, [](WGPULoggingType type, const char* message, void*) {
            LOG_INFO("Dawn Logging: type = {} message = {}", type, message);
        }, nullptr);

        // setup command queue
        m_CmdQueue = m_Device.GetQueue();
        return true;
    }

    void RDevice::GatherCaps()
    {
        // limits
        LOG_INFO("---------------Limits-----------------");
        WGPUSupportedLimits limits = {};
        if (m_Adapter.GetLimits(&limits))
        {
            memcpy(&m_Caps, &limits.limits, sizeof(WGPULimits));

            LOG_INFO("maxTextureDimension1D = {}", limits.limits.maxTextureDimension1D);
            LOG_INFO("maxTextureDimension2D = {}", limits.limits.maxTextureDimension2D);
            LOG_INFO("maxTextureDimension3D = {}", limits.limits.maxTextureDimension3D);
            LOG_INFO("maxTextureArrayLayers = {}", limits.limits.maxTextureArrayLayers);
            LOG_INFO("maxBindGroups = {}", limits.limits.maxBindGroups);
            LOG_INFO("maxBindGroupsPlusVertexBuffers = {}", limits.limits.maxBindGroupsPlusVertexBuffers);
            LOG_INFO("maxBindingsPerBindGroup = {}", limits.limits.maxBindingsPerBindGroup);
            LOG_INFO("maxDynamicUniformBuffersPerPipelineLayout = {}", limits.limits.maxDynamicUniformBuffersPerPipelineLayout);
            LOG_INFO("maxDynamicStorageBuffersPerPipelineLayout = {}", limits.limits.maxDynamicStorageBuffersPerPipelineLayout);
            LOG_INFO("maxSampledTexturesPerShaderStage = {}", limits.limits.maxSampledTexturesPerShaderStage);
            LOG_INFO("maxSamplersPerShaderStage = {}", limits.limits.maxSamplersPerShaderStage);
            LOG_INFO("maxStorageBuffersPerShaderStage = {}", limits.limits.maxStorageBuffersPerShaderStage);
            LOG_INFO("maxStorageTexturesPerShaderStage = {}", limits.limits.maxStorageTexturesPerShaderStage);
            LOG_INFO("maxUniformBuffersPerShaderStage = {}", limits.limits.maxUniformBuffersPerShaderStage);
            LOG_INFO("maxUniformBufferBindingSize = {}", limits.limits.maxUniformBufferBindingSize);
            LOG_INFO("maxStorageBufferBindingSize = {}", limits.limits.maxStorageBufferBindingSize);
            LOG_INFO("minUniformBufferOffsetAlignment = {}", limits.limits.minUniformBufferOffsetAlignment);
            LOG_INFO("minStorageBufferOffsetAlignment = {}", limits.limits.minStorageBufferOffsetAlignment);
            LOG_INFO("maxVertexBuffers = {}", limits.limits.maxVertexBuffers);
            LOG_INFO("maxBufferSize = {}", limits.limits.maxBufferSize);
            LOG_INFO("maxVertexAttributes = {}", limits.limits.maxVertexAttributes);
            LOG_INFO("maxVertexBufferArrayStride = {}", limits.limits.maxVertexBufferArrayStride);
            LOG_INFO("maxVertexBufferArrayStride = {}", limits.limits.maxVertexBufferArrayStride);
            LOG_INFO("maxInterStageShaderComponents = {}", limits.limits.maxInterStageShaderComponents);
            LOG_INFO("maxInterStageShaderVariables = {}", limits.limits.maxInterStageShaderVariables);
            LOG_INFO("maxColorAttachments = {}", limits.limits.maxColorAttachments);
            LOG_INFO("maxColorAttachmentBytesPerSample = {}", limits.limits.maxColorAttachmentBytesPerSample);
            LOG_INFO("maxComputeWorkgroupStorageSize = {}", limits.limits.maxComputeWorkgroupStorageSize);
            LOG_INFO("maxComputeInvocationsPerWorkgroup = {}", limits.limits.maxComputeInvocationsPerWorkgroup);
            LOG_INFO("maxComputeWorkgroupSizeX = {}", limits.limits.maxComputeWorkgroupSizeX);
            LOG_INFO("maxComputeWorkgroupSizeY = {}", limits.limits.maxComputeWorkgroupSizeY);
            LOG_INFO("maxComputeWorkgroupSizeZ = {}", limits.limits.maxComputeWorkgroupSizeZ);
            LOG_INFO("maxComputeWorkgroupsPerDimension = {}", limits.limits.maxComputeWorkgroupsPerDimension);
        }

        // features
        LOG_INFO("---------------Adapter Features-----------------");
        auto features = m_Adapter.GetSupportedFeatures();
        for (auto feature : features)
        {
            LOG_INFO("{}", feature);
        }
    }

}