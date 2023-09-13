#include "stdafx.h"
#include "render/passes/TransientResources.h"
#include "render/RenderContext.h"
#include "tracy/Tracy.hpp"
#include "core/Common.h"

namespace std {

    template <> struct hash<rush::FrameGraphTexture::Desc> {
        std::size_t operator()(const rush::FrameGraphTexture::Desc& desc) const noexcept {
            std::size_t h{0};
            rush::hashCombine(h, desc.extent.x, desc.extent.y, desc.depth,
                desc.numMipLevels, desc.layers, desc.format, desc.shadowSampler,
                desc.wrapMode, desc.filter);
            return h;
        }
    };

    template <> struct hash<rush::FrameGraphBuffer::Desc> {
        std::size_t operator()(const rush::FrameGraphBuffer::Desc& desc) const noexcept {
            std::size_t h{0};
            rush::hashCombine(h, desc.size);
            return h;
        }
    };

} // namespace std


//
// TransientResources class:
//
namespace rush
{

    TransientResources::~TransientResources() 
    {
        m_textures.clear();
        m_buffers.clear();
//         for (auto& texture : m_textures)
//             m_renderContext.destroy(*texture);
//         for (auto& buffer : m_buffers)
//             m_renderContext.destroy(*buffer);
    }

    void TransientResources::update(float dt) 
    {
        ZoneScoped;

        const auto deleter = [&](auto& object) { object.Destroy(); };
        heartbeat(m_textures, m_texturePools, dt, deleter);
        heartbeat(m_buffers, m_bufferPools, dt, deleter);
    }

    RTexture* TransientResources::acquireTexture(const FrameGraphTexture::Desc& desc) 
    {
        const auto h = std::hash<FrameGraphTexture::Desc>{}(desc);
        auto& pool = m_texturePools[h];
        if (pool.empty()) {
            if (desc.depth > 0) {
                m_textures.push_back(CreateUnique<RTexture>(desc.extent.x, desc.extent.y, desc.format, 1, desc.depth, TextureDimension::e3D));
            }
            else {
                m_textures.push_back(CreateUnique<RTexture>(desc.extent.x, desc.extent.y, desc.format, desc.numMipLevels, desc.layers, TextureDimension::e2D));
            }

//             glm::vec4 borderColor{0.0f};
//             auto addressMode = AddressMode::ClampToEdge;
//             switch (desc.wrapMode) {
//             case WrapMode::ClampToEdge:
//                 addressMode = AddressMode::ClampToEdge;
//                 break;
//             case WrapMode::ClampToOpaqueBlack:
//                 addressMode = AddressMode::ClampToEdge;
//                 borderColor = glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f };
//                 break;
//             case WrapMode::ClampToOpaqueWhite:
//                 addressMode = AddressMode::ClampToBorder;
//                 borderColor = glm::vec4{ 1.0f };
//                 break;
//             }
//             SamplerInfo samplerInfo{
//               .minFilter = desc.filter,
//               .mipmapMode =
//                 desc.numMipLevels > 1 ? MipmapMode::Nearest : MipmapMode::None,
//               .magFilter = desc.filter,
//               .addressModeS = addressMode,
//               .addressModeT = addressMode,
//               .addressModeR = addressMode,
//               .borderColor = borderColor,
//             };
//             if (desc.shadowSampler) samplerInfo.compareOp = CompareOp::LessOrEqual;
//             m_renderContext.setupSampler(texture, samplerInfo);

            auto* ptr = m_textures.back().get();
            LOG_INFO("Created Transient texture: {}", fmt::ptr(ptr));
            return ptr;
        }
        else {
            auto* texture = pool.back().resource;
            pool.pop_back();
            return texture;
        }
    }
    void TransientResources::releaseTexture(const FrameGraphTexture::Desc& desc, RTexture* texture) 
    {
        const auto h = std::hash<FrameGraphTexture::Desc>{}(desc);
        m_texturePools[h].push_back({ texture, 0.0f });
    }

    RStorageBuffer* TransientResources::acquireBuffer(const FrameGraphBuffer::Desc& desc) 
    {
        const auto h = std::hash<FrameGraphBuffer::Desc>{}(desc);
        auto& pool = m_bufferPools[h];
        if (pool.empty()) {
            m_buffers.push_back(CreateUnique<RStorageBuffer>(desc.size));
            auto* ptr = m_buffers.back().get();
            SPDLOG_INFO("Created Transient storage buffer: {}", fmt::ptr(ptr));
            return ptr;
        }
        else {
            auto* buffer = pool.back().resource;
            pool.pop_back();
            return buffer;
        }
    }

    void TransientResources::releaseBuffer(const FrameGraphBuffer::Desc& desc, RStorageBuffer* buffer)
    {
        const auto h = std::hash<FrameGraphBuffer::Desc>{}(desc);
        m_bufferPools[h].push_back({ std::move(buffer), 0.0f });
    }

}