#ifndef RGeometry_h__
#define RGeometry_h__

#include "core/Core.h"
#include "RBuffer.h"

namespace rush
{
    
    class RGeometry
    {
    public:
        PrimitiveTopology type = PrimitiveTopology::TriangleList;
        List<Ref<RVertexBuffer>> vertexBuffers;
        Ref<RIndexBuffer> indexBuffer;
    };

}

#endif // RGeometry_h__
