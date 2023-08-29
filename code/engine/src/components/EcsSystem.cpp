#include "stdafx.h"
#include "components/EcsSystem.h"

namespace rush
{
    struct ComponentEnable {};

    entt::registry g_Registry;

    static HMap<String, EntityID> g_EntitiesByName;
    static List<EntityID> g_EntityRecycle;

    Component::Component(EntityID entityID)
        : m_EntityID(entityID)
    {
        RUSH_ASSERT(g_Registry.valid(m_EntityID));
    }

    void Component::SetEnable(bool enable)
    {
        g_Registry.emplace<ComponentEnable>(m_EntityID);
    }

    bool Component::GetEnalbe() const
    {
        return g_Registry.ctx().contains<ComponentEnable>();
    }

    EntityID Entity::Create()
    {
        return g_Registry.create();
    }

    EntityID Entity::Create(const StringView& name)
    {
        auto entity = g_Registry.create();
        g_EntitiesByName[String(name)] = entity;
        return entity;
    }

    EntityID Entity::Find(const StringView& name)
    {
        auto iter = g_EntitiesByName.find(String(name));
        if (iter != g_EntitiesByName.end())
        {
            return iter->second;
        }
        else
        {
            return entt::null;
        }
    }

    void Entity::Destroy(EntityID entityID)
    {
        g_EntityRecycle.push_back(entityID);
    }

    void Entity::ClearRecycle()
    {
        for (auto entity : g_EntityRecycle)
        {
            g_Registry.destroy(entity);
        }
        g_EntityRecycle.clear();
    }



}