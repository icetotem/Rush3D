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
        struct Part
        {
            List<Ref<RVertexBuffer>> vertexBuffers;
            Ref<RIndexBuffer> indexBuffer;
            Ref<RMaterialInst> material;
        };

        MeshRenderer(Entity owner);
        ~MeshRenderer();

        int AddPart();
        MeshRenderer::Part* GetPart(int slot);

    protected:
        friend class RenderManager;
        DArray<Part> m_Parts;
    };

}

#endif // Geometry_h__


