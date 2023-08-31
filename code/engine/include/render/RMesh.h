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

        struct SubMesh
        {
            List<Ref<RVertexBuffer>> vertexBuffers;
            Ref<RIndexBuffer> indexBuffer;
        };

        List<SubMesh> subMeshes;
    };

}

#endif // RMesh_h__
