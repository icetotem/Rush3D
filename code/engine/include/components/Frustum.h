#ifndef Frustum_h__
#define Frustum_h__

#include "Core/MathUtils.h"
#include "components/EcsSystem.h"

namespace rush
{

	enum class FrustumSide
	{
		FS_Near,
		FS_Far,
		FS_Right,
		FS_Left,
        FS_Bottom,
        FS_Top,
		Count
	};

	enum class FrustumCullResult
	{
        Outside,
        Inside
	};

	class Frustum : public Component
	{
	public:
		Frustum(Entity owner);

		FrustumCullResult CullPoint(const Vector3& point) const;

		FrustumCullResult CullAABB(const AABB& aabb) const;

		FrustumCullResult CullOBB(const OBB& obb) const;

		const Plane& GetPlane(FrustumSide side) { return m_Planes[(int)side]; }

		void SetPlane(FrustumSide side, const Plane& plane) { m_Planes[(int)side] = plane; }

    private:
		Plane m_Planes[(int)FrustumSide::Count];
	};
}

#endif // Frustum_h__
