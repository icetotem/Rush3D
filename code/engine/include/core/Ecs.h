#pragma once

#include "entt/single_include/entt/entt.hpp"

namespace rush
{

    typedef entt::entity EntityID;
    constexpr entt::null_t nullEntity = entt::null;

    extern entt::registry g_Registry;

}