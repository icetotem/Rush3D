#include "stdafx.h"
#include "components/EcsSystem.h"

namespace rush
{

    entt::registry EntityContainer::registry;
    HMap<String, EntityID> EntityContainer::entitiesNameMap;
    HMap<EntityID, String> EntityContainer::entitiesIDMap;

}