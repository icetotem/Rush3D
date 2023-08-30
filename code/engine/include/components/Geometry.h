#ifndef Geometry_h__
#define Geometry_h__

#include "components/EcsSystem.h"
#include "render/RBuffer.h"

namespace rush
{

    class Geometry : public Component
    {
    public:
        Geometry(Entity owner);
        ~Geometry();

    protected:
        friend class RenderManager;
        List<Ref<RVertexBuffer>> m_VertexBuffers;
        Ref<RIndexBuffer> m_IndexBuffer;
    };

}

#endif // Geometry_h__


