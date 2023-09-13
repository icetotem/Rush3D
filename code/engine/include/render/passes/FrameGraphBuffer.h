#ifndef FrameGraphBuffer_h__
#define FrameGraphBuffer_h__

#include "render/RBuffer.h"

namespace rush
{

    class FrameGraphBuffer 
    {
    public:
        struct Desc 
        {
            uint64_t size;
        };

        void create(const Desc&, void* allocator);
        void destroy(const Desc&, void* allocator);

        static std::string toString(const Desc&);

        RStorageBuffer* buffer = nullptr;
    };

}

#endif // FrameGraphBuffer_h__
