#ifndef CommonComponents_h__
#define CommonComponents_h__

#include "components/EcsSystem.h"

namespace rush
{
    class InFrustumFlag : public Component
    {
    public:
        InFrustumFlag(Entity owner) : Component(owner) {}

        Entity m_ByCamera;
    };

}

#endif // CommonComponents_h__
