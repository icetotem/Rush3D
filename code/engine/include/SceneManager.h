#ifndef Scene_h__
#define Scene_h__

#include "components/EcsSystem.h"

namespace rush
{

    class Scene
    {
    public:
        Scene();
        ~Scene();

        void Update(float deltaTime);

        void LoadFromScn(const StringView& fileName);

        void LoadFromGltf(const StringView& fileName);

        void Unload();

    protected:
    };


    class SceneManager
    {
    public:
        SceneManager();
        ~SceneManager();

        void Update();

    protected:
        void Cull();
    };

}

#endif // Scene_h__
