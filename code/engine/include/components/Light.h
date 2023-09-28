#ifndef Light_h__
#define Light_h__

#include "core/Core.h"
#include "components/EcsSystem.h"

namespace rush
{
    
    enum LightType
    {
        LT_Directional,
        LT_Point,
        LT_Spot,
    };
    
    class Light : public Component
    {
    public:
        Light(Entity owner);

        void SetType(LightType type) { m_Type = type; }
        LightType GetType() const { return m_Type; }

        void SetColor(const Vector3& color) { m_Color = color; }
        const Vector3& GetColor() const { return m_Color; }

        void SetIntensity(float intensity) { m_Intensity = intensity; }
        float GetIntensity() const { return m_Intensity; }

        void SetRadius(float radius);
        float GetRaidus() const { return m_Radius; }

        void SetSpotAngle(float angle) { m_SpotAngle = glm::clamp(angle, 0.0f, 80.0f); }
        float GetSpotAngle() const { return m_SpotAngle; }

        void UpdateBoundings();
        const AABB& GetAABB() const { return m_AABB; }

        void SetCastShadow(bool castShadow) { m_CastShadow = castShadow; }
        bool GetCastShadow() const { return m_CastShadow; }

    protected:
        LightType m_Type = LightType::LT_Point;
        float m_Radius = 1.0f;
        float m_SpotAngle = 45.0f;
        Vector3 m_Color = Vector3(1.0f, 1.0f, 1.0f);
        float m_Intensity = 3.14f;
        AABB m_AABB;
        bool m_CastShadow = true;
    };

}


#endif // Light_h__
