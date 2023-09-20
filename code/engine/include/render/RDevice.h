#ifndef RDevice_h__
#define RDevice_h__

#include "core/Core.h"
#include "render/RDefines.h"

namespace rush
{

    struct RenderCaps
    {
        uint32_t maxTextureDimension1D;
        uint32_t maxTextureDimension2D;
        uint32_t maxTextureDimension3D;
        uint32_t maxTextureArrayLayers;
        uint32_t maxBindGroups;
        uint32_t maxBindGroupsPlusVertexBuffers;
        uint32_t maxBindingsPerBindGroup;
        uint32_t maxDynamicUniformBuffersPerPipelineLayout;
        uint32_t maxDynamicStorageBuffersPerPipelineLayout;
        uint32_t maxSampledTexturesPerShaderStage;
        uint32_t maxSamplersPerShaderStage;
        uint32_t maxStorageBuffersPerShaderStage;
        uint32_t maxStorageTexturesPerShaderStage;
        uint32_t maxUniformBuffersPerShaderStage;
        uint64_t maxUniformBufferBindingSize;
        uint64_t maxStorageBufferBindingSize;
        uint32_t minUniformBufferOffsetAlignment;
        uint32_t minStorageBufferOffsetAlignment;
        uint32_t maxVertexBuffers;
        uint64_t maxBufferSize;
        uint32_t maxVertexAttributes;
        uint32_t maxVertexBufferArrayStride;
        uint32_t maxInterStageShaderComponents;
        uint32_t maxInterStageShaderVariables;
        uint32_t maxColorAttachments;
        uint32_t maxColorAttachmentBytesPerSample;
        uint32_t maxComputeWorkgroupStorageSize;
        uint32_t maxComputeInvocationsPerWorkgroup;
        uint32_t maxComputeWorkgroupSizeX;
        uint32_t maxComputeWorkgroupSizeY;
        uint32_t maxComputeWorkgroupSizeZ;
        uint32_t maxComputeWorkgroupsPerDimension;
    };

    class RDevice : public Singleton<RDevice>
    {
    public:
        RDevice();
        ~RDevice() = default;

        bool Create(BackendType backend);

        dawn::native::Instance& GetDawnInstance() { return m_DawnInstance; }

        wgpu::Device& GetDevice() { return m_Device; }

        wgpu::Queue& GetCmdQueue() { return m_CmdQueue; }

        const RenderCaps& GetCaps() const { return m_Caps; }

    private:
        void GatherCaps();

    private:
        dawn::native::Instance m_DawnInstance;
        wgpu::Device m_Device;
        wgpu::Queue m_CmdQueue;
        dawn::native::Adapter m_Adapter;
        RenderCaps m_Caps;
    };

}

#endif // RDevice_h__
