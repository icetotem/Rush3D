#ifndef Components_h__
#define Components_h__

#include "core/Core.h"

namespace rush
{

    struct Camera
    {
        
    };

    enum class LightType
    {
        Directional,
        Point,
        Spot,
    };

    struct Light
    {
        LightType Type;

    };

    struct Transform
    {
        EntityID Parent = nullEntity;
        Ref<List<EntityID>> Children;
        Vector3 Position = Vector3(0);
        Quat Rotation = glm::identity<Quat>();
        Vector3 Scale = Vector3(1);
    };


}

#endif // Components_h__
