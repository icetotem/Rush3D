#ifndef FrameGraphTexture_h__
#define FrameGraphTexture_h__

#include "core/Core.h"
#include "render/RDefines.h"
#include "render/RTexture.h"

namespace rush
{

    class FrameGraphTexture
    {
    public:
        struct Desc 
        {
            IVector2 extent;
            uint32_t depth = 0;
            uint32_t numMipLevels = 1;
            uint32_t layers = 0;
            TextureFormat format{ TextureFormat::Undefined };

            bool shadowSampler{ false };
            AddressMode wrapMode{ AddressMode::ClampToEdge };
            FilterMode filter{ FilterMode::Linear };
        };

        void create(const Desc&, void* allocator);
        void destroy(const Desc&, void* allocator);

        static std::string toString(const Desc&);

        RTexture* texture = nullptr;
    };

}

#endif // FrameGraphTexture_h__
