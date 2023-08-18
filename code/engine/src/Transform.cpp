#include "stdafx.h"
#include "Transform.h"

namespace rush
{
    
    Transform::Transform()
    {
        m_Position = Vector3(0);
        m_Rotation = glm::identity<Quat>();
        m_Scale = Vector3(1, 1, 1);
        m_EulerAngles = Vector3(0);
        m_LocalRotation = m_Rotation;
        m_LocalPosition = m_Position;
        m_LocalScale = m_Scale;
        m_LocalEulerAngles = m_EulerAngles;
        m_Forward = zAxis;
        m_Up = yAxis;
        m_Left = xAxis;
        m_IsDirty = false;
        m_WorldMatrix = glm::identity<Matrix4>();
        m_Parent = nullptr;
    }

    Transform::~Transform()
    {
        SetParent(nullptr);
    }

    void Transform::Reset()
    {
        SetLocalPosition(Vector3(0));
        SetLocalRotation(glm::identity<Quat>());
        SetLocalScale(Vector3(1));
    }


}