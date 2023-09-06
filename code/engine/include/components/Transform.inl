
namespace rush
{

    inline void Transform::GetRelative(const Vector3& PosA, const Quat& RotA, const Vector3& ScaleA,
        const Vector3& PosB, const Quat& RotB, const Vector3& ScaleB,
        Vector3& OutPos, Quat& OutRot, Vector3& OutScale)
    {
        Vector3 SafeRecipScale3D = getSafeScaleReciprocal(ScaleB);
        OutScale = ScaleA * SafeRecipScale3D;
        Quat Inverse = glm::inverse(RotB);
        OutRot = Inverse * RotA;
        Vector3 pos = glm::rotate(Inverse, (PosA - PosB));
        OutPos = pos * SafeRecipScale3D;
    }

    inline void Transform::DoTransform(const Vector3& Pos, const Quat& Rot, const Vector3& Scale,
        const Vector3& RelPos, const Quat& RelRot, const Vector3& RelScale,
        Vector3& OutPos, Quat& OutRot, Vector3& OutScale)
    {
        OutRot = Rot * RelRot;
        OutScale = Scale * RelScale;
        Vector3 delta = glm::rotate(Rot, Scale * RelPos);
        OutPos = delta + Pos;
    }

    inline void Transform::UpdateLocal()
    {
        if (!m_Parent.Valid())
        {
            m_LocalRotation = m_Rotation;
            m_LocalScale = m_Scale;
            m_LocalPosition = m_Position;
            m_LocalEulerAngles = m_EulerAngles;
        }
        else
        {
            auto trans = m_Parent.Get<Transform>();
            GetRelative(m_Position, m_Rotation, m_Scale, trans->m_Position, trans->m_Rotation, trans->m_Scale, m_LocalPosition, m_LocalRotation, m_LocalScale);
            m_LocalEulerAngles = quatToEuler(m_LocalRotation);
        }
    }

    inline void Transform::UpdateWorld()
    {
        if (!m_Parent.Valid())
        {
            m_Rotation = m_LocalRotation;
            m_EulerAngles = m_LocalEulerAngles;
            m_Scale = m_LocalScale;
            m_Position = m_LocalPosition;
        }
        else
        {
            auto trans = m_Parent.Get<Transform>();
            DoTransform(trans->m_Position, trans->m_Rotation, trans->m_Scale, m_LocalPosition, m_LocalRotation, m_LocalScale, m_Position, m_Rotation, m_Scale);
            m_EulerAngles = quatToEuler(m_Rotation);
        }

        UpdateDirections();
        UpdateWorldMatrix();
    }

    inline void Transform::SetParent(Entity parent)
    {
        if (m_Parent == parent)
            return;

        if (!m_Parent.Valid())
            m_Parent.Get<Transform>()->RemoveChild(GetOwner());

        if (!parent.Valid())
        {
            parent.Get<Transform>()->AddChild(GetOwner());
            parent.Get<Transform>()->CheckUpdateTree();
        }

        m_Parent = parent;

        UpdateLocal();
    }

    inline void Transform::DetachChildren()
    {
        for (auto child : m_Children)
        {
            child.Get<Transform>()->SetParent(Entity());
        }
    }

    inline void Transform::UpdateDirections()
    {
        m_Forward = glm::rotate(m_Rotation, Vector3(0, 0, 1));
        m_Up = glm::rotate(m_Rotation, Vector3(0, 1, 0));
        m_Left = glm::rotate(m_Rotation, Vector3(-1, 0, 0));
    }

    inline void Transform::UpdateWorldMatrix()
    {
        m_WorldMatrix = matrix4FromTRS(m_Position, m_Rotation, m_Scale);
    }

    inline Entity Transform::GetChild(int index)
    {
        if (index >= 0 && index < GetChildCount())
            return m_Children[index];
        return Entity();
    }

    inline int Transform::GetChildCount() const
    {
        return m_Children.size();
    }

    inline Vector3 Transform::TransformDirection(float x, float y, float z)
    {
        Vector3 dir = Vector3(x, y, z);
        float len = length(dir);
        Vector4 v = (m_WorldMatrix * Vector4(dir / len, 0.0f)) * len;
        return Vector3(v.x, v.y, v.z);
    }

    inline Vector3 Transform::TransformDirection(const Vector3& direction)
    {
        return TransformDirection(direction.x, direction.y, direction.z);
    }

    inline Vector3 Transform::TransformPoint(float x, float y, float z)
    {
        Vector4 v = (m_WorldMatrix * Vector4(x, y, z, 0));
        return Vector3(v.x, v.y, v.z);
    }

    inline Vector3 Transform::TransformPoint(const Vector3& position)
    {
        Vector4 v = (m_WorldMatrix * Vector4(position, 0));
        return Vector3(v.x, v.y, v.z);
    }

    inline Vector3 Transform::InverseTransformDirection(float x, float y, float z)
    {
        Vector3 dir = Vector3(x, y, z);
        float len = length(dir);
        Vector4 v = (glm::inverse(m_WorldMatrix) * Vector4(dir / len, 0)) * len;
        return Vector3(v.x, v.y, v.z);
    }

    inline Vector3 Transform::InverseTransformDirection(const Vector3& direction)
    {
        return InverseTransformDirection(direction.x, direction.y, direction.z);
    }

    inline Vector3 Transform::InverseTransformPoint(float x, float y, float z)
    {
        Vector4 v = (glm::inverse(m_WorldMatrix) * Vector4(x, y, z, 1.0f));
        return Vector3(v.x, v.y, v.z);
    }

    inline Vector3 Transform::InverseTransformPoint(const Vector3& position)
    {
        Vector4 v = (glm::inverse(m_WorldMatrix) * Vector4(position, 1.0f));
        return Vector3(v.x, v.y, v.z);
    }

    inline bool Transform::IsChildOf(Entity parent)
    {
        if (!parent.Valid() || !m_Parent.Valid())
            return false;
        else if (parent == m_Parent)
            return true;
        else
            return m_Parent.Get<Transform>()->IsChildOf(parent);
    }

    inline void Transform::LookAt(Transform* target, Vector3 worldUp /*= yAxis*/)
    {
        if (target == nullptr)
            return;
        const Vector3& pos = target->GetPosition();
        LookAt(pos, worldUp);
    }

    inline void Transform::LookAt(float x, float y, float z, Vector3 worldUp /*= yAxis*/)
    {
        LookAt(Vector3(x, y, z), yAxis);
    }

    inline void Transform::LookAt(const Vector3& worldPosition, Vector3 worldUp /*= yAxis*/)
    {
        Vector3 forward = normalize(worldPosition - GetPosition());
        if (length(worldUp - forward) <= 0.001
            || length(worldUp + forward) <= 0.001)
        {
            worldUp = cross(m_Left, forward);
        }
        Vector3 right = cross(worldUp, forward);
        SetRotation(quatFromXZ(right, forward));
    }

    inline void Transform::Rotate(const Quat& quat, Space relativeTo /*= Space::Local*/)
    {
        if (relativeTo == Space::Local)
        {
            SetRotation(GetRotation() * quat);
        }
        else
        {
            SetRotation(quat * GetRotation());
        }
    }

    inline void Transform::Rotate(const Vector3& eulers, Space relativeTo /*= Space::Local*/)
    {
        if (relativeTo == Space::Local)
        {
            SetRotation(GetRotation() * eularToQuat(eulers));
        }
        else
        {
            SetRotation(eularToQuat(eulers) * GetRotation());
        }
    }

    inline void Transform::Rotate(float xAngle, float yAngle, float zAngle, Space relativeTo /*= Space::Local*/)
    {
        Vector3 eulers(xAngle, yAngle, zAngle);
        if (relativeTo == Space::Local)
        {
            SetRotation(GetRotation() * eularToQuat(eulers));
        }
        else
        {
            SetRotation(eularToQuat(eulers) * GetRotation());
        }
    }

    inline void Transform::Rotate(const Vector3& axis, float angle, Space relativeTo /*= Space::Local*/)
    {
        Quat rot(degToRad(angle), axis);
        if (relativeTo == Space::Local)
        {
            SetRotation(GetRotation() * rot);
        }
        else
        {
            SetRotation(rot * GetRotation());
        }
    }

    inline void Transform::SetPositionAndRotation(const Vector3& position, Quat rotation)
    {
        SetPosition(position);
        SetRotation(rotation);
    }

    inline void Transform::CopyPositionAndRotation(Transform* other)
    {
        SetPosition(other->GetPosition());
        SetRotation(other->GetRotation());
    }

    inline void Transform::CopyTransform(Transform* other)
    {
        SetPosition(other->GetPosition());
        SetRotation(other->GetRotation());
        SetScale(other->GetScale());
    }

    inline void Transform::Translate(float x, float y, float z, Space relativeTo /*= Space::Local*/)
    {
        Translate(Vector3(x, y, z), relativeTo);
    }

    inline void Transform::Translate(const Vector3& translation, Space relativeTo /*= Space::Local*/)
    {
        if (relativeTo == Space::Parent)
        {
            SetLocalPosition(GetLocalPosition() + translation);
        }
        else if (relativeTo == Space::World)
        {
            SetPosition(GetPosition() + translation);
        }
        else if (relativeTo == Space::Local)
        {
            SetPosition(GetPosition() + GetRotation() * translation);
        }
    }

    inline void Transform::SetRight(const Vector3& right)
    {
        assert(0);
    }

    inline const Vector3 Transform::GetRight() const
    {
        CheckUpdateTree();
        return -m_Left;
    }

    inline void Transform::SetLeft(const Vector3& left)
    {
        assert(0);

    }

    inline const Vector3 Transform::GetLeft() const
    {
        CheckUpdateTree();
        return m_Left;
    }

    inline void Transform::SetUp(const Vector3& up)
    {
        assert(0);
    }

    inline const Vector3 Transform::GetUp() const
    {
        CheckUpdateTree();
        return m_Up;
    }

    inline void Transform::SetDown(const Vector3& down)
    {
        assert(0);

    }

    inline const Vector3 Transform::GetDown() const
    {
        CheckUpdateTree();
        return -m_Up;
    }

    inline void Transform::SetForward(const Vector3& forward)
    {
        assert(0);
    }

    inline const Vector3 Transform::GetForward() const
    {
        CheckUpdateTree();
        return m_Forward;
    }

    inline void Transform::SetBack(const Vector3& back)
    {
        assert(0);

    }

    inline const Vector3 Transform::GetBack() const
    {
        CheckUpdateTree();
        return -m_Forward;
    }

    inline void Transform::SetLocalPosition(const Vector3& position)
    {
        if (!m_Parent.Valid())
        {
            m_Parent.Get<Transform>()->CheckUpdateTree();
        }

        m_LocalPosition = position;
        MakeDirty();
    }

    inline void Transform::SetLocalPosition(float x, float y, float z)
    {
        SetLocalPosition(Vector3(x, y, z));
    }

    inline void Transform::SetLocalEulerAngles(const Vector3& euler)
    {
        if (!m_Parent.Valid())
        {
            m_Parent.Get<Transform>()->CheckUpdateTree();
        }

        m_LocalEulerAngles = euler;
        m_LocalRotation = eularToQuat(euler);
        MakeDirty();
    }

    inline void Transform::SetLocalEulerAngles(float pitch, float yaw, float roll)
    {
        SetLocalEulerAngles(Vector3(pitch, yaw, roll));
    }

    inline void Transform::SetLocalRotation(const Quat& quat)
    {
        if (!m_Parent.Valid())
        {
            m_Parent.Get<Transform>()->CheckUpdateTree();
        }

        m_LocalRotation = quat;
        m_LocalEulerAngles = quatToEuler(quat);
        MakeDirty();
    }

    inline void Transform::SetLocalRotation(float x, float y, float z)
    {
        SetLocalRotation(Vector3(x, y, z));
    }

    inline void Transform::SetLocalScale(const Vector3& scale)
    {
        if (!m_Parent.Valid())
        {
            m_Parent.Get<Transform>()->CheckUpdateTree();
        }

        m_LocalScale = scale;
        MakeDirty();
    }

    inline void Transform::SetLocalScale(float scale)
    {
        SetLocalScale(Vector3(scale));
    }

    inline void Transform::SetLocalScale(float x, float y, float z)
    {
        SetLocalScale(Vector3(x, y, z));
    }

    inline void Transform::SetRotation(const Quat& quat)
    {
        if (m_Parent.Valid())
        {
            m_Parent.Get<Transform>()->CheckUpdateTree();
            m_Rotation = quat;
            m_EulerAngles = quatToEuler(m_Rotation);
            m_LocalRotation = inverse(m_Parent.Get<Transform>()->m_Rotation) * m_Rotation;
            m_LocalEulerAngles = quatToEuler(m_LocalRotation);
        }
        else
        {
            m_Rotation = quat;
            m_EulerAngles = quatToEuler(m_Rotation);
            m_LocalRotation = m_Rotation;
            m_LocalEulerAngles = m_EulerAngles;
        }

        MakeDirty();
    }

    inline const Quat& Transform::GetRotation() const
    {
        CheckUpdateTree();
        return m_Rotation;
    }

    inline void Transform::SetEulerAngles(const Vector3& euler)
    {
        if (!m_Parent.Valid())
        {
            m_Parent.Get<Transform>()->CheckUpdateTree();
            m_Rotation = eularToQuat(euler);
            m_EulerAngles = euler;
            m_LocalRotation = glm::inverse(m_Parent.Get<Transform>()->GetRotation()) * eularToQuat(euler);
            m_LocalEulerAngles = euler;
        }
        else
        {
            m_Rotation = eularToQuat(euler);
            m_EulerAngles = euler;
            m_LocalRotation = m_Rotation;
            m_LocalEulerAngles = m_EulerAngles;
        }

        MakeDirty();
    }

    inline void Transform::SetEulerAngles(float pitch, float yaw, float roll)
    {
        SetEulerAngles(Vector3(pitch, yaw, roll));
    }

    inline const Vector3& Transform::GetEulerAngles() const
    {
        CheckUpdateTree();
        return m_EulerAngles;
    }

    inline void Transform::SetPosition(const Vector3& pos)
    {
        if (m_Parent.Valid())
        {
            auto trans = m_Parent.Get<Transform>();
            trans->CheckUpdateTree();
            Vector3 SafeRecipScale3D = getSafeScaleReciprocal(trans->m_Scale);
            Quat Inverse = glm::inverse(trans->m_Rotation);
            m_Position = pos;
            m_LocalPosition = glm::rotate(Inverse, (m_Position - trans->m_Position)) * SafeRecipScale3D;
        }
        else
        {
            m_Position = pos;
            m_LocalPosition = pos;
        }
        MakeDirty();
    }

    inline void Transform::SetPosition(float x, float y, float z)
    {
        SetPosition(Vector3(x, y, z));
    }

    inline const Vector3& Transform::GetPosition() const
    {
        CheckUpdateTree();
        return m_Position;
    }

    inline void Transform::SetScale(const Vector3& sc)
    {
        if (!m_Parent.Valid())
        {
            auto trans = m_Parent.Get<Transform>();
            trans->CheckUpdateTree();
            m_Scale = sc;
            Vector3 SafeRecipScale3D = getSafeScaleReciprocal(trans->m_Scale);
            m_LocalScale = m_Scale * SafeRecipScale3D;
        }
        else
        {
            m_Scale = sc;
            m_LocalScale = m_Scale;
        }
        MakeDirty();
    }

    inline void Transform::SetScale(float x, float y, float z)
    {
        SetScale(Vector3(x, y, z));
    }

    inline const Vector3& Transform::GetScale() const
    {
        CheckUpdateTree();
        return m_Scale;
    }

    inline const Matrix4& Transform::GetWorldMatrix() const
    {
        CheckUpdateTree();
        return m_WorldMatrix;
    }

    //inline const Matrix34& Transform::GetWorldMatrix34() const
    //{
    //    CheckUpdateTree();
    //    m_WorldMatrix34 = glm::transpose(m_WorldMatrix);
    //    return m_WorldMatrix34;
    //}

    inline void Transform::SetLocalIdentity()
    {
        SetLocalScale(Vector3(1));
        SetLocalRotation(glm::identity<Quat>());
        SetLocalPosition(Vector3(0));
    }

    inline int Transform::GetHierarchyDeep()
    {
        if (!m_Parent.Valid())
        {
            return 0;
        }

        auto result = GetOwner();
        int deep = 0;
        while (result.Get<Transform>()->GetParent().Valid())
        {
            result = result.Get<Transform>()->GetParent();
            ++deep;
        }

        return deep;
    }

    inline void Transform::AddChild(Entity child)
    {
        m_Children.push_back(child);
    }

    inline void Transform::RemoveChild(Entity child)
    {
        for (auto iter = m_Children.begin(); iter != m_Children.end(); ++iter)
        {
            if (*iter == child)
            {
                m_Children.erase(iter);
                break;
            }
        }
    }

    inline void Transform::MakeDirty()
    {
        m_IsDirty = true;
        for (auto child : m_Children)
        {
            auto trans = child.Get<Transform>();
            trans->m_IsDirty = true;
            trans->MakeDirty();
        }
    }

    inline void Transform::CheckUpdateTree() const
    {
        if (m_IsDirty)
        {
            auto ancestor = GetDirtyAncestor();
            if (ancestor)
            {
                const_cast<Transform*>(ancestor)->UpdateTree();
            }
        }
    }

    inline void Transform::UpdateTree()
    {
        if (m_IsDirty)
        {
            UpdateWorld();
            m_IsDirty = false;
        }

        for (auto child : m_Children)
        {
            child.Get<Transform>()->UpdateTree();
        }
    }

    inline const Transform* Transform::GetDirtyAncestor() const
    {
        const Transform* result = this;
        if (!m_Parent.Valid() && m_IsDirty)
        {
            return result;
        }

        while (result->GetParent().Valid() && result->GetParent().Get<Transform>()->m_IsDirty)
        {
            result = result->GetParent().Get<Transform>();
        }

        if (!result->m_IsDirty)
        {
            return nullptr;
        }

        return result;
    }
}