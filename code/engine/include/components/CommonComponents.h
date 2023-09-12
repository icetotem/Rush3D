#ifndef CommonComponents_h__
#define CommonComponents_h__

#include "components/EcsSystem.h"

namespace rush
{
    class InFrustumFlag : public Component
    {
    public:
        InFrustumFlag(Entity owner) : Component(owner) {}

        Entity camera;
    };

}

#endif // CommonComponents_h__
