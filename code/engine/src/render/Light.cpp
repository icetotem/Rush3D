#include "stdafx.h"
#include "components/Light.h"
#include "components/Transform.h"

namespace rush
{

    Light::Light(Entity owner)
        : Component(owner)
    {
        SetRadius(1.0f);
    }

    void Light::SetRadius(float radius)
    {
        m_Radius = radius;
    }

    void Light::UpdateBoundings()
    {
        if (m_Type == LT_Directional)
        {
            m_AABB = AABB();
        }
        else
        {
            auto transform = Get<Transform>();
            auto m = matrix4FromTRS(transform->GetPosition(), glm::identity<Quat>(), Vector3(1.0f));
            m_AABB = AABB::Transform({ Vector3(-m_Radius), Vector3(m_Radius) }, m);
        }
    }

}