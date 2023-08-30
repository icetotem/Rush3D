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

    protected:
        Component(Entity owner);
        Component() = default;
        Entity m_Owner;
        bool m_Enable = true;
    };

    class EntityContainer
    {
        friend class Entity;
        friend class Component;
        struct EntityDisable {};
        struct PendingDestroy {};
        static entt::registry registry;
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

    inline void Entity::Destroy()
    {
        if (Valid())
        {
            EntityContainer::registry.emplace_or_replace<EntityContainer::PendingDestroy>(m_EntityID);
        }
    }

    inline bool Entity::Valid() const
    {
        return EntityContainer::registry.valid(m_EntityID);
    }

    template<typename ComponentType>
    ComponentType* Entity::Add()
    {
        RUSH_ASSERT(!Has<ComponentType>());
        return &EntityContainer::registry.emplace<ComponentType>(m_EntityID, *this);
    }

    template<typename ComponentType>
    ComponentType* Entity::Get() const
    {
        return &EntityContainer::registry.get<ComponentType>(m_EntityID);
    }

    template<typename ComponentType>
    void Entity::Remove()
    {
        if (Has<ComponentType>())
            EntityContainer::registry.remove<ComponentType>(m_EntityID);
    }

    template<typename ComponentType>
    bool Entity::Has() const
    {
        return EntityContainer::registry.any_of<ComponentType>(m_EntityID);
    }

    inline void Entity::SetEnable(bool enable)
    {
        if (enable && Has<EntityContainer::EntityDisable>())
        {
            EntityContainer::registry.remove<EntityContainer::EntityDisable>(m_EntityID);
        }
        else if (!enable && !Has<EntityContainer::EntityDisable>())
        {
            EntityContainer::registry.emplace<EntityContainer::EntityDisable>(m_EntityID);
        }
    }

    inline bool Entity::GetEnalbe() const
    {
        return !Has<EntityContainer::EntityDisable>();
    }

    inline Entity Entity::Create()
    {
        auto id = EntityContainer::registry.create();
        return Entity(id);
    }

    inline Entity Entity::Create(const StringView& name)
    {
        auto id = EntityContainer::registry.create();
        EntityContainer::entitiesNameMap[String(name)] = id;
        EntityContainer::entitiesIDMap[id] = String(name);
        return Entity(id);
    }

    inline Entity Entity::Find(const StringView& name)
    {
        auto iter = EntityContainer::entitiesNameMap.find(String(name));
        if (iter != EntityContainer::entitiesNameMap.end())
        {
            return Entity(iter->second);
        }
        else
        {
            return Entity();
        }
    }

    inline void Entity::ClearRecycle()
    {
        auto view = EntityContainer::registry.view<EntityContainer::PendingDestroy>();
        for (auto entity : view)
        {
            EntityContainer::registry.destroy(entity);
            auto iter = EntityContainer::entitiesIDMap.find(entity);
            if (iter != EntityContainer::entitiesIDMap.end())
            {
                EntityContainer::entitiesIDMap.erase(entity);
                EntityContainer::entitiesNameMap.erase(iter->second);
            }
        }
    }

}

#endif // EcsSystem_h__
