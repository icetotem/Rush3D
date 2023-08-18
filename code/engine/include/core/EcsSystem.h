#pragma once

#include "Core/Common.h"
#include "entt/single_include/entt/entt.hpp"
#include "Core/Json.h"

namespace rush
{

    class ComponentFactory;
    class Component;

    typedef entt::entity EntityID;

    class Entity : public std::enable_shared_from_this<Entity>
    {
    public:
        Entity(const std::string& name = "")
        {
            m_EntityID = s_Registry.create();
            m_Name = name;
        }

        ~Entity()
        {
            if (Valid())
            {
                s_Registry.destroy(m_EntityID);
            }
        }

        const std::string& GetName() const { return m_Name; }

        EntityID GetID() const { return m_EntityID; }

        static Ref<Entity> Create()
        {
            Ref<Entity> newEntity = CreateRef<Entity>();
            s_EntitiesByID[newEntity->m_EntityID] = newEntity;
            return newEntity;
        }

        static Ref<Entity> Create(const std::string& name)
        {
            Ref<Entity> newEntity = CreateRef<Entity>(name);
            s_EntitiesByID[newEntity->m_EntityID] = newEntity;
            if (name != "")
            {
                s_EntitiesByName[name] = newEntity;
            }
            return newEntity;
        }

        static Ref<Entity> CreateFromPrefab(const std::string& jsonString);
        static Ref<Entity> CreateFromPrefab(const std::string& jsonString, const std::string& name);

        bool SavePrefab(const std::string& prefabFile);

        static void Destroy(EntityID id)
        {
            auto ent = Find(id);
            if (ent != nullptr)
            {
                ent->Destroy();
            }
        }

        static void DestroyAll();

        void Destroy();

        static Ref<Entity> Find(EntityID id)
        {
            auto iter = s_EntitiesByID.find(id);
            if (iter != s_EntitiesByID.end())
            {
                return iter->second;
            }
            else
            {
                return nullptr;
            }
        }

        static Ref<Entity> FindByName(const std::string& name)
        {
            auto iter = s_EntitiesByName.find(name);
            if (iter != s_EntitiesByName.end())
            {
                return iter->second;
            }
            else
            {
                return nullptr;
            }
        }

        template<typename ComponentType>
        ComponentType* AddComponent()
        {
            ComponentType* ret = GetComponent<ComponentType>();
            if (ret != nullptr)
                return ret;

            auto factory = GetGlobalFactory(ComponentType::GetComponentName());
            RUSH_ASSERT(factory);
            ret = &s_Registry.emplace<ComponentType>(m_EntityID);
            m_ComponentFactories.insert({ ComponentType::GetComponentName(), factory });
            m_ComponentNamesInOrder.push_back(ComponentType::GetComponentName());
            ret->m_EntityID = m_EntityID;
            ret->OnInit();
            return ret;
        }

        template<typename ComponentType>
        ComponentType* GetComponent() const
        {
            return s_Registry.try_get<ComponentType>(m_EntityID);
        }

        template<typename ComponentType>
        ComponentType& GetComponentRef() const
        {
            return s_Registry.get<ComponentType>(m_EntityID);
        }

        template<typename ComponentType>
        void RemoveComponent()
        {            
            const auto comName = ComponentType::GetComponentName();
            auto com = GetComponentByName(comName);
            if (com)
            {
                com->OnDestroy();
                m_ComponentFactories.erase(comName);
                m_ComponentNamesInOrder.erase(std::find(m_ComponentNamesInOrder.begin(), m_ComponentNamesInOrder.end(), comName));
                s_Registry.remove<ComponentType>(m_EntityID);                
            }
        }

        void GetComponentNames(std::vector<std::string>& names)
        {
            for (auto iter : m_ComponentFactories)
            {
                names.push_back(iter.first);
            }
        }

        Component* AddComponentByName(const std::string& name);

        Component* GetComponentByName(const std::string& name);

        bool Valid() const
        {
            return s_Registry.valid(GetID());
        }

        bool IsActive() const { return m_Active; }
        void SetActive(bool active) { m_Active = active; }

        template<typename ComponentType, typename... Other, typename... Exclude>
        static entt::basic_view<entt::entity, entt::get_t<ComponentType, Other...>, entt::exclude_t<Exclude...>> View(entt::exclude_t<Exclude...> = {})
        {
            return s_Registry.view<ComponentType, Other..., Exclude...>();
        }

        static entt::registry& GetRegistry() { return s_Registry; }

        void SetScene(const std::string& scene) { m_Scene = scene; }
        const std::string& GetScene() const { return m_Scene; }

        const std::string& GetPrefab() const { return m_Prefab; }

    private:
        friend class Component;
        friend class ComponentFactory;

        EntityID m_EntityID;
        std::string m_Name;
        std::string m_Scene;
        std::string m_Prefab;
        bool m_Active = true;
        std::unordered_map<std::string, ComponentFactory*> m_ComponentFactories;
        std::list<std::string> m_ComponentNamesInOrder;

        static entt::registry s_Registry;
        static std::unordered_map<EntityID, Ref<Entity>> s_EntitiesByID;
        static std::unordered_map<std::string, Ref<Entity>> s_EntitiesByName;

        ComponentFactory* GetGlobalFactory(const std::string& name);
    };

    /**
     * Component
     */
    class Component
    {
    public:
        Component() = default;
        virtual ~Component() = default;

        template<typename Component>
        [[nodiscard]] decltype(auto) GetComponent() const
        {
            return Entity::s_Registry.try_get<Component>(m_EntityID);
        }

        template<typename Component>
        [[nodiscard]] decltype(auto) GetComponentRef() const
        {
            return Entity::s_Registry.get<Component>(m_EntityID);
        }

        EntityID GetOwnerID() const
        { 
            return m_EntityID; 
        }

        Ref<Entity> GetOwner() const
        {
            return Entity::Find(m_EntityID);
        }

        void SetEnable(bool enable) { m_Enable = enable; }
        bool GetEnable() const { return m_Enable; }

        virtual void OnInit() {}
        virtual void OnDestroy() {}

        // Serialization
        virtual void Serialize(Json& json) {}
        virtual void Deserialize(const Json& json) {}

    protected:
        friend class Entity;
        EntityID m_EntityID;

    public:
        bool m_Enable = true;
    };

    #define REGISTER_COMPONENT_TYPE(ComponentName) \
        static std::string GetComponentName() { return #ComponentName; } \
        friend class ComponentName##Factory; \
        friend class Entity; \
        friend class Component; \
        friend class ComponentFactory;

    /**
     * ComponentFactory
     */
    class ComponentFactory
    {
    public:
        ComponentFactory() {}
        virtual ~ComponentFactory() {}

        const std::string& GetName() const { return m_Name; }

    protected:
        friend class Entity;

        virtual Component* GetComponent(Entity* entity) = 0;

        virtual Component* AddComponent(Entity* entity) = 0;

        void AddFactory(Entity* entity)
        {
            entity->m_ComponentFactories.insert({ m_Name, this });
        }

        std::string m_Name;
        static std::unordered_map<std::string, ComponentFactory*>* s_Factories;
    };

#define REGISTER_COMPONENT_FACTORY(ComponentName)   \
        class ComponentName##Factory : public rush::ComponentFactory \
        {   \
        public: \
            ComponentName##Factory() { \
                m_Name = #ComponentName; \
                if (s_Factories == nullptr)  \
                    s_Factories = new std::unordered_map<std::string, rush::ComponentFactory*>; \
                s_Factories->insert({m_Name, this}); \
            } \
            virtual ~ComponentName##Factory() {}  \
            virtual rush::Component* AddComponent(rush::Entity* entity) override { \
                ComponentName* com = entity->AddComponent<ComponentName>(); \
                AddFactory(entity); \
                return static_cast<rush::Component*>(com); \
            }   \
            virtual rush::Component* GetComponent(rush::Entity* entity) override { \
                return entity->GetComponent<ComponentName>(); \
            } \
        };  \
        static ComponentName##Factory g_##ComponentName##Factory;

}
