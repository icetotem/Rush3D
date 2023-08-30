#ifndef EcsSystem_h__
#define EcsSystem_h__

#include "core/Common.h"
#include <entt/single_include/entt/entt.hpp>

namespace rush
{

    typedef entt::entity EntityID;

    //////////////////////////////////////////////////////////////////////////

    class Entity
    {
    public:
        Entity() { m_EntityID = entt::null; }
        Entity(const Entity& other) { m_EntityID = other.m_EntityID; }

        ~Entity() = default;

        void operator=(const Entity& other) { m_EntityID = other.m_EntityID; }
        bool operator==(const Entity& other) const { return m_EntityID == other.m_EntityID; }

        static Entity Create();
        static Entity Create(const StringView& name);
        static Entity Find(const StringView& name);
        static Entity Find(EntityID entityID);
        static void ClearRecycle();

        void Destroy();

        template<typename ComponentType>
        ComponentType* Add();

        template<typename ComponentType>
        ComponentType* Get() const;

        template<typename ComponentType>
        void Remove();

        template<typename ComponentType>
        bool Has() const;

        bool Valid() const;

        bool IsPendingDestroy() const;

        void SetEnable(bool enable);
        bool GetEnalbe() const;

    private:
        Entity(EntityID entityID)
        {
            m_EntityID = entityID;
            RUSH_ASSERT(Valid());
        }

    private:
        friend class Component;
        EntityID m_EntityID;
    };


    //////////////////////////////////////////////////////////////////////////

    class Component
    {
    public:
        virtual ~Component() = default;

        Entity GetOwner() const { return m_Owner; }

        void SetEnable(bool enable) { m_Enable = enable; }
        bool GetEnalbe() const { return m_Enable; }

        virtual void OnInit() {}
        virtual void OnDestroy() {}

        template<typename ComponentType>
        ComponentType* Get() const;

    protected:
        Component(Entity owner);
        Component() = default;
        Entity m_Owner;
        bool m_Enable = true;
    };

    class EcsSystem
    {
    public:
        static entt::registry registry;
    private:
        friend class Entity;
        friend class Component;
        struct EntityDisable {};
        struct PendingDestroy {};
        static HMap<String, EntityID> entitiesNameMap;
        static HMap<EntityID, String> entitiesIDMap;
    };

    //////////////////////////////////////////////////////////////////////////
    inline Component::Component(Entity owner)
        : m_Owner(owner)
        , m_Enable(true)
    {
        RUSH_ASSERT(m_Owner.Valid());
    }

    template<typename ComponentType>
    ComponentType* Component::Get() const
    {
        return m_Owner.Get<ComponentType>();
    }

    inline void Entity::Destroy()
    {
        if (Valid())
        {
            EcsSystem::registry.emplace_or_replace<EcsSystem::PendingDestroy>(m_EntityID);
        }
    }

    inline bool Entity::Valid() const
    {
        return EcsSystem::registry.valid(m_EntityID);
    }

    template<typename ComponentType>
    ComponentType* Entity::Add()
    {
        RUSH_ASSERT(!Has<ComponentType>());
        ComponentType* ret = &EcsSystem::registry.emplace<ComponentType>(m_EntityID, *this);
        ret->OnInit();
        return ret;
    }

    template<typename ComponentType>
    ComponentType* Entity::Get() const
    {
        RUSH_ASSERT(Has<ComponentType>());
        return &EcsSystem::registry.get<ComponentType>(m_EntityID);
    }

    template<typename ComponentType>
    void Entity::Remove()
    {
        if (Has<ComponentType>())
        {
            Get<Component>()->OnDestroy();
            EcsSystem::registry.remove<ComponentType>(m_EntityID);
        }
    }

    template<typename ComponentType>
    bool Entity::Has() const
    {
        return EcsSystem::registry.any_of<ComponentType>(m_EntityID);
    }

    inline void Entity::SetEnable(bool enable)
    {
        if (enable && Has<EcsSystem::EntityDisable>())
        {
            EcsSystem::registry.remove<EcsSystem::EntityDisable>(m_EntityID);
        }
        else if (!enable && !Has<EcsSystem::EntityDisable>())
        {
            EcsSystem::registry.emplace<EcsSystem::EntityDisable>(m_EntityID);
        }
    }

    inline bool Entity::GetEnalbe() const
    {
        return !Has<EcsSystem::EntityDisable>();
    }

    inline Entity Entity::Create()
    {
        auto id = EcsSystem::registry.create();
        return Entity(id);
    }

    inline Entity Entity::Create(const StringView& name)
    {
        auto id = EcsSystem::registry.create();
        EcsSystem::entitiesNameMap[String(name)] = id;
        EcsSystem::entitiesIDMap[id] = String(name);
        return Entity(id);
    }

    inline Entity Entity::Find(const StringView& name)
    {
        auto iter = EcsSystem::entitiesNameMap.find(String(name));
        if (iter != EcsSystem::entitiesNameMap.end())
        {
            return Entity(iter->second);
        }
        else
        {
            return Entity();
        }
    }

    inline Entity Entity::Find(EntityID entityID)
    {
        return Entity(entityID);
    }

    inline void Entity::ClearRecycle()
    {
        auto view = EcsSystem::registry.view<EcsSystem::PendingDestroy>();
        for (auto entity : view)
        {
            EcsSystem::registry.destroy(entity);
            auto iter = EcsSystem::entitiesIDMap.find(entity);
            if (iter != EcsSystem::entitiesIDMap.end())
            {
                EcsSystem::entitiesIDMap.erase(entity);
                EcsSystem::entitiesNameMap.erase(iter->second);
            }
        }
    }

    inline bool Entity::IsPendingDestroy() const
    {
        return Has<EcsSystem::PendingDestroy>();
    }

}

#endif // EcsSystem_h__
