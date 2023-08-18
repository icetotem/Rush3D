#pragma once

#include "Core/Common.h"
#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/functions.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/matrix_decompose.hpp"
#include "glm/gtx/string_cast.hpp"
#include "glm/gtx/color_space.hpp"
#include "glm/gtx/color_encoding.hpp"
#include "glm/gtc/random.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/matrix.hpp"
#include "glm/gtx/matrix_operation.inl"
#include "glm/gtc/type_ptr.hpp"


// constants
#ifndef  PI
#define PI 3.14159265358979323846f
#endif

#ifndef INV_PI
#define INV_PI  (0.31830988618f)
#endif

#ifndef HALF_PI
#define HALF_PI	(1.57079632679f)
#endif

#define SMALL_NUMBER (1.e-8f)
#define KINDA_SMALL_NUMBER (1.e-4f)

#define USE_POSITIVE_PLANE_SIGN

#define MATH_RANDOM_MINUS1_1()      ((2.0f*((float)rand()/RAND_MAX))-1.0f)      // Returns a random float between -1 and 1.
#define MATH_RANDOM_0_1()           ((float)rand()/RAND_MAX)                    // Returns a random float between 0 and 1.

namespace rush
{

    typedef glm::vec2 Vector2;
    typedef glm::vec3 Vector3;
    typedef glm::vec4 Vector4;
    typedef glm::ivec2 IVector2;
    typedef glm::ivec3 IVector3;
    typedef glm::ivec4 IVector4;
    typedef glm::mat3x3 Matrix3;
    typedef glm::mat4x4 Matrix4;
    typedef glm::mat3x4 Matrix34;
    typedef glm::mat4x3 Matrix43;
    typedef glm::quat Quat;

    static const Vector3 xAxis = Vector3(1.0f, 0.0f, 0.0f);
    static const Vector3 yAxis = Vector3(0.0f, 1.0f, 0.0f);
    static const Vector3 zAxis = Vector3(0.0f, 0.0f, 1.0f);

    static const float piMulTwo = 6.283185307179586476925f;        //!< pi*2 constant
    static const float piDivTwo = 1.570796326794896619231f;        //!< pi/2 constant
    static const float kEpsilon = 0.000001F;

    inline Vector4 colorConv(uint32_t rgba)
    {
        uint8_t r = uint8_t(rgba >> 24);
        uint8_t g = uint8_t((rgba & 0x00FF0000) >> 16);
        uint8_t b = uint8_t((rgba & 0x0000FF00) >> 8);
        uint8_t a = uint8_t(rgba & 0x000000FF);
        const float inv = 1.0f / 255.0f;
        return Vector4(r * inv, g * inv, b * inv, a * inv);
    }

    inline uint32_t colorConvRgba(const Vector4& color)
    {
        uint32_t r = (uint32_t)(color.r * 255) << 24;
        uint32_t g = (uint32_t)(color.g * 255) << 16;
        uint32_t b = (uint32_t)(color.b * 255) << 8;
        uint32_t a = (uint32_t)(color.a * 255);
        return r | g | b | a;
    }

    inline uint32_t colorConvBgra(const Vector4& color)
    {
        uint32_t b = (uint32_t)(color.b * 255) << 24;
        uint32_t g = (uint32_t)(color.g * 255) << 16;
        uint32_t r = (uint32_t)(color.r * 255) << 8;
        uint32_t a = (uint32_t)(color.a * 255);
        return b | g | r | a;
    }

    inline uint32_t colorConvAbgr(const Vector4& color)
    {
        uint32_t a = (uint32_t)(color.a * 255) << 24;
        uint32_t b = (uint32_t)(color.b * 255) << 16;
        uint32_t g = (uint32_t)(color.g * 255) << 8;
        uint32_t r = (uint32_t)(color.r * 255);
        return a | b | g | r;
    }

    // Computes a/b, rounded up
    // To be used for positive a and b and small numbers (beware of overflows)
    inline int idivceil(int a, int b)
    {
        return (a + b - 1) / b;
    }

    inline float degToRad(float degrees) { return (degrees * PI / 180.0f); }
    inline float radToDeg(float radians) { return (radians * 180.0f / PI); }

    inline float* toFloatPtr(Vector2& v) { return reinterpret_cast<float*>(&v); } //  2 floats - default alignment
    inline float* toFloatPtr(Vector3& v) { return reinterpret_cast<float*>(&v); } //  4 floats - 16 bytes aligned
    inline float* toFloatPtr(Vector4& v) { return reinterpret_cast<float*>(&v); } //  4 floats - 16 bytes aligned
    inline float* toFloatPtr(Quat& q) { return reinterpret_cast<float*>(&q); } //  4 floats - 16 bytes aligned
    inline float* toFloatPtr(Matrix3& m) { return reinterpret_cast<float*>(&m); } // 12 floats - 16 bytes aligned
    inline float* toFloatPtr(Matrix34& m) { return reinterpret_cast<float*>(&m); } // 12 floats - 16 bytes aligned
    inline float* toFloatPtr(Matrix4& m) { return reinterpret_cast<float*>(&m); } // 16 floats - 16 bytes aligned

    inline const float* toFloatPtr(const Vector2& v) { return reinterpret_cast<const float*>(&v); }
    inline const float* toFloatPtr(const Vector3& v) { return reinterpret_cast<const float*>(&v); }
    inline const float* toFloatPtr(const Vector4& v) { return reinterpret_cast<const float*>(&v); }
    inline const float* toFloatPtr(const Quat& q) { return reinterpret_cast<const float*>(&q); }
    inline const float* toFloatPtr(const Matrix3& m) { return reinterpret_cast<const float*>(&m); }
    inline const float* toFloatPtr(const Matrix34& m) { return reinterpret_cast<const float*>(&m); }
    inline const float* toFloatPtr(const Matrix4& m) { return reinterpret_cast<const float*>(&m); }

    inline float* toFloatPtr(Vector2* v) { return reinterpret_cast<float*>(v); } //  2 floats - default alignment
    inline float* toFloatPtr(Vector3* v) { return reinterpret_cast<float*>(v); } //  4 floats - 16 bytes aligned
    inline float* toFloatPtr(Vector4* v) { return reinterpret_cast<float*>(v); } //  4 floats - 16 bytes aligned
    inline float* toFloatPtr(Quat* q) { return reinterpret_cast<float*>(q); } //  4 floats - 16 bytes aligned
    inline float* toFloatPtr(Matrix3* m) { return reinterpret_cast<float*>(m); } // 12 floats - 16 bytes aligned
    inline float* toFloatPtr(Matrix34* m) { return reinterpret_cast<float*>(m); } // 12 floats - 16 bytes aligned
    inline float* toFloatPtr(Matrix4* m) { return reinterpret_cast<float*>(m); } // 16 floats - 16 bytes aligned

    inline const float* toFloatPtr(const Vector2* v) { return reinterpret_cast<const float*>(v); }
    inline const float* toFloatPtr(const Vector3* v) { return reinterpret_cast<const float*>(v); }
    inline const float* toFloatPtr(const Vector4* v) { return reinterpret_cast<const float*>(v); }
    inline const float* toFloatPtr(const Quat* q) { return reinterpret_cast<const float*>(q); }
    inline const float* toFloatPtr(const Matrix3* m) { return reinterpret_cast<const float*>(m); }
    inline const float* toFloatPtr(const Matrix34* m) { return reinterpret_cast<const float*>(m); }
    inline const float* toFloatPtr(const Matrix4* m) { return reinterpret_cast<const float*>(m); }

    inline float angleBetween(const Vector3& u, const Vector3& v)
    {
        float AngleCos = dot(normalize(u), normalize(v));
        AngleCos = glm::clamp(AngleCos, -1.0f, 1.0f);
        return radToDeg(acosf(AngleCos));
    }

    inline void orthoNormalize(Vector3& Normal, Vector3& Tangent)
    {
        Normal = normalize(Normal);
        Tangent = Tangent - (Normal * dot(Tangent, Normal));
        Tangent = normalize(Tangent);
    }

    inline bool isEqualUsingDot(float dot)
    {
        // Returns false in the presence of NaN values.
        return dot > 1.0f - kEpsilon;
    }

    inline float clampAxis(float Angle)
    {
        // returns Angle in the range (-360,360)
        Angle = fmod(Angle, 360.f);

        if (Angle < 0.f)
        {
            // shift to [0,360) range
            Angle += 360.f;
        }

        return Angle;
    }

    inline float normalizeAxis(float Angle)
    {
        // returns Angle in the range [0,360)
        Angle = clampAxis(Angle);

        if (Angle > 180.f)
        {
            // shift to (-180,180]
            Angle -= 360.f;
        }

        return Angle;
    }

#define FASTASIN_HALF_PI (1.5707963050f)
    inline float FastAsin(float Value)
    {
        // Clamp input to [-1,1].
        bool nonnegative = (Value >= 0.0f);
        float x = fabsf(Value);
        float omx = 1.0f - x;
        if (omx < 0.0f)
        {
            omx = 0.0f;
        }
        float root = sqrtf(omx);
        // 7-degree minimax approximation
        float result = ((((((-0.0012624911f * x + 0.0066700901f) * x - 0.0170881256f) * x + 0.0308918810f) * x - 0.0501743046f) * x + 0.0889789874f) * x - 0.2145988016f) * x + FASTASIN_HALF_PI;
        result *= root;  // acos(|x|)
        // acos(x) = pi - acos(-x) when x < 0, asin(x) = pi/2 - acos(x)
        return (nonnegative ? FASTASIN_HALF_PI - result : result - FASTASIN_HALF_PI);
    }
#undef FASTASIN_HALF_PI

    inline void FastSinCos(float* sinValue, float* cosValue, float value)
    {
        // Map Value to y in [-pi,pi], x = 2*pi*quotient + remainder.
        float quotient = (INV_PI * 0.5f) * value;
        if (value >= 0.0f)
        {
            quotient = (float)((int)(quotient + 0.5f));
        }
        else
        {
            quotient = (float)((int)(quotient - 0.5f));
        }
        float y = value - (2.0f * PI) * quotient;

        // Map y to [-pi/2,pi/2] with sin(y) = sin(Value).
        float sign;
        if (y > HALF_PI)
        {
            y = PI - y;
            sign = -1.0f;
        }
        else if (y < -HALF_PI)
        {
            y = -PI - y;
            sign = -1.0f;
        }
        else
        {
            sign = +1.0f;
        }

        float y2 = y * y;

        // 11-degree minimax approximation
        *sinValue = (((((-2.3889859e-08f * y2 + 2.7525562e-06f) * y2 - 0.00019840874f) * y2 + 0.0083333310f) * y2 - 0.16666667f) * y2 + 1.0f) * y;

        // 10-degree minimax approximation
        float p = ((((-2.6051615e-07f * y2 + 2.4760495e-05f) * y2 - 0.0013888378f) * y2 + 0.041666638f) * y2 - 0.5f) * y2 + 1.0f;
        *cosValue = sign * p;
    }

    // roll-pitch-yaw, z-x-y
    inline const Vector3 quatToEuler(const Quat& q)
    {
        Vector3 angles;

        // roll (z-axis rotation)
        float sinr_cosp = 2.0f * (q.w * q.z + q.x * q.y);
        float cosr_cosp = 1.0f - 2.0f * (q.x * q.x + q.z * q.z);
        angles.z = (std::atan2(sinr_cosp, cosr_cosp));

        // pitch (x-axis rotation)
        float sinp = 2.0f * (q.w * q.x - q.z * q.y);
        if (std::abs(sinp) >= 1.0f)
            angles.x = (std::copysign(PI / 2.0f, sinp)); // use 90 degrees if out of range
        else
            angles.x = (std::asin(sinp));

        // yaw (y-axis rotation)
        float siny_cosp = 2.0f * (q.w * q.y + q.x * q.z);
        float cosy_cosp = 1.0f - 2.0f * (q.y * q.y + q.x * q.x);
        angles.y = (std::atan2(siny_cosp, cosy_cosp));

        const float RAD_TO_DEG = (180.f) / PI;

        return angles * RAD_TO_DEG;
    }

    // roll-pitch-yaw, z-x-y
    inline const Quat eularToQuat(const Vector3& e)
    {
        const float DEG_TO_RAD = PI / (180.f);

        float roll = DEG_TO_RAD * e.z;
        float pitch = DEG_TO_RAD * e.x;
        float yaw = DEG_TO_RAD * e.y;

        // Abbreviations for the various angular functions
        float cy = cos(yaw * 0.5f);
        float sy = sin(yaw * 0.5f);
        float cp = cos(pitch * 0.5f);
        float sp = sin(pitch * 0.5f);
        float cr = cos(roll * 0.5f);
        float sr = sin(roll * 0.5f);

        Quat q;
        q.x = (cr * sp * cy + sr * cp * sy);
        q.y = (cr * cp * sy - sr * sp * cy);
        q.z = (sr * cp * cy - cr * sp * sy);
        q.w = (cr * cp * cy + sr * sp * sy);

        return q;
    }

    inline bool IsNearlyEqual(float A, float B)
    {
        return fabs(A - B) <= SMALL_NUMBER;
    }

    /// <summary>
    /// Ray
    /// </summary>
    class Ray
    {
    public:
        Ray()
        {
            origin = Vector3(0.0f);
            direction = Vector3(0.0f);
        }

        Ray(const Vector3& argsOrigin, const Vector3& argsDirection)
        {
            origin = argsOrigin;
            direction = normalize(argsDirection);
        }

        inline void Transform(const Matrix4& mat)
        {
            Vector4 temp = mat * Vector4(origin, 1.0f);
            origin = temp / temp.w;
            direction = mat * Vector4(direction, 0.0f);
            direction = glm::normalize(direction);
        }

        inline Vector3 Eval(float t) const
        {
            return origin + (direction * t);
        }

        Vector3 origin;
        Vector3 direction;
    };

    /// <summary>
    /// Plane
    /// </summary>
    class Plane
    {
    public:
        Plane()
        {
            normal = Vector3(0, 0, 0);
            distance = 0.0f;
        }

        Plane(const Vector4& vec4)
        {
            normal = Vector3(vec4.x, vec4.y, vec4.z);
            distance = -vec4.w;
        }

        Plane(const Vector3& argsNormal, float argsDistance)
        {
            normal = normalize(argsNormal);
            distance = argsDistance;
        }

        Plane(const Vector3& argsNormal, const Vector3& argsPoint)
        {
            normal = normalize(argsNormal);
            distance = dot(normal, argsPoint);
        }

        Plane(const Vector3& v0, const Vector3& v1, const Vector3& v2)
        {
            normal = normalize(cross(normalize(v1 - v0), normalize(v2 - v0)));
            distance = dot(normal, v0);
        }

        Vector4 ToVec4() const
        {
            return Vector4(normal, -distance);
        }

        Vector3 normal;
        float distance;
    };

    /// <summary>
    /// AABB
    /// </summary>
    struct AABB
    {	
    public:
        Vector3 minBounds, maxBounds;

    public:
        AABB()
        {
            minBounds = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
            maxBounds = Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
        }

        AABB(const Vector3& argsMinBounds, const Vector3& argsMaxBounds)
        {
            minBounds = argsMinBounds;
            maxBounds = argsMaxBounds;
        }

        void Merge(const AABB& other)
        {
            minBounds = glm::min(minBounds, other.minBounds);
            maxBounds = glm::max(maxBounds, other.maxBounds);
        }

        void Merge(const Vector3& point)
        {
            minBounds = glm::min(minBounds, point);
            maxBounds = glm::max(maxBounds, point);
        }

        inline void InitFromVerts(const Vector3* verts, int num)
        {
            minBounds = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
            maxBounds = Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

            for (int i = 0; i < num; ++i)
            {
                const Vector3& vert = verts[i];

                minBounds = min(minBounds, vert);
                maxBounds = max(maxBounds, vert);
            }
        }

        static AABB Transform(const AABB& aabb, const Matrix4& matrix)
        {
            AABB result;

            Vector3 newCorners[8];

            newCorners[0] = Vector3(aabb.minBounds.x, aabb.maxBounds.y, aabb.maxBounds.z);
            newCorners[1] = Vector3(aabb.minBounds.x, aabb.minBounds.y, aabb.maxBounds.z);
            newCorners[2] = Vector3(aabb.maxBounds.x, aabb.minBounds.y, aabb.maxBounds.z);
            newCorners[3] = Vector3(aabb.maxBounds.x, aabb.maxBounds.y, aabb.maxBounds.z);
            newCorners[4] = Vector3(aabb.maxBounds.x, aabb.maxBounds.y, aabb.minBounds.z);
            newCorners[5] = Vector3(aabb.maxBounds.x, aabb.minBounds.y, aabb.minBounds.z);
            newCorners[6] = Vector3(aabb.minBounds.x, aabb.minBounds.y, aabb.minBounds.z);
            newCorners[7] = Vector3(aabb.minBounds.x, aabb.maxBounds.y, aabb.minBounds.z);

            result.minBounds = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
            result.maxBounds = Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
            
            for (int i = 0; i < 8; ++i)
            {
                newCorners[i] = Vector3((matrix * Vector4(newCorners[i], 1.0f)));

                result.minBounds = min(result.minBounds, newCorners[i]);
                result.maxBounds = max(result.maxBounds, newCorners[i]);
            }

            return result;
        }

    };

    /// <summary>
    /// OBB
    /// </summary>
    struct OBB
    {
        OBB()
        {
            center = Vector3(0.0f);
            halfExtents = Vector3(0.0f);
            x = xAxis;
            y = yAxis;
            z = zAxis;
        }

        OBB(const AABB& aabb)
        {
            center = (aabb.minBounds + aabb.maxBounds) * 0.5f;
            halfExtents = (aabb.maxBounds - aabb.minBounds) * 0.5f;
            x = xAxis;
            y = yAxis;
            z = zAxis;
        }

        static OBB Transform(const OBB& obb, const Matrix4& mat)
        {
            OBB result;
            result.halfExtents = obb.halfExtents;

            float xAxisLength, yAxisLength, zAxisLength;
            result.center = (mat * Vector4(obb.center, 1.0f));
            result.x = mat * Vector4(obb.halfExtents.x * obb.x, 0.0f);
            result.y = mat * Vector4(obb.halfExtents.y * obb.y, 0.0f);
            result.z = mat * Vector4(obb.halfExtents.z * obb.z, 0.0f);

            xAxisLength = length(result.x);
            yAxisLength = length(result.y);
            zAxisLength = length(result.z);

            result.x /= xAxisLength;
            result.y /= yAxisLength;
            result.z /= zAxisLength;

            result.halfExtents.x = xAxisLength;
            result.halfExtents.y = yAxisLength;
            result.halfExtents.z = zAxisLength;

            return result;
        }

        Vector3 center;
        Vector3 halfExtents; // half of each axis length
        Vector3 x, y, z; // axises
    };



    inline bool rayPlaneDistance(const Ray& ray, const Plane& plane, float* distance)
    {
#ifndef USE_POSITIVE_PLANE_SIGN
        float denom = dot(plane.normal, ray.direction);
        if (abs(denom) > 1e-4f)
        {
            *(distance) = (-plane.distance - dot(plane.normal, ray.origin)) / denom;;
            return true;
        }

        if (denom != 0.f)
        {
            *(distance) = (-plane.distance - dot(plane.normal, ray.origin)) / denom;
            if (abs(*distance) < 1e4f)
                return true;
        }

        *(distance) = 0.f;
        return abs(dot(plane.normal, ray.origin) + plane.distance) < 1e-3f;
#else
        float denom = dot(plane.normal, ray.direction);
        if (abs(denom) > 1e-4f)
        {
            *(distance) = (plane.distance - dot(plane.normal, ray.origin)) / denom;;
            return true;
        }

        if (denom != 0.f)
        {
            *(distance) = (plane.distance - dot(plane.normal, ray.origin)) / denom;
            if (abs(*distance) < 1e4f)
                return true;
        }

        *(distance) = 0.f;
        return abs(dot(plane.normal, ray.origin) - plane.distance) < 1e-3f;
#endif
    }

    inline bool rayIntersectsPlane(const Ray& ray, const Plane& plane, Vector3* hitPoint)
    {
#ifndef USE_POSITIVE_PLANE_SIGN
        float t;
        bool hit = rayPlaneDistance(ray, plane, &t);
        *(hitPoint) = ray.origin + ray.direction * -t;
        return hit && t >= 0.0f;
#else	
        float t;
        bool hit = rayPlaneDistance(ray, plane, &t);
        *(hitPoint) = ray.origin + ray.direction * t;
        return hit && t >= 0.0f;
#endif
    }

    inline Matrix3 matrixFromXY(Vector3 const& XAxis, Vector3 const& YAxis)
    {
        Vector3 NewX = normalize(XAxis);
        Vector3 Norm = normalize(YAxis);

        // if they're almost same, we need to find arbitrary vector
        if (IsNearlyEqual(fabs(dot(NewX, Norm)), 1.f))
        {
            // make sure we don't ever pick the same as NewX
            Norm = (fabs(NewX.z) < (1.f - KINDA_SMALL_NUMBER)) ? Vector3(0, 0, 1.f) : Vector3(1.f, 0, 0);
        }

        const Vector3 NewZ = normalize(cross(NewX, Norm));
        const Vector3 NewY = cross(NewZ, NewX);

        return Matrix3(NewX, NewY, NewZ);
    }

    inline Matrix3 matrixFromXZ(Vector3 const& XAxis, Vector3 const& ZAxis)
    {
        Vector3 const NewX = normalize(XAxis);
        Vector3 Norm = normalize(ZAxis);

        // if they're almost same, we need to find arbitrary vector
        if (IsNearlyEqual(fabs(dot(NewX, Norm)), 1.f))
        {
            // make sure we don't ever pick the same as NewX
            Norm = (fabs(NewX.z) < (1.f - KINDA_SMALL_NUMBER)) ? Vector3(0, 0, 1.f) : Vector3(1.f, 0, 0);
        }

        const Vector3 NewY = normalize(cross(Norm, NewX));
        const Vector3 NewZ = cross(NewX, NewY);

        return Matrix3(NewX, NewY, NewZ);
    }

    inline Matrix3 matrixFromYZ(Vector3 const& YAxis, Vector3 const& ZAxis)
    {
        Vector3 const NewY = normalize(YAxis);;
        Vector3 Norm = normalize(ZAxis);

        // if they're almost same, we need to find arbitrary vector
        if (IsNearlyEqual(fabs(dot(NewY, Norm)), 1.f))
        {
            // make sure we don't ever pick the same as NewX
            Norm = (fabs(NewY.z) < (1.f - KINDA_SMALL_NUMBER)) ? Vector3(0, 0, 1.f) : Vector3(1.f, 0, 0);
        }

        const Vector3 NewX = normalize(cross(NewY, Norm));;
        const Vector3 NewZ = cross(NewX, NewY);

        return Matrix3(NewX, NewY, NewZ);
    }

    inline Quat quatFromXY(Vector3 const& XAxis, Vector3 const& YAxis)
    {
        return Quat(matrixFromXY(XAxis, YAxis));
    }

    inline Quat quatFromYZ(Vector3 const& YAxis, Vector3 const& ZAxis)
    {
        return Quat(matrixFromYZ(YAxis, ZAxis));
    }

    inline Quat quatFromXZ(Vector3 const& XAxis, Vector3 const& ZAxis)
    {
        return Quat(matrixFromXZ(XAxis, ZAxis));
    }

    inline const Matrix4 matrix4FromTRS(const Vector3& position, const Quat& rotation, const Vector3& scale)
    {
        return glm::translate(position) * glm::toMat4(rotation) * glm::scale(scale);
    }

    inline void matrix4Decompose(const Matrix4& m, Vector3& translate, Quat& rotation, Vector3& scale)
    {
        Vector3 skew;
        Vector4 pers;
        glm::decompose(m, scale, rotation, translate, skew, pers);
    }

    inline Quat lookRotation(const Vector3& forward, const Vector3& up)
    {
        return quatFromYZ(normalize(up), normalize(forward));
    }

    inline Vector3 getSafeScaleReciprocal(const Vector3& InScale)
    {
        Vector3 SafeReciprocalScale;
        if (std::fabsf(InScale.x) <= SMALL_NUMBER)
        {
            SafeReciprocalScale.x = (0);
        }
        else
        {
            SafeReciprocalScale.x = (1.0f / InScale.x);
        }

        if (std::fabsf(InScale.y) <= SMALL_NUMBER)
        {
            SafeReciprocalScale.y = (0);
        }
        else
        {
            SafeReciprocalScale.y = (1.0f / InScale.y);
        }

        if (std::fabsf(InScale.z) <= SMALL_NUMBER)
        {
            SafeReciprocalScale.z = (0);
        }
        else
        {
            SafeReciprocalScale.z = (1.0f / InScale.z);
        }

        return SafeReciprocalScale;
    }

    //Returns true if the two input quaternions are close to each other. This can
    //be used to check whether or not one of two quaternions which are supposed to
    //be very similar but has its component signs reversed (q has the same rotation as
    //-q)
    inline bool areQuaternionsClose(Quat q1, Quat q2)
    {
        float dot = q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;
        if (dot < 0.0f)
            return false;
        else
            return true;
    }

    inline Quat averageTwoQuatertions(Vector4& cumulative, Quat newRotation, Quat firstRotation, int addAmount)
    {
        float w = 0.0f;
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;

        //Before we add the new rotation to the average (mean), we have to check whether the quaternion has to be inverted. Because
        //q and -q are the same rotation, but cannot be averaged, we have to make sure they are all the same.
        if (!areQuaternionsClose(newRotation, firstRotation))
        {
            newRotation = -(newRotation);
        }

        //Average the values
        float addDet = 1.f / (float)addAmount;
        cumulative.w = (cumulative.w + newRotation.w);
        w = cumulative.w * addDet;
        cumulative.x = (cumulative.x + newRotation.x);
        x = cumulative.x * addDet;
        cumulative.y = (cumulative.y + newRotation.y);
        y = cumulative.y * addDet;
        cumulative.z = (cumulative.z + newRotation.z);
        z = cumulative.z * addDet;

        //note: if speed is an issue, you can skip the normalization step
        return normalize(Quat(x, y, z, w));
    }

    inline Quat averageQuaternions(const std::vector<Quat>& quatArray)
    {
        if (quatArray.size() == 0)
            return glm::identity<Quat>();

        Vector4 c(0);
        Quat r;
        for (int i = 0; i < quatArray.size(); ++i)
        {
            r = averageTwoQuatertions(c, quatArray[i], quatArray[0], i + 1);
        }
        return r;
    }

    inline Vector3 averageVectors(const std::vector<Vector3>& vecArray)
    {
        if (vecArray.size() == 0)
            return Vector3(0);

        Vector3 result(0);
        for (auto p : vecArray)
        {
            result += p;
        }
        result /= (float)vecArray.size();
        return result;
    }

    inline const char* toString(const Vector2& value)
    {
        const int BUFFER_SIZE = 4096;
        static char buffer[BUFFER_SIZE];
        static int index = 0;

        char valueStr[100];
        sprintf(valueStr, "{%.3f, %.3f}", (float)value.x, (float)value.y);
        int len = strlen(valueStr);

        int startIndex = index;
        if (BUFFER_SIZE - index > len + 1)
        {
            memcpy(buffer + index, valueStr, len + 1);
            index += len + 1;
        }
        else
        {
            index = 0;
        }

        return (const char*)(buffer + startIndex);
    }

    inline const char* toString(const Vector3& value)
    {
        const int BUFFER_SIZE = 4096;
        static char buffer[BUFFER_SIZE];
        static int index = 0;

        char valueStr[100];
        sprintf(valueStr, "{%.3f, %.3f, %.3f}", (float)value.x, (float)value.y, (float)value.z);
        int len = strlen(valueStr);

        int startIndex = index;
        if (BUFFER_SIZE - index > len + 1)
        {
            memcpy(buffer + index, valueStr, len + 1);
            index += len + 1;
        }
        else
        {
            index = 0;
        }

        return (const char*)(buffer + startIndex);
    }

    inline const char* toString(const Vector4& value)
    {
        const int BUFFER_SIZE = 4096;
        static char buffer[BUFFER_SIZE];
        static int index = 0;

        char valueStr[100];
        sprintf(valueStr, "{%.3f, %.3f, %.3f, %.3f}", (float)value.x, (float)value.y, (float)value.z, (float)value.w);
        int len = strlen(valueStr);

        int startIndex = index;
        if (BUFFER_SIZE - index > len + 1)
        {
            memcpy(buffer + index, valueStr, len + 1);
            index += len + 1;
        }
        else
        {
            index = 0;
        }

        return (const char*)(buffer + startIndex);
    }

    inline const char* toString(const IVector2& value)
    {
        const int BUFFER_SIZE = 4096;
        static char buffer[BUFFER_SIZE];
        static int index = 0;

        char valueStr[100];
        sprintf(valueStr, "{%d, %d}", (int)value.x, (int)value.y);
        int len = strlen(valueStr);

        int startIndex = index;
        if (BUFFER_SIZE - index > len + 1)
        {
            memcpy(buffer + index, valueStr, len + 1);
            index += len + 1;
        }
        else
        {
            index = 0;
        }

        return (const char*)(buffer + startIndex);
    }

    inline const char* toString(const IVector3& value)
    {
        const int BUFFER_SIZE = 4096;
        static char buffer[BUFFER_SIZE];
        static int index = 0;

        char valueStr[100];
        sprintf(valueStr, "{%d, %d, %d|", (int)value.x, (int)value.y, (int)value.z);
        int len = strlen(valueStr);

        int startIndex = index;
        if (BUFFER_SIZE - index > len + 1)
        {
            memcpy(buffer + index, valueStr, len + 1);
            index += len + 1;
        }
        else
        {
            index = 0;
        }

        return (const char*)(buffer + startIndex);
    }

    inline const char* toString(const IVector4& value)
    {
        const int BUFFER_SIZE = 4096;
        static char buffer[BUFFER_SIZE];
        static int index = 0;

        char valueStr[100];
        sprintf(valueStr, "{%d, %d, %d, %d}", (int)value.x, (int)value.y, (int)value.z, (int)value.w);
        int len = strlen(valueStr);

        int startIndex = index;
        if (BUFFER_SIZE - index > len + 1)
        {
            memcpy(buffer + index, valueStr, len + 1);
            index += len + 1;
        }
        else
        {
            index = 0;
        }

        return (const char*)(buffer + startIndex);
    }

    inline std::string toString(const Quat& value)
    {
        return glm::to_string(value);
    }


    ////////////////////////////pixel format/////////////////////////////
    inline uint32_t toUnorm(float _value, float _scale)
    {
        return uint32_t(glm::round(glm::clamp(_value, 0.0f, 1.0f) * _scale));
    }

    inline float fromUnorm(uint32_t _value, float _scale)
    {
        return float(_value) / _scale;
    }

    inline void packRgba8(void* _dst, const float* _src)
    {
        uint8_t* dst = (uint8_t*)_dst;
        dst[0] = uint8_t(toUnorm(_src[0], 255.0f));
        dst[1] = uint8_t(toUnorm(_src[1], 255.0f));
        dst[2] = uint8_t(toUnorm(_src[2], 255.0f));
        dst[3] = uint8_t(toUnorm(_src[3], 255.0f));
    }

    inline void unpackRgba8(float* _dst, const void* _src)
    {
        const uint8_t* src = (const uint8_t*)_src;
        _dst[0] = fromUnorm(src[0], 255.0f);
        _dst[1] = fromUnorm(src[1], 255.0f);
        _dst[2] = fromUnorm(src[2], 255.0f);
        _dst[3] = fromUnorm(src[3], 255.0f);
    }

    inline Vector3 encodeNormalUint(const Vector3& _normal)
    {
        return _normal * 0.5f + 0.5f;
    }

    inline Vector3 decodeNormalUint(const Vector3& _encodedNormal)
    {
        return _encodedNormal * 2.0f - 1.0f;
    }

    inline Vector2 encodeNormalSphereMap(const Vector3& _normal)
    {
        return glm::normalize(Vector2(_normal.x, _normal.y)) * sqrtf(_normal.z * 0.5f + 0.5f);
    }

    inline Vector3 decodeNormalSphereMap(const Vector2& _encodedNormal)
    {
        float zz = glm::dot(_encodedNormal, _encodedNormal) * 2.0f - 1.0f;
        Vector2 temp = glm::normalize(Vector2(_encodedNormal.x, _encodedNormal.y));
        return Vector3(temp * sqrtf(1.0f - zz * zz), zz);
    }


}