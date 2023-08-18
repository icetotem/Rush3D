
#include "stdafx.h"
#include "Core/EcsSystem.h"

namespace rush
{

    entt::registry Entity::s_Registry;
    std::unordered_map<rush::EntityID, Ref<Entity>> Entity::s_EntitiesByID;
    std::unordered_map<std::string, Ref<Entity>> Entity::s_EntitiesByName;
    std::unordered_map<std::string, ComponentFactory*>* ComponentFactory::s_Factories;

    ComponentFactory* Entity::GetGlobalFactory(const std::string& name)
    {
        auto iter = ComponentFactory::s_Factories->find(name);
        if (iter != ComponentFactory::s_Factories->end())
        {
            return iter->second;
        }
        else
        {
            return nullptr;
        }
    }


    struct PrefabJson
    {
        std::string prefabFile;
        std::vector<std::string> components;

        RUSH_DEFINE_PROPERTIES_NON_INTRUSIVE_WITH_DEFAULT(PrefabJson, prefabFile, components);

    };

    Ref<Entity> Entity::CreateFromPrefab(const std::string& jsonString, const std::string& name)
    {
        PrefabJson prefabData = Json::parse(jsonString);

        auto newEntity = Create(name);

        Json json = Json::parse(jsonString);

        for (const auto& com : prefabData.components)
        {
            auto component = newEntity->AddComponentByName(com);
            if (component)
                component->Deserialize(json);
            else
                LOG_ERROR("Cannot find component {} in entity file {}", com, prefabData.prefabFile);
        }
        newEntity->m_Prefab = prefabData.prefabFile;
        return newEntity;
    }

    Ref<Entity> Entity::CreateFromPrefab(const std::string& jsonString)
    {
        PrefabJson prefabData = Json::parse(jsonString);

        auto newEntity = Create();

        Json json = Json::parse(jsonString);

        for (const auto& com : prefabData.components)
        {
            auto component = newEntity->AddComponentByName(com);
            if (component)
                component->Deserialize(json);
            else
                LOG_ERROR("Cannot find component {} in entity file {}", com, prefabData.prefabFile);
        }

        newEntity->m_Prefab = prefabData.prefabFile;
        return newEntity;
    }

    bool Entity::SavePrefab(const std::string& prefabFile/* = ""*/)
    {
        Ref<File> newFile = CreateRef<File>();

        PrefabJson prefabData;
        prefabData.prefabFile = Path(prefabFile).stem().string();

        for (auto comName : m_ComponentNamesInOrder)
        {
            prefabData.components.push_back(comName);
        }

        Json json;
        to_json(json, prefabData);

        for (auto comName : m_ComponentNamesInOrder)
        {
            auto component = GetComponentByName(comName);
            if (component)
            {
                component->Serialize(json);
            }
        }

        std::string jsonStr = json.dump();
        auto fullName = FileSystem::GetWorkingDir() + prefabFile;

        if (File::New(fullName, jsonStr.size(), true) && newFile->Open(fullName, false, 0, jsonStr.size()))
        {
            newFile->WriteBuffer((uint8_t*)jsonStr.c_str(), jsonStr.size());
            newFile->Close();
            return true;
        }
        else
        {
            return false;
        }
    }

    void Entity::DestroyAll()
    {
        for (auto& ent : s_EntitiesByID)
        {
            for (auto factory : ent.second->m_ComponentFactories)
            {
                factory.second->GetComponent(ent.second.get())->OnDestroy();
            }
            ent.second->m_ComponentFactories.clear();            

            s_Registry.destroy(ent.second->GetID());
        }
        s_EntitiesByID.clear();
        s_EntitiesByName.clear();
    }


    void Entity::Destroy()
    {
        for (auto factory : m_ComponentFactories)
        {
            factory.second->GetComponent(this)->OnDestroy();
        }
        m_ComponentFactories.clear();

        if (s_Registry.valid(m_EntityID))
            s_Registry.destroy(m_EntityID);

        s_EntitiesByID.erase(m_EntityID);
        if (m_Name != "")
        {
            s_EntitiesByName.erase(m_Name);
        }
    }

    Component* Entity::AddComponentByName(const std::string& name)
    {
        auto iter = ComponentFactory::s_Factories->find(name);
        if (iter != ComponentFactory::s_Factories->end())
        {
            return iter->second->AddComponent(this);
        }
        else
        {
            return nullptr;
        }
    }

    Component* Entity::GetComponentByName(const std::string& name)
    {
        auto iter = m_ComponentFactories.find(name);
        if (iter != m_ComponentFactories.end())
        {
            return iter->second->GetComponent(this);
        }
        else
        {
            return nullptr;
        }
    }

}