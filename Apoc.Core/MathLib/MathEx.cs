using System;
using System.Drawing;

namespace Apoc.MathLib
{
    public struct PolarCoord
    {
        public float len;
        public float sita; //在屏幕坐标系

        public PolarCoord(float l, float s)
        {
            len = l;
            sita = s;
        }
    }

    public static class MathEx
    {
        #region 常量

        public const float PIf = (float)Math.PI;
        public const float Root2 = 1.4142135623730950488016887242097f;
        public const float Root3 = 1.7320508075688772935274463415059f;
        public const float PiSquare = (float)(Math.PI * Math.PI);
        public const float PiOver2 = 1.570796f;
        public const float PiOver4 = 0.7853982f;
        public const float GravityAcceleration = 9.80665f;

        #endregion

        public static float Sqr(float d)
        {
            return d * d;
        }
        public static double Sqr(double d)
        {
            return d * d;
        }

        public static int Sqr(int d)
        {
            return d * d;
        }
        public static long Sqr(long d)
        {
            return d * d;
        }


        public static bool IsFloatNormal(float f)
        {
            return !(float.IsInfinity(f) || float.IsNaN(f));
        }

        #region 向量

        public static float Vec3Dot(ref Vector3 a, ref Vector3 b)
        {
            return a.X * b.X + a.Y * b.Y + a.Z * b.Z;
        }
        public static Vector3 Vec3Mul(Vector3 a, Vector3 b)
        {
            return new Vector3(a.X * b.X, a.Y * b.Y, a.Z * b.Z);
        }
        public static Vector3 Vec3Mul(ref Vector3 a, ref Vector3 b)
        {
            return new Vector3(a.X * b.X, a.Y * b.Y, a.Z * b.Z);
        }
        public static Vector3 Vec3Div(Vector3 vector3, Vector3 aabbSize)
        {
            return new Vector3(vector3.X / aabbSize.X, vector3.Y / aabbSize.Y, vector3.Z / aabbSize.Z);
        }
        public static float Vec3Cross(Vector3 a, Vector3 b)
        {
            return (float)(a.Length() * b.Length() * Math.Sin(Vec3AngleAbs(a, b)));
        }
        public static float Vec3AngleAbs(Vector3 v1, Vector3 v2)
        {
            if (v1.Length() < 0.01f || (v2.Length() < 0.01f))
            {
                return 0f;
            }
            float a = Vector3.Dot(v1, v2) / (v1.Length() * v2.Length());
            a = MathEx.Clamp(-1f, 1f, a);
            return (float)Math.Acos(a);
        }

        public static float Vec2Cross(Vector2 a, Vector2 b)
        {
            return (float)(a.Length() * b.Length() * Math.Sin(Vec2AngleAbs(a, b)));
        }
        public static float Vec2AngleAbs(Vector2 v1, Vector2 v2)
        {
            return (float)Math.Acos(Vector2.Dot(v1, v2) / (v1.Length() * v2.Length()));
        }
        public static Vector2 GetRotateVector2(Vector2 v1, float sita)
        {
            float len = v1.Length();
            v1.Normalize();
            return len * new Vector2(
                (float)(v1.X * Math.Cos(sita) - v1.Y * Math.Sin(sita)),
                (float)(v1.X * Math.Sin(sita) + v1.Y * Math.Cos(sita)));
        }

        public static float Vector2DirAngle(Vector2 v1)
        {
            //v1.Normalize();
            if (v1.X < 0)
            {
                return -(float)Math.Acos(v1.Y);
            }
            return (float)Math.Acos(v1.Y) - MathEx.PIf * 2;
        }
        public static PolarCoord VectorToPolar(Vector2 v1)
        {
            PolarCoord pc = new PolarCoord();
            pc.len = v1.Length();
            ////pc.sita = (float)Math.Acos(v1.X / v1.Length());
            ////if (v1.Y < 0)
            ////{
            ////    pc.sita = (float)Math.PI * 2 - pc.sita;
            ////}
            //pc.sita = (float)Math.Atan2(v1.Y, v1.X);
            //return pc;
            v1.Normalize();

            if (v1.X < 0)
            {
                pc.sita = -(float)Math.Acos(v1.Y);
            }
            else
            {
                pc.sita = (float)Math.Acos(v1.Y) - MathEx.PIf * 2;
            }
            return pc;
        }

        #endregion

        #region 距离

        public static float DistanceSquared(ref Vector3 a, ref Vector3 b)
        {
            return Sqr(a.X - b.X) + Sqr(a.Y - b.Y) + Sqr(a.Z - b.Z);
        }

        public static float Distance(ref Vector3 a, ref Vector3 b)
        {
            return (float)Math.Sqrt(Sqr(a.X - b.X) + Sqr(a.Y - b.Y) + Sqr(a.Z - b.Z));
        }

        public static int ManhattanDis(Point p1, Point p2)
        {
            return Math.Abs(p1.X - p2.X) + Math.Abs(p1.Y - p2.Y);
        }

        public static float ManhattanDis(Vector2 p1, Vector2 p2)
        {
            return Math.Abs(p1.X - p2.X) + Math.Abs(p1.Y - p2.Y);
        }

        #endregion

        public static Vector3 GetEulerAngles(ref Quaternion q)
        {
            float r11, r21, r31, r32, r33, r12, r13;
            float w2, x2, y2, z2;
            float tmp;

            w2 = q.W * q.W;
            x2 = q.X * q.X;
            y2 = q.Y * q.Y;
            z2 = q.Z * q.Z;

            r11 = w2 + x2 - y2 - z2;
            r21 = 2 * (q.X * q.Y + q.W * q.Z);
            r31 = 2 * (q.X * q.Z - q.W * q.Y);
            r32 = 2 * (q.Y * q.Z + q.W * q.X);
            r33 = w2 - x2 - y2 + z2;

            tmp = Math.Abs(r31);
            if (tmp > 0.999999)
            {
                r12 = 2 * (q.X * q.Y - q.W * q.Z);
                r13 = 2 * (q.X * q.Z + q.W * q.Y);

                return new Vector3(0, -(MathEx.PIf * 0.5f) * r31 / tmp, (float)Math.Atan2(-r12, -r31 * r13));
            }
            else
            {
                return new Vector3((float)Math.Atan2(r32, r33), (float)Math.Asin(-r31), (float)Math.Atan2(r21, r11));
            }

        }
        public static void GetEulerAngles(ref Quaternion q, out float yaw, out float pitch, out float roll)
        {
            float r11, r21, r31, r32, r33, r12, r13;
            float w2, x2, y2, z2;
            float tmp;

            w2 = q.W * q.W;
            x2 = q.X * q.X;
            y2 = q.Y * q.Y;
            z2 = q.Z * q.Z;

            r11 = w2 + x2 - y2 - z2;
            r21 = 2 * (q.X * q.Y + q.W * q.Z);
            r31 = 2 * (q.X * q.Z - q.W * q.Y);
            r32 = 2 * (q.Y * q.Z + q.W * q.X);
            r33 = w2 - x2 - y2 + z2;

            tmp = Math.Abs(r31);
            if (tmp > 0.999999)
            {
                r12 = 2 * (q.X * q.Y - q.W * q.Z);
                r13 = 2 * (q.X * q.Z + q.W * q.Y);

                yaw = 0;
                pitch = -(MathEx.PIf / 2) * r31 / tmp;
                roll = (float)Math.Atan2(-r12, -r31 * r13);
                //return new Vector3(0, -(Math.PI / 2) * r31 / tmp, Math.Atan2(-r12, -r31 * r13));
            }
            else
            {
                yaw = (float)Math.Atan2(r32, r33);
                pitch = (float)Math.Asin(-r31);
                roll = (float)Math.Atan2(r21, r11);
                //return new Vector3(Math.Atan2(r32, r33), Math.Asin(-r31), Math.Atan2(r21, r11));
            }
        }

        public static Vector3 GetMatrixFront(ref Matrix m) 
        {
            return new Vector3(m.M31, m.M32, m.M33);
        }
        public static Vector3 GetMatrixUp(ref Matrix m) 
        {
            return new Vector3(m.M21, m.M22, m.M23);

        }
        public static Vector3 GetMatrixRight(ref Matrix m) 
        {
            return new Vector3(m.M11, m.M12, m.M13);

        }

        #region 四元数

        public static Quaternion QuaternionFromEulerAngles(Vector3 ea)
        {
            float cyaw, cpitch, croll, syaw, spitch, sroll;
            float cyawcpitch, syawspitch, cyawspitch, syawcpitch;

            cyaw = (float)Math.Cos(0.5f * ea.Z);
            cpitch = (float)Math.Cos(0.5f * ea.Y);
            croll = (float)Math.Cos(0.5f * ea.X);
            syaw = (float)Math.Sin(0.5f * ea.Z);
            spitch = (float)Math.Sin(0.5f * ea.Y);
            sroll = (float)Math.Sin(0.5f * ea.X);

            cyawcpitch = cyaw * cpitch;
            syawspitch = syaw * spitch;
            cyawspitch = cyaw * spitch;
            syawcpitch = syaw * cpitch;

            Quaternion q;
            q.W = (cyawcpitch * croll + syawspitch * sroll);
            q.X = (cyawcpitch * sroll - syawspitch * croll);
            q.Y = (cyawspitch * croll + syawcpitch * sroll);
            q.Z = (syawcpitch * croll - cyawspitch * sroll);
            return q;
        }
        public static Quaternion CreateFromRotationMatrix(Matrix matrix)
        {
            float num8 = (matrix.M11 + matrix.M22) + matrix.M33;
            Quaternion quaternion = new Quaternion();
            if (num8 > 0f)
            {
                float num = (float)Math.Sqrt((double)(num8 + 1f));
                quaternion.W = num * 0.5f;
                num = 0.5f / num;
                quaternion.X = (matrix.M23 - matrix.M32) * num;
                quaternion.Y = (matrix.M31 - matrix.M13) * num;
                quaternion.Z = (matrix.M12 - matrix.M21) * num;
                return quaternion;
            }
            if ((matrix.M11 >= matrix.M22) && (matrix.M11 >= matrix.M33))
            {
                float num7 = (float)Math.Sqrt((double)(((1f + matrix.M11) - matrix.M22) - matrix.M33));
                float num4 = 0.5f / num7;
                quaternion.X = 0.5f * num7;
                quaternion.Y = (matrix.M12 + matrix.M21) * num4;
                quaternion.Z = (matrix.M13 + matrix.M31) * num4;
                quaternion.W = (matrix.M23 - matrix.M32) * num4;
                return quaternion;
            }
            if (matrix.M22 > matrix.M33)
            {
                float num6 = (float)Math.Sqrt((double)(((1f + matrix.M22) - matrix.M11) - matrix.M33));
                float num3 = 0.5f / num6;
                quaternion.X = (matrix.M21 + matrix.M12) * num3;
                quaternion.Y = 0.5f * num6;
                quaternion.Z = (matrix.M32 + matrix.M23) * num3;
                quaternion.W = (matrix.M31 - matrix.M13) * num3;
                return quaternion;
            }
            float num5 = (float)Math.Sqrt((double)(((1f + matrix.M33) - matrix.M11) - matrix.M22));
            float num2 = 0.5f / num5;
            quaternion.X = (matrix.M31 + matrix.M13) * num2;
            quaternion.Y = (matrix.M32 + matrix.M23) * num2;
            quaternion.Z = 0.5f * num5;
            quaternion.W = (matrix.M12 - matrix.M21) * num2;
            return quaternion;
        }
        public static Vector3 QuaternionRotate(Quaternion q, Vector3 v)
        {
            Quaternion iq = q;
            iq.X = -iq.X;
            iq.Y = -iq.Y;
            iq.Z = -iq.Z;

            Quaternion res = q * new Quaternion(v.X, v.Y, v.Z, 0) * iq;
            return new Vector3(res.X, res.Y, res.Z);
        }

        public static void QuaternionRotate(ref Quaternion q, ref Vector3 v, out Vector3 result)
        {
            Quaternion iq = q;
            iq.X = -iq.X;
            iq.Y = -iq.Y;
            iq.Z = -iq.Z;

            Quaternion res = q * new Quaternion(v.X, v.Y, v.Z, 0) * iq;

            result = new Vector3(res.X, res.Y, res.Z);
        }

        public static void QuaternionToMatrix(ref Quaternion q, ref Vector3 tl, out Matrix m)
        {
            float w2, x2, y2, z2;
            w2 = q.W * q.W;
            x2 = q.X * q.X;
            y2 = q.Y * q.Y;
            z2 = q.Z * q.Z;

            m.M11 = w2 + x2 - y2 - z2;
            m.M12 = 2 * (q.X * q.Y + q.W * q.Z);
            m.M13 = 2 * (q.X * q.Z - q.W * q.Y);

            m.M21 = 2 * (q.X * q.Y - q.W * q.Z);
            m.M22 = w2 - x2 + y2 - z2;
            m.M23 = 2 * (q.Y * q.Z + q.W * q.X);

            m.M31 = 2 * (q.X * q.Z + q.W * q.Y);
            m.M32 = 2 * (q.Y * q.Z - q.W * q.X);
            m.M33 = w2 - x2 - y2 + z2;

            m.M14 = 0;
            m.M24 = 0;
            m.M34 = 0;

            m.M41 = tl.X;
            m.M42 = tl.Y;
            m.M43 = tl.Z;
            m.M44 = w2 + x2 + y2 + z2;

        }
        public static void QuaternionToMatrix(ref Quaternion q, out Matrix m)
        {
            float w2, x2, y2, z2;
            w2 = q.W * q.W;
            x2 = q.X * q.X;
            y2 = q.Y * q.Y;
            z2 = q.Z * q.Z;

            m.M11 = w2 + x2 - y2 - z2;
            m.M12 = 2 * (q.X * q.Y + q.W * q.Z);
            m.M13 = 2 * (q.X * q.Z - q.W * q.Y);

            m.M21 = 2 * (q.X * q.Y - q.W * q.Z);
            m.M22 = w2 - x2 + y2 - z2;
            m.M23 = 2 * (q.Y * q.Z + q.W * q.X);

            m.M31 = 2 * (q.X * q.Z + q.W * q.Y);
            m.M32 = 2 * (q.Y * q.Z - q.W * q.X);
            m.M33 = w2 - x2 - y2 + z2;

            m.M14 = 0;
            m.M24 = 0;
            m.M34 = 0;

            m.M41 = 0;
            m.M42 = 0;
            m.M43 = 0;
            m.M44 = w2 + x2 + y2 + z2;

        }
        public static Matrix QuaternionToMatrix(Quaternion q, Vector3 tl)
        {
            float w2, x2, y2, z2;

            Matrix m;

            w2 = q.W * q.W;
            x2 = q.X * q.X;
            y2 = q.Y * q.Y;
            z2 = q.Z * q.Z;

            m.M11 = w2 + x2 - y2 - z2;
            m.M12 = 2 * (q.X * q.Y + q.W * q.Z);
            m.M13 = 2 * (q.X * q.Z - q.W * q.Y);

            m.M21 = 2 * (q.X * q.Y - q.W * q.Z);
            m.M22 = w2 - x2 + y2 - z2;
            m.M23 = 2 * (q.Y * q.Z + q.W * q.X);

            m.M31 = 2 * (q.X * q.Z + q.W * q.Y);
            m.M32 = 2 * (q.Y * q.Z - q.W * q.X);
            m.M33 = w2 - x2 - y2 + z2;

            m.M14 = 0;
            m.M24 = 0;
            m.M34 = 0;

            m.M41 = tl.X;
            m.M42 = tl.Y;
            m.M43 = tl.Z;
            m.M44 = w2 + x2 + y2 + z2;

            return m;
        }

        public static Matrix QuaternionToMatrix(Quaternion q)
        {
            float w2, x2, y2, z2;

            Matrix m;

            w2 = q.W * q.W;
            x2 = q.X * q.X;
            y2 = q.Y * q.Y;
            z2 = q.Z * q.Z;

            m.M11 = w2 + x2 - y2 - z2;
            m.M12 = 2 * (q.X * q.Y + q.W * q.Z);
            m.M13 = 2 * (q.X * q.Z - q.W * q.Y);

            m.M21 = 2 * (q.X * q.Y - q.W * q.Z);
            m.M22 = w2 - x2 + y2 - z2;
            m.M23 = 2 * (q.Y * q.Z + q.W * q.X);

            m.M31 = 2 * (q.X * q.Z + q.W * q.Y);
            m.M32 = 2 * (q.Y * q.Z - q.W * q.X);
            m.M33 = w2 - x2 - y2 + z2;

            m.M14 = 0;
            m.M24 = 0;
            m.M34 = 0;

            m.M41 = 0;
            m.M42 = 0;
            m.M43 = 0;
            m.M44 = w2 + x2 + y2 + z2;

            return m;
        }

        public static Quaternion QuaternionMultiplyVector(Quaternion a, Vector3 b)
        {
            return new Quaternion(-(a.X * b.X + a.Y * b.Y + a.Z * b.Z),
                                    a.W * b.X + a.Y * b.Z - a.Z * b.Y,
                                    a.W * b.Y + a.Z * b.X - a.X * b.Z,
                                    a.W * b.Z + a.X * b.Y - a.Y * b.X);
        }

        public static void QuaternionMultiplyVector(ref Quaternion a, ref Vector3 b, out Quaternion res)
        {
            res = new Quaternion(-(a.X * b.X + a.Y * b.Y + a.Z * b.Z),
                                   a.W * b.X + a.Y * b.Z - a.Z * b.Y,
                                   a.W * b.Y + a.Z * b.X - a.X * b.Z,
                                   a.W * b.Z + a.X * b.Y - a.Y * b.X);
        }

        #endregion

        #region 矩阵

        /// <summary>
        /// 变换向量
        /// </summary>
        public static Vector3 MatrixTransformVec(ref Matrix m, Vector3 v)
        {
            return new Vector3(m.M11 * v.X + m.M21 * v.Y + m.M31 * v.Z,
                             m.M12 * v.X + m.M22 * v.Y + m.M32 * v.Z,
                             m.M13 * v.X + m.M23 * v.Y + m.M33 * v.Z);
        }
        /// <summary>
        /// 变换点
        /// </summary>
        public static Vector3 MatrixTransformPoint(ref Matrix m, Vector3 p)
        {
            return new Vector3(m.M11 * p.X + m.M21 * p.Y + m.M31 * p.Z + m.M41,
                            m.M12 * p.X + m.M22 * p.Y + m.M32 * p.Z + m.M42,
                            m.M13 * p.X + m.M23 * p.Y + m.M33 * p.Z + m.M43);
        }

        /// <summary>
        /// 变换向量
        /// </summary>
        public static void MatrixTransformVec(ref Matrix m, ref Vector3 v)
        {
            v = new Vector3(m.M11 * v.X + m.M21 * v.Y + m.M31 * v.Z,
                            m.M12 * v.X + m.M22 * v.Y + m.M32 * v.Z,
                            m.M13 * v.X + m.M23 * v.Y + m.M33 * v.Z);
        }
        /// <summary>
        /// 变换点
        /// </summary>
        public static void MatrixTransformPoint(ref Matrix m, ref Vector3 p)
        {
            p = new Vector3(m.M11 * p.X + m.M21 * p.Y + m.M31 * p.Z + m.M41,
                            m.M12 * p.X + m.M22 * p.Y + m.M32 * p.Z + m.M42,
                            m.M13 * p.X + m.M23 * p.Y + m.M33 * p.Z + m.M43);
        }

        public static void MatrixTranspose3x3(ref Matrix m, out Matrix3x3 ret)
        {
            ret.M11 = m.M11;
            ret.M12 = m.M21;
            ret.M13 = m.M31;
            ret.M21 = m.M12;
            ret.M22 = m.M22;
            ret.M23 = m.M32;
            ret.M31 = m.M13;
            ret.M32 = m.M23;
            ret.M33 = m.M33;
        }

        #endregion

        #region 平面

        public static Vector3 ComputePlaneNormal(Vector3 a, Vector3 b, Vector3 c)
        {
            Vector3 n;
            Vector3 v1;
            Vector3 v2;

            Vector3.Subtract(ref b, ref a, out v1);
            Vector3.Subtract(ref c, ref a, out v2);

            Vector3.Cross(ref v1, ref v2, out n);
            n.Normalize();
            return n;
        }

        public static void ComputePlaneNormal(ref Vector3 a, ref Vector3 b, ref Vector3 c, out Vector3 n)
        {
            Vector3 v1;
            Vector3 v2;

            Vector3.Subtract(ref b, ref a, out v1);
            Vector3.Subtract(ref c, ref a, out v2);

            Vector3.Cross(ref v1, ref v2, out n);
            n.Normalize();
        }

        #endregion

        public static int SmallestPowerOf2(int v)
        {
            int res = 1;
            for (int i = 0; i < 31; i++)
            {
                if (res >= v)
                {
                    return res;
                }
                res *= 2;
            }
            return res;
        }

        public static float Radian2Degree(float rad)
        {
            return rad * (180f / PIf);
        }
        public static float Degree2Radian(float ang)
        {
            return ang * (PIf / 180f);
        }

        public static float AngleDifference(float a, float b)
        {
            float minus = a - b;
            return Math.Min(Math.Min(Math.Abs(minus), Math.Abs(minus + 360)), Math.Abs(minus - 360));
        }
        public static float RadianDifference(float a, float b)
        {
            float minus = a - b;
            float inv;

            if (minus > 0)
            {
                inv = PIf * 2 - minus;
            }
            else
            {
                inv = -PIf * 2 + minus;
            }
            return Math.Min(minus, inv);
        }

        /// <summary>
        /// 点是否在AABB内
        /// </summary>
        public static bool AABBIsIn(ref BoundingBox bb, ref Vector3 p)
        {
            return (bb.Minimum.X <= p.X & p.X <= bb.Maximum.X &
                    bb.Minimum.Y <= p.Y & p.Y <= bb.Maximum.Y &
                    bb.Minimum.Z <= p.Z & p.Z <= bb.Maximum.Z);
        }

        public static float PlaneRelative(ref Plane pl, ref Vector3 p)
        {
            return pl.Normal.X * p.X + pl.Normal.Y * p.Y + pl.Normal.Z * p.Z + pl.D;
        }

        #region Vector2ARGB

        public static unsafe int Vector2ARGB(ref Vector3 n)
        {
            return (0xff << 24) | (((byte)(127f * n.X + 128f)) << 16) | (((byte)(127f * n.Y + 128f)) << 8) | ((byte)(127f * n.Z + 128f));
        }
        public static unsafe int Vector2ARGB(ref Vector3 n, int w)
        {
            return (w << 24) | (((byte)(127f * n.X + 128f)) << 16) | (((byte)(127f * n.Y + 128f)) << 8) | ((byte)(127f * n.Z + 128f));
        }
        public static unsafe int Vector2ARGB(Vector3 n, int w)
        {
            return (w << 24) | (((byte)(127f * n.X + 128f)) << 16) | (((byte)(127f * n.Y + 128f)) << 8) | ((byte)(127f * n.Z + 128f));
        }

        #endregion

        #region 高斯滤镜
        public static float[][] ComputeGuassFilter2D(float delta, int size)
        {
            float invd1 = 1.0f / ((float)Math.Sqrt(2 * Math.PI) * delta * delta);
            float invd2 = 1.0f / (2 * delta * delta);

            float halfSize = (float)size * 0.5f;


            float[][] result = new float[size][];
            for (int i = 0; i < size; i++)
            {
                result[i] = new float[size];

                for (int j = 0; j < size; j++)
                {
                    float x = j - halfSize;
                    float y = i - halfSize;
                    result[i][j] = (float)(invd1 * Math.Exp(-invd2 * (x * x + y * y)));
                }
            }

            return result;
        }

        public static float[] ComputeGuassFilter1D(float delta, int size)
        {
            float invd1 = 1.0f / ((float)Math.Sqrt(2 * Math.PI) * delta * delta);
            float invd2 = 1.0f / (2 * delta * delta);

            float halfSize = (float)size * 0.5f;

            float[] result = new float[size];

            for (int i = 0; i < size; i++)
            {
                float x = i - halfSize;

                result[i] = (float)(invd1 * Math.Exp(-invd2 * (x * x)));
            }

            return result;
        }
        #endregion

        #region 包围球相交检测
        /// <summary>
        /// 球和线段
        /// </summary>
        /// <param name="bs"></param>
        /// <param name="vStart"></param>
        /// <param name="vEnd"></param>
        /// <param name="dist"></param>
        /// <param name="n"></param>
        /// <param name="pos"></param>
        /// <returns></returns>
        public static bool BoundingSphereIntersects(ref BoundingSphere bs, ref Vector3 vStart, ref Vector3 vEnd, out float dist, out Vector3 n, out Vector3 pos)
        {
            Vector3 v = 0.5f * (vStart + vEnd);
            float r = 0.5f * Distance(ref vStart, ref vEnd);// (vStart & vEnd);

            if (DistanceSquared(ref v, ref bs.Center) <= Sqr(r + bs.Radius))
            {
                float cx = bs.Center.X - vStart.X;
                float cy = bs.Center.Y - vStart.Y;
                float cz = bs.Center.Z - vStart.Z;

                n = vEnd - vStart;
                n.Normalize();

                float dl1 = n.X * cx + n.Y * cy + n.Z * cz;

                n.X = cx - n.X * dl1;
                n.Y = cy - n.Y * dl1;
                n.Z = cz - n.Z * dl1;

                n.Normalize();

                dl1 = Math.Abs(-(n.X * cx + n.Y * cy + n.Z * cz));

                dist = dl1;
                pos = bs.Center - dist * n;
                return (dl1 <= bs.Radius);
            }
            dist = 0;
            n = new Vector3();
            pos = new Vector3();
            return false;
        }

        public static bool BoundingSphereContains(ref BoundingSphere a, ref BoundingSphere b)
        {
            float distSq = DistanceSquared(ref a.Center, ref b.Center);

            return distSq <= Sqr(a.Radius - b.Radius);
        }

        public static bool BoundingSphereIntersects(ref BoundingSphere a, ref BoundingSphere b)
        {
            float distSq = DistanceSquared(ref a.Center, ref b.Center);

            return distSq <= Sqr(a.Radius + b.Radius);
        }

        public static void BoundingSphereMerge(ref BoundingSphere sphere1, ref BoundingSphere sphere2, out BoundingSphere res)
        {
            BoundingSphere sphere3;
            Vector3 vector4;
            Vector3.Subtract(ref sphere2.Center, ref sphere1.Center, out vector4);

            float num = vector4.Length();
            float radius = sphere1.Radius;
            float num2 = sphere2.Radius;
            if ((radius + num2) >= num)
            {
                if ((radius - num2) >= num)
                {
                    res = sphere1;
                }
                if ((num2 - radius) >= num)
                {
                    res = sphere2;
                }
            }

            Vector3.Multiply(ref vector4, 1f / num, out vector4);

            float num5 = Math.Min(-radius, num - num2);
            float num4 = (Math.Max(radius, num + num2) - num5) * 0.5f;

            Vector3.Multiply(ref vector4, num4 + num5, out vector4);

            Vector3.Add(ref sphere1.Center, ref vector4, out sphere3.Center);

            sphere3.Radius = num4;
            res = sphere3;
        }

        /// <summary>
        /// 判断三角形是否和包围球相交
        /// 剪枝用
        /// </summary>
        public static bool BoundingSphereIntersects(ref BoundingSphere bs, ref Triangle t)
        {
            //有两种情况，球心在包围平面之内或之外
            //内：距离<r
            //外：边线相交测试

            //float res1 = (vCentre - t.vA) * ((t.vB - t.vA) % t.vN);
            //float res2 = (vCentre - t.vB) * ((t.vC - t.vB) % t.vN);
            //float res3 = (vCentre - t.vC) * ((t.vA - t.vC) % t.vN);
            float res1 = Vector3.Dot((bs.Center - t.vA), Vector3.Cross(t.vB - t.vA, t.vN));// ((t.vB - t.vA) % t.vN);
            float res2 = Vector3.Dot((bs.Center - t.vB), Vector3.Cross(t.vC - t.vB, t.vN));// ((t.vC - t.vB) % t.vN);
            float res3 = Vector3.Dot((bs.Center - t.vC), Vector3.Cross(t.vA - t.vA, t.vN));// ((t.vA - t.vC) % t.vN);

            //float res1 = (vCentre.x - t.vA.x) *( (t.vB.x - t.vA.x)*)

            if ((res1 >= -bs.Radius & res2 >= -bs.Radius & res3 >= -bs.Radius) |
                (res1 <= bs.Radius & res2 <= bs.Radius & res3 <= bs.Radius))
                return Math.Abs(Vector3.Dot(t.vN, bs.Center - t.vA)) <= bs.Radius;

            return false;
        }

        /// <summary>
        /// 判断三角形是否和包围球相交
        /// </summary>
        public static bool BoundingSphereIntersects(ref BoundingSphere bs, ref Triangle t, out Vector3 vPos, out Vector3 n, out float nDepth)
        {
            //有两种情况，球心在包围平面之内或之外
            //内：距离<r
            //外：边线相交测试
            //计算投影长，判断符号

            //bool res1 = (vCentre - t.vA) * ((t.vB - t.vA) % t.vN) >= 0;//ab
            //bool res2 = (vCentre - t.vB) * ((t.vC - t.vB) % t.vN) >= 0;//bc
            //bool res3 = (vCentre - t.vC) * ((t.vA - t.vC) % t.vN) >= 0;//ca

            bool res1 = Vector3.Dot((bs.Center - t.vA), Vector3.Cross(t.vB - t.vA, t.vN)) >= 0;//ab
            bool res2 = Vector3.Dot((bs.Center - t.vB), Vector3.Cross(t.vC - t.vB, t.vN)) >= 0;//bc
            bool res3 = Vector3.Dot((bs.Center - t.vC), Vector3.Cross(t.vA - t.vC, t.vN)) >= 0;//ca
            float dist;

            if ((!res1 & !res2 & !res3) | (res1 & res2 & res3))
            {
                n = t.vN;
                dist = Vector3.Dot(n, (bs.Center - t.vA));

                if (dist < 0)
                {
                    n.X = -n.X; n.Y = -n.Y; n.Z = -n.Z;
                    dist = -dist;
                }

                if (dist <= bs.Radius)
                {
                    vPos = bs.Center - dist * n;
                    nDepth = dist - bs.Radius;
                    return true;
                }
            }

            bool ab = (res1 != res2 & res1 != res3);
            bool bc = (res2 != res1 & res2 != res3);
            bool ca = (res3 != res1 & res3 != res2);

            if (ab && BoundingSphereIntersects(ref bs, ref t.vB, ref t.vA, out dist, out n, out vPos))
            {
                nDepth = dist - bs.Radius;
                return true;
            }
            if (bc && BoundingSphereIntersects(ref bs, ref t.vC, ref t.vB, out dist, out n, out vPos))
            {
                nDepth = dist - bs.Radius;
                return true;
            }
            if (ca && BoundingSphereIntersects(ref bs, ref t.vA, ref t.vC, out dist, out n, out vPos))
            {
                nDepth = dist - bs.Radius;
                return true;
            }
            //和顶点碰撞
            if (bc)
            {
                dist = MathEx.Distance(ref t.vA, ref bs.Center);
                if (dist <= bs.Radius)
                {
                    nDepth = dist - bs.Radius;
                    vPos = t.vA;

                    n = bs.Center - t.vA;
                    n.Normalize();

                    return true;
                }
            }
            if (ca)
            {
                dist = MathEx.Distance(ref t.vB, ref bs.Center);//t.vB & vCentre;
                if (dist <= bs.Radius)
                {
                    nDepth = dist - bs.Radius;
                    vPos = t.vB;

                    n = bs.Center - t.vB;
                    n.Normalize();

                    return true;
                }
            }
            if (ab)
            {
                dist = MathEx.Distance(ref t.vC, ref bs.Center);// t.vC & vCentre;
                if (dist <= bs.Radius)
                {
                    nDepth = dist - bs.Radius;
                    vPos = t.vC;

                    n = bs.Center - t.vC;
                    n.Normalize();

                    return true;
                }
            }

            vPos = new Vector3();
            nDepth = 0;
            n = new Vector3();
            return false;

        }

        /// <summary>
        /// 判断AABB是否和包围球相交
        /// 剪枝用
        /// </summary>
        public static bool BoundingSphereIntersects(ref BoundingSphere bs, ref BoundingBox aabb)
        {
            return ((aabb.Minimum.X - bs.Radius <= bs.Center.X) & (bs.Center.X <= aabb.Maximum.X + bs.Radius) &
                    (aabb.Minimum.Y - bs.Radius <= bs.Center.Y) & (bs.Center.Y <= aabb.Maximum.Y + bs.Radius) &
                    (aabb.Minimum.Z - bs.Radius <= bs.Center.Z) & (bs.Center.Z <= aabb.Maximum.Z + bs.Radius));
        }

        public static bool BoundingSphereIntersects(ref BoundingSphere bs, ref Ray ra)
        {
            float cx = bs.Center.X - ra.Position.X;
            float cy = bs.Center.Y - ra.Position.Y;
            float cz = bs.Center.Z - ra.Position.Z;


            float dl1 = ra.Direction.X * cx + ra.Direction.Y * cy + ra.Direction.Z * cz;

            Vector3 n = new Vector3(cx - ra.Direction.X * dl1, cy - ra.Direction.Y * dl1, cz - ra.Direction.Z * dl1);
            //n.X = cx - n.X * dl1;
            //n.Y = cy - n.Y * dl1;
            //n.Z = cz - n.Z * dl1;

            n.Normalize();

            dl1 = Math.Abs(-(n.X * cx + n.Y * cy + n.Z * cz));

            return (dl1 <= bs.Radius);
        }

        public static bool BoundingSphereIntersects(ref BoundingSphere bs, ref Vector3 start, ref Vector3 end)
        {
            //Vector3 v1 = (vCentre - vEnd);
            //Vector3 v2 = (vStart - vEnd).UnitVector;

            //float dist = v1.Length;
            //v1.Normalise();
            //float cosine = v1 * v2;
            //float sinine = (float)Math.Sqrt(1.0 - cosine * cosine);
            //dist *= sinine;

            //return (dist <= dRange);

            float cx = bs.Center.X - start.X;
            float cy = bs.Center.Y - start.Y;
            float cz = bs.Center.Z - start.Z;

            Vector3 n = end - start;
            n.Normalize();

            float dl1 = n.X * cx + n.Y * cy + n.Z * cz;

            n.X = cx - n.X * dl1;
            n.Y = cy - n.Y * dl1;
            n.Z = cz - n.Z * dl1;

            n.Normalize();

            dl1 = Math.Abs(-(n.X * cx + n.Y * cy + n.Z * cz));

            return (dl1 <= bs.Radius);

        }

        #endregion

        #region AABB相交检测
        /// <summary>
        /// 判断球是否和AABB相交
        /// 剪枝用
        /// </summary>
        public static bool AABBIntersects(ref BoundingBox a, ref BoundingSphere bs)
        {
            return ((a.Minimum.X - bs.Radius <= bs.Center.X) & (bs.Center.X <= a.Maximum.X + bs.Radius) &
                    (a.Minimum.Y - bs.Radius <= bs.Center.Y) & (bs.Center.Y <= a.Maximum.Y + bs.Radius) &
                    (a.Minimum.Z - bs.Radius <= bs.Center.Z) & (bs.Center.Z <= a.Maximum.Z + bs.Radius));
        }

        public static bool AABBIntersects(ref BoundingBox a, ref BoundingBox b)
        {
            if ((a.Maximum.X < b.Minimum.X) || (a.Minimum.X > b.Maximum.X))
                return false;

            if ((a.Maximum.Y < b.Minimum.Y) || (a.Minimum.Y > b.Maximum.Y))
                return false;

            return ((a.Maximum.Z >= b.Minimum.Z) && (a.Minimum.Z <= b.Maximum.Z));
            //bool overlap = true;
            //overlap = (xMin > aabb.xMax || xMax < aabb.xMin) ? false : overlap;
            //overlap = (zMin > aabb.zMax || zMax < aabb.zMin) ? false : overlap;
            //overlap = (yMin > aabb.yMax || yMax < aabb.yMin) ? false : overlap;
            //return overlap;
        }


        /// <summary>
        /// 判断射线是否和AABB相交
        /// 剪枝用
        /// </summary>
        public static bool AABBIntersects(ref BoundingBox aabb, ref Vector3 vStart, ref Vector3 vEnd)
        {
            float t;
            Vector3 vHit;
            Vector3 vDir = vEnd - vStart;

            //先检查在盒子内
            if (vStart.X >= aabb.Minimum.X & vStart.Y >= aabb.Minimum.Y & vStart.Z >= aabb.Minimum.Z &
                vStart.X <= aabb.Maximum.X & vStart.Y <= aabb.Maximum.Y & vStart.Z <= aabb.Maximum.Z)
                return true;
            if (vEnd.X >= aabb.Minimum.X & vEnd.Y >= aabb.Minimum.Y & vEnd.Z >= aabb.Minimum.Z &
                vEnd.X <= aabb.Maximum.X & vEnd.Y <= aabb.Maximum.Y & vEnd.Z <= aabb.Maximum.Z)
                return true;

            //依次检查各面的相交情况
            if (vStart.X < aabb.Minimum.X && vDir.X > 0)
            {
                t = (aabb.Minimum.X - vStart.X) / vDir.X;
                if (t > 0)
                {
                    vHit = vStart + vDir * t;
                    if (vHit.Y >= aabb.Minimum.Y && vHit.Y <= aabb.Maximum.Y &&
                        vHit.Z >= aabb.Minimum.Z && vHit.Z <= aabb.Maximum.Z)
                        return true;

                }
            }

            if (vStart.X > aabb.Maximum.X && vDir.X < 0)
            {
                t = (aabb.Maximum.X - vStart.X) / vDir.X;
                if (t > 0)
                {
                    vHit = vStart + vDir * t;
                    if (vHit.Y > aabb.Minimum.Y && vHit.Y <= aabb.Maximum.Y &&
                        vHit.Z >= aabb.Minimum.Z && vHit.Y <= aabb.Maximum.Z)
                        return true;
                }
            }

            if (vStart.Y < aabb.Minimum.Y && vDir.Y > 0)
            {
                t = (aabb.Minimum.Y - vStart.Y) / vDir.Y;
                if (t > 0)
                {
                    vHit = vStart + vDir * t;
                    if (vHit.X >= aabb.Minimum.X && vHit.X <= aabb.Maximum.X &&
                        vHit.Z >= aabb.Minimum.Z && vHit.Z <= aabb.Maximum.Z)
                        return true;
                }
            }

            if (vStart.Y > aabb.Maximum.Y && vDir.Y < 0)
            {
                t = (aabb.Maximum.Y - vStart.Y) / vDir.Y;
                if (t > 0)
                {
                    vHit = vStart + vDir * t;
                    if (vHit.X >= aabb.Minimum.X && vHit.X <= aabb.Maximum.X &&
                        vHit.Z >= aabb.Minimum.Z && vHit.Z <= aabb.Maximum.Z)
                        return true;
                }
            }

            if (vStart.Z < aabb.Minimum.Z && vDir.Z > 0)
            {
                t = (aabb.Minimum.Z - vStart.Z) / vDir.Z;
                if (t > 0)
                {
                    vHit = vStart + vDir * t;
                    if (vHit.X >= aabb.Minimum.X && vHit.X <= aabb.Maximum.X &&
                        vHit.Y >= aabb.Minimum.Y && vHit.Y <= aabb.Maximum.Y)
                        return true;
                }
            }

            if (vStart.Z > aabb.Maximum.Z && vDir.Z < 0)
            {
                t = (aabb.Maximum.Z - vStart.Z) / vDir.Z;
                if (t > 0)
                {
                    vHit = vStart + vDir * t;
                    if (vHit.X >= aabb.Minimum.X && vHit.X <= aabb.Maximum.X &&
                        vHit.Y >= aabb.Minimum.Y && vHit.Y <= aabb.Maximum.Y)
                        return true;
                }
            }
            return false;

        }
        public static bool AABBIntersects(ref BoundingBox aabb, ref Triangle t)
        {
            Vector3 center = (t.vA + t.vB + t.vC) / 3f;

            float r = MathEx.Distance(ref  t.vA, ref center);

            return ((aabb.Minimum.X - r <= center.X) & (center.X <= aabb.Maximum.X + r) &
                    (aabb.Minimum.Y - r <= center.Y) & (center.Y <= aabb.Maximum.Y + r) &
                    (aabb.Minimum.Z - r <= center.Z) & (center.Z <= aabb.Maximum.Z + r));

        }
        #endregion

        #region 差值
        /// <summary>
        /// 线形插值
        /// </summary>
        /// <param name="f1">第一个值</param>
        /// <param name="f2">第二个值</param>
        /// <param name="amount">插值系数</param>
        /// <returns></returns>
        public static float LinearInterpose(float f1, float f2, float amount)
        {
            return (f2 - f1) * amount + f1;
        }

        public static Vector3 LinearInterpose(Vector3 f1, Vector3 f2, float amount)
        {
            return (f2 - f1) * amount + f1;
        }

        public static float CatmullRom(float value1, float value2, float value3, float value4, float amount)
        {
            //float num = amount * amount;
            //float num2 = amount * num;

            float a0 = (2f * value2);
            float a1 = (-value1 + value3);
            float a2 = (2f * value1 - 5f * value2 + 4f * value3 - value4);
            float a3 = (-value1 + 3f * value2 - 3f * value3 + value4);

            return 0.5f * ((
                (a3 * amount + a2) * amount
                + a1) * amount
                + a0);

            //<Edit by yanghaitao 2008-12-12.>
            //return 0.5f * (a3 * num2 + a2 * num + a1 * amount + a0);

            //<simplified form>
            //return (0.5f * (
            //        2f * value2
            //                    + 
            //        (-value1 + value3) * amount
            //                     +
            //        (2f * value1 - 5f * value2 + 4f * value3 - value4) * num
            //                     +
            //         ( -value1 + 3f * value2 - 3f * value3 + value4 ) * num2                     
            //        )
            //      );

            //<original form>
            //return(
            //        0.5f * 
            //        ((((2f * value2) + ((-value1 + value3) * amount)) +
            //        (((((2f * value1) - (5f * value2)) + (4f * value3)) - value4) * num)) +
            //        ((((-value1 + (3f * value2)) - (3f * value3)) + value4) * num2))
            //      );
        }

        public static Vector3 CatmullRom(Vector3 value1, Vector3 value2, Vector3 value3, Vector3 value4, float amount)
        {
            Vector3 vector;
            float num = amount * amount;
            float num2 = amount * num;
            vector.X = 0.5f * ((((2f * value2.X) + ((-value1.X + value3.X) * amount)) + (((((2f * value1.X) - (5f * value2.X)) + (4f * value3.X)) - value4.X) * num)) + ((((-value1.X + (3f * value2.X)) - (3f * value3.X)) + value4.X) * num2));
            vector.Y = 0.5f * ((((2f * value2.Y) + ((-value1.Y + value3.Y) * amount)) + (((((2f * value1.Y) - (5f * value2.Y)) + (4f * value3.Y)) - value4.Y) * num)) + ((((-value1.Y + (3f * value2.Y)) - (3f * value3.Y)) + value4.Y) * num2));
            vector.Z = 0.5f * ((((2f * value2.Z) + ((-value1.Z + value3.Z) * amount)) + (((((2f * value1.Z) - (5f * value2.Z)) + (4f * value3.Z)) - value4.Z) * num)) + ((((-value1.Z + (3f * value2.Z)) - (3f * value3.Z)) + value4.Z) * num2));
            return vector;
        }

        #endregion

        #region 数值计算
        public static float Clamp(float min, float max, float a)
        {
            if (a < min)
            {
                return min;
            }
            return a > max ? max : a;
        }
        #endregion
    }
}
