#ifndef Scene_h__
#define Scene_h__

#include "core/Core.h"
#include "components/EcsSystem.h"
#include "components/Camera.h"
#include "components/Frustum.h"
#include "components/Transform.h"
#include "components/Bounding.h"
#include "components/CommonComponents.h"
#include "render/RMaterial.h"

namespace rush
{

    template <class Tag>
    class Scene
    {
    public:
        Scene() = default;
        ~Scene();

        Entity CreateEntity(const StringView& name = "");

        void Update(float deltaTime);

        void LoadFromScn(const StringView& fileName);

        void LoadFromGltf(const StringView& fileName);

        void Unload();

    protected:
    };



    class SceneManager
    {
    public:
        SceneManager() = default;
        ~SceneManager() = default;

        template <class Tag>
        Ref<Scene<Tag>> CreateScene();

        template <class Tag>
        void Update(Ref<Renderer> renderer, Ref<RSurface> surface);

    protected:
        template <class Tag>
        void Cull();

        template <class Tag>
        void Render(Ref<Renderer> renderer, Ref<RSurface> surface);

    };

}

#include "SceneManager.inl"

#endif // Scene_h__
