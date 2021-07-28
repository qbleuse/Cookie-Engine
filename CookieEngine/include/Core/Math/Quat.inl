#ifndef __QUAT_INL__
#define __QUAT_INL__

#include "Calc.hpp"
#include <cmath>
#include <assert.h>

namespace Cookie
{
	namespace Core
	{
		namespace Math
		{

			inline void Quat::Debug() const { std::cout << "{" << w << ", {" << x << ", " << y << ", " << z << "} }\n"; }
			inline void Quat::DebugAllTest()
			{
				std::cout << "/////////////////////////////////////////////////\n";
				std::cout << "Quat : DebugAllTest\n";

				std::cout << "display initial values,  angle : Pi/2, axis : {1, 2, 3}, expected {0.70, {0.19, 0.38, 0.57} } get : ";
				Quat{ ToRadians(90), {1, 2, 3} }.Debug();

				{
					std::cout << "test operator- with {1, 1, 1, 1}, expected : {-1, {-1, -1, -1}} get : ";
					(-Quat{ 1, 1, 1, 1 }).Debug();

					std::cout << "test operator+ with {1, 1, 1, 1} + {1, 2, 3, 4}, expected : {2, {3, 4, 5}} get : ";
					(Quat{ 1, 1, 1, 1 } + Quat{ 1, 2, 3, 4 }).Debug();

					std::cout << "test operator- with {1, 1, 1, 1} - {1, 2, 3, 4}, expected : {0, {-1, -2, -3}} get : ";
					(Quat{ 1, 1, 1, 1 } - Quat{ 1, 2, 3, 4 }).Debug();

					std::cout << "test operator* with {1, 1, 1, 1} * {1, 2, 3, 4}, expected : {-8, {4, 2, 6} get : ";
					(Quat{ 1, 1, 1, 1 } *Quat{ 1, 2, 3, 4 }).Debug();
				}

				{
					std::cout << "test operator* with {1, 1, 1, 1} + 2, expected : {3, {3, 3, 3}} get : ";
					(Quat{ 1, 1, 1, 1 } + 2).Debug();

					std::cout << "test operator/ with {1, 1, 1, 1}  - 2, expected : {-1, {-1, -1, -1}} get : ";
					(Quat{ 1, 1, 1, 1 } - 2).Debug();

					std::cout << "test operator* with {1, 1, 1, 1}  * 2, expected : {2, {2, 2, 2}} get : ";
					(Quat{ 1, 1, 1, 1 } *2).Debug();

					std::cout << "test operator/ with {1, 1, 1, 1}  /2, expected : {0.5, {0.5, 0.5, 0.5}} get : ";
					(Quat{ 1, 1, 1, 1 } / 2).Debug();
				}

				{
					std::cout << "test Length with {1, 1, 1, 1}, expected : 2 get : " << Quat{ 1, 1, 1, 1 }.Length() << "\n";

					std::cout << "test Normalize with {1, 1, 1, 1}, expected : {0.5, {0.5, 0.5, 0.5}} get : ";
					(Quat{ 1, 1, 1, 1 }.Normalize()).Debug();

					std::cout << "test Conjugate with {1, 1, 1, 1}, expected : {1, {-1, -1, -1}} get : ";
					(Quat{ 1, 1, 1, 1 }.Conjugate()).Debug();

					std::cout << "test Inverse with {1, 1, 1, 1}, expected : {0.25, {-0.25, -0.25, -0.25}} get : ";
					(Quat{ 1, 1, 1, 1 }.Inverse()).Debug();


				}

				{

					std::cout << "test Dot with {1, 1, 1, 1} and {1, 2, 3, 4}, expected : 10 get : " << Quat{ 1, 1, 1, 1 }.Dot(Quat{ 1, 2, 3, 4 }) << "\n";

					Quat q1Unit = Quat{ 1, 1, 1, 1 }.Normalize();
					Quat q2Unit = Quat{ 1, 2, 3, 4 }.Normalize();
					q2Unit.Debug();
					std::cout << "test Slerp (need to be unit quat) with {0.5, 0.5, 0.5, 0.5}, {0.18, 0.37, 0.54, 0.73} and t = 0, expected : {0.5, {0.5, 0.5, 0.5}} get : ";
					Slerp(q1Unit, q2Unit, 0).Debug();
					std::cout << "test Slerp (need to be unit quat) with {0.5, 0.5, 0.5, 0.5}, {1, 2, 3, 4} and t = 0.5, expected : {0.35, {0.44, 0.54, 0.63}} get : ";
					Slerp(q1Unit, q2Unit, 0.5).Debug();
					std::cout << "test Slerp (need to be unit quat) with {0.5, 0.5, 0.5, 0.5}, {1, 2, 3, 4} and t = 1, expected : {0.18, {0.37, 0.54, 0.73}} get : ";
					Slerp(q1Unit, q2Unit, 1).Debug();

					std::cout << "test Nlerp with {1, 1, 1, 1}, {1, 2, 3, 4} and t = 0, expected : {1, {1, 1, 1}} get : ";
					(Nlerp(Quat{ 1, 1, 1, 1 }, Quat{ 1, 2, 3, 4 }, 0)).Debug();
					std::cout << "test Nlerp with {1, 1, 1, 1}, {1, 2, 3, 4} and t = 0.5, expected : {1, {1.5, 2, 2.5}} get : ";
					(Nlerp(Quat{ 1, 1, 1, 1 }, Quat{ 1, 2, 3, 4 }, 0.5)).Debug();
					std::cout << "test Nlerp with {1, 1, 1, 1}, {1, 2, 3, 4} and t = 1, expected : {1, {2, 3, 4}} get : ";
					(Nlerp(Quat{ 1, 1, 1, 1 }, Quat{ 1, 2, 3, 4 }, 1)).Debug();


					std::cout << "test RotateVector with {90, {1, 0, 10}} and {0, 1, 0}, expected : {0, 0, 1} get : ";
					Quat{ ToRadians(90), {1, 0, 0} }.RotateVector(Vec3{ 0, 1, 0 }).Debug();
				}

				std::cout << "/////////////////////////////////////////////////\n";

			}


			inline float Quat::Length() const { return sqrt(w * w + x * x + y * y + z * z); }
			inline Quat Quat::Normalize() { return *this / Length(); }

			inline Quat Quat::Conjugate() const { return { w , -x, -y, -z }; }
			inline Quat Quat::Inverse() const { float length = Length(); return Conjugate() / (length * length); }

			inline float Quat::Dot(const Quat& other) const { return (w * other.w + x * other.x + y * other.y + z * other.z); }
			inline Quat Quat::Slerp(const Quat& q1, const Quat& q2, float t)
			{
				float theta = acos(q1.Dot(q2));
				return  (q1 * sin((1 - t) * theta) + q2 * sin(t * theta))
					/ sin(theta);
			}
			inline Quat Quat::Nlerp(const Quat& q1, const Quat& q2, float t) { return  q1 * (1 - t) + q2 * t; }
			Vec3 Quat::RotateVector(const Vec3& vec) const
			{
				return   vec * (2 * w * w - 1)
					+ axis * 2 * (axis.Dot(vec))
					+ axis.Cross(vec) * 2 * w;
			}
			inline Vec3 Quat::ToEulerAngle(const Quat& q)
			{
				Vec3 angles{0.0f,0.0f,0.0f};

				// roll (x-axis rotation)
				float sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
				float cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
				angles.x = ToDegrees(std::atan2(sinr_cosp, cosr_cosp));

				// pitch (y-axis rotation)
				double sinp = 2 * (q.w * q.y - q.z * q.x);
				//if (std::abs(sinp) >= 1)
				//	angles.y = ToDegrees(std::copysign(PI / 2, sinp)); // use 90 degrees if out of range
				//else
					angles.y = ToDegrees(std::asin(sinp));

				// yaw (z-axis rotation)
				double siny_cosp = 2 * (q.w * q.z + q.x * q.y);
				double cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
				angles.z = ToDegrees(std::atan2(siny_cosp, cosy_cosp));

				return angles;
			}
			inline Quat Quat::ToQuat(const Vec3& euler)
			{
				Vec3 angles = euler;
				angles.x = ToRadians(euler.x);
				angles.y = ToRadians(euler.y);
				angles.z = ToRadians(euler.z);

				float cy = cos(angles.z * 0.5);
				float sy = sin(angles.z * 0.5);
				float cp = cos(angles.y * 0.5);
				float sp = sin(angles.y * 0.5);
				float cr = cos(angles.x * 0.5);
				float sr = sin(angles.x * 0.5);

				Quat q;
				q.w = cr * cp * cy + sr * sp * sy;
				q.x = sr * cp * cy - cr * sp * sy;
				q.y = cr * sp * cy + sr * cp * sy;
				q.z = cr * cp * sy - sr * sp * cy;

				return q;
			}


			inline Quat Quat::operator-() const { return { -w , -axis.x, -axis.y, -axis.z }; }
			inline Quat Quat::operator+(const Quat& other) const { return { w + other.w, x + other.x, y + other.y, z + other.z }; }
			inline Quat Quat::operator-(const Quat& other) const { return { w - other.w, x - other.x, y - other.y, z - other.z }; }
			inline Quat Quat::operator*(const Quat& other) const
			{
				Vec3 newAxis = axis * other.w + other.axis * w + axis.Cross(other.axis);
				return { w * other.w - axis.Dot(other.axis), newAxis.x, newAxis.y, newAxis.z };
			}

			inline Quat Quat::operator+(float other) const { return { w + other, x + other, y + other, z + other }; }
			inline Quat Quat::operator-(float other) const { return { w - other, x - other, y - other, z - other }; }
			inline Quat Quat::operator*(float other) const { return { w * other, x * other, y * other, z * other }; }
			inline Quat Quat::operator/(float other) const { assert(other); return { w / other, x / other, y / other, z / other }; }


			inline Vec3 Quat::operator*(const Vec3& vec) const
			{
				return   vec * (2 * w * w - 1)
					+ axis * 2 * (axis.Dot(vec))
					+ axis.Cross(vec) * 2 * w;
			}

		}
	}
}

#endif