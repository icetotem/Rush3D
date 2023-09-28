#ifndef Scene_h__
#define Scene_h__

#include "core/Core.h"
#include "components/EcsSystem.h"
#include "components/Camera.h"
#include "components/Frustum.h"
#include "components/Transform.h"
#include "components/Bounding.h"
#include "components/CommonComponents.h"
#include "components/MeshRenderer.h"
#include "components/Light.h"

namespace rush
{
    
    struct MainSceneTag : public Component
    {
    public:
        MainSceneTag(Entity owner) 
            : Component(owner) 
        {}
    };

    template <class Tag>
    class Scene
    {
    public:
        Scene();
        ~Scene();

        Entity CreateEntity(const StringView& name = "");

        void Update(float deltaTime);

        void LoadFromScn(const StringView& fileName);

        void LoadFromGltf(const StringView& fileName);

        void Unload();

        Entity GetMainCamera() const { return m_MainCamera; }

    protected:
        Entity m_MainCamera;
    };

    template <class Tag>
    rush::Scene<Tag>::Scene()
    {
        m_MainCamera = CreateEntity("MainCamera");
        auto transform = m_MainCamera.Add<Transform>();
        transform->SetPosition(0, 15, 15);
        transform->LookAt(0, 0, 0);
        m_MainCamera.Add<Camera>();
        m_MainCamera.Add<Frustum>();
    }

    class SceneManager
    {
    public:
        SceneManager();
        ~SceneManager() = default;

        template <class Tag>
        Ref<Scene<Tag>> CreateScene();

        template <class Tag>
        void Update(Ref<Renderer> renderer, Ref<RSurface> surface);

        Ref<Scene<MainSceneTag>> GetMainScene() { return m_MainScene; }

    protected:
        template <class Tag>
        void Cull();

        template <class Tag>
        void Render(Ref<Renderer> renderer, Ref<RSurface> surface);

    private:
        Ref<Scene<MainSceneTag>> m_MainScene;
    };

}

#include "SceneManager.inl"

#endif // Scene_h__
