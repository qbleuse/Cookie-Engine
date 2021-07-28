#ifndef __VEC2_HPP__
#define __VEC2_HPP__

#include <iostream>

namespace Cookie
{
    namespace Core
    {
        namespace Math
        {

            union Vec2
            {
                struct { float x; float y; };
                struct { float u; float v; };
                float e[2];


                inline void Debug() const;
                inline static void DebugAllTest();

                inline Vec2 operator-() const;

                inline Vec2 operator+(const Vec2& other) const;
                inline Vec2 operator-(const Vec2& other) const;
                inline Vec2 operator*(const Vec2& other) const;
                inline Vec2 operator/(const Vec2& other) const;

                inline Vec2 operator+(float other) const;
                inline Vec2 operator-(float other) const;
                inline Vec2 operator*(float other) const;
                inline Vec2 operator/(float other) const;

                inline Vec2& operator+=(const Vec2& other);
                inline Vec2& operator-=(const Vec2& other);
                inline Vec2& operator*=(const Vec2& other);
                inline Vec2& operator/=(const Vec2& other);

                inline Vec2& operator+=(float other);
                inline Vec2& operator-=(float other);
                inline Vec2& operator*=(float other);
                inline Vec2& operator/=(float other);
            
                inline float Length() const;
            };


        }
    }
}

#include "Vec2.inl"

#endif