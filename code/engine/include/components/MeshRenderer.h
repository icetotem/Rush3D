#ifndef MeshRenderer_h__
#define MeshRenderer_h__

#include "components/EcsSystem.h"
#include "render/RBuffer.h"
#include "render/RMaterial.h"
#include "render/RGeometry.h"
#include "render/RenderQueue.h"

namespace rush
{

    class MeshRenderer : public Component
    {
    public:
        struct Primitive
        {
            Ref<RGeometry> geometry;
            Ref<RMaterial> material;
        };

        MeshRenderer(Entity owner);
        ~MeshRenderer();

        void AddMesh(const StringView& meshPath);

        int GetPartCount() const { return m_Primitives.size(); }

        void SetMaterial(int part, const StringView& material);

        void SubmitRenderQueue(Ref<RenderQueue> renderQueue);

    protected:

    protected:
        friend class SceneManager;
        DArray<Primitive> m_Primitives;
    };

}

#endif // Geometry_h__


