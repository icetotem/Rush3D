#ifndef Frustum_h__
#define Frustum_h__

#include "Core/MathUtils.h"
#include "components/EcsSystem.h"

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


	class Frustum : public Component
	{
	public:
		Frustum(Entity owner);

		bool CullPoint(const Vector3& point) const;

		bool CullAABB(const AABB& aabb) const;

		bool CullOBB(const OBB& obb) const;

		const Plane& GetPlane(FrustumSide side) { return m_Planes[side]; }

		void SetPlane(FrustumSide side, const Plane& plane) { m_Planes[side] = plane; }

    private:
		Plane m_Planes[FS_Plane_Max];
	};
}

#endif // Frustum_h__
