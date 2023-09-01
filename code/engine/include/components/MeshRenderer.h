#ifndef MeshRenderer_h__
#define MeshRenderer_h__

#include "components/EcsSystem.h"
#include "render/RBuffer.h"
#include "render/RMaterial.h"

namespace rush
{

    class MeshRenderer : public Component
    {
    public:
        struct Primitive
        {
            List<Ref<RVertexBuffer>> vertexBuffers;
            Ref<RIndexBuffer> indexBuffer;
            Ref<RMaterialInst> material;
        };

        MeshRenderer(Entity owner);
        ~MeshRenderer();

        int AddPart();
        MeshRenderer::Primitive* GetPart(int slot);

    protected:
        friend class RenderManager;
        DArray<Primitive> m_Primitives;
    };

}

#endif // Geometry_h__


