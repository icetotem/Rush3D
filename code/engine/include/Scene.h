#pragma once
#include "core/Common.h"
#include "core/ecs.h"

namespace rush
{

    class Scene
    {
    public:
        Scene();
        ~Scene();

        EntityID CreateBasicEntity();

        EntityID CreateCamera();

        EntityID CreateLight();

        void DestroyEntity(EntityID entityID);

        void Update(float deltaTime);

        void LoadFromScn(const StringView& fileName);

        void LoadFromGltf(const StringView& fileName);

        void Unload();

    protected:
        void RecycleUpdate();

    private:
        List<EntityID> m_CommonEntities;
        List<EntityID> m_Lights;
        List<EntityID> m_Cameras;
        HSet<EntityID> m_PendingDeleteList;
    };

}