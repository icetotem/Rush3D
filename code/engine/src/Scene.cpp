#include "stdafx.h"
#include "Scene.h"
#include "components/Camera.h"
#include "components/Transform.h"
#include "components/Light.h"

namespace rush
{

    Scene::Scene()
    {

    }

    Scene::~Scene()
    {
        Unload();
    }

    EntityID Scene::CreateBasicEntity()
    {
        auto id = g_Registry.create();
        g_Registry.emplace<Transform>(id);
        m_CommonEntities.push_back(id);
        return id;
    }

    EntityID Scene::CreateCamera()
    {
        auto id = g_Registry.create();
        g_Registry.emplace<Transform>(id);
        g_Registry.emplace<Camera>(id);
        m_Cameras.push_back(id);
        return id;
    }

    EntityID Scene::CreateLight()
    {
        auto id = g_Registry.create();
        g_Registry.emplace<Transform>(id);
        g_Registry.emplace<Light>(id);
        m_Lights.push_back(id);
        return id;
    }

    void Scene::DestroyEntity(EntityID entityID)
    {
        m_PendingDeleteList.insert(entityID);
    }

    void Scene::Update(float deltaTime)
    {


        RecycleUpdate();
    }

    void Scene::LoadFromScn(const StringView& fileName)
    {

    }

    void Scene::LoadFromGltf(const StringView& fileName)
    {

    }

    void Scene::Unload()
    {
        for (auto ent : m_CommonEntities)
        {
            g_Registry.destroy(ent);
        }
        for (auto ent : m_Cameras)
        {
            g_Registry.destroy(ent);
        }
        for (auto ent : m_Lights)
        {
            g_Registry.destroy(ent);
        }
        m_CommonEntities.clear();
        m_Cameras.clear();
        m_Lights.clear();
    }

    void Scene::RecycleUpdate()
    {
        for (auto ent : m_PendingDeleteList)
        {
            g_Registry.destroy(ent);
        }
        m_PendingDeleteList.clear();
    }

}