#ifndef __MAT4_HPP__
#define __MAT4_HPP__

#include "Vec4.hpp"
#include "Quat.hpp"
 
namespace Cookie
{
    namespace Core
    {
        namespace Math
        {

            union Mat4
            {
                float e[16];
                Vec4 c[4];

                inline void Debug() const;
                inline static void DebugAllTest();

                inline static Mat4 Identity();
                inline static Mat4 Scale(float s);
                inline static Mat4 Scale(const Vec3& s);
                inline static Mat4 Translate(const Vec3& t);
                inline static Mat4 Rotate(const Quat& quat);
                inline static Mat4 RotateX(float radians);
                inline static Mat4 RotateY(float radians);
                inline static Mat4 RotateZ(float radians);
                inline static Mat4 TRS(const Vec3& t, const Vec3& r, const Vec3& s);
                inline static Mat4 Perspective(float yFov, float aspect, float n, float f);
                inline static Mat4 Ortho(float left, float right, float top, float bottom, float n, float f);
                inline static Mat4 Inverse(const Mat4& _mat);
                inline static Mat4 LookAt(const Vec3& eye, const Vec3& center, const Vec3& up);
                
                inline Vec3 GetEuler()const;
                inline Vec3 GetTranslate()const;
                inline Mat4 Transpose()const;
                inline float Det()const;
                inline Vec4 operator*(const Vec4& other) const;
                inline Vec3 operator*(const Vec3& other) const;
                inline Mat4 operator*(const Mat4& other) const;
                inline Mat4& operator*=(const Mat4& other);
            };

        }
    }
}

#include "Mat4.inl"

#endif
