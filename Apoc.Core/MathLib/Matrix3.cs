using System;

namespace Apoc.MathLib
{
    [Obsolete()]
    public struct Matrix3x3
    {
        public float M11, M12, M13;
        public float M21, M22, M23;
        public float M31, M32, M33;

        public Matrix3x3(float[] data)
        {
            M11 = data[0]; M12 = data[1]; M13 = data[2];
            M21 = data[3]; M22 = data[4]; M23 = data[5];
            M31 = data[6]; M32 = data[7]; M33 = data[8];
        }

        /// <summary>
        /// 得到转置矩阵
        /// </summary>
        public Matrix3x3 GetTranspose()
        {
            Matrix3x3 ret;
            ret.M11 = M11;
            ret.M12 = M21;
            ret.M13 = M31;
            ret.M21 = M12;
            ret.M22 = M22;
            ret.M23 = M32;
            ret.M31 = M13;
            ret.M32 = M23;
            ret.M33 = M33;
            return ret;
        }
        /// <summary>
        /// 得到逆矩阵
        /// </summary>
        public Matrix3x3 GetInverse()
        {
            Matrix3x3 ret;
            float d = M11 * M22 * M33 -
                      M11 * M32 * M23 +
                      M21 * M32 * M13 -
                      M21 * M12 * M33 +
                      M31 * M12 * M23 -
                      M31 * M22 * M13;

            if (d == 0)
            {
                ret.M11 = (M22 * M33 - M23 * M32);
                ret.M12 = -(M12 * M33 - M13 * M32);
                ret.M13 = (M12 * M23 - M13 * M22);
                ret.M21 = -(M21 * M33 - M23 * M31);
                ret.M22 = (M11 * M33 - M13 * M31);
                ret.M23 = -(M11 * M23 - M13 * M21);
                ret.M31 = (M21 * M32 - M22 * M31);
                ret.M32 = -(M11 * M32 - M12 * M31);
                ret.M33 = (M11 * M22 - M12 * M21);
            }
            else
            {
                d = 1 / d;
                ret.M11 = (M22 * M33 - M23 * M32) * d;
                ret.M12 = -(M12 * M33 - M13 * M32) * d;
                ret.M13 = (M12 * M23 - M13 * M22) * d;
                ret.M21 = -(M21 * M33 - M23 * M31) * d;
                ret.M22 = (M11 * M33 - M13 * M31) * d;
                ret.M23 = -(M11 * M23 - M13 * M21) * d;
                ret.M31 = (M21 * M32 - M22 * M31) * d;
                ret.M32 = -(M11 * M32 - M12 * M31) * d;
                ret.M33 = (M11 * M22 - M12 * M21) * d;
            }
            return ret;
        }
        /// <summary>
        /// 将矩阵变为逆矩阵
        /// </summary>
        public void Inverse()
        {
            Matrix3x3 ret = this.GetInverse();

            M11 = ret.M11;
            M12 = ret.M12;
            M13 = ret.M13;
            M21 = ret.M21;
            M22 = ret.M22;
            M23 = ret.M23;
            M31 = ret.M31;
            M32 = ret.M32;
            M33 = ret.M33;
        }
        /// <summary>
        /// 加载单位矩阵
        /// </summary>
        public void LoadIdentify()
        {
            M11 = 1; M12 = 0; M13 = 0;
            M21 = 0; M22 = 1; M23 = 0;
            M31 = 0; M32 = 0; M33 = 1;
        }
        /// <summary>
        /// 变换向量 
        /// </summary>
        public void Transform(ref Vector3 v)
        {
            v = new Vector3(M11 * v.X + M12 * v.Y + M13 * v.Z,
                            M21 * v.X + M22 * v.Y + M23 * v.Z,
                            M31 * v.X + M32 * v.Y + M33 * v.Z);
        }


        public static Matrix3x3 operator +(Matrix3x3 a, Matrix3x3 b)
        {
            a.M11 += b.M11;
            a.M12 += b.M12;
            a.M13 += b.M13;
            a.M21 += b.M21;
            a.M22 += b.M22;
            a.M23 += b.M23;
            a.M31 += b.M31;
            a.M32 += b.M32;
            a.M33 += b.M33;
            return a;
        }
        public static Matrix3x3 operator -(Matrix3x3 a, Matrix3x3 b)
        {
            a.M11 -= b.M11;
            a.M12 -= b.M12;
            a.M13 -= b.M13;
            a.M21 -= b.M21;
            a.M22 -= b.M22;
            a.M23 -= b.M23;
            a.M31 -= b.M31;
            a.M32 -= b.M32;
            a.M33 -= b.M33;
            return a;
        }
        public static Matrix3x3 operator *(Matrix3x3 a, float b)
        {
            a.M11 *= b;
            a.M12 *= b;
            a.M13 *= b;
            a.M21 *= b;
            a.M22 *= b;
            a.M23 *= b;
            a.M31 *= b;
            a.M32 *= b;
            a.M33 *= b;
            return a;
        }
        public static Matrix3x3 operator /(Matrix3x3 a, float b)
        {
            b = 1.0f / b;
            a.M11 /= b;
            a.M12 /= b;
            a.M13 /= b;
            a.M21 /= b;
            a.M22 /= b;
            a.M23 /= b;
            a.M31 /= b;
            a.M32 /= b;
            a.M33 /= b;
            return a;
        }

        public static Matrix3x3 operator *(Matrix3x3 a, Matrix3x3 b)
        {
            Matrix3x3 ret;
            ret.M11 = a.M11 * b.M11 + a.M12 * b.M21 + a.M13 * b.M31;
            ret.M12 = a.M11 * b.M12 + a.M12 * b.M22 + a.M13 * b.M32;
            ret.M13 = a.M11 * b.M13 + a.M12 * b.M23 + a.M13 * b.M33;

            ret.M21 = a.M21 * b.M11 + a.M22 * b.M21 + a.M23 * b.M31;
            ret.M22 = a.M21 * b.M12 + a.M22 * b.M22 + a.M23 * b.M32;
            ret.M23 = a.M21 * b.M13 + a.M22 * b.M23 + a.M23 * b.M33;

            ret.M31 = a.M31 * b.M11 + a.M32 * b.M21 + a.M33 * b.M31;
            ret.M32 = a.M31 * b.M12 + a.M32 * b.M22 + a.M33 * b.M32;
            ret.M33 = a.M31 * b.M13 + a.M32 * b.M23 + a.M33 * b.M33;
            return ret;
        }
        public static Matrix3x3 operator *(Matrix3x3 a, Matrix b)
        {
            Matrix3x3 ret;
            ret.M11 = a.M11 * b.M11 + a.M12 * b.M21 + a.M13 * b.M31;
            ret.M12 = a.M11 * b.M12 + a.M12 * b.M22 + a.M13 * b.M32;
            ret.M13 = a.M11 * b.M13 + a.M12 * b.M23 + a.M13 * b.M33;

            ret.M21 = a.M21 * b.M11 + a.M22 * b.M21 + a.M23 * b.M31;
            ret.M22 = a.M21 * b.M12 + a.M22 * b.M22 + a.M23 * b.M32;
            ret.M23 = a.M21 * b.M13 + a.M22 * b.M23 + a.M23 * b.M33;

            ret.M31 = a.M31 * b.M11 + a.M32 * b.M21 + a.M33 * b.M31;
            ret.M32 = a.M31 * b.M12 + a.M32 * b.M22 + a.M33 * b.M32;
            ret.M33 = a.M31 * b.M13 + a.M32 * b.M23 + a.M33 * b.M33;
            return ret;
        }
        public static Matrix3x3 operator *(Matrix a, Matrix3x3 b)
        {
            Matrix3x3 ret;
            ret.M11 = a.M11 * b.M11 + a.M12 * b.M21 + a.M13 * b.M31;
            ret.M12 = a.M11 * b.M12 + a.M12 * b.M22 + a.M13 * b.M32;
            ret.M13 = a.M11 * b.M13 + a.M12 * b.M23 + a.M13 * b.M33;

            ret.M21 = a.M21 * b.M11 + a.M22 * b.M21 + a.M23 * b.M31;
            ret.M22 = a.M21 * b.M12 + a.M22 * b.M22 + a.M23 * b.M32;
            ret.M23 = a.M21 * b.M13 + a.M22 * b.M23 + a.M23 * b.M33;

            ret.M31 = a.M31 * b.M11 + a.M32 * b.M21 + a.M33 * b.M31;
            ret.M32 = a.M31 * b.M12 + a.M32 * b.M22 + a.M33 * b.M32;
            ret.M33 = a.M31 * b.M13 + a.M32 * b.M23 + a.M33 * b.M33;

            return ret;
        }
        public static Vector3 operator *(Matrix3x3 a, Vector3 b)
        {
            return new Vector3(a.M11 * b.X + a.M12 * b.Y + a.M13 * b.Z,
                              a.M21 * b.X + a.M22 * b.Y + a.M23 * b.Z,
                              a.M31 * b.X + a.M32 * b.Y + a.M33 * b.Z);
        }

        public override string ToString()
        {
            return base.ToString();
        }
        public string ToString(int dig)
        {
            return Math.Round(M11, dig).ToString() + " " + Math.Round(M12, dig).ToString() + " " + Math.Round(M13, dig).ToString() + "\r\n" +
                   Math.Round(M21, dig).ToString() + " " + Math.Round(M22, dig).ToString() + " " + Math.Round(M23, dig).ToString() + "\r\n" +
                   Math.Round(M31, dig).ToString() + " " + Math.Round(M32, dig).ToString() + " " + Math.Round(M33, dig).ToString();
        }

        public float[] ToFloats() 
        {
            return new float[] { (float)M11, (float)M12, (float)M13, 
                                 (float)M21, (float)M22, (float)M23, 
                                 (float)M31, (float)M32, (float)M33 };
        }

        public string GetString 
        {
            get 
            {
                return Math.Round(M11, 2).ToString() + " " + Math.Round(M12, 2).ToString() + " " + Math.Round(M13, 2).ToString() + "\r\n" +
                       Math.Round(M21, 2).ToString() + " " + Math.Round(M22, 2).ToString() + " " + Math.Round(M23, 2).ToString() + "\r\n" +
                       Math.Round(M31, 2).ToString() + " " + Math.Round(M32, 2).ToString() + " " + Math.Round(M33, 2).ToString();
            }
        }
        public float[] GetFloats 
        {
            get 
            {
                return new float[] { M11, M12, M13, 
                                     M21, M22, M23, 
                                     M31, M32, M33 };
            }
        }

    }
}