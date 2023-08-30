#include "stdafx.h"
#include "components/EcsSystem.h"

namespace rush
{

    entt::registry EcsSystem::registry;
    HMap<String, EntityID> EcsSystem::entitiesNameMap;
    HMap<EntityID, String> EcsSystem::entitiesIDMap;

}