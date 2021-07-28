#ifndef __CALC_HPP__
#define __CALC_HPP__

#include "Vec4.hpp"

namespace Cookie
{
	namespace Core
	{
		namespace Math
		{
			constexpr float PI = 3.141592f;
			constexpr float TAU = 6.283185f;

			inline float ToRadians(float degrees) { return degrees * (PI / 180.f); }
			inline float ToDegrees(float radians) { return radians * (180.f / PI); }
			inline float Lerp(float a, float b, float f) { return a + f * (b - a); }
			inline Vec4 Random(Vec4& min, Vec4& max)
			{
				return Vec4(float(rand()) / float((RAND_MAX)) * (max.x - min.x) + min.x,
					float(rand()) / float((RAND_MAX)) * (max.y - min.y) + min.y,
					float(rand()) / float((RAND_MAX)) * (max.z - min.z) + min.z,
					float(rand()) / float((RAND_MAX)) * (max.w - min.w) + min.w);
			}
			inline Vec3 Random(Vec3& min, Vec3& max)
			{
				return Vec3(float(rand()) / float((RAND_MAX)) * (max.x - min.x) + min.x,
					float(rand()) / float((RAND_MAX)) * (max.y - min.y) + min.y,
					float(rand()) / float((RAND_MAX)) * (max.z - min.z) + min.z);
			}
		}
	}
}
#endif