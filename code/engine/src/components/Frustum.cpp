#include "stdafx.h"
#include "components/Frustum.h"
#include "components/Transform.h"
#include "components/Camera.h"

namespace rush
{

    Frustum::Frustum(Entity owner)
        : Component(owner)
    {
    }

    FrustumCullResult Frustum::CullPoint(const Vector3& point) const
	{
		for (int i = 0; i < 6; ++i)
		{
			if (dot(m_Planes[i].ToVec4(), Vector4(point, 1.0f)) < 0)
				return FrustumCullResult::Outside;
		}
		return FrustumCullResult::Inside;
	}

    FrustumCullResult Frustum::CullAABB(const AABB& aabb) const
	{
		for (int i = 0; i < 6; i++)
		{
			int out = 0;
			out += ((dot(m_Planes[i].ToVec4(), Vector4(aabb.minBounds.x, aabb.minBounds.y, aabb.minBounds.z, 1.0f)) < 0.0) ? 1 : 0);
			out += ((dot(m_Planes[i].ToVec4(), Vector4(aabb.maxBounds.x, aabb.minBounds.y, aabb.minBounds.z, 1.0f)) < 0.0) ? 1 : 0);
			out += ((dot(m_Planes[i].ToVec4(), Vector4(aabb.minBounds.x, aabb.maxBounds.y, aabb.minBounds.z, 1.0f)) < 0.0) ? 1 : 0);
			out += ((dot(m_Planes[i].ToVec4(), Vector4(aabb.maxBounds.x, aabb.maxBounds.y, aabb.minBounds.z, 1.0f)) < 0.0) ? 1 : 0);
			out += ((dot(m_Planes[i].ToVec4(), Vector4(aabb.minBounds.x, aabb.minBounds.y, aabb.maxBounds.z, 1.0f)) < 0.0) ? 1 : 0);
			out += ((dot(m_Planes[i].ToVec4(), Vector4(aabb.maxBounds.x, aabb.minBounds.y, aabb.maxBounds.z, 1.0f)) < 0.0) ? 1 : 0);
			out += ((dot(m_Planes[i].ToVec4(), Vector4(aabb.minBounds.x, aabb.maxBounds.y, aabb.maxBounds.z, 1.0f)) < 0.0) ? 1 : 0);
			out += ((dot(m_Planes[i].ToVec4(), Vector4(aabb.maxBounds.x, aabb.maxBounds.y, aabb.maxBounds.z, 1.0f)) < 0.0) ? 1 : 0);

			if (out == 8)
				return FrustumCullResult::Outside;
		}

		return FrustumCullResult::Inside;
	}

    FrustumCullResult Frustum::CullOBB(const OBB& obb) const
    {
        Vector3 corner[8] =
        {
            obb.center + (obb.x * obb.halfExtents.x + obb.y * obb.halfExtents.y + obb.z * obb.halfExtents.z),
            obb.center + (obb.x * obb.halfExtents.x + obb.y * obb.halfExtents.y - obb.z * obb.halfExtents.z),
            obb.center + (obb.x * obb.halfExtents.x - obb.y * obb.halfExtents.y + obb.z * obb.halfExtents.z),
            obb.center + (obb.x * obb.halfExtents.x - obb.y * obb.halfExtents.y - obb.z * obb.halfExtents.z),
            obb.center + (-obb.x * obb.halfExtents.x + obb.y * obb.halfExtents.y + obb.z * obb.halfExtents.z),
            obb.center + (-obb.x * obb.halfExtents.x + obb.y * obb.halfExtents.y - obb.z * obb.halfExtents.z),
            obb.center + (-obb.x * obb.halfExtents.x - obb.y * obb.halfExtents.y + obb.z * obb.halfExtents.z),
            obb.center + (-obb.x * obb.halfExtents.x - obb.y * obb.halfExtents.y - obb.z * obb.halfExtents.z),
        };


        for (int i = 0; i < 6; i++)
        {
            int out = 0;
            out += ((dot(m_Planes[i].ToVec4(), Vector4(corner[0], 1.0f)) < 0.0) ? 1 : 0);
            out += ((dot(m_Planes[i].ToVec4(), Vector4(corner[1], 1.0f)) < 0.0) ? 1 : 0);
            out += ((dot(m_Planes[i].ToVec4(), Vector4(corner[2], 1.0f)) < 0.0) ? 1 : 0);
            out += ((dot(m_Planes[i].ToVec4(), Vector4(corner[3], 1.0f)) < 0.0) ? 1 : 0);
            out += ((dot(m_Planes[i].ToVec4(), Vector4(corner[4], 1.0f)) < 0.0) ? 1 : 0);
            out += ((dot(m_Planes[i].ToVec4(), Vector4(corner[5], 1.0f)) < 0.0) ? 1 : 0);
            out += ((dot(m_Planes[i].ToVec4(), Vector4(corner[6], 1.0f)) < 0.0) ? 1 : 0);
            out += ((dot(m_Planes[i].ToVec4(), Vector4(corner[7], 1.0f)) < 0.0) ? 1 : 0);

            if (out == 8)
                return FrustumCullResult::Outside;
        }

        return FrustumCullResult::Inside;
    }


    void Frustum::UpdateFromCamera(Entity camEntity)
    {
        auto transform = camEntity.Get<Transform>();
        auto camera = camEntity.Get<Camera>();
        const auto& position = transform->GetPosition();
        const auto& front = transform->GetForward();
        const auto& left = transform->GetLeft();
        const auto& up = transform->GetUp();

        const float halfVSide = camera->GetFarClip() * tanf(degToRad(camera->GetFov()) * 0.5f);
        const float halfHSide = halfVSide * camera->GetAspect();
        const Vector3 frontMultFar = camera->GetFarClip() * front;

        SetPlane(FrustumSide::FS_Near, Plane(front, position + camera->GetNearClip() * front)); // near
        SetPlane(FrustumSide::FS_Far, Plane(-front, position + frontMultFar)); // far
        SetPlane(FrustumSide::FS_Right, Plane(cross(frontMultFar + left * halfHSide, up), position)); // right
        SetPlane(FrustumSide::FS_Left, Plane(cross(up, frontMultFar - left * halfHSide), position)); // left
        SetPlane(FrustumSide::FS_Bottom, Plane(cross(frontMultFar - up * halfVSide, left), position)); // bottom
        SetPlane(FrustumSide::FS_Top, Plane(cross(left, frontMultFar + up * halfVSide), position)); // top
    }

}