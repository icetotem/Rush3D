#include "stdafx.h"
#include "components/Bounding.h"
#include "components/Transform.h"

namespace rush
{  

    Bounding::Bounding(Entity owner)
        : Component(owner)
    {
    }

    void Bounding::Update()
    {
        auto transform = Get<Transform>();
        m_AABB = AABB::Transform(m_InitAABB, transform->GetWorldMatrix());
        m_OBB = OBB::Transform(m_InitOBB, transform->GetWorldMatrix());
    }

}