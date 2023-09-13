#ifndef TransientResources_h__
#define TransientResources_h__

#include "render/passes/FrameGraphTexture.h"
#include "render/passes/FrameGraphBuffer.h"
#include <memory>
#include <unordered_map>

namespace rush
{

    class RenderContext;

    class TransientResources 
    {
    public:
        TransientResources() = default;
        TransientResources(const TransientResources&) = delete;
        TransientResources(TransientResources&&) noexcept = delete;
        ~TransientResources();

        TransientResources& operator=(const TransientResources&) = delete;
        TransientResources& operator=(TransientResources&&) noexcept = delete;

        void update(float dt);

        [[nodiscard]] RTexture* acquireTexture(const FrameGraphTexture::Desc&);
        void releaseTexture(const FrameGraphTexture::Desc&, RTexture*);

        [[nodiscard]] RStorageBuffer* acquireBuffer(const FrameGraphBuffer::Desc&);
        void releaseBuffer(const FrameGraphBuffer::Desc&, RStorageBuffer*);

    private:
        std::vector<std::unique_ptr<RTexture>> m_textures;
        std::vector<std::unique_ptr<RStorageBuffer>> m_buffers;

        template <typename T> struct ResourceEntry {
            T resource;
            float life;
        };
        template <typename T> using ResourcePool = std::vector<ResourceEntry<T>>;

        std::unordered_map<std::size_t, ResourcePool<RTexture*>> m_texturePools;
        std::unordered_map<std::size_t, ResourcePool<RStorageBuffer*>> m_bufferPools;
    };

    namespace {

        template <typename OBJECT, typename POOL, typename DELETER>
        void heartbeat(OBJECT& objects, POOL& pools, float dt, DELETER&& deleter)
        {
            constexpr auto kMaxIdleTime = 1.0f; // in seconds

            auto poolIt = pools.begin();
            while (poolIt != pools.end()) {
                auto& [_, pool] = *poolIt;
                if (pool.empty()) {
                    poolIt = pools.erase(poolIt);
                }
                else {
                    auto objectIt = pool.begin();
                    while (objectIt != pool.cend()) {
                        auto& [object, idleTime] = *objectIt;
                        idleTime += dt;
                        if (idleTime >= kMaxIdleTime) {
                            deleter(*object);
                            LOG_INFO("Released resource: {}", fmt::ptr(object));
                            objectIt = pool.erase(objectIt);
                        }
                        else {
                            ++objectIt;
                        }
                    }
                    ++poolIt;
                }
            }
            objects.erase(std::remove_if(objects.begin(), objects.end(),
                [](auto& object) { return object->IsValid(); }),
                objects.end());
        }

    } // namespace
}

#endif // TransientResources_h__
