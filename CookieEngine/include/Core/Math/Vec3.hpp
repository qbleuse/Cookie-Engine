#ifndef __VEC3_HPP__
#define __VEC3_HPP__

#include "Vec2.hpp"

namespace Cookie
{
    namespace Core
    {
        namespace Math
        {
            union Vec3
            {
                Vec3() = default;
                Vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
                Vec3(float _xyz[3]) : x(_xyz[0]), y(_xyz[1]), z(_xyz[2]) {}
                Vec3(Vec2 _xy, float _z) : x(_xy.x), y(_xy.y), z(_z) {}
                Vec3(const Vec3& other) : x{ other.x }, y{ other.y }, z{ other.z } {}

                struct { float x; float y; float z; };
                struct { float r; float g; float b; };
                float e[3];


                inline void Debug() const;
                inline static void DebugAllTest();

                inline Vec3 operator-() const;

                inline Vec3 operator+(const Vec3& other) const;
                inline Vec3 operator-(const Vec3& other) const;
                inline Vec3 operator*(const Vec3& other) const;
                inline Vec3 operator/(const Vec3& other) const;

                inline Vec3 operator+(float other) const;
                inline Vec3 operator-(float other) const;
                inline Vec3 operator*(float other) const;
                inline Vec3 operator/(float other) const;

                inline Vec3& operator+=(const Vec3& other);
                inline Vec3& operator-=(const Vec3& other);
                inline Vec3& operator*=(const Vec3& other);
                inline Vec3& operator/=(const Vec3& other);

                inline Vec3& operator+=(float other);
                inline Vec3& operator-=(float other);
                inline Vec3& operator*=(float other);
                inline Vec3& operator/=(float other);

                inline bool operator==(const Vec3& other);
                inline bool operator!=(const Vec3& other);

                inline float Length() const;
                inline Vec3 Normalize() const;
                inline float Dot(const Vec3& other) const;
                inline Vec3 Cross(const Vec3& other) const;
            };


        }
    }
}

#include "Vec3.inl"

#endif