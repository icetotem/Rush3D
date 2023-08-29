#ifndef EcsSystem_h__
#define EcsSystem_h__

#include "core/Common.h"
#include <entt/single_include/entt/entt.hpp>

namespace rush
{

    typedef entt::entity EntityID;

    class Component
    {
    public:
        Component(EntityID entityID);

        virtual ~Component() = default;

        void SetEnable(bool enable);
        bool GetEnalbe() const;

        virtual void OnCreate() {}
        virtual void OnDestroy() {}

    protected:
        EntityID m_EntityID;
    };


    namespace Entity
    {
        EntityID Create();

        EntityID Create(const StringView& name);

        EntityID Find(const StringView& name);

        void Destroy(EntityID entityID);

        void ClearRecycle();
    }

}

#endif // EcsSystem_h__
