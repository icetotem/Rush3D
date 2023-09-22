#ifndef RMesh_h__
#define RMesh_h__

#include "core/Core.h"
#include "Asset.h"
#include "RBuffer.h"
#include "RGeometry.h"

namespace rush
{

    /// <summary>
    /// 
    /// </summary>
    class RMesh : public Asset
    {
    public:
        struct SubMesh
        {
            Ref<RGeometry> geometry;
            String material;
        };

        virtual bool Load(const StringView& path);
        
        const List<RMesh::SubMesh>& GetSubmeshes() const { return m_Submeshes; }

    private:
        List<SubMesh> m_Submeshes;
    };

}

#endif // RMesh_h__
