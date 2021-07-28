#ifndef __VEC3_INL__
#define __VEC3_INL__

#include <cmath>

namespace Cookie
{
	namespace Core
	{
		namespace Math
		{
			constexpr float EPSILON = 1 / 4060;

			inline void Vec3::Debug() const
			{ 
				std::cout << "{" << x << " ," << y << " ," << z << "}\n";
			}
			inline void Vec3::DebugAllTest()
			{
				std::cout << "/////////////////////////////////////////////////\n";
				std::cout << "Vec3 : DebugAllTest\n";

				Vec3 temp{ 1, 1, 1 };
				std::cout << "display initial values, expected {1, 1, 1} get : ";
				temp.Debug();



				{
					temp = -temp;
					std::cout << "test operator-, expected : {-1, -1; -1} get : ";
					temp.Debug();

					temp = temp + Vec3{ 1, 1, 1 };
					std::cout << "test operator+ with {1, 1, 1}, expected : {0, 0, 0} get : ";
					temp.Debug();

					temp = temp - Vec3{ 1, 1, 1 };
					std::cout << "test operator- with {1, 1, 1}, expected : {-1, -1, -1} get : ";
					temp.Debug();

					temp = temp * Vec3{ 2, 2, 2 };
					std::cout << "test operator* with {2, 2}, expected : {-2, -2, -2} get : ";
					temp.Debug();

					temp = temp / Vec3{ 2, 2, 2 };
					std::cout << "test operator/ with {2, 2}, expected : {-1, -1, -1} get : ";
					temp.Debug();
				}

				{
					temp = temp + 1;
					std::cout << "test operator+ with 1, expected : {0, 0, 0} get : ";
					temp.Debug();

					temp = temp - 1;
					std::cout << "test operator- with 1, expected : {-1, -1, -1} get : ";
					temp.Debug();

					temp = temp * 2;
					std::cout << "test operator* with 2, expected : {-2, -2, -2} get : ";
					temp.Debug();

					temp = temp / 2;
					std::cout << "test operator/ with 2, expected : {-1, -1, -1} get : ";
					temp.Debug();
				}

				{
					temp += Vec3{ 1, 1, 1 };
					std::cout << "test operator+= with {1, 1, 1}, expected : {0, 0, 0} get : ";
					temp.Debug();

					temp -= Vec3{ 1, 1, 1 };
					std::cout << "test operator-= with {1, 1, 1}, expected : {-1, -1, -1} get : ";
					temp.Debug();

					temp *= Vec3{ 2, 2, 2 };
					std::cout << "test operator*= with {2, 2, 2}, expected : {-2, -2, -2} get : ";
					temp.Debug();

					temp /= Vec3{ 2, 2, 2 };
					std::cout << "test operator/= with {2, 2, 2}, expected : {-1, -1, -1} get : ";
					temp.Debug();
				}

				{
					temp += 1;
					std::cout << "test operator+= with 1, expected : {0, 0, 0} get : ";
					temp.Debug();

					temp -= 1;
					std::cout << "test operator-= with 1, expected : {-1, -1, -1} get : ";
					temp.Debug();

					temp *= 2;
					std::cout << "test operator*= with 2, expected : {-2, -2, -2} get : ";
					temp.Debug();

					temp /= 2;
					std::cout << "test operator/= with 2, expected : {-1, -1, -1} get : ";
					temp.Debug();
				}

				{
					std::cout << "test Length with {-1, -1, -1}, expected : ~1.73 get : " << temp.Length() << "\n";

					std::cout << "test Normalize with {-1, -1, -1}, expected : ~{-0.58, -0.58; -0.58} get : ";
					(temp.Normalize()).Debug();

					std::cout << "test Dot with {-1, -1, -1} and {1, 2, 3}, expected : -6 get : " << temp.Dot(Vec3{ 1, 2, 3 }) << "\n";

					std::cout << "test Cross with {-1, -1, -1} and {1, 2, 3}, expected : {-1, 2, -1} get : ";
					(temp.Cross(Vec3{ 1, 2, 3 })).Debug();

				}


				std::cout << "/////////////////////////////////////////////////\n";
			}


			inline Vec3 Vec3::operator-() const { return { -x, -y, -z }; }

			inline Vec3 Vec3::operator+(const Vec3& other) const { return { x + other.x, y + other.y, z + other.z }; }
			inline Vec3 Vec3::operator-(const Vec3& other) const { return { x - other.x, y - other.y, z - other.z }; }
			inline Vec3 Vec3::operator*(const Vec3& other) const { return { x * other.x, y * other.y, z * other.z }; }
			inline Vec3 Vec3::operator/(const Vec3& other) const { return { x / other.x, y / other.y, z / other.z }; }

			inline Vec3  Vec3::operator+(float other) const { return{ x + other, y + other, z + other }; }
			inline Vec3  Vec3::operator-(float other) const { return{ x - other, y - other, z - other }; }
			inline Vec3  Vec3::operator*(float other) const { return{ x * other, y * other, z * other }; }
			inline Vec3  Vec3::operator/(float other) const { return{ x / other, y / other, z / other }; }

			inline Vec3& Vec3::operator+=(const Vec3& other) { *this = *this + other; return *this; }
			inline Vec3& Vec3::operator-=(const Vec3& other) { *this = *this - other; return *this; }
			inline Vec3& Vec3::operator*=(const Vec3& other) { *this = *this * other; return *this; }
			inline Vec3& Vec3::operator/=(const Vec3& other) { *this = *this / other; return *this; }

			inline Vec3& Vec3::operator+=(float other) { *this = *this + other; return *this; }
			inline Vec3& Vec3::operator-=(float other) { *this = *this - other; return *this; }
			inline Vec3& Vec3::operator*=(float other) { *this = *this * other; return *this; }
			inline Vec3& Vec3::operator/=(float other) { *this = *this / other; return *this; }

			inline bool Vec3::operator==(const Vec3& other) 
			{ 
				return this->x - other.x >= -Math::EPSILON && this->x - other.x <= Math::EPSILON
					&& this->y - other.y >= -Math::EPSILON && this->y - other.y <= Math::EPSILON
					&& this->z - other.z >= -Math::EPSILON && this->z - other.z <= Math::EPSILON;
			}
			inline bool Vec3::operator!=(const Vec3& other)
			{
				return this->x - other.x < -Math::EPSILON || this->x - other.x > Math::EPSILON
					|| this->y - other.y < -Math::EPSILON || this->y - other.y > Math::EPSILON
					|| this->z - other.z < -Math::EPSILON || this->z - other.z > Math::EPSILON;
			}

			inline float Vec3::Length() const { return std::sqrt(x * x + y * y + z * z); }
			inline Vec3 Vec3::Normalize() const { return *this / Length(); }

			inline float Vec3::Dot(const Vec3& other) const { return (x * other.x + y * other.y + z * other.z); }
			inline Vec3 Vec3::Cross(const Vec3& other) const {
				return { y * other.z - z * other.y,
						z * other.x - x * other.z,
						x * other.y - y * other.x };
			}


		}
	}
}

#endif