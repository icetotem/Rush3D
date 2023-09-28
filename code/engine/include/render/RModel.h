#ifndef RModel_h__
#define RModel_h__

#include "core/Core.h"
#include "Asset.h"
#include "RBuffer.h"
#include "RGeometry.h"

namespace rush
{

    /// <summary>
    /// 
    /// </summary>
    class RModel : public Asset
    {
    public:
        struct Primitive
        {
            Ref<RGeometry> geometry;
            String material;
        };
        struct Mesh
        {
            List<Primitive> primitives;
        };

        virtual bool Load(const StringView& path);
        
        const List<RModel::Mesh>& GeMeshes() const { return m_Meshes; }

    private:
        List<Mesh> m_Meshes;
        AABB m_AABB;
    };

}

#endif // RModel_h__
