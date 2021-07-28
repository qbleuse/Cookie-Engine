#ifndef __QUAT_HPP__
#define __QUAT_HPP__

#include "Vec3.hpp"

namespace Cookie
{
    namespace Core
    {
        namespace Math
        {
            union Quat
            {
                Quat() = default;
                Quat(float _w, float _x, float _y, float _z) : w(_w), x(_x), y(_y), z(_z) {}
                Quat(float _angle, const Vec3& _axis) : angle{ cosf(_angle / 2) }, axis{ _axis.Normalize() * sinf(_angle / 2) } {}
                Quat(const Quat& other) : w{ other.w }, x{ other.x }, y{ other.y }, z{ other.z } {}


                struct { float w; float x; float y; float z; };
                struct { float angle; Vec3 axis; };
                float e[4];

                inline void Debug() const;
                inline static void DebugAllTest();

                inline float Length() const;
                inline Quat Normalize();
                inline Quat Conjugate() const;
                inline Quat Inverse() const;

                inline float Dot(const Quat& other) const;
                inline static Quat Slerp(const Quat& q1, const Quat& q2, float t);
                inline static Quat Nlerp(const Quat& q1, const Quat& q2, float t);
                inline static Vec3 ToEulerAngle(const Quat& q);
                inline static Quat ToQuat(const Vec3& euler);
                inline Vec3 RotateVector(const Vec3& vec) const;

                inline Quat operator-() const;
                inline Quat operator+(const Quat& other) const;
                inline Quat operator-(const Quat& other) const;
                inline Quat operator*(const Quat& other) const;
                inline Quat operator+(float other) const;
                inline Quat operator-(float other) const;
                inline Quat operator*(float other) const;
                inline Quat operator/(float other) const;
                inline Vec3 operator*(const Vec3& other) const;
            };

        }
    }
}

#include "Quat.inl"

#endif