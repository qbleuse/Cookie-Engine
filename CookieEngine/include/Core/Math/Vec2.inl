#ifndef __VEC2_INL__
#define __VEC2_INL__

namespace Cookie
{
	namespace Core
	{
		namespace Math
		{

			inline void Vec2::Debug() const { std::cout << "{" << x << " ," << y << "}\n"; }

			inline void Vec2::DebugAllTest()
			{
				std::cout << "/////////////////////////////////////////////////\n";
				std::cout << "Vec2 : DebugAllTest\n";

				Vec2 temp{ {1, 1} };
				std::cout << "display initial values, expected {1, 1} get : ";
				temp.Debug();

				temp = -temp;
				std::cout << "test operator-, expected : {-1, -1} get : ";
				temp.Debug();

				{
					temp = temp + Vec2{ {1, 1} };
					std::cout << "test operator+ with {1, 1}, expected : {0, 0} get : ";
					temp.Debug();

					temp = temp - Vec2{ {1, 1} };
					std::cout << "test operator- with {1, 1}, expected : {-1, -1} get : ";
					temp.Debug();

					temp = temp * Vec2{ {2, 2} };
					std::cout << "test operator* with {2, 2}, expected : {-2, -2} get : ";
					temp.Debug();

					temp = temp / Vec2{ {2, 2} };
					std::cout << "test operator/ with {2, 2}, expected : {-1, -1} get : ";
					temp.Debug();
				}

				{
					temp = temp + 1;
					std::cout << "test operator+ with 1, expected : {0, 0} get : ";
					temp.Debug();

					temp = temp - 1;
					std::cout << "test operator- with 1, expected : {-1, -1} get : ";
					temp.Debug();

					temp = temp * 2;
					std::cout << "test operator* with 2, expected : {-2, -2} get : ";
					temp.Debug();

					temp = temp / 2;
					std::cout << "test operator/ with 2, expected : {-1, -1} get : ";
					temp.Debug();
				}

				{
					temp += Vec2{ {1, 1} };
					std::cout << "test operator+= with {1, 1}, expected : {0, 0} get : ";
					temp.Debug();

					temp -= Vec2{ {1, 1 } };
					std::cout << "test operator-= with {1, 1}, expected : {-1, -1} get : ";
					temp.Debug();

					temp *= Vec2{ {2, 2} };
					std::cout << "test operator*= with {2, 2}, expected : {-2, -2} get : ";
					temp.Debug();

					temp /= Vec2{ {2, 2 } };
					std::cout << "test operator/= with {2, 2}, expected : {-1, -1} get : ";
					temp.Debug();
				}

				{
					temp += 1;
					std::cout << "test operator+= with 1, expected : {0, 0} get : ";
					temp.Debug();

					temp -= 1;
					std::cout << "test operator-= with 1, expected : {-1, -1} get : ";
					temp.Debug();

					temp *= 2;
					std::cout << "test operator*= with 2, expected : {-2, -2} get : ";
					temp.Debug();

					temp /= 2;
					std::cout << "test operator/= with 2, expected : {-1, -1} get : ";
					temp.Debug();
				}

				std::cout << "/////////////////////////////////////////////////\n";
			}

			inline Vec2 Vec2::operator-() const { return { {-x, -y} }; }

			inline Vec2 Vec2::operator+(const Vec2& other) const { return { {x + other.x, y + other.y} }; }
			inline Vec2 Vec2::operator-(const Vec2& other) const { return { {x - other.x, y - other.y} }; }
			inline Vec2 Vec2::operator*(const Vec2& other) const { return { {x * other.x, y * other.y} }; }
			inline Vec2 Vec2::operator/(const Vec2& other) const { return { {x / other.x, y / other.y} }; }

			inline Vec2  Vec2::operator+(float other) const { return{ {x + other, y + other} }; }
			inline Vec2  Vec2::operator-(float other) const { return{ {x - other, y - other} }; }
			inline Vec2  Vec2::operator*(float other) const { return{ {x * other, y * other} }; }
			inline Vec2  Vec2::operator/(float other) const { return{ {x / other, y / other} }; }

			inline Vec2& Vec2::operator+=(const Vec2& other) { *this = *this + other; return *this; }
			inline Vec2& Vec2::operator-=(const Vec2& other) { *this = *this - other; return *this; }
			inline Vec2& Vec2::operator*=(const Vec2& other) { *this = *this * other; return *this; }
			inline Vec2& Vec2::operator/=(const Vec2& other) { *this = *this / other; return *this; }

			inline Vec2& Vec2::operator+=(float other) { *this = *this + other; return *this; }
			inline Vec2& Vec2::operator-=(float other) { *this = *this - other; return *this; }
			inline Vec2& Vec2::operator*=(float other) { *this = *this * other; return *this; }
			inline Vec2& Vec2::operator/=(float other) { *this = *this / other; return *this; }

			inline float Vec2::Length() const { return std::sqrt(x * x + y * y); }
		}
	}
}

#endif