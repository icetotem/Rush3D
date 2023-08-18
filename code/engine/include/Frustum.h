#pragma once

#include "Core/MathUtils.h"

namespace rush
{

	enum FrustumSide
	{
		FS_Near,
		FS_Far,
		FS_Right,
		FS_Left,
        FS_Bottom,
        FS_Top,

		FS_Plane_Max
	};

	class Frustum
	{
	public:
		Frustum();
		~Frustum();

		bool CullPoint(const Vector3& point) const;

		bool CullAABB(const AABB& aabb) const;

		bool CullOBB(const OBB& obb) const;

		const Plane& GetPlane(FrustumSide side) { return m_Planes[side]; }

	private:
		Plane m_Planes[FS_Plane_Max];

	};
}