#pragma once

#include "core/ecs.h"
#include "core/MathUtils.h"

namespace rush
{

    struct Transform
    {
        EntityID Parent = nullEntity;
        Ref<List<EntityID>> Children;
        Vector3 Position = Vector3(0);
        Quat Rotation = glm::identity<Quat>();
        Vector3 Scale = Vector3(1);
    };



}