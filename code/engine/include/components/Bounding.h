#ifndef Bounding_h__
#define Bounding_h__

#include "components/EcsSystem.h"
#include "core/MathUtils.h"

namespace rush
{
    
    class Bounding : public Component
    {
    public:
        Bounding(Entity owner);

        AABB m_InitAABB;
        AABB m_AABB;
        OBB m_InitOBB;
        OBB m_OBB;

        void Update();
    };

}

#endif // Bounding_h__

