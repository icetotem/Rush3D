#pragma once

#include "components/EcsSystem.h"

namespace rush
{
    enum class Space
    {
        World,
        Parent,
        Local,
    };

    class Transform : public Component
    {
    public:
        Transform();

        void OnDestroy() override;

        void SetParent(Ref<Entity> parent);

        void DetachChildren();

        Ref<Entity> GetParent() const
        { 
            return m_Parent;
        }

        Ref<Entity> GetChild(int index);
        int GetChildCount() const;
        bool IsChildOf(Ref<Entity> parent);

        Vector3 TransformDirection(float x, float y, float z);
        Vector3 TransformDirection(const Vector3& direction);
        Vector3 TransformPoint(float x, float y, float z);
        Vector3 TransformPoint(const Vector3& position);

        Vector3 InverseTransformDirection(const Vector3& direction);
        Vector3 InverseTransformDirection(float x, float y, float z);
        Vector3 InverseTransformPoint(float x, float y, float z);
        Vector3 InverseTransformPoint(const Vector3& position);

        void Reset();

        void LookAt(Transform* target, Vector3 worldUp = yAxis);
        void LookAt(const Vector3& worldPosition, Vector3 worldUp = yAxis);

        void Rotate(const Quat& quat, Space relativeTo = Space::Local);
        void Rotate(const Vector3& eulers, Space relativeTo = Space::Local);
        void Rotate(float xAngle, float yAngle, float zAngle, Space relativeTo = Space::Local);
        void Rotate(const Vector3& axis, float angle, Space relativeTo = Space::Local);

        void SetPositionAndRotation(const Vector3& position, Quat rotation);
        void CopyPositionAndRotation(Transform* other);
        void CopyTransform(Transform* other);

        void Translate(float x, float y, float z, Space relativeTo = Space::Local);
        void Translate(const Vector3& translation, Space relativeTo = Space::Local);

        void SetLocalPosition(float x, float y, float z);
        void SetLocalPosition(const Vector3& position);
        const Vector3& GetLocalPosition() const { return m_LocalPosition; }

        void SetLocalEulerAngles(float pitch, float yaw, float roll);
        void SetLocalEulerAngles(const Vector3& euler);
        const Vector3& GetLocalEulerAngles() const { return m_LocalEulerAngles; }

        void SetLocalRotation(float x, float y, float z);
        void SetLocalRotation(const Quat& quat);
        const Quat& GetLocalRotation() const { return m_LocalRotation; }

        void SetLocalScale(const Vector3& scale);
        const Vector3& GetLocalScale() const { return m_LocalScale; }

        // +x
        void SetRight(const Vector3& right);
        const Vector3 GetRight() const;

        // -x
        void SetLeft(const Vector3& left);
        const Vector3 GetLeft() const;

        // +y
        void SetUp(const Vector3& up);
        const Vector3 GetUp() const;

        // -y
        void SetDown(const Vector3& down);
        const Vector3 GetDown() const;

        // -z
        void SetForward(const Vector3& forward);
        const Vector3 GetForward() const;

        // z
        void SetBack(const Vector3& back);
        const Vector3 GetBack() const;

        void SetRotation(const Quat& quat);
        const Quat& GetRotation() const;

        void SetPosition(float x, float y, float z);
        void SetPosition(const Vector3& position);
        const Vector3& GetPosition() const;

        void SetEulerAngles(float pitch, float yaw, float roll);
        void SetEulerAngles(const Vector3& euler);
        const Vector3& GetEulerAngles() const;

        void SetScale(float x, float y, float z);
        void SetScale(const Vector3& scale);
        const Vector3& GetScale() const;

        const Matrix4& GetWorldMatrix() const;
        //const Matrix34& GetWorldMatrix34() const;

        void SetLocalIdentity();

        int GetHierarchyDeep();

        static void GetRelative(const Vector3& PosA, const Quat& RotA, const Vector3& ScaleA, 
            const Vector3& PosB, const Quat& RotB, const Vector3& ScaleB, 
            Vector3& OutPos, Quat& OutRot, Vector3& OutScale);

        static void DoTransform(const Vector3& Pos, const Quat& Rot, const Vector3& Scale,
            const Vector3& RelPos, const Quat& RelRot, const Vector3& RelScale,
            Vector3& OutPos, Quat& OutRot, Vector3& OutScale);

        void Serialize(Json& json) override;
        void Deserialize(const Json& json) override;

    protected:
        void AddChild(Ref<Entity> child);
        void RemoveChild(Ref<Entity> child);
        void MakeDirty();
        void UpdateLocal();
        void UpdateWorld();
        void UpdateDirections();
        void UpdateWorldMatrix();
        void UpdateTree();
        const Transform* GetDirtyAncestor() const;
        void CheckUpdateTree() const;

    protected:
        Ref<Entity> m_Parent;
        std::vector<Ref<Entity>> m_Children;

        Vector3 m_Position;
        Quat m_Rotation;
        Vector3 m_Scale;
        Vector3 m_LocalPosition;
        Vector3 m_EulerAngles;
        Vector3 m_LocalEulerAngles;
        Vector3 m_LocalScale;
        Vector3 m_Left;
        Vector3 m_Up;
        Vector3 m_Forward;
        Quat m_LocalRotation;
        Matrix4 m_WorldMatrix;
        //mutable Matrix34 m_WorldMatrix34;
        bool m_IsDirty;

    };

}

#include "Transform.inl"
