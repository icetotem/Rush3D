#ifndef RMesh_h__
#define RMesh_h__

#include "core/Core.h"
#include "Asset.h"
#include "RBuffer.h"

namespace rush
{

    /// <summary>
    /// 
    /// </summary>
    class RMesh : public Asset
    {
    public:
        virtual bool Load(const StringView& path);

        struct Primitive
        {
            List<Ref<RVertexBuffer>> vertexBuffers;
            Ref<RIndexBuffer> indexBuffer;
        };

        List<Primitive> primitives;
    };

}

#endif // RMesh_h__
