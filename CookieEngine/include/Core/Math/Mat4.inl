#ifndef __MAT4_INL__
#define __MAT4_INL__


namespace Cookie
{
    namespace Core
    {
        namespace Math
        {

            inline void Mat4::Debug() const
            {
                for (int i = 0; i < 4; ++i)
                    c[i].Debug();
            }

            inline void Mat4::DebugAllTest()
            {
                std::cout << "/////////////////////////////////////////////////\n";
                std::cout << "Mat4 : DebugAllTest\n";

                Mat4 temp = Mat4::Identity();

                //Transformations
                {
                    std::cout << "test Identity :\n";
                    temp.Debug();

                    temp = Mat4::Scale(3);
                    std::cout << "test Scale Uniform with 3 :\n";
                    temp.Debug();

                    temp = Mat4::Scale({ 1, 2, 3 });
                    std::cout << "test Scale with {1, 2, 3} :\n";
                    temp.Debug();

                    temp = Mat4::Translate({ 1, 2, 3 });
                    std::cout << "test Translate with {1, 2, 3} :\n";
                    temp.Debug();

                    temp = Mat4::RotateX(90);
                    std::cout << "test RotateX with 90 :\n";
                    temp.Debug();

                    temp = Mat4::RotateY(90);
                    std::cout << "test RotateY with 90 :\n";
                    temp.Debug();

                    temp = Mat4::RotateZ(90);
                    std::cout << "test RotateZ with 90 :\n";
                    temp.Debug();

                    temp = Mat4::TRS(Vec3(10, 20, 30), Vec3(ToRadians(90), ToRadians(180), ToRadians(270)), Vec3(1, 2, 3));
                    std::cout << "test TRS with T(10, 20, 30), R(rad(90), rad(180), rad(270)), S(1, 2, 3) :\n";
                    temp.Debug();

                    temp = Mat4::Perspective(45, 800 / 600, 0.01f, 50.f);
                    std::cout << "test Perspective with Fov = 45, Ratio = 800/600, Near = 0.01, Far = 50 :\n";
                    temp.Debug();
                }

                //Operations
                {
                    temp = Mat4::Identity().Transpose();
                    std::cout << "test Identity Transposed :\n";
                    temp.Debug();


                    Vec4 tempVec4 = Mat4::Translate({ 1, 2, 3 }) * Vec4 { 1, 2, 3, 4 };
                    std::cout << "test Mat4 * Vec4, Mat4 is a translate with {1, 2, 3}, Vec4 is {1, 2, 3, 4} :\n";
                    tempVec4.Debug();

                    temp = Mat4::Translate({ 1, 2, 3 }) * Mat4::Translate({ 1, 2, 3 });
                    std::cout << "test Mat4 * Mat4, both are a translate with {1, 2, 3} :\n";
                    temp.Debug();

                    temp = Mat4::Translate({ 1, 2, 3 });
                    temp *= Mat4::Translate({ 1, 2, 3 });
                    std::cout << "test Mat4 *= Mat4, both are a translate with {1, 2, 3} :\n";
                    temp.Debug();
                }

                std::cout << "/////////////////////////////////////////////////\n";
            }

            inline Mat4 Mat4::Identity()
            {
                return
                { {
                    1.f, 0.f, 0.f, 0.f,
                    0.f, 1.f, 0.f, 0.f,
                    0.f, 0.f, 1.f, 0.f,
                    0.f, 0.f, 0.f, 1.f
                } };
            }
            inline Mat4 Mat4::Scale(float s)
            {
                return
                { {
                     s,  0.f, 0.f, 0.f,
                    0.f,   s, 0.f, 0.f,
                    0.f, 0.f,   s, 0.f,
                    0.f, 0.f, 0.f, 1.f
                } };
            }
            inline Mat4 Mat4::Scale(const Vec3& s)
            {
                return
                { {
                    s.x, 0.f, 0.f, 0.f,
                    0.f, s.y, 0.f, 0.f,
                    0.f, 0.f, s.z, 0.f,
                    0.f, 0.f, 0.f, 1.f
                } };
            }
            inline Mat4 Mat4::Translate(const Vec3& t)
            {
                return
                { {
                    1.f, 0.f, 0.f, t.x,
                    0.f, 1.f, 0.f, t.y,
                    0.f, 0.f, 1.f, t.z,
                    0.f, 0.f, 0.f, 1.f
                } };
            }
            inline Mat4 Mat4::Rotate(const Quat& quat)
            {
                return { {
                        1.0f - (2.0f * quat.y * quat.y) - (2.0f * quat.z * quat.z), 2.0f * quat.x * quat.y - 2.0f * quat.w * quat.z, 2.0f * quat.x * quat.z + 2.0f * quat.w * quat.y, 0.0f,
                        2.0f * quat.x * quat.y + 2.0f * quat.w * quat.z, 1.0f - (2.0f * quat.x * quat.x) - (2.0f * quat.z * quat.z), 2.0f * quat.y * quat.z - 2.0f * quat.w * quat.x, 0.0f,
                        2.0f * quat.x * quat.z - 2.0f * quat.w * quat.y, 2.0f * quat.y * quat.z + 2.0f * quat.w * quat.x, 1.0f - (2.0f * quat.x * quat.x) - (2.0f * quat.y * quat.y), 0.0f,
                        0.0f, 0.0f, 0.0f, 1.0f
                } };
            }
            inline Mat4 Mat4::RotateX(float radians)
            {
                float c = std::cos(radians);
                float s = std::sin(radians);
                return
                { {
                    1.f, 0.f, 0.f, 0.f,
                    0.f,   c,   -s, 0.f,
                    0.f,   s,   c, 0.f,
                    0.f, 0.f, 0.f, 1.f,
                } };
            }
            inline Mat4 Mat4::RotateY(float radians)
            {
                float c = std::cos(radians);
                float s = std::sin(radians);
                return
                { {
                     c, 0.f,  s, 0.f,
                    0.f, 1.f, 0.f, 0.f,
                     -s, 0.f,   c, 0.f,
                    0.f, 0.f, 0.f, 1.f,
                } };
            }
            inline Mat4 Mat4::RotateZ(float radians)
            {
                float c = std::cos(radians);
                float s = std::sin(radians);
                return
                { {
                        c,   -s,  0.f, 0.f,
                        s,   c,  0.f, 0.f,
                        0.f, 0.f, 1.f, 0.f,
                        0.f, 0.f, 0.f, 1.f,
                } };
            }
            inline Mat4 Mat4::TRS(const Vec3& t, const Vec3& r, const Vec3& s)
            {
                return Mat4::Scale(s) * Mat4::RotateZ(r.z) * Mat4::RotateX(r.x) * Mat4::RotateY(r.y) * Mat4::Translate(t);
            }
            inline Mat4 Mat4::Perspective(float yFov, float aspect, float n, float f)
            {
                Mat4 m;
                float const a = 1.f / std::tan(yFov / 2.f);

                m.c[0].e[0] = a / aspect;
                m.c[0].e[1] = 0.f;
                m.c[0].e[2] = 0.f;
                m.c[0].e[3] = 0.f;

                m.c[1].e[0] = 0.f;
                m.c[1].e[1] = a;
                m.c[1].e[2] = 0.f;
                m.c[1].e[3] = 0.f;

                m.c[2].e[0] = 0.f;
                m.c[2].e[1] = 0.f;
                m.c[2].e[2] = -((f + n) / (f - n));
                m.c[2].e[3] = -((2.f * f * n) / (f - n));

                m.c[3].e[0] = 0.f;
                m.c[3].e[1] = 0.f;
                m.c[3].e[2] = -1.f;
                m.c[3].e[3] = 0.f;

                return m;
            }
            inline Mat4 Mat4::Ortho(float left, float right, float bottom, float top, float n, float f)
            {
                Mat4 m;
                float r_min_l = right - left;
                float t_min_b = top - bottom;
                float f_min_n = f - n;

                m.c[0].e[0] = 2.0f/r_min_l;
                m.c[0].e[1] = 0.f;
                m.c[0].e[2] = 0.f;
                m.c[0].e[3] = 0.0f;

                m.c[1].e[0] = 0.f;
                m.c[1].e[1] = 2.0f/t_min_b;
                m.c[1].e[2] = 0.f;
                m.c[1].e[3] = 0.0f;

                m.c[2].e[0] = 0.f;
                m.c[2].e[1] = 0.f;
                m.c[2].e[2] = -(2.0f/f_min_n);
                m.c[2].e[3] = 0.0f;

                m.c[3].e[0] = -(right + left) / (r_min_l);
                m.c[3].e[1] = -(top + bottom) / (t_min_b);
                m.c[3].e[2] = -(f + n) / (f_min_n);
                m.c[3].e[3] = 1.0f;

                return m.Transpose();
            }
            inline Mat4 Mat4::LookAt(const Vec3& eye, const Vec3& center, const Vec3& up)
            {
                Mat4 m;

                Core::Math::Vec3 z = (center - eye).Normalize();
                Core::Math::Vec3 y = up;
                Core::Math::Vec3 x = z.Cross(y);
                x = x.Normalize();
                y = x.Cross(z);
                y = y.Normalize();

                m.c[0] = { x, x.Dot(eye) };
                m.c[1] = { y, y.Dot(eye) };
                m.c[2] = { -z, z.Dot(eye) };
                
                m.c[3] = { 0.0f,0.0f,0.0f,1.0f };

                return m;
            }
            inline Mat4 Mat4::Inverse(const Mat4& _mat)
            {
                float det = _mat.Det();
            
                if (det == 0.0f)
                    return _mat;
            
                Mat4 invMat;
            
                invMat.e[0] = _mat.e[5]  * _mat.e[10] * _mat.e[15] -
                          _mat.e[5]  * _mat.e[11] * _mat.e[14] -
                          _mat.e[9]  * _mat.e[6]  * _mat.e[15] +
                          _mat.e[9]  * _mat.e[7]  * _mat.e[14] +
                          _mat.e[13] * _mat.e[6]  * _mat.e[11] -
                          _mat.e[13] * _mat.e[7]  * _mat.e[10];
            
                invMat.e[4] = -_mat.e[4]  * _mat.e[10] * _mat.e[15] +
                          _mat.e[4]  * _mat.e[11] * _mat.e[14] +
                          _mat.e[8]  * _mat.e[6]  * _mat.e[15] -
                          _mat.e[8]  * _mat.e[7]  * _mat.e[14] -
                          _mat.e[12] * _mat.e[6]  * _mat.e[11] +
                          _mat.e[12] * _mat.e[7]  * _mat.e[10];
            
                invMat.e[8] = _mat.e[4]  * _mat.e[9] * _mat.e[15] -
                         _mat.e[4]  * _mat.e[11] * _mat.e[13] -
                         _mat.e[8]  * _mat.e[5] *  _mat.e[15] +
                         _mat.e[8]  * _mat.e[7] *  _mat.e[13] +
                         _mat.e[12] * _mat.e[5] *  _mat.e[11] -
                         _mat.e[12] * _mat.e[7] *  _mat.e[9];
            
                invMat.e[12] = - _mat.e[4]  * _mat.e[9] * _mat.e[14] +
                           _mat.e[4]  * _mat.e[10] * _mat.e[13] +
                           _mat.e[8]  * _mat.e[5] * _mat.e[14] -
                           _mat.e[8]  * _mat.e[6] * _mat.e[13] -
                           _mat.e[12] * _mat.e[5] * _mat.e[10] +
                           _mat.e[12] * _mat.e[6] * _mat.e[9];
            
                invMat.e[1] = -_mat.e[1]  * _mat.e[10] * _mat.e[15] +
                          _mat.e[1]  * _mat.e[11] * _mat.e[14] +
                          _mat.e[9]  * _mat.e[2] * _mat.e[15] -
                          _mat.e[9]  * _mat.e[3] * _mat.e[14] -
                          _mat.e[13] * _mat.e[2] * _mat.e[11] +
                          _mat.e[13] * _mat.e[3] * _mat.e[10];
            
                invMat.e[5] = _mat.e[0]  * _mat.e[10] * _mat.e[15] -
                         _mat.e[0]  * _mat.e[11] * _mat.e[14] -
                         _mat.e[8]  * _mat.e[2] * _mat.e[15] +
                         _mat.e[8]  * _mat.e[3] * _mat.e[14] +
                         _mat.e[12] * _mat.e[2] * _mat.e[11] -
                         _mat.e[12] * _mat.e[3] * _mat.e[10];
            
                invMat.e[9] = -_mat.e[0]  * _mat.e[9] * _mat.e[15] +
                          _mat.e[0]  * _mat.e[11] * _mat.e[13] +
                          _mat.e[8]  * _mat.e[1] * _mat.e[15] -
                          _mat.e[8]  * _mat.e[3] * _mat.e[13] -
                          _mat.e[12] * _mat.e[1] * _mat.e[11] +
                          _mat.e[12] * _mat.e[3] * _mat.e[9];
            
                invMat.e[13] = _mat.e[0]  * _mat.e[9] * _mat.e[14] -
                          _mat.e[0]  * _mat.e[10] * _mat.e[13] -
                          _mat.e[8]  * _mat.e[1] * _mat.e[14] +
                          _mat.e[8]  * _mat.e[2] * _mat.e[13] +
                          _mat.e[12] * _mat.e[1] * _mat.e[10] -
                          _mat.e[12] * _mat.e[2] * _mat.e[9];
            
                invMat.e[2] = _mat.e[1]  * _mat.e[6] * _mat.e[15] -
                         _mat.e[1]  * _mat.e[7] * _mat.e[14] -
                         _mat.e[5]  * _mat.e[2] * _mat.e[15] +
                         _mat.e[5]  * _mat.e[3] * _mat.e[14] +
                         _mat.e[13] * _mat.e[2] * _mat.e[7] -
                         _mat.e[13] * _mat.e[3] * _mat.e[6];
            
                invMat.e[6] = -_mat.e[0]  * _mat.e[6] * _mat.e[15] +
                          _mat.e[0]  * _mat.e[7] * _mat.e[14] +
                          _mat.e[4]  * _mat.e[2] * _mat.e[15] -
                          _mat.e[4]  * _mat.e[3] * _mat.e[14] -
                          _mat.e[12] * _mat.e[2] * _mat.e[7] +
                          _mat.e[12] * _mat.e[3] * _mat.e[6];
            
                invMat.e[10] = _mat.e[0]  * _mat.e[5] * _mat.e[15] -
                          _mat.e[0]  * _mat.e[7] * _mat.e[13] -
                          _mat.e[4]  * _mat.e[1] * _mat.e[15] +
                          _mat.e[4]  * _mat.e[3] * _mat.e[13] +
                          _mat.e[12] * _mat.e[1] * _mat.e[7] -
                          _mat.e[12] * _mat.e[3] * _mat.e[5];
            
                invMat.e[14] = -_mat.e[0]  * _mat.e[5] * _mat.e[14] +
                           _mat.e[0]  * _mat.e[6] * _mat.e[13] +
                           _mat.e[4]  * _mat.e[1] * _mat.e[14] -
                           _mat.e[4]  * _mat.e[2] * _mat.e[13] -
                           _mat.e[12] * _mat.e[1] * _mat.e[6] +
                           _mat.e[12] * _mat.e[2] * _mat.e[5];
            
                invMat.e[3] = -_mat.e[1] * _mat.e[6] * _mat.e[11] +
                          _mat.e[1] * _mat.e[7] * _mat.e[10] +
                          _mat.e[5] * _mat.e[2] * _mat.e[11] -
                          _mat.e[5] * _mat.e[3] * _mat.e[10] -
                          _mat.e[9] * _mat.e[2] * _mat.e[7] +
                          _mat.e[9] * _mat.e[3] * _mat.e[6];
            
                invMat.e[7] = _mat.e[0] * _mat.e[6] * _mat.e[11] -
                         _mat.e[0] * _mat.e[7] * _mat.e[10] -
                         _mat.e[4] * _mat.e[2] * _mat.e[11] +
                         _mat.e[4] * _mat.e[3] * _mat.e[10] +
                         _mat.e[8] * _mat.e[2] * _mat.e[7] -
                         _mat.e[8] * _mat.e[3] * _mat.e[6];
            
                invMat.e[11] = -_mat.e[0] * _mat.e[5] * _mat.e[11] +
                           _mat.e[0] * _mat.e[7] * _mat.e[9] +
                           _mat.e[4] * _mat.e[1] * _mat.e[11] -
                           _mat.e[4] * _mat.e[3] * _mat.e[9] -
                           _mat.e[8] * _mat.e[1] * _mat.e[7] +
                           _mat.e[8] * _mat.e[3] * _mat.e[5];
            
                invMat.e[15] = _mat.e[0] * _mat.e[5] * _mat.e[10] -
                          _mat.e[0] * _mat.e[6] * _mat.e[9] -
                          _mat.e[4] * _mat.e[1] * _mat.e[10] +
                          _mat.e[4] * _mat.e[2] * _mat.e[9] +
                          _mat.e[8] * _mat.e[1] * _mat.e[6] -
                          _mat.e[8] * _mat.e[2] * _mat.e[5];
            
                det = 1.0f/det;
            
                for (int i = 0; i < 16; i++)
                    invMat.e[i] = invMat.e[i] * det;
            
                return invMat;
            }

            inline Mat4 Mat4::Transpose()const
            {
                return
                { {
                    c[0].e[0], c[1].e[0], c[2].e[0], c[3].e[0],
                    c[0].e[1], c[1].e[1], c[2].e[1], c[3].e[1],
                    c[0].e[2], c[1].e[2], c[2].e[2], c[3].e[2],
                    c[0].e[3], c[1].e[3], c[2].e[3], c[3].e[3],
                } };
            }

            inline Vec3 Mat4::GetEuler()const
            {
                Vec3 euler;
                euler.x = -asinf(c[2].e[1]);//Pitch

                if (cosf(euler.x) > 0.0001)                 // Not at poles
                {
                    euler.y = Core::Math::ToDegrees(atan2f(c[2].e[0], c[2].e[2]));     // Yaw
                    euler.z = Core::Math::ToDegrees(atan2f(c[0].e[1], c[1].e[1]));     // Roll
                }
                else
                {
                    euler.y = 0.0f;                         // Yaw
                    euler.z = Core::Math::ToDegrees(atan2f(-c[1].e[0], c[0].e[0]));    // Roll
                }

                euler.x = Core::Math::ToDegrees(euler.x);

                return euler;
            }

            inline Vec3 Mat4::GetTranslate()const
            {
                return Vec3{c[0].e[3],
                            c[1].e[3],
                            c[2].e[3]};
            }

            inline float Mat4::Det()const
            {
                return    e[0] * (e[5] * ((e[10] * e[15]) - (e[14] * e[11]))
                        - e[6] * ((e[9] * e[15]) - (e[13] * e[11]))
                        + e[7] * ((e[9] * e[14]) - (e[13] * e[10])))
                        - e[1] * (e[4] * ((e[10] * e[15]) - (e[14] * e[11]))
                        - e[6] * ((e[8] * e[15]) - (e[12] * e[11]))
                        + e[7] * ((e[8] * e[14]) - (e[12] * e[10])))
                        + e[2] * (e[4] * ((e[9] * e[15]) - (e[13] * e[11]))
                        - e[5] * ((e[8] * e[15]) - (e[12] * e[11]))
                        + e[7] * ((e[8] * e[13]) - (e[12] * e[9])))
                        - e[3] * (e[4] * ((e[9] * e[14]) - (e[13] * e[10]))
                        - e[5] * ((e[8] * e[14]) - (e[14] * e[10]))
                        + e[6] * ((e[8] * e[13]) - (e[12] * e[9])));
            }

            inline Mat4 Mat4::operator*(const Mat4& other) const
            {
                Mat4 res{};
                for (int c = 0; c < 4; ++c)
                    for (int r = 0; r < 4; ++r)
                        for (int k = 0; k < 4; ++k)
                            res.c[c].e[r] += this->c[k].e[r] * other.c[c].e[k];
                return res;
            }
            inline Vec4 Mat4::operator*(const Vec4& other) const
            {
                Vec4 r;
                r.x = other.x * c[0].e[0] + other.y * c[0].e[1] + other.z * c[0].e[2] + other.w * c[0].e[3];
                r.y = other.x * c[1].e[0] + other.y * c[1].e[1] + other.z * c[1].e[2] + other.w * c[1].e[3];
                r.z = other.x * c[2].e[0] + other.y * c[2].e[1] + other.z * c[2].e[2] + other.w * c[2].e[3];
                r.w = other.x * c[3].e[0] + other.y * c[3].e[1] + other.z * c[3].e[2] + other.w * c[3].e[3];
                return r;
            }
            inline Vec3 Mat4::operator*(const Vec3& other) const
            {
                return (*this * Vec4(other, 1)).ToVec3();
            }
            inline Mat4& Mat4::operator*=(const Mat4& other) { *this = *this * other; return *this; }

        }
    }
}

#endif