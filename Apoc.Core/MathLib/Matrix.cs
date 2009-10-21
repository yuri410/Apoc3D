using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Globalization;
using System.Runtime.InteropServices;
using System.Text;
using Apoc.Design;

namespace Apoc.MathLib
{
    /// <summary>
    /// Defines a 4x4 matrix.
    /// </summary>
    [Serializable, StructLayout(LayoutKind.Sequential), TypeConverter(typeof(MatrixConverter))]
    public unsafe struct Matrix : IEquatable<Matrix>
    {
        #region Fields(字段)

        /// <summary>
        /// Gets or sets the element of the matrix that exists in the first row and first column. 
        /// </summary>
        public float M11;

        /// <summary>
        /// Gets or sets the element of the matrix that exists in the first row and second column. 
        /// </summary>
        public float M12;

        /// <summary>
        /// Gets or sets the element of the matrix that exists in the first row and third column. 
        /// </summary>
        public float M13;

        /// <summary>
        /// Gets or sets the element of the matrix that exists in the first row and fourth column. 
        /// </summary>
        public float M14;

        /// <summary>
        /// Gets or sets the element of the matrix that exists in the second row and first column. 
        /// </summary>
        public float M21;

        /// <summary>
        /// Gets or sets the element of the matrix that exists in the second row and second column. 
        /// </summary>
        public float M22;

        /// <summary>
        /// Gets or sets the element of the matrix that exists in the second row and third column. 
        /// </summary>
        public float M23;

        /// <summary>
        /// Gets or sets the element of the matrix that exists in the second row and fourth column. 
        /// </summary>
        public float M24;

        /// <summary>
        /// Gets or sets the element of the matrix that exists in the third row and first column. 
        /// </summary>
        public float M31;

        /// <summary>
        /// Gets or sets the element of the matrix that exists in the third row and second column. 
        /// </summary>
        public float M32;

        /// <summary>
        /// Gets or sets the element of the matrix that exists in the third row and third column. 
        /// </summary>
        public float M33;

        /// <summary>
        /// Gets or sets the element of the matrix that exists in the third row and fourth column. 
        /// </summary>
        public float M34;

        /// <summary>
        /// Gets or sets the element of the matrix that exists in the fourth row and first column. 
        /// </summary>
        public float M41;

        /// <summary>
        /// Gets or sets the element of the matrix that exists in the fourth row and second column. 
        /// </summary>
        public float M42;

        /// <summary>
        /// Gets or sets the element of the matrix that exists in the fourth row and third column. 
        /// </summary>
        public float M43;

        /// <summary>
        /// Gets or sets the element of the matrix that exists in the fourth row and fourth column. 
        /// </summary>
        public float M44;

        #endregion

        #region Properties(属性)

        /// <summary>
        /// Get or set the element of the matrix.
        /// </summary>
        /// <param name="r">the row of the element</param>
        /// <param name="c">the column of the element</param>
        /// <returns></returns>
        [Browsable(false)]
        public float this[int r, int c]
        {
            get
            {
                if (r < 0 || r > 3 || c < 0 || c > 3)
                    throw new IndexOutOfRangeException("Rows and columns for matrices run from 0 to 3, inclusive.");

                int index = r * 4 + c;
                switch (index)
                {
                    case 0: return M11;
                    case 1: return M12;
                    case 2: return M13;
                    case 3: return M14;
                    case 4: return M21;
                    case 5: return M22;
                    case 6: return M23;
                    case 7: return M24;
                    case 8: return M31;
                    case 9: return M32;
                    case 10: return M33;
                    case 11: return M34;
                    case 12: return M41;
                    case 13: return M42;
                    case 14: return M43;
                    case 15: return M44;
                }

                return 0.0f;
            }
            set
            {
                if (r < 0 || r > 3 || c < 0 || c > 3)
                    throw new IndexOutOfRangeException("Rows and columns for matrices run from 0 to 3, inclusive.");

                int index = r * 4 + c;
                switch (index)
                {
                    case 0: M11 = value; break;
                    case 1: M12 = value; break;
                    case 2: M13 = value; break;
                    case 3: M14 = value; break;
                    case 4: M21 = value; break;
                    case 5: M22 = value; break;
                    case 6: M23 = value; break;
                    case 7: M24 = value; break;
                    case 8: M31 = value; break;
                    case 9: M32 = value; break;
                    case 10: M33 = value; break;
                    case 11: M34 = value; break;
                    case 12: M41 = value; break;
                    case 13: M42 = value; break;
                    case 14: M43 = value; break;
                    case 15: M44 = value; break;
                }
            }
        }

        public Vector4 GetRow(int row)
        {
            if (row < 0 || row > 3)
            {
                throw new IndexOutOfRangeException("Rows for matrices run from 0 to 3, inclusive.");
            }
            switch (row)
            {
                case 0: return new Vector4(M11, M12, M13, M14);
                case 1: return new Vector4(M21, M22, M23, M24);
                case 2: return new Vector4(M31, M32, M33, M34);
                case 3: return new Vector4(M41, M42, M43, M44);
            }
            return Vector4.Zero;
        }
        public Vector4 GetCol(int col)
        {
            if (col < 0 || col > 3)
            {
                throw new IndexOutOfRangeException("Columns for matrices run from 0 to 3, inclusive.");
            }
            switch (col)
            {
                case 0: return new Vector4(M11, M21, M31, M41);
                case 1: return new Vector4(M12, M22, M32, M42);
                case 2: return new Vector4(M13, M23, M33, M43);
                case 3: return new Vector4(M14, M24, M34, M44);
            }
            return Vector4.Zero;
        }
        public void SetRow(int row, Vector4 value)
        {
            if (row < 0 || row > 3)
            {
                throw new IndexOutOfRangeException("Rows for matrices run from 0 to 3, inclusive.");
            }
            switch (row)
            {
                case 0: M11 = value.X; M12 = value.Y; M13 = value.Z; M14 = value.W; break;
                case 1: M21 = value.X; M22 = value.Y; M23 = value.Z; M24 = value.W; break;
                case 2: M31 = value.X; M32 = value.Y; M33 = value.Z; M34 = value.W; break;
                case 3: M41 = value.X; M42 = value.Y; M43 = value.Z; M44 = value.W; break;
            }

        }
        public void SetCol(int col, Vector4 value)
        {
            if (col < 0 || col > 3)
            {
                throw new IndexOutOfRangeException("Columns for matrices run from 0 to 3, inclusive.");
            }
            switch (col)
            {
                case 0: M11 = value.X; M21 = value.Y; M31 = value.Z; M41 = value.W; break;
                case 1: M12 = value.X; M22 = value.Y; M32 = value.Z; M42 = value.W; break;
                case 2: M13 = value.X; M23 = value.Y; M33 = value.Z; M43 = value.W; break;
                case 3: M14 = value.X; M24 = value.Y; M34 = value.Z; M44 = value.W; break;
            }
        }

        public Vector3 Up
        {
            get
            {
                Vector3 vector;
                vector.X = this.M21;
                vector.Y = this.M22;
                vector.Z = this.M23;
                return vector;
            }
            set
            {
                this.M21 = value.X;
                this.M22 = value.Y;
                this.M23 = value.Z;
            }
        }
        public Vector3 Down
        {
            get
            {
                Vector3 vector;
                vector.X = -this.M21;
                vector.Y = -this.M22;
                vector.Z = -this.M23;
                return vector;
            }
            set
            {
                this.M21 = -value.X;
                this.M22 = -value.Y;
                this.M23 = -value.Z;
            }
        }
        public Vector3 Right
        {
            get
            {
                Vector3 vector;
                vector.X = this.M11;
                vector.Y = this.M12;
                vector.Z = this.M13;
                return vector;
            }
            set
            {
                this.M11 = value.X;
                this.M12 = value.Y;
                this.M13 = value.Z;
            }
        }
        public Vector3 Left
        {
            get
            {
                Vector3 vector;
                vector.X = -this.M11;
                vector.Y = -this.M12;
                vector.Z = -this.M13;
                return vector;
            }
            set
            {
                this.M11 = -value.X;
                this.M12 = -value.Y;
                this.M13 = -value.Z;
            }
        }
        public Vector3 Forward
        {
            get
            {
                Vector3 vector;
                vector.X = -this.M31;
                vector.Y = -this.M32;
                vector.Z = -this.M33;
                return vector;
            }
            set
            {
                this.M31 = -value.X;
                this.M32 = -value.Y;
                this.M33 = -value.Z;
            }
        }
        public Vector3 Backward
        {
            get
            {
                Vector3 vector;
                vector.X = this.M31;
                vector.Y = this.M32;
                vector.Z = this.M33;
                return vector;
            }
            set
            {
                this.M31 = value.X;
                this.M32 = value.Y;
                this.M33 = value.Z;
            }
        }
        public Vector3 TranslationValue
        {
            get
            {
                Vector3 vector;
                vector.X = this.M41;
                vector.Y = this.M42;
                vector.Z = this.M43;
                return vector;
            }
            set
            {
                this.M41 = value.X;
                this.M42 = value.Y;
                this.M43 = value.Z;
            }
        }

        /// <summary>
        /// Gets a <see cref="Matrix"/> that represents an identity matrix.
        /// </summary>
        public static Matrix Identity
        {
            get
            {
                Matrix result = new Matrix();
                result.M11 = 1.0f;
                result.M22 = 1.0f;
                result.M33 = 1.0f;
                result.M44 = 1.0f;

                return result;
            }
        }

        /// <summary>
        /// Gets a value indicating whether this instance is an identity matrix.
        /// </summary>
        /// <remarks>
        /// The identity matrix is a matrix in which all coefficients are 0 except the [1,1][2,2][3,3][4,4] coefficients, 
        /// which are set to 1. The identity matrix is special in that when it is applied to vertices, they are unchanged. 
        /// The identity matrix is used as the starting point for matrices that will modify vertex values to create rotations,
        /// translations, and any other transformations that can be represented by a 4 x4 matrix.
        /// </remarks>
        [Browsable(false)]
        public bool IsIdentity
        {
            get
            {
                if (M11 != 1.0f || M22 != 1.0f || M33 != 1.0f || M44 != 1.0f)
                    return false;

                if (M12 != 0.0f || M13 != 0.0f || M14 != 0.0f ||
                    M21 != 0.0f || M23 != 0.0f || M24 != 0.0f ||
                    M31 != 0.0f || M32 != 0.0f || M34 != 0.0f ||
                    M41 != 0.0f || M42 != 0.0f || M43 != 0.0f)
                    return false;

                return true;
            }
        }


        #endregion

        #region Methods(方法)
        /// <summary>
        ///  Convert the matrix to a array of float.
        /// </summary>
        /// <returns></returns>
        public float[] ToArray()
        {
            float[] result = new float[16];
            result[0] = M11;
            result[1] = M12;
            result[2] = M13;
            result[3] = M14;
            result[4] = M21;
            result[5] = M22;
            result[6] = M23;
            result[7] = M24;
            result[8] = M31;
            result[9] = M32;
            result[10] = M33;
            result[11] = M34;
            result[12] = M41;
            result[13] = M42;
            result[14] = M43;
            result[15] = M44;
            return result;
        }

        /// <summary>
        /// Inverts the matrix.
        /// </summary>
        public void Invert()
        {
            float m11 = M11;
            float m12 = M12;
            float m13 = M13;
            float m14 = M14;
            float m21 = M21;
            float m22 = M22;
            float m23 = M23;
            float m24 = M24;
            float m31 = M31;
            float m32 = M32;
            float m33 = M33;
            float m34 = M34;
            float m41 = M41;
            float m42 = M42;
            float m43 = M43;
            float m44 = M44;
            float num23 = m33 * m44 - m34 * m43;
            float num22 = m32 * m44 - m34 * m42;
            float num21 = m32 * m43 - m33 * m42;
            float num20 = m31 * m44 - m34 * m41;
            float num19 = m31 * m43 - m33 * m41;
            float num18 = m31 * m42 - m32 * m41;
            float num39 = m22 * num23 - m23 * num22 + m24 * num21;
            float num38 = -(m21 * num23 - m23 * num20 + m24 * num19);
            float num37 = m21 * num22 - m22 * num20 + m24 * num18;
            float num36 = -(m21 * num21 - m22 * num19 + m23 * num18);
            float num = 1f / (m11 * num39 + m12 * num38 + m13 * num37 + m14 * num36);
            M11 = num39 * num;
            M21 = num38 * num;
            M31 = num37 * num;
            M41 = num36 * num;
            M12 = -(m12 * num23 - m13 * num22 + m14 * num21) * num;
            M22 = (m11 * num23 - m13 * num20 + m14 * num19) * num;
            M32 = -(m11 * num22 - m12 * num20 + m14 * num18) * num;
            M42 = (m11 * num21 - m12 * num19 + m13 * num18) * num;
            float num35 = m23 * m44 - m24 * m43;
            float num34 = m22 * m44 - m24 * m42;
            float num33 = m22 * m43 - m23 * m42;
            float num32 = m21 * m44 - m24 * m41;
            float num31 = m21 * m43 - m23 * m41;
            float num30 = m21 * m42 - m22 * m41;
            M13 = (m12 * num35 - m13 * num34 + m14 * num33) * num;
            M23 = -(m11 * num35 - m13 * num32 + m14 * num31) * num;
            M33 = (m11 * num34 - m12 * num32 + m14 * num30) * num;
            M43 = -(m11 * num33 - m12 * num31 + m13 * num30) * num;
            float num29 = m23 * m34 - m24 * m33;
            float num28 = m22 * m34 - m24 * m32;
            float num27 = m22 * m33 - m23 * m32;
            float num26 = m21 * m34 - m24 * m31;
            float num25 = m21 * m33 - m23 * m31;
            float num24 = m21 * m32 - m22 * m31;
            M14 = -(m12 * num29 - m13 * num28 + m14 * num27) * num;
            M24 = (m11 * num29 - m13 * num26 + m14 * num25) * num;
            M34 = -(m11 * num28 - m12 * num26 + m14 * num24) * num;
            M44 = (m11 * num27 - m12 * num25 + m13 * num24) * num;
        }

        /// <summary>
        /// Decomposes the matrix into its scalar, rotational, and translational elements.
        /// </summary>
        /// <param name="scale">When the method completes, contains the scalar element of the matrix.</param>
        /// <param name="rotation">When the method completes, contains the translational element of the matrix.</param>
        /// <param name="translation">When the method completes, contains the rotational element of the matrix.</param>
        /// <returns><c>true</c> if the decomposition was successful; otherwise, <c>false</c>.</returns>
        public bool Decompose(out Vector3 scale, out Quaternion rotation, out Vector3 translation)
        {
            bool flag = true;
            fixed (float* numRef = &scale.X)
            {
                int num;
                int num3;
                int num4;
                VectorBasis basis2;
                Vector3** vectorPtr = (Vector3**)&basis2;
                Matrix identity = Identity;
                CanonicalBasis basis = new CanonicalBasis();
                Vector3* vectorPtr2 = &basis.Row0;
                basis.Row0 = new Vector3(1f, 0f, 0f);
                basis.Row1 = new Vector3(0f, 1f, 0f);
                basis.Row2 = new Vector3(0f, 0f, 1f);
                translation.X = this.M41;
                translation.Y = this.M42;
                translation.Z = this.M43;
                *((float**)(vectorPtr + 1)) = &identity.M11;
                *((float**)(vectorPtr + 1)) = &identity.M21;
                *((float**)(vectorPtr + 2)) = &identity.M31;
                **(vectorPtr + 1) = new Vector3(this.M11, this.M12, this.M13);
                **(vectorPtr + 1) = new Vector3(this.M21, this.M22, this.M23);
                **(vectorPtr + 2) = new Vector3(this.M31, this.M32, this.M33);
                scale.X = (*(vectorPtr + 1))->Length();
                scale.Y = (*(vectorPtr + 1))->Length();
                scale.Z = (*(vectorPtr + 2))->Length();
                float num11 = numRef[0];
                float num10 = numRef[4];
                float num7 = numRef[8];
                if (num11 < num10)
                {
                    if (num10 < num7)
                    {
                        num = 2;
                        num3 = 1;
                        num4 = 0;
                    }
                    else
                    {
                        num = 1;
                        if (num11 < num7)
                        {
                            num3 = 2;
                            num4 = 0;
                        }
                        else
                        {
                            num3 = 0;
                            num4 = 2;
                        }
                    }
                }
                else if (num11 < num7)
                {
                    num = 2;
                    num3 = 0;
                    num4 = 1;
                }
                else
                {
                    num = 0;
                    if (num10 < num7)
                    {
                        num3 = 2;
                        num4 = 1;
                    }
                    else
                    {
                        num3 = 1;
                        num4 = 2;
                    }
                }
                if (numRef[num * 4] < 0.0001f)
                {
                    **(vectorPtr + num * sizeof(Vector3*)) = vectorPtr2[(int)(num * sizeof(Vector3))];
                }
                (*(vectorPtr + num * sizeof(Vector3*)))->Normalize();
                if (numRef[num3 * 4] < 0.0001f)
                {
                    uint num5;
                    float num9 = Math.Abs((*(vectorPtr + num * sizeof(Vector3*)))->X);
                    float num8 = Math.Abs((*(vectorPtr + num * sizeof(Vector3*)))->Y);
                    float num6 = Math.Abs((*(vectorPtr + num * sizeof(Vector3*)))->Z);
                    if (num9 < num8)
                    {
                        if (num8 < num6)
                        {
                            num5 = 0;
                        }
                        else if (num9 < num6)
                        {
                            num5 = 0;
                        }
                        else
                        {
                            num5 = 2;
                        }
                    }
                    else if (num9 < num6)
                    {
                        num5 = 1;
                    }
                    else if (num8 < num6)
                    {
                        num5 = 1;
                    }
                    else
                    {
                        num5 = 2;
                    }
                    *(vectorPtr2 + num5 * sizeof(Vector3)) =
                        Vector3.Cross(**(vectorPtr + num3 * sizeof(Vector3*)), **(vectorPtr + num * sizeof(Vector3*)));
                }
                (*(((Vector3**)(vectorPtr + (num3 * sizeof(Vector3*))))))->Normalize();
                if (numRef[num4 * 4] < 0.0001f)
                {
                    **(vectorPtr + num3 * sizeof(Vector3*)) =
                        Vector3.Cross(**(vectorPtr + num4 * sizeof(Vector3*)), **(vectorPtr + num * sizeof(Vector3*)));
                }
                (*(((Vector3**)(vectorPtr + (num4 * sizeof(Vector3*))))))->Normalize();
                float num2 = identity.Determinant();
                if (num2 < 0f)
                {
                    numRef[num * 4] = -numRef[num * 4];
                    **(vectorPtr + num * sizeof(Vector3*)) = -(**(vectorPtr + num * sizeof(Vector3*)));
                    num2 = -num2;
                }
                num2--;
                num2 *= num2;
                if (0.0001f < num2)
                {
                    rotation = Quaternion.Identity;
                    flag = false;
                }
                else
                {
                    Quaternion.RotationMatrix(ref identity, out rotation);
                }
            }
            return flag;


        }

        /// <summary>
        /// Calculates the determinant of the matrix.
        /// </summary>
        /// <returns>The determinant of the matrix.</returns>
        public float Determinant()
        {
            float temp1 = (M33 * M44) - (M34 * M43);
            float temp2 = (M32 * M44) - (M34 * M42);
            float temp3 = (M32 * M43) - (M33 * M42);
            float temp4 = (M31 * M44) - (M34 * M41);
            float temp5 = (M31 * M43) - (M33 * M41);
            float temp6 = (M31 * M42) - (M32 * M41);

            return ((((M11 * (((M22 * temp1) - (M23 * temp2)) + (M24 * temp3))) - (M12 * (((M21 * temp1) -
                (M23 * temp4)) + (M24 * temp5)))) + (M13 * (((M21 * temp2) - (M22 * temp4)) + (M24 * temp6)))) -
                (M14 * (((M21 * temp3) - (M22 * temp5)) + (M23 * temp6))));
        }
        #endregion

        #region Static Methods(静态方法)

        /// <summary>
        /// Determines the sum of two matrices.
        /// </summary>
        /// <param name="left">The first matrix to add.</param>
        /// <param name="right">The second matrix to add.</param>
        /// <returns>The sum of the two matrices.</returns>
        public static Matrix Add(Matrix left, Matrix right)
        {
            Matrix result;
            result.M11 = left.M11 + right.M11;
            result.M12 = left.M12 + right.M12;
            result.M13 = left.M13 + right.M13;
            result.M14 = left.M14 + right.M14;
            result.M21 = left.M21 + right.M21;
            result.M22 = left.M22 + right.M22;
            result.M23 = left.M23 + right.M23;
            result.M24 = left.M24 + right.M24;
            result.M31 = left.M31 + right.M31;
            result.M32 = left.M32 + right.M32;
            result.M33 = left.M33 + right.M33;
            result.M34 = left.M34 + right.M34;
            result.M41 = left.M41 + right.M41;
            result.M42 = left.M42 + right.M42;
            result.M43 = left.M43 + right.M43;
            result.M44 = left.M44 + right.M44;
            return result;
        }

        /// <summary>
        /// Determines the sum of two matrices.
        /// </summary>
        /// <param name="left">The first matrix to add.</param>
        /// <param name="right">The second matrix to add.</param>
        /// <param name="result">When the method completes, contains the sum of the two matrices.</param>
        public static void Add(ref Matrix left, ref Matrix right, out Matrix result)
        {
            Matrix r;
            r.M11 = left.M11 + right.M11;
            r.M12 = left.M12 + right.M12;
            r.M13 = left.M13 + right.M13;
            r.M14 = left.M14 + right.M14;
            r.M21 = left.M21 + right.M21;
            r.M22 = left.M22 + right.M22;
            r.M23 = left.M23 + right.M23;
            r.M24 = left.M24 + right.M24;
            r.M31 = left.M31 + right.M31;
            r.M32 = left.M32 + right.M32;
            r.M33 = left.M33 + right.M33;
            r.M34 = left.M34 + right.M34;
            r.M41 = left.M41 + right.M41;
            r.M42 = left.M42 + right.M42;
            r.M43 = left.M43 + right.M43;
            r.M44 = left.M44 + right.M44;

            result = r;
        }

        /// <summary>
        /// Determines the difference between two matrices.
        /// </summary>
        /// <param name="left">The first matrix to subtract.</param>
        /// <param name="right">The second matrix to subtract.</param>
        /// <returns>The difference between the two matrices.</returns>
        public static Matrix Subtract(Matrix left, Matrix right)
        {
            Matrix result;
            result.M11 = left.M11 - right.M11;
            result.M12 = left.M12 - right.M12;
            result.M13 = left.M13 - right.M13;
            result.M14 = left.M14 - right.M14;
            result.M21 = left.M21 - right.M21;
            result.M22 = left.M22 - right.M22;
            result.M23 = left.M23 - right.M23;
            result.M24 = left.M24 - right.M24;
            result.M31 = left.M31 - right.M31;
            result.M32 = left.M32 - right.M32;
            result.M33 = left.M33 - right.M33;
            result.M34 = left.M34 - right.M34;
            result.M41 = left.M41 - right.M41;
            result.M42 = left.M42 - right.M42;
            result.M43 = left.M43 - right.M43;
            result.M44 = left.M44 - right.M44;
            return result;
        }

        /// <summary>
        /// Determines the difference between two matrices.
        /// </summary>
        /// <param name="left">The first matrix to subtract.</param>
        /// <param name="right">The second matrix to subtract.</param>
        /// <param name="result">When the method completes, contains the difference between the two matrices.</param>
        public static void Subtract(ref Matrix left, ref Matrix right, out Matrix result)
        {
            Matrix r;
            r.M11 = left.M11 - right.M11;
            r.M12 = left.M12 - right.M12;
            r.M13 = left.M13 - right.M13;
            r.M14 = left.M14 - right.M14;
            r.M21 = left.M21 - right.M21;
            r.M22 = left.M22 - right.M22;
            r.M23 = left.M23 - right.M23;
            r.M24 = left.M24 - right.M24;
            r.M31 = left.M31 - right.M31;
            r.M32 = left.M32 - right.M32;
            r.M33 = left.M33 - right.M33;
            r.M34 = left.M34 - right.M34;
            r.M41 = left.M41 - right.M41;
            r.M42 = left.M42 - right.M42;
            r.M43 = left.M43 - right.M43;
            r.M44 = left.M44 - right.M44;

            result = r;
        }


        /// <summary>
        /// Determines the product of two matrices.
        /// </summary>
        /// <param name="left">The first matrix to multiply.</param>
        /// <param name="right">The second matrix to multiply.</param>
        /// <returns>The product of the two matrices.</returns>
        /// <remarks>The result represents the transformation M1 followed by the transformation M2 (Out = M1 * M2).</remarks>
        public static Matrix Multiply(Matrix left, Matrix right)
        {
            Matrix result;
            result.M11 = (left.M11 * right.M11) + (left.M12 * right.M21) + (left.M13 * right.M31) + (left.M14 * right.M41);
            result.M12 = (left.M11 * right.M12) + (left.M12 * right.M22) + (left.M13 * right.M32) + (left.M14 * right.M42);
            result.M13 = (left.M11 * right.M13) + (left.M12 * right.M23) + (left.M13 * right.M33) + (left.M14 * right.M43);
            result.M14 = (left.M11 * right.M14) + (left.M12 * right.M24) + (left.M13 * right.M34) + (left.M14 * right.M44);
            result.M21 = (left.M21 * right.M11) + (left.M22 * right.M21) + (left.M23 * right.M31) + (left.M24 * right.M41);
            result.M22 = (left.M21 * right.M12) + (left.M22 * right.M22) + (left.M23 * right.M32) + (left.M24 * right.M42);
            result.M23 = (left.M21 * right.M13) + (left.M22 * right.M23) + (left.M23 * right.M33) + (left.M24 * right.M43);
            result.M24 = (left.M21 * right.M14) + (left.M22 * right.M24) + (left.M23 * right.M34) + (left.M24 * right.M44);
            result.M31 = (left.M31 * right.M11) + (left.M32 * right.M21) + (left.M33 * right.M31) + (left.M34 * right.M41);
            result.M32 = (left.M31 * right.M12) + (left.M32 * right.M22) + (left.M33 * right.M32) + (left.M34 * right.M42);
            result.M33 = (left.M31 * right.M13) + (left.M32 * right.M23) + (left.M33 * right.M33) + (left.M34 * right.M43);
            result.M34 = (left.M31 * right.M14) + (left.M32 * right.M24) + (left.M33 * right.M34) + (left.M34 * right.M44);
            result.M41 = (left.M41 * right.M11) + (left.M42 * right.M21) + (left.M43 * right.M31) + (left.M44 * right.M41);
            result.M42 = (left.M41 * right.M12) + (left.M42 * right.M22) + (left.M43 * right.M32) + (left.M44 * right.M42);
            result.M43 = (left.M41 * right.M13) + (left.M42 * right.M23) + (left.M43 * right.M33) + (left.M44 * right.M43);
            result.M44 = (left.M41 * right.M14) + (left.M42 * right.M24) + (left.M43 * right.M34) + (left.M44 * right.M44);
            return result;
        }

        /// <summary>
        /// Determines the product of two matrices.
        /// </summary>
        /// <param name="left">The first matrix to multiply.</param>
        /// <param name="right">The second matrix to multiply.</param>
        /// <param name="result">The product of the two matrices.</param>
        /// <remarks>The result represents the transformation M1 followed by the transformation M2 (Out = M1 * M2).</remarks>
        public static void Multiply(ref Matrix left, ref Matrix right, out Matrix result)
        {
            Matrix r;
            r.M11 = (left.M11 * right.M11) + (left.M12 * right.M21) + (left.M13 * right.M31) + (left.M14 * right.M41);
            r.M12 = (left.M11 * right.M12) + (left.M12 * right.M22) + (left.M13 * right.M32) + (left.M14 * right.M42);
            r.M13 = (left.M11 * right.M13) + (left.M12 * right.M23) + (left.M13 * right.M33) + (left.M14 * right.M43);
            r.M14 = (left.M11 * right.M14) + (left.M12 * right.M24) + (left.M13 * right.M34) + (left.M14 * right.M44);
            r.M21 = (left.M21 * right.M11) + (left.M22 * right.M21) + (left.M23 * right.M31) + (left.M24 * right.M41);
            r.M22 = (left.M21 * right.M12) + (left.M22 * right.M22) + (left.M23 * right.M32) + (left.M24 * right.M42);
            r.M23 = (left.M21 * right.M13) + (left.M22 * right.M23) + (left.M23 * right.M33) + (left.M24 * right.M43);
            r.M24 = (left.M21 * right.M14) + (left.M22 * right.M24) + (left.M23 * right.M34) + (left.M24 * right.M44);
            r.M31 = (left.M31 * right.M11) + (left.M32 * right.M21) + (left.M33 * right.M31) + (left.M34 * right.M41);
            r.M32 = (left.M31 * right.M12) + (left.M32 * right.M22) + (left.M33 * right.M32) + (left.M34 * right.M42);
            r.M33 = (left.M31 * right.M13) + (left.M32 * right.M23) + (left.M33 * right.M33) + (left.M34 * right.M43);
            r.M34 = (left.M31 * right.M14) + (left.M32 * right.M24) + (left.M33 * right.M34) + (left.M34 * right.M44);
            r.M41 = (left.M41 * right.M11) + (left.M42 * right.M21) + (left.M43 * right.M31) + (left.M44 * right.M41);
            r.M42 = (left.M41 * right.M12) + (left.M42 * right.M22) + (left.M43 * right.M32) + (left.M44 * right.M42);
            r.M43 = (left.M41 * right.M13) + (left.M42 * right.M23) + (left.M43 * right.M33) + (left.M44 * right.M43);
            r.M44 = (left.M41 * right.M14) + (left.M42 * right.M24) + (left.M43 * right.M34) + (left.M44 * right.M44);

            result = r;
        }

        /// <summary>
        /// Determines the products of two arrays of matrices.
        /// </summary>
        /// <param name="left">The first matrix array to multiply.</param>
        /// <param name="right">The second matrix array to multiply.</param>
        /// <param name="result">The array of products of the two matrices.</param>
        public static void Multiply(Matrix* left, Matrix* right, Matrix* result, int count) 
        {
            while (--count > 0)
            {
                Matrix* r = result + count;
                Matrix* a = left + count;
                Matrix* b = right + count;

                r->M11 = (a->M11 * b->M11) + (a->M12 * b->M21) + (a->M13 * b->M31) + (a->M14 * b->M41);
                r->M12 = (a->M11 * b->M12) + (a->M12 * b->M22) + (a->M13 * b->M32) + (a->M14 * b->M42);
                r->M13 = (a->M11 * b->M13) + (a->M12 * b->M23) + (a->M13 * b->M33) + (a->M14 * b->M43);
                r->M14 = (a->M11 * b->M14) + (a->M12 * b->M24) + (a->M13 * b->M34) + (a->M14 * b->M44);
                r->M21 = (a->M21 * b->M11) + (a->M22 * b->M21) + (a->M23 * b->M31) + (a->M24 * b->M41);
                r->M22 = (a->M21 * b->M12) + (a->M22 * b->M22) + (a->M23 * b->M32) + (a->M24 * b->M42);
                r->M23 = (a->M21 * b->M13) + (a->M22 * b->M23) + (a->M23 * b->M33) + (a->M24 * b->M43);
                r->M24 = (a->M21 * b->M14) + (a->M22 * b->M24) + (a->M23 * b->M34) + (a->M24 * b->M44);
                r->M31 = (a->M31 * b->M11) + (a->M32 * b->M21) + (a->M33 * b->M31) + (a->M34 * b->M41);
                r->M32 = (a->M31 * b->M12) + (a->M32 * b->M22) + (a->M33 * b->M32) + (a->M34 * b->M42);
                r->M33 = (a->M31 * b->M13) + (a->M32 * b->M23) + (a->M33 * b->M33) + (a->M34 * b->M43);
                r->M34 = (a->M31 * b->M14) + (a->M32 * b->M24) + (a->M33 * b->M34) + (a->M34 * b->M44);
                r->M41 = (a->M41 * b->M11) + (a->M42 * b->M21) + (a->M43 * b->M31) + (a->M44 * b->M41);
                r->M42 = (a->M41 * b->M12) + (a->M42 * b->M22) + (a->M43 * b->M32) + (a->M44 * b->M42);
                r->M43 = (a->M41 * b->M13) + (a->M42 * b->M23) + (a->M43 * b->M33) + (a->M44 * b->M43);
                r->M44 = (a->M41 * b->M14) + (a->M42 * b->M24) + (a->M43 * b->M34) + (a->M44 * b->M44);


            }
        }

        /// <summary>
        /// Determines the products of two arrays of matrices.
        /// </summary>
        /// <param name="left">The first matrix array to multiply.</param>
        /// <param name="right">The second matrix array to multiply.</param>
        /// <param name="result">The array of products of the two matrices.</param>
        /// <param name="offset">The offset at which to begin the multiplication.</param>
        /// <param name="count">The number of matrices to multiply, or 0 to process the entire array.</param>
        public static void Multiply(Matrix[] left, Matrix[] right, Matrix[] result, int offset, int count)
        {
            fixed (Matrix* lp = &left[offset], rp = &right[offset], resp = &result[offset])
            {
                Multiply(lp, rp, resp, count);
            }
        }

        /// <summary>
        /// Determines the products of two arrays of matrices.
        /// </summary>
        /// <param name="left">The first matrix array to multiply.</param>
        /// <param name="right">The second matrix array to multiply.</param>
        /// <param name="result">The array of products of the two matrices.</param>
        public static void Multiply(Matrix[] left, Matrix[] right, Matrix[] result)
        {
            Multiply(left, right, result, 0, 0);
        }

        /// <summary>
        /// Determines the products of of an array of matrices by a single matrix.
        /// </summary>
        /// <param name="left">The first matrix array to multiply.</param>
        /// <param name="right">The matrix to multiply the matrices in the array by.</param>
        /// <param name="result">The array of products of the matrices.</param>
        /// <param name="offset">The offset at which to begin the multiplication.</param>
        /// <param name="count">The number of matrices to multiply, or 0 to process the entire array.</param>
        public static void Multiply(Matrix[] left, Matrix right, Matrix[] result, int offset, int count)
        {
            fixed (Matrix* lp = &left[offset], resp = &result[offset])
            {
                Multiply(lp, &right, resp, count);
            }
        }

        /// <summary>
        /// Determines the products of of an array of matrices by a single matrix.
        /// </summary>
        /// <param name="left">The first matrix array to multiply.</param>
        /// <param name="right">The matrix to multiply the matrices in the array by.</param>
        /// <param name="result">The array of products of the matrices.</param>
        public static void Multiply(Matrix[] left, Matrix right, Matrix[] result)
        {
            Multiply(left, right, result, 0, 0);
        }

        /// <summary>
        /// Scales a matrix by the given value.
        /// </summary>
        /// <param name="left">The matrix to scale.</param>
        /// <param name="right">The amount by which to scale.</param>
        /// <returns>The scaled matrix.</returns>
        public static Matrix Multiply(Matrix left, float right)
        {
            Matrix result;
            result.M11 = left.M11 * right;
            result.M12 = left.M12 * right;
            result.M13 = left.M13 * right;
            result.M14 = left.M14 * right;
            result.M21 = left.M21 * right;
            result.M22 = left.M22 * right;
            result.M23 = left.M23 * right;
            result.M24 = left.M24 * right;
            result.M31 = left.M31 * right;
            result.M32 = left.M32 * right;
            result.M33 = left.M33 * right;
            result.M34 = left.M34 * right;
            result.M41 = left.M41 * right;
            result.M42 = left.M42 * right;
            result.M43 = left.M43 * right;
            result.M44 = left.M44 * right;
            return result;
        }

        /// <summary>
        /// Scales a matrix by the given value.
        /// </summary>
        /// <param name="left">The matrix to scale.</param>
        /// <param name="right">The amount by which to scale.</param>
        /// <param name="result">When the method completes, contains the scaled matrix.</param>
        public static void Multiply(ref Matrix left, float right, out Matrix result)
        {
            Matrix r;
            r.M11 = left.M11 * right;
            r.M12 = left.M12 * right;
            r.M13 = left.M13 * right;
            r.M14 = left.M14 * right;
            r.M21 = left.M21 * right;
            r.M22 = left.M22 * right;
            r.M23 = left.M23 * right;
            r.M24 = left.M24 * right;
            r.M31 = left.M31 * right;
            r.M32 = left.M32 * right;
            r.M33 = left.M33 * right;
            r.M34 = left.M34 * right;
            r.M41 = left.M41 * right;
            r.M42 = left.M42 * right;
            r.M43 = left.M43 * right;
            r.M44 = left.M44 * right;

            result = r;
        }

        /// <summary>
        /// Determines the quotient of two matrices.
        /// </summary>
        /// <param name="left">The first matrix to divide.</param>
        /// <param name="right">The second matrix to divide.</param>
        /// <returns>The quotient of the two matrices.</returns>
        public static Matrix Divide(Matrix left, Matrix right)
        {
            Matrix result;
            result.M11 = left.M11 / right.M11;
            result.M12 = left.M12 / right.M12;
            result.M13 = left.M13 / right.M13;
            result.M14 = left.M14 / right.M14;
            result.M21 = left.M21 / right.M21;
            result.M22 = left.M22 / right.M22;
            result.M23 = left.M23 / right.M23;
            result.M24 = left.M24 / right.M24;
            result.M31 = left.M31 / right.M31;
            result.M32 = left.M32 / right.M32;
            result.M33 = left.M33 / right.M33;
            result.M34 = left.M34 / right.M34;
            result.M41 = left.M41 / right.M41;
            result.M42 = left.M42 / right.M42;
            result.M43 = left.M43 / right.M43;
            result.M44 = left.M44 / right.M44;
            return result;
        }

        /// <summary>
        /// Determines the quotient of two matrices.
        /// </summary>
        /// <param name="left">The first matrix to divide.</param>
        /// <param name="right">The second matrix to divide.</param>
        /// <param name="result">When the method completes, contains the quotient of the two matrices.</param>
        public static void Divide(ref Matrix left, ref Matrix right, out Matrix result)
        {
            Matrix r;
            r.M11 = left.M11 / right.M11;
            r.M12 = left.M12 / right.M12;
            r.M13 = left.M13 / right.M13;
            r.M14 = left.M14 / right.M14;
            r.M21 = left.M21 / right.M21;
            r.M22 = left.M22 / right.M22;
            r.M23 = left.M23 / right.M23;
            r.M24 = left.M24 / right.M24;
            r.M31 = left.M31 / right.M31;
            r.M32 = left.M32 / right.M32;
            r.M33 = left.M33 / right.M33;
            r.M34 = left.M34 / right.M34;
            r.M41 = left.M41 / right.M41;
            r.M42 = left.M42 / right.M42;
            r.M43 = left.M43 / right.M43;
            r.M44 = left.M44 / right.M44;

            result = r;
        }

        /// <summary>
        /// Scales a matrix by the given value.
        /// </summary>
        /// <param name="left">The matrix to scale.</param>
        /// <param name="right">The amount by which to scale.</param>
        /// <returns>The scaled matrix.</returns>
        public static Matrix Divide(Matrix left, float right)
        {
            Matrix result;
            float inv = 1.0f / right;

            result.M11 = left.M11 * inv;
            result.M12 = left.M12 * inv;
            result.M13 = left.M13 * inv;
            result.M14 = left.M14 * inv;
            result.M21 = left.M21 * inv;
            result.M22 = left.M22 * inv;
            result.M23 = left.M23 * inv;
            result.M24 = left.M24 * inv;
            result.M31 = left.M31 * inv;
            result.M32 = left.M32 * inv;
            result.M33 = left.M33 * inv;
            result.M34 = left.M34 * inv;
            result.M41 = left.M41 * inv;
            result.M42 = left.M42 * inv;
            result.M43 = left.M43 * inv;
            result.M44 = left.M44 * inv;
            return result;
        }

        /// <summary>
        /// Scales a matrix by the given value.
        /// </summary>
        /// <param name="left">The matrix to scale.</param>
        /// <param name="right">The amount by which to scale.</param>
        /// <param name="result">When the method completes, contains the scaled matrix.</param>
        public static void Divide(ref Matrix left, float right, out Matrix result)
        {
            float inv = 1.0f / right;

            Matrix r;
            r.M11 = left.M11 * inv;
            r.M12 = left.M12 * inv;
            r.M13 = left.M13 * inv;
            r.M14 = left.M14 * inv;
            r.M21 = left.M21 * inv;
            r.M22 = left.M22 * inv;
            r.M23 = left.M23 * inv;
            r.M24 = left.M24 * inv;
            r.M31 = left.M31 * inv;
            r.M32 = left.M32 * inv;
            r.M33 = left.M33 * inv;
            r.M34 = left.M34 * inv;
            r.M41 = left.M41 * inv;
            r.M42 = left.M42 * inv;
            r.M43 = left.M43 * inv;
            r.M44 = left.M44 * inv;

            result = r;
        }

        /// <summary>
        /// Negates a matrix.
        /// </summary>
        /// <param name="matrix">The matrix to be negated.</param>
        /// <returns>The negated matrix.</returns>
        public static Matrix Negate(Matrix matrix)
        {
            Matrix result;
            result.M11 = -matrix.M11;
            result.M12 = -matrix.M12;
            result.M13 = -matrix.M13;
            result.M14 = -matrix.M14;
            result.M21 = -matrix.M21;
            result.M22 = -matrix.M22;
            result.M23 = -matrix.M23;
            result.M24 = -matrix.M24;
            result.M31 = -matrix.M31;
            result.M32 = -matrix.M32;
            result.M33 = -matrix.M33;
            result.M34 = -matrix.M34;
            result.M41 = -matrix.M41;
            result.M42 = -matrix.M42;
            result.M43 = -matrix.M43;
            result.M44 = -matrix.M44;
            return result;
        }

        /// <summary>
        /// Negates a matrix.
        /// </summary>
        /// <param name="matrix">The matrix to be negated.</param>
        /// <param name="result">When the method completes, contains the negated matrix.</param>
        public static void Negate(ref Matrix matrix, out Matrix result)
        {
            Matrix r;
            r.M11 = -matrix.M11;
            r.M12 = -matrix.M12;
            r.M13 = -matrix.M13;
            r.M14 = -matrix.M14;
            r.M21 = -matrix.M21;
            r.M22 = -matrix.M22;
            r.M23 = -matrix.M23;
            r.M24 = -matrix.M24;
            r.M31 = -matrix.M31;
            r.M32 = -matrix.M32;
            r.M33 = -matrix.M33;
            r.M34 = -matrix.M34;
            r.M41 = -matrix.M41;
            r.M42 = -matrix.M42;
            r.M43 = -matrix.M43;
            r.M44 = -matrix.M44;

            result = r;
        }

        /// <summary>
        /// Performs a linear interpolation between two matricies.
        /// </summary>
        /// <param name="start">Start matrix.</param>
        /// <param name="end">End matrix.</param>
        /// <param name="amount">Value between 0 and 1 indicating the weight of <paramref name="end"/>.</param>
        /// <returns>The linear interpolation of the two matrices.</returns>
        /// <remarks>
        /// This method performs the linear interpolation based on the following formula.
        /// <code>start + (end - start) * amount</code>
        /// Passing <paramref name="amount"/> a value of 0 will cause <paramref name="start"/> to be returned; a value of 1 will cause <paramref name="end"/> to be returned. 
        /// </remarks>
        public static Matrix Lerp(Matrix value1, Matrix value2, float amount)
        {
            Matrix result;
            result.M11 = value1.M11 + ((value2.M11 - value1.M11) * amount);
            result.M12 = value1.M12 + ((value2.M12 - value1.M12) * amount);
            result.M13 = value1.M13 + ((value2.M13 - value1.M13) * amount);
            result.M14 = value1.M14 + ((value2.M14 - value1.M14) * amount);
            result.M21 = value1.M21 + ((value2.M21 - value1.M21) * amount);
            result.M22 = value1.M22 + ((value2.M22 - value1.M22) * amount);
            result.M23 = value1.M23 + ((value2.M23 - value1.M23) * amount);
            result.M24 = value1.M24 + ((value2.M24 - value1.M24) * amount);
            result.M31 = value1.M31 + ((value2.M31 - value1.M31) * amount);
            result.M32 = value1.M32 + ((value2.M32 - value1.M32) * amount);
            result.M33 = value1.M33 + ((value2.M33 - value1.M33) * amount);
            result.M34 = value1.M34 + ((value2.M34 - value1.M34) * amount);
            result.M41 = value1.M41 + ((value2.M41 - value1.M41) * amount);
            result.M42 = value1.M42 + ((value2.M42 - value1.M42) * amount);
            result.M43 = value1.M43 + ((value2.M43 - value1.M43) * amount);
            result.M44 = value1.M44 + ((value2.M44 - value1.M44) * amount);
            return result;
        }

        /// <summary>
        /// Performs a linear interpolation between two matricies.
        /// </summary>
        /// <param name="start">Start matrix.</param>
        /// <param name="end">End matrix.</param>
        /// <param name="amount">Value between 0 and 1 indicating the weight of <paramref name="end"/>.</param>
        /// <param name="result">When the method completes, contains the linear interpolation of the two matricies.</param>
        /// <remarks>
        /// This method performs the linear interpolation based on the following formula.
        /// <code>start + (end - start) * amount</code>
        /// Passing <paramref name="amount"/> a value of 0 will cause <paramref name="start"/> to be returned; a value of 1 will cause <paramref name="end"/> to be returned. 
        /// </remarks>
        public static void Lerp(ref Matrix value1, ref Matrix value2, float amount, out Matrix result)
        {
            Matrix r;
            r.M11 = value1.M11 + ((value2.M11 - value1.M11) * amount);
            r.M12 = value1.M12 + ((value2.M12 - value1.M12) * amount);
            r.M13 = value1.M13 + ((value2.M13 - value1.M13) * amount);
            r.M14 = value1.M14 + ((value2.M14 - value1.M14) * amount);
            r.M21 = value1.M21 + ((value2.M21 - value1.M21) * amount);
            r.M22 = value1.M22 + ((value2.M22 - value1.M22) * amount);
            r.M23 = value1.M23 + ((value2.M23 - value1.M23) * amount);
            r.M24 = value1.M24 + ((value2.M24 - value1.M24) * amount);
            r.M31 = value1.M31 + ((value2.M31 - value1.M31) * amount);
            r.M32 = value1.M32 + ((value2.M32 - value1.M32) * amount);
            r.M33 = value1.M33 + ((value2.M33 - value1.M33) * amount);
            r.M34 = value1.M34 + ((value2.M34 - value1.M34) * amount);
            r.M41 = value1.M41 + ((value2.M41 - value1.M41) * amount);
            r.M42 = value1.M42 + ((value2.M42 - value1.M42) * amount);
            r.M43 = value1.M43 + ((value2.M43 - value1.M43) * amount);
            r.M44 = value1.M44 + ((value2.M44 - value1.M44) * amount);

            result = r;
        }

        /// <summary>
        /// Creates a spherical billboard that rotates around a specified object position.
        /// </summary>
        /// <param name="objectPosition">The position of the object around which the billboard will rotate.</param>
        /// <param name="cameraPosition">The position of the camera.</param>
        /// <param name="cameraUpVector">The up vector of the camera.</param>
        /// <param name="cameraForwardVector">The forward vector of the camera.</param>
        /// <returns>The created billboard matrix.</returns>
        public static Matrix Billboard(Vector3 objectPosition, Vector3 cameraPosition, Vector3 cameraUpVector, Vector3 cameraForwardVector)
        {
            Matrix result;
            Vector3 difference = objectPosition - cameraPosition;
            Vector3 crossed;
            Vector3 final;

            float lengthSq = difference.LengthSquared();
            if (lengthSq < 0.0001f)
                difference = -cameraForwardVector;
            else
                difference *= (float)(1.0f / Math.Sqrt(lengthSq));

            Vector3.Cross(ref cameraUpVector, ref  difference, out crossed);
            crossed.Normalize();
            Vector3.Cross(ref  difference, ref  crossed, out final);

            result.M11 = final.X;
            result.M12 = final.Y;
            result.M13 = final.Z;
            result.M14 = 0.0f;
            result.M21 = crossed.X;
            result.M22 = crossed.Y;
            result.M23 = crossed.Z;
            result.M24 = 0.0f;
            result.M31 = difference.X;
            result.M32 = difference.Y;
            result.M33 = difference.Z;
            result.M34 = 0.0f;
            result.M41 = objectPosition.X;
            result.M42 = objectPosition.Y;
            result.M43 = objectPosition.Z;
            result.M44 = 1.0f;

            return result;
        }

        /// <summary>
        /// Creates a spherical billboard that rotates around a specified object position.
        /// </summary>
        /// <param name="objectPosition">The position of the object around which the billboard will rotate.</param>
        /// <param name="cameraPosition">The position of the camera.</param>
        /// <param name="cameraUpVector">The up vector of the camera.</param>
        /// <param name="cameraForwardVector">The forward vector of the camera.</param>
        /// <param name="result">When the method completes, contains the created billboard matrix.</param>
        public static void Billboard(ref Vector3 objectPosition, ref Vector3 cameraPosition, ref Vector3 cameraUpVector,
            ref Vector3 cameraForwardVector, out Matrix result)
        {
            Vector3 difference = objectPosition - cameraPosition;
            Vector3 crossed;
            Vector3 final;

            float lengthSq = difference.LengthSquared();
            if (lengthSq < 0.0001f)
                difference = -cameraForwardVector;
            else
                difference *= (float)(1.0f / Math.Sqrt(lengthSq));

            Vector3.Cross(ref cameraUpVector, ref difference, out crossed);
            crossed.Normalize();
            Vector3.Cross(ref difference, ref crossed, out final);

            result.M11 = final.X;
            result.M12 = final.Y;
            result.M13 = final.Z;
            result.M14 = 0.0f;
            result.M21 = crossed.X;
            result.M22 = crossed.Y;
            result.M23 = crossed.Z;
            result.M24 = 0.0f;
            result.M31 = difference.X;
            result.M32 = difference.Y;
            result.M33 = difference.Z;
            result.M34 = 0.0f;
            result.M41 = objectPosition.X;
            result.M42 = objectPosition.Y;
            result.M43 = objectPosition.Z;
            result.M44 = 1.0f;
        }

        public static Matrix ConstrainedBillboard(Vector3 objectPosition, Vector3 cameraPosition, Vector3 rotateAxis,
            Vector3? cameraForwardVector, Vector3? objectForwardVector)
        {
            float num;
            Vector3 vector;
            Matrix matrix;
            Vector3 vector2;
            Vector3 vector3;
            vector2.X = objectPosition.X - cameraPosition.X;
            vector2.Y = objectPosition.Y - cameraPosition.Y;
            vector2.Z = objectPosition.Z - cameraPosition.Z;
            float num2 = vector2.LengthSquared();
            if (num2 < 0.0001f)
            {
                vector2 = cameraForwardVector.HasValue ? -cameraForwardVector.Value : -Vector3.UnitZ;
            }
            else
            {
                Vector3.Multiply(ref vector2, (float)(1f / ((float)Math.Sqrt((double)num2))), out vector2);
            }
            Vector3 vector4 = rotateAxis;
            num = Vector3.Dot(ref rotateAxis, ref vector2);
            if (Math.Abs(num) > 0.9982547f)
            {
                if (objectForwardVector.HasValue)
                {
                    vector = objectForwardVector.Value;
                    num = Vector3.Dot(ref rotateAxis, ref vector);
                    if (Math.Abs(num) > 0.9982547f)
                    {
                        num = ((rotateAxis.X * -Vector3.UnitZ.X) + (rotateAxis.Y * -Vector3.UnitZ.Y)) + (rotateAxis.Z * -Vector3.UnitZ.Z);
                        vector = (Math.Abs(num) > 0.9982547f) ? Vector3.UnitX : -Vector3.UnitZ;
                    }
                }
                else
                {
                    num = ((rotateAxis.X * -Vector3.UnitZ.X) + (rotateAxis.Y * -Vector3.UnitZ.Y)) + (rotateAxis.Z * -Vector3.UnitZ.Z);
                    vector = (Math.Abs(num) > 0.9982547f) ? Vector3.UnitX : -Vector3.UnitZ;
                }
                Vector3.Cross(ref rotateAxis, ref vector, out vector3);
                vector3.Normalize();
                Vector3.Cross(ref vector3, ref rotateAxis, out vector);
                vector.Normalize();
            }
            else
            {
                Vector3.Cross(ref rotateAxis, ref vector2, out vector3);
                vector3.Normalize();
                Vector3.Cross(ref vector3, ref vector4, out vector);
                vector.Normalize();
            }
            matrix.M11 = vector3.X;
            matrix.M12 = vector3.Y;
            matrix.M13 = vector3.Z;
            matrix.M14 = 0f;
            matrix.M21 = vector4.X;
            matrix.M22 = vector4.Y;
            matrix.M23 = vector4.Z;
            matrix.M24 = 0f;
            matrix.M31 = vector.X;
            matrix.M32 = vector.Y;
            matrix.M33 = vector.Z;
            matrix.M34 = 0f;
            matrix.M41 = objectPosition.X;
            matrix.M42 = objectPosition.Y;
            matrix.M43 = objectPosition.Z;
            matrix.M44 = 1f;
            return matrix;
        }

        public static void ConstrainedBillboard(ref Vector3 objectPosition, ref Vector3 cameraPosition, ref Vector3 rotateAxis,
            Vector3? cameraForwardVector, Vector3? objectForwardVector, out Matrix result)
        {
            float num;
            Vector3 vector;
            Vector3 vector2;
            Vector3 vector3;
            vector2.X = objectPosition.X - cameraPosition.X;
            vector2.Y = objectPosition.Y - cameraPosition.Y;
            vector2.Z = objectPosition.Z - cameraPosition.Z;
            float num2 = vector2.LengthSquared();
            if (num2 < 0.0001f)
            {
                vector2 = cameraForwardVector.HasValue ? -cameraForwardVector.Value : -Vector3.UnitZ;
            }
            else
            {
                Vector3.Multiply(ref vector2, (float)(1f / ((float)Math.Sqrt((double)num2))), out vector2);
            }
            Vector3 vector4 = rotateAxis;
            num = Vector3.Dot(ref rotateAxis, ref vector2);
            if (Math.Abs(num) > 0.9982547f)
            {
                if (objectForwardVector.HasValue)
                {
                    vector = objectForwardVector.Value;
                    num = Vector3.Dot(ref rotateAxis, ref vector);
                    if (Math.Abs(num) > 0.9982547f)
                    {
                        num = ((rotateAxis.X * -Vector3.UnitZ.X) + (rotateAxis.Y * -Vector3.UnitZ.Y)) + (rotateAxis.Z * -Vector3.UnitZ.Z);
                        vector = (Math.Abs(num) > 0.9982547f) ? Vector3.UnitX : -Vector3.UnitZ;
                    }
                }
                else
                {
                    num = ((rotateAxis.X * -Vector3.UnitZ.X) + (rotateAxis.Y * -Vector3.UnitZ.Y)) + (rotateAxis.Z * -Vector3.UnitZ.Z);
                    vector = (Math.Abs(num) > 0.9982547f) ? Vector3.UnitX : -Vector3.UnitZ;
                }
                Vector3.Cross(ref rotateAxis, ref vector, out vector3);
                vector3.Normalize();
                Vector3.Cross(ref vector3, ref rotateAxis, out vector);
                vector.Normalize();
            }
            else
            {
                Vector3.Cross(ref rotateAxis, ref vector2, out vector3);
                vector3.Normalize();
                Vector3.Cross(ref vector3, ref vector4, out vector);
                vector.Normalize();
            }
            result.M11 = vector3.X;
            result.M12 = vector3.Y;
            result.M13 = vector3.Z;
            result.M14 = 0f;
            result.M21 = vector4.X;
            result.M22 = vector4.Y;
            result.M23 = vector4.Z;
            result.M24 = 0f;
            result.M31 = vector.X;
            result.M32 = vector.Y;
            result.M33 = vector.Z;
            result.M34 = 0f;
            result.M41 = objectPosition.X;
            result.M42 = objectPosition.Y;
            result.M43 = objectPosition.Z;
            result.M44 = 1f;
        }


        /// <summary>
        /// Creates a matrix that rotates around the x-axis.
        /// </summary>
        /// <param name="angle">Angle of rotation in radians. Angles are measured clockwise when looking along the rotation axis toward the origin.</param>
        /// <returns>The created rotation matrix.</returns>
        public static Matrix RotationX(float angle)
        {
            Matrix result;
            float cos = (float)Math.Cos(angle);
            float sin = (float)Math.Sin(angle);

            result.M11 = 1.0f;
            result.M12 = 0.0f;
            result.M13 = 0.0f;
            result.M14 = 0.0f;
            result.M21 = 0.0f;
            result.M22 = cos;
            result.M23 = sin;
            result.M24 = 0.0f;
            result.M31 = 0.0f;
            result.M32 = -sin;
            result.M33 = cos;
            result.M34 = 0.0f;
            result.M41 = 0.0f;
            result.M42 = 0.0f;
            result.M43 = 0.0f;
            result.M44 = 1.0f;

            return result;
        }

        /// <summary>
        /// Creates a matrix that rotates around the x-axis.
        /// </summary>
        /// <param name="angle">Angle of rotation in radians. Angles are measured clockwise when looking along the rotation axis toward the origin.</param>
        /// <param name="result">When the method completes, contains the created rotation matrix.</param>
        public static void RotationX(float angle, out Matrix result)
        {
            float cos = (float)Math.Cos(angle);
            float sin = (float)Math.Sin(angle);

            result.M11 = 1.0f;
            result.M12 = 0.0f;
            result.M13 = 0.0f;
            result.M14 = 0.0f;
            result.M21 = 0.0f;
            result.M22 = cos;
            result.M23 = sin;
            result.M24 = 0.0f;
            result.M31 = 0.0f;
            result.M32 = -sin;
            result.M33 = cos;
            result.M34 = 0.0f;
            result.M41 = 0.0f;
            result.M42 = 0.0f;
            result.M43 = 0.0f;
            result.M44 = 1.0f;
        }

        /// <summary>
        /// Creates a matrix that rotates around the y-axis.
        /// </summary>
        /// <param name="angle">Angle of rotation in radians. Angles are measured clockwise when looking along the rotation axis toward the origin.</param>
        /// <returns>The created rotation matrix.</returns>
        public static Matrix RotationY(float angle)
        {
            Matrix result;
            float cos = (float)Math.Cos(angle);
            float sin = (float)Math.Sin(angle);

            result.M11 = cos;
            result.M12 = 0.0f;
            result.M13 = -sin;
            result.M14 = 0.0f;
            result.M21 = 0.0f;
            result.M22 = 1.0f;
            result.M23 = 0.0f;
            result.M24 = 0.0f;
            result.M31 = sin;
            result.M32 = 0.0f;
            result.M33 = cos;
            result.M34 = 0.0f;
            result.M41 = 0.0f;
            result.M42 = 0.0f;
            result.M43 = 0.0f;
            result.M44 = 1.0f;

            return result;
        }

        /// <summary>
        /// Creates a matrix that rotates around the y-axis.
        /// </summary>
        /// <param name="angle">Angle of rotation in radians. Angles are measured clockwise when looking along the rotation axis toward the origin.</param>
        /// <param name="result">When the method completes, contains the created rotation matrix.</param>
        public static void RotationY(float angle, out Matrix result)
        {
            float cos = (float)Math.Cos(angle);
            float sin = (float)Math.Sin(angle);

            result.M11 = cos;
            result.M12 = 0.0f;
            result.M13 = -sin;
            result.M14 = 0.0f;
            result.M21 = 0.0f;
            result.M22 = 1.0f;
            result.M23 = 0.0f;
            result.M24 = 0.0f;
            result.M31 = sin;
            result.M32 = 0.0f;
            result.M33 = cos;
            result.M34 = 0.0f;
            result.M41 = 0.0f;
            result.M42 = 0.0f;
            result.M43 = 0.0f;
            result.M44 = 1.0f;
        }

        /// <summary>
        /// Creates a matrix that rotates around the z-axis.
        /// </summary>
        /// <param name="angle">Angle of rotation in radians. Angles are measured clockwise when looking along the rotation axis toward the origin.</param>
        /// <returns>The created rotation matrix.</returns>
        public static Matrix RotationZ(float angle)
        {
            Matrix result;
            float cos = (float)Math.Cos(angle);
            float sin = (float)Math.Sin(angle);

            result.M11 = cos;
            result.M12 = sin;
            result.M13 = 0.0f;
            result.M14 = 0.0f;
            result.M21 = -sin;
            result.M22 = cos;
            result.M23 = 0.0f;
            result.M24 = 0.0f;
            result.M31 = 0.0f;
            result.M32 = 0.0f;
            result.M33 = 1.0f;
            result.M34 = 0.0f;
            result.M41 = 0.0f;
            result.M42 = 0.0f;
            result.M43 = 0.0f;
            result.M44 = 1.0f;

            return result;
        }

        /// <summary>
        /// Creates a matrix that rotates around the z-axis.
        /// </summary>
        /// <param name="angle">Angle of rotation in radians. Angles are measured clockwise when looking along the rotation axis toward the origin.</param>
        /// <param name="result">When the method completes, contains the created rotation matrix.</param>
        public static void RotationZ(float angle, out Matrix result)
        {
            float cos = (float)Math.Cos(angle);
            float sin = (float)Math.Sin(angle);

            result.M11 = cos;
            result.M12 = sin;
            result.M13 = 0.0f;
            result.M14 = 0.0f;
            result.M21 = -sin;
            result.M22 = cos;
            result.M23 = 0.0f;
            result.M24 = 0.0f;
            result.M31 = 0.0f;
            result.M32 = 0.0f;
            result.M33 = 1.0f;
            result.M34 = 0.0f;
            result.M41 = 0.0f;
            result.M42 = 0.0f;
            result.M43 = 0.0f;
            result.M44 = 1.0f;
        }

        /// <summary>
        /// Creates a matrix that rotates around an arbitary axis.
        /// </summary>
        /// <param name="axis">The axis around which to rotate.</param>
        /// <param name="angle">Angle of rotation in radians. Angles are measured clockwise when looking along the rotation axis toward the origin.</param>
        /// <returns>The created rotation matrix.</returns>
        public static Matrix RotationAxis(Vector3 axis, float angle)
        {
            if (axis.LengthSquared() != 1.0f)
                axis.Normalize();

            Matrix result;
            float x = axis.X;
            float y = axis.Y;
            float z = axis.Z;
            float cos = (float)Math.Cos(angle);
            float sin = (float)Math.Sin(angle);
            float xx = x * x;
            float yy = y * y;
            float zz = z * z;
            float xy = x * y;
            float xz = x * z;
            float yz = y * z;

            result.M11 = xx + (cos * (1.0f - xx));
            result.M12 = (xy - (cos * xy)) + (sin * z);
            result.M13 = (xz - (cos * xz)) - (sin * y);
            result.M14 = 0.0f;
            result.M21 = (xy - (cos * xy)) - (sin * z);
            result.M22 = yy + (cos * (1.0f - yy));
            result.M23 = (yz - (cos * yz)) + (sin * x);
            result.M24 = 0.0f;
            result.M31 = (xz - (cos * xz)) + (sin * y);
            result.M32 = (yz - (cos * yz)) - (sin * x);
            result.M33 = zz + (cos * (1.0f - zz));
            result.M34 = 0.0f;
            result.M41 = 0.0f;
            result.M42 = 0.0f;
            result.M43 = 0.0f;
            result.M44 = 1.0f;

            return result;
        }

        /// <summary>
        /// Creates a matrix that rotates around an arbitary axis.
        /// </summary>
        /// <param name="axis">The axis around which to rotate.</param>
        /// <param name="angle">Angle of rotation in radians. Angles are measured clockwise when looking along the rotation axis toward the origin.</param>
        /// <param name="result">When the method completes, contains the created rotation matrix.</param>
        public static void RotationAxis(ref Vector3 axis, float angle, out Matrix result)
        {
            if (axis.LengthSquared() != 1.0f)
                axis.Normalize();

            float x = axis.X;
            float y = axis.Y;
            float z = axis.Z;
            float cos = (float)Math.Cos(angle);
            float sin = (float)Math.Sin(angle);
            float xx = x * x;
            float yy = y * y;
            float zz = z * z;
            float xy = x * y;
            float xz = x * z;
            float yz = y * z;

            result.M11 = xx + (cos * (1.0f - xx));
            result.M12 = (xy - (cos * xy)) + (sin * z);
            result.M13 = (xz - (cos * xz)) - (sin * y);
            result.M14 = 0.0f;
            result.M21 = (xy - (cos * xy)) - (sin * z);
            result.M22 = yy + (cos * (1.0f - yy));
            result.M23 = (yz - (cos * yz)) + (sin * x);
            result.M24 = 0.0f;
            result.M31 = (xz - (cos * xz)) + (sin * y);
            result.M32 = (yz - (cos * yz)) - (sin * x);
            result.M33 = zz + (cos * (1.0f - zz));
            result.M34 = 0.0f;
            result.M41 = 0.0f;
            result.M42 = 0.0f;
            result.M43 = 0.0f;
            result.M44 = 1.0f;
        }

        /// <summary>
        /// Creates a rotation matrix from a quaternion.
        /// </summary>
        /// <param name="rotation">The quaternion to use to build the matrix.</param>
        /// <returns>The created rotation matrix.</returns>
        public static Matrix RotationQuaternion(Quaternion quaternion)
        {
            Matrix result;

            float xx = quaternion.X * quaternion.X;
            float yy = quaternion.Y * quaternion.Y;
            float zz = quaternion.Z * quaternion.Z;
            float xy = quaternion.X * quaternion.Y;
            float zw = quaternion.Z * quaternion.W;
            float zx = quaternion.Z * quaternion.X;
            float yw = quaternion.Y * quaternion.W;
            float yz = quaternion.Y * quaternion.Z;
            float xw = quaternion.X * quaternion.W;
            result.M11 = 1.0f - (2.0f * (yy + zz));
            result.M12 = 2.0f * (xy + zw);
            result.M13 = 2.0f * (zx - yw);
            result.M14 = 0.0f;
            result.M21 = 2.0f * (xy - zw);
            result.M22 = 1.0f - (2.0f * (zz + xx));
            result.M23 = 2.0f * (yz + xw);
            result.M24 = 0.0f;
            result.M31 = 2.0f * (zx + yw);
            result.M32 = 2.0f * (yz - xw);
            result.M33 = 1.0f - (2.0f * (yy + xx));
            result.M34 = 0.0f;
            result.M41 = 0.0f;
            result.M42 = 0.0f;
            result.M43 = 0.0f;
            result.M44 = 1.0f;

            return result;
        }

        /// <summary>
        /// Creates a rotation matrix from a quaternion.
        /// </summary>
        /// <param name="rotation">The quaternion to use to build the matrix.</param>
        /// <param name="result">The created rotation matrix.</param>
        public static void RotationQuaternion(ref Quaternion rotation, out Matrix result)
        {
            float xx = rotation.X * rotation.X;
            float yy = rotation.Y * rotation.Y;
            float zz = rotation.Z * rotation.Z;
            float xy = rotation.X * rotation.Y;
            float zw = rotation.Z * rotation.W;
            float zx = rotation.Z * rotation.X;
            float yw = rotation.Y * rotation.W;
            float yz = rotation.Y * rotation.Z;
            float xw = rotation.X * rotation.W;
            result.M11 = 1.0f - (2.0f * (yy + zz));
            result.M12 = 2.0f * (xy + zw);
            result.M13 = 2.0f * (zx - yw);
            result.M14 = 0.0f;
            result.M21 = 2.0f * (xy - zw);
            result.M22 = 1.0f - (2.0f * (zz + xx));
            result.M23 = 2.0f * (yz + xw);
            result.M24 = 0.0f;
            result.M31 = 2.0f * (zx + yw);
            result.M32 = 2.0f * (yz - xw);
            result.M33 = 1.0f - (2.0f * (yy + xx));
            result.M34 = 0.0f;
            result.M41 = 0.0f;
            result.M42 = 0.0f;
            result.M43 = 0.0f;
            result.M44 = 1.0f;
        }

        /// <summary>
        /// Creates a rotation matrix with a specified yaw, pitch, and roll.
        /// </summary>
        /// <param name="yaw">Yaw around the y-axis, in radians.</param>
        /// <param name="pitch">Pitch around the x-axis, in radians.</param>
        /// <param name="roll">Roll around the z-axis, in radians.</param>
        /// <returns>The created rotation matrix.</returns>
        public static Matrix RotationYawPitchRoll(float yaw, float pitch, float roll)
        {
            Matrix result;
            Quaternion quaternion;
            Quaternion.RotationYawPitchRoll(yaw, pitch, roll, out quaternion);
            RotationQuaternion(ref quaternion, out result);
            return result;
        }

        /// <summary>
        /// Creates a rotation matrix with a specified yaw, pitch, and roll.
        /// </summary>
        /// <param name="yaw">Yaw around the y-axis, in radians.</param>
        /// <param name="pitch">Pitch around the x-axis, in radians.</param>
        /// <param name="roll">Roll around the z-axis, in radians.</param>
        /// <param name="result">When the method completes, contains the created rotation matrix.</param>
        public static void RotationYawPitchRoll(float yaw, float pitch, float roll, out Matrix result)
        {
            Quaternion quaternion;
            Quaternion.RotationYawPitchRoll(yaw, pitch, roll, out quaternion);
            RotationQuaternion(ref quaternion, out result);
        }


        /// <summary>
        /// Creates a left-handed, look-at matrix.
        /// </summary>
        /// <param name="eye">The position of the viewer's eye.</param>
        /// <param name="target">The camera look-at target.</param>
        /// <param name="up">The camera's up vector.</param>
        /// <returns>The created look-at matrix.</returns>
        /// <remarks>
        /// This function uses the following formula to compute the returned matrix.
        ///
        /// zaxis = normal(At - Eye)
        /// xaxis = normal(cross(Up, zaxis))
        /// yaxis = cross(zaxis, xaxis)
        /// 
        ///  xaxis.x           yaxis.x           zaxis.x          0
        ///  xaxis.y           yaxis.y           zaxis.y          0
        ///  xaxis.z           yaxis.z           zaxis.z          0
        /// -dot(xaxis, eye)  -dot(yaxis, eye)  -dot(zaxis, eye)  l
        /// </remarks>
        public static Matrix LookAtLH(Vector3 cameraPosition, Vector3 cameraTarget, Vector3 up)
        {
            Vector3 zaxis = Vector3.Normalize(cameraTarget - cameraPosition);
            Vector3 xaxis = Vector3.Normalize(Vector3.Cross(up, zaxis));
            Vector3 yaxis = Vector3.Cross(zaxis, xaxis);

            Matrix matrix;
            matrix.M11 = xaxis.X;
            matrix.M12 = yaxis.X;
            matrix.M13 = zaxis.X;
            matrix.M14 = 0f;
            matrix.M21 = xaxis.Y;
            matrix.M22 = yaxis.Y;
            matrix.M23 = zaxis.Y;
            matrix.M24 = 0f;
            matrix.M31 = xaxis.Z;
            matrix.M32 = yaxis.Z;
            matrix.M33 = zaxis.Z;
            matrix.M34 = 0f;
            matrix.M41 = -Vector3.Dot(xaxis, cameraPosition);
            matrix.M42 = -Vector3.Dot(yaxis, cameraPosition);
            matrix.M43 = -Vector3.Dot(zaxis, cameraPosition);
            matrix.M44 = 1f;
            return matrix;
        }

        /// <summary>
        /// Creates a left-handed, look-at matrix.
        /// </summary>
        /// <param name="eye">The position of the viewer's eye.</param>
        /// <param name="target">The camera look-at target.</param>
        /// <param name="up">The camera's up vector.</param>
        /// <param name="result">When the method completes, contains the created look-at matrix.</param>
        /// <remarks>
        /// This function uses the following formula to compute the returned matrix.
        ///
        /// zaxis = normal(At - Eye)
        /// xaxis = normal(cross(Up, zaxis))
        /// yaxis = cross(zaxis, xaxis)
        /// 
        ///  xaxis.x           yaxis.x           zaxis.x          0
        ///  xaxis.y           yaxis.y           zaxis.y          0
        ///  xaxis.z           yaxis.z           zaxis.z          0
        /// -dot(xaxis, eye)  -dot(yaxis, eye)  -dot(zaxis, eye)  l
        /// </remarks>
        public static void LookAtLH(ref Vector3 cameraPosition, ref Vector3 cameraTarget, ref Vector3 up, out Matrix result)
        {
            Vector3 zaxis = Vector3.Normalize(cameraTarget - cameraPosition);
            Vector3 xaxis = Vector3.Normalize(Vector3.Cross(up, zaxis));
            Vector3 yaxis = Vector3.Cross(zaxis, xaxis);

            result.M11 = xaxis.X;
            result.M12 = yaxis.X;
            result.M13 = zaxis.X;
            result.M14 = 0f;
            result.M21 = xaxis.Y;
            result.M22 = yaxis.Y;
            result.M23 = zaxis.Y;
            result.M24 = 0f;
            result.M31 = xaxis.Z;
            result.M32 = yaxis.Z;
            result.M33 = zaxis.Z;
            result.M34 = 0f;
            result.M41 = -Vector3.Dot(xaxis, cameraPosition);
            result.M42 = -Vector3.Dot(yaxis, cameraPosition);
            result.M43 = -Vector3.Dot(zaxis, cameraPosition);
            result.M44 = 1f;
        }

        /// <summary>
        /// Creates a right-handed, look-at matrix.
        /// </summary>
        /// <param name="cameraPosition">The position of the viewer's eye.</param>
        /// <param name="cameraTarget">The camera look-at target.</param>
        /// <param name="cameraUpVector">The camera's up vector.</param>
        /// <returns>The created look-at matrix.</returns>
        /// <remarks>
        /// This function uses the following formula to compute the returned matrix. 
        ///
        /// zaxis = normal(Eye - At)
        /// xaxis = normal(cross(Up, zaxis))
        /// yaxis = cross(zaxis, xaxis)
        ///
        ///  xaxis.x           yaxis.x           zaxis.x          0
        ///  xaxis.y           yaxis.y           zaxis.y          0
        ///  xaxis.z           yaxis.z           zaxis.z          0
        /// -dot(xaxis, eye)  -dot(yaxis, eye)  -dot(zaxis, eye)  l
        /// </remarks>
        public static Matrix LookAtRH(Vector3 cameraPosition, Vector3 cameraTarget, Vector3 cameraUpVector)
        {
            Matrix matrix;
            Vector3 zaxis = Vector3.Normalize(cameraPosition - cameraTarget);
            Vector3 xaxis = Vector3.Normalize(Vector3.Cross(cameraUpVector, zaxis));
            Vector3 yaxis = Vector3.Cross(zaxis, xaxis);
            matrix.M11 = xaxis.X;
            matrix.M12 = yaxis.X;
            matrix.M13 = zaxis.X;
            matrix.M14 = 0f;
            matrix.M21 = xaxis.Y;
            matrix.M22 = yaxis.Y;
            matrix.M23 = zaxis.Y;
            matrix.M24 = 0f;
            matrix.M31 = xaxis.Z;
            matrix.M32 = yaxis.Z;
            matrix.M33 = zaxis.Z;
            matrix.M34 = 0f;
            matrix.M41 = -Vector3.Dot(xaxis, cameraPosition);
            matrix.M42 = -Vector3.Dot(yaxis, cameraPosition);
            matrix.M43 = -Vector3.Dot(zaxis, cameraPosition);
            matrix.M44 = 1f;
            return matrix;
        }

        /// <summary>
        /// Creates a right-handed, look-at matrix.
        /// </summary>
        /// <param name="cameraPosition">The position of the viewer's eye.</param>
        /// <param name="cameraTarget">The camera look-at target.</param>
        /// <param name="cameraUpVector">The camera's up vector.</param>
        /// <param name="result">When the method completes, contains the created look-at matrix.</param>
        /// <remarks>
        /// This function uses the following formula to compute the returned matrix. 
        ///
        /// zaxis = normal(Eye - At)
        /// xaxis = normal(cross(Up, zaxis))
        /// yaxis = cross(zaxis, xaxis)
        ///
        ///  xaxis.x           yaxis.x           zaxis.x          0
        ///  xaxis.y           yaxis.y           zaxis.y          0
        ///  xaxis.z           yaxis.z           zaxis.z          0
        /// -dot(xaxis, eye)  -dot(yaxis, eye)  -dot(zaxis, eye)  l
        /// </remarks>
        public static void LookAtRH(ref Vector3 cameraPosition, ref Vector3 cameraTarget, ref Vector3 cameraUpVector,
            out Matrix result)
        {
            Vector3 zaxis = Vector3.Normalize(cameraPosition - cameraTarget);
            Vector3 xaxis = Vector3.Normalize(Vector3.Cross(cameraUpVector, zaxis));
            Vector3 yaxis = Vector3.Cross(zaxis, xaxis);
            result.M11 = xaxis.X;
            result.M12 = yaxis.X;
            result.M13 = zaxis.X;
            result.M14 = 0f;
            result.M21 = xaxis.Y;
            result.M22 = yaxis.Y;
            result.M23 = zaxis.Y;
            result.M24 = 0f;
            result.M31 = xaxis.Z;
            result.M32 = yaxis.Z;
            result.M33 = zaxis.Z;
            result.M34 = 0f;
            result.M41 = -Vector3.Dot(xaxis, cameraPosition);
            result.M42 = -Vector3.Dot(yaxis, cameraPosition);
            result.M43 = -Vector3.Dot(zaxis, cameraPosition);
            result.M44 = 1f;
        }


        /// <summary>
        /// Creates a left-handed, orthographic projection matrix.
        /// </summary>
        /// <param name="width">Width of the viewing volume.</param>
        /// <param name="height">Height of the viewing volume.</param>
        /// <param name="zNearPlane">Minimum z-value of the viewing volume.</param>
        /// <param name="zFarPlane">Maximum z-value of the viewing volume.</param>
        /// <returns>The created projection matrix.</returns>
        /// <remarks>
        /// All the parameters of the function are distances in camera space. The parameters describe the dimensions of the view volume.
        ///
        /// The return value for this function is the same value returned in the pOut parameter. In this way, the function can be used as a parameter for another function.
        ///
        /// This function uses the following formula to compute the returned matrix. 
        ///
        /// 2/w  0    0           0
        /// 0    2/h  0           0
        /// 0    0    1/(zf-zn)   0
        /// 0    0   -zn/(zf-zn)  1
        /// </remarks>
        public static Matrix OrthoLH(float width, float height, float zNearPlane, float zFarPlane)
        {
            Matrix result;
            result.M11 = 2f / width;
            result.M22 = 2f / height;

            result.M12 = result.M13 = result.M14 = 0f;
            result.M21 = result.M23 = result.M24 = 0f;
            result.M31 = result.M32 = result.M34 = 0f;
            result.M41 = result.M42 = 0f;

            result.M33 = 1f / (zFarPlane - zNearPlane);
            result.M43 = -zNearPlane / (zFarPlane - zNearPlane);
            result.M44 = 1f;
            return result;
        }

        /// <summary>
        /// Creates a left-handed, orthographic projection matrix.
        /// </summary>
        /// <param name="width">Width of the viewing volume.</param>
        /// <param name="height">Height of the viewing volume.</param>
        /// <param name="zNearPlane">Minimum z-value of the viewing volume.</param>
        /// <param name="zNearPlane">Maximum z-value of the viewing volume.</param>
        /// <param name="result">When the method completes, contains the created projection matrix.</param>
        /// <remarks>
        /// All the parameters of the function are distances in camera space. The parameters describe the dimensions of the view volume.
        ///
        /// The return value for this function is the same value returned in the pOut parameter. In this way, the function can be used as a parameter for another function.
        ///
        /// This function uses the following formula to compute the returned matrix. 
        ///
        /// 2/w  0    0           0
        /// 0    2/h  0           0
        /// 0    0    1/(zf-zn)   0
        /// 0    0   -zn/(zf-zn)  1
        /// </remarks>
        public static void OrthoLH(float width, float height, float zNearPlane, float zFarPlane, out Matrix result)
        {
            result.M11 = 2f / width;
            result.M22 = 2f / height;

            result.M12 = result.M13 = result.M14 = 0f;
            result.M21 = result.M23 = result.M24 = 0f;
            result.M31 = result.M32 = result.M34 = 0f;
            result.M41 = result.M42 = 0f;

            result.M33 = 1f / (zFarPlane - zNearPlane);
            result.M43 = -zNearPlane / (zFarPlane - zNearPlane);
            result.M44 = 1f;
        }

        /// <summary>
        /// Creates a right-handed, orthographic projection matrix.
        /// </summary>
        /// <param name="width">Width of the viewing volume.</param>
        /// <param name="height">Height of the viewing volume.</param>
        /// <param name="zNearPlane">Minimum z-value of the viewing volume.</param>
        /// <param name="zFarPlane">Maximum z-value of the viewing volume.</param>
        /// <returns>The created projection matrix.</returns>
        /// <remarks>
        /// All the parameters of the function are distances in camera space.
        /// The parameters describe the dimensions of the view volume.
        ///
        /// The return value for this function is the same value returned in the pOut parameter. 
        /// In this way, the function can be used as a parameter for another function.
        ///
        /// This function uses the following formula to compute the returned matrix. 
        /// 
        /// 2/w  0    0           0
        /// 0    2/h  0           0
        /// 0    0    1/(zn-zf)   0
        /// 0    0    zn/(zn-zf)  l
        /// </remarks>
        public static Matrix OrthoRH(float width, float height, float zNearPlane, float zFarPlane)
        {
            Matrix matrix;
            matrix.M11 = 2f / width;
            matrix.M22 = 2f / height;

            matrix.M12 = matrix.M13 = matrix.M14 = 0f;
            matrix.M21 = matrix.M23 = matrix.M24 = 0f;
            matrix.M31 = matrix.M32 = matrix.M34 = 0f;
            matrix.M41 = matrix.M42 = 0f;

            matrix.M33 = 1f / (zNearPlane - zFarPlane);
            matrix.M43 = zNearPlane / (zNearPlane - zFarPlane);
            matrix.M44 = 1f;
            return matrix;
        }

        /// <summary>
        /// Creates a right-handed, orthographic projection matrix.
        /// </summary>
        /// <param name="width">Width of the viewing volume.</param>
        /// <param name="height">Height of the viewing volume.</param>
        /// <param name="zNearPlane">Minimum z-value of the viewing volume.</param>
        /// <param name="zFarPlane">Maximum z-value of the viewing volume.</param>
        /// <param name="result">When the method completes, contains the created projection matrix.</param>
        /// <remarks>
        /// All the parameters of the function are distances in camera space.
        /// The parameters describe the dimensions of the view volume.
        ///
        /// The return value for this function is the same value returned in the pOut parameter. 
        /// In this way, the function can be used as a parameter for another function.
        ///
        /// This function uses the following formula to compute the returned matrix. 
        ///
        /// 2/w  0    0           0
        /// 0    2/h  0           0
        /// 0    0    1/(zn-zf)   0
        /// 0    0    zn/(zn-zf)  l
        /// </remarks>
        public static void OrthoRH(float width, float height, float zNearPlane, float zFarPlane,
            out Matrix result)
        {
            result.M11 = 2f / width;
            result.M22 = 2f / height;

            result.M12 = result.M13 = result.M14 = 0f;
            result.M21 = result.M23 = result.M24 = 0f;
            result.M31 = result.M32 = result.M34 = 0f;
            result.M41 = result.M42 = 0f;

            result.M33 = 1f / (zNearPlane - zFarPlane);
            result.M43 = zNearPlane / (zNearPlane - zFarPlane);
            result.M44 = 1f;
        }


        /// <summary>
        /// Creates a left-handed, customized orthographic projection matrix.
        /// </summary>
        /// <param name="left">Minimum x-value of the viewing volume.</param>
        /// <param name="right">Maximum x-value of the viewing volume.</param>
        /// <param name="bottom">Minimum y-value of the viewing volume.</param>
        /// <param name="top">Maximum y-value of the viewing volume.</param>
        /// <param name="zNearPlane">Minimum z-value of the viewing volume.</param>
        /// <param name="zFarPlane">Maximum z-value of the viewing volume.</param>
        /// <returns>The created projection matrix.</returns>
        /// <remarks>
        /// This function uses the following formula to compute the returned matrix. 
        ///
        /// 2/(r-l)      0            0           0
        /// 0            2/(t-b)      0           0
        /// 0            0            1/(zf-zn)   0
        /// (l+r)/(l-r)  (t+b)/(b-t)  zn/(zn-zf)  l
        /// </remarks>
        public static Matrix OrthoOffCenterLH(float left, float right, float bottom, float top,
            float zNearPlane, float zFarPlane)
        {
            Matrix matrix;
            matrix.M11 = 2f / (right - left);
            matrix.M22 = 2f / (top - bottom);

            matrix.M12 = matrix.M13 = matrix.M14 = 0f;
            matrix.M21 = matrix.M23 = matrix.M24 = 0f;
            matrix.M31 = matrix.M32 = matrix.M34 = 0f;

            matrix.M33 = 1f / (zNearPlane - zFarPlane);
            matrix.M41 = (left + right) / (left - right);
            matrix.M42 = (top + bottom) / (bottom - top);
            matrix.M43 = zNearPlane / (zNearPlane - zFarPlane);
            matrix.M44 = 1f;
            return matrix;
        }

        /// <summary>
        /// Creates a left-handed, customized orthographic projection matrix.
        /// </summary>
        /// <param name="left">Minimum x-value of the viewing volume.</param>
        /// <param name="right">Maximum x-value of the viewing volume.</param>
        /// <param name="bottom">Minimum y-value of the viewing volume.</param>
        /// <param name="top">Maximum y-value of the viewing volume.</param>
        /// <param name="zNearPlane">Minimum z-value of the viewing volume.</param>
        /// <param name="zFarPlane">Maximum z-value of the viewing volume.</param>
        /// <param name="result">When the method completes, contains the created projection matrix.</param>
        /// <remarks>
        /// This function uses the following formula to compute the returned matrix. 
        ///
        /// 2/(r-l)      0            0           0
        /// 0            2/(t-b)      0           0
        /// 0            0            1/(zf-zn)   0
        /// (l+r)/(l-r)  (t+b)/(b-t)  zn/(zn-zf)  l
        /// </remarks>
        public static void OrthoOffCenterLH(float left, float right, float bottom, float top,
            float zNearPlane, float zFarPlane, out Matrix result)
        {
            result.M11 = 2f / (right - left);
            result.M22 = 2f / (top - bottom);

            result.M12 = result.M13 = result.M14 = 0f;
            result.M21 = result.M23 = result.M24 = 0f;
            result.M31 = result.M32 = result.M34 = 0f;

            result.M33 = 1f / (zNearPlane - zFarPlane);
            result.M41 = (left + right) / (left - right);
            result.M42 = (top + bottom) / (bottom - top);
            result.M43 = zNearPlane / (zNearPlane - zFarPlane);
            result.M44 = 1f;
        }

        /// <summary>
        /// Creates a right-handed, customized orthographic projection matrix.
        /// </summary>
        /// <param name="left">Minimum x-value of the viewing volume.</param>
        /// <param name="right">Maximum x-value of the viewing volume.</param>
        /// <param name="bottom">Minimum y-value of the viewing volume.</param>
        /// <param name="top">Maximum y-value of the viewing volume.</param>
        /// <param name="zNearPlane">Minimum z-value of the viewing volume.</param>
        /// <param name="zFarPlane">Maximum z-value of the viewing volume.</param>
        /// <returns>The created projection matrix.</returns>
        /// <remarks>
        /// This function uses the following formula to compute the returned matrix. 
        ///
        /// 2/(r-l)      0            0           0
        /// 0            2/(t-b)      0           0
        /// 0            0            1/(zn-zf)   0
        /// (l+r)/(l-r)  (t+b)/(b-t)  zn/(zn-zf)  l
        /// </remarks>
        public static Matrix OrthoOffCenterRH(float left, float right, float bottom, float top,
            float zNearPlane, float zFarPlane)
        {
            Matrix matrix;
            matrix.M11 = 2f / (right - left);
            matrix.M22 = 2f / (top - bottom);

            matrix.M12 = matrix.M13 = matrix.M14 = 0f;
            matrix.M31 = matrix.M32 = matrix.M34 = 0f;
            matrix.M21 = matrix.M23 = matrix.M24 = 0f;

            matrix.M33 = 1f / (zNearPlane - zFarPlane);
            matrix.M41 = (left + right) / (left - right);
            matrix.M42 = (top + bottom) / (bottom - top);
            matrix.M43 = zNearPlane / (zNearPlane - zFarPlane);
            matrix.M44 = 1f;
            return matrix;
        }

        /// <summary>
        /// Creates a right-handed, customized orthographic projection matrix.
        /// </summary>
        /// <param name="left">Minimum x-value of the viewing volume.</param>
        /// <param name="right">Maximum x-value of the viewing volume.</param>
        /// <param name="bottom">Minimum y-value of the viewing volume.</param>
        /// <param name="top">Maximum y-value of the viewing volume.</param>
        /// <param name="zNearPlane">Minimum z-value of the viewing volume.</param>
        /// <param name="zFarPlane">Maximum z-value of the viewing volume.</param>
        /// <param name="result">When the method completes, contains the created projection matrix.</param>
        /// <remarks>
        /// This function uses the following formula to compute the returned matrix. 
        ///
        /// 2/(r-l)      0            0           0
        /// 0            2/(t-b)      0           0
        /// 0            0            1/(zn-zf)   0
        /// (l+r)/(l-r)  (t+b)/(b-t)  zn/(zn-zf)  l
        /// </remarks>
        public static void OrthoOffCenterRH(float left, float right, float bottom, float top,
            float zNearPlane, float zFarPlane, out Matrix result)
        {
            result.M11 = 2f / (right - left);
            result.M22 = 2f / (top - bottom);

            result.M12 = result.M13 = result.M14 = 0f;
            result.M21 = result.M23 = result.M24 = 0f;
            result.M31 = result.M32 = result.M34 = 0f;

            result.M33 = 1f / (zNearPlane - zFarPlane);
            result.M41 = (left + right) / (left - right);
            result.M42 = (top + bottom) / (bottom - top);
            result.M43 = zNearPlane / (zNearPlane - zFarPlane);
            result.M44 = 1f;
        }

        /// <summary>
        /// Creates a left-handed, perspective projection matrix.
        /// </summary>
        /// <param name="width">Width of the viewing volume.</param>
        /// <param name="height">Height of the viewing volume.</param>
        /// <param name="nearPlaneDistance">Minimum z-value of the viewing volume.</param>
        /// <param name="farPlaneDistance">Maximum z-value of the viewing volume.</param>
        /// <returns>The created projection matrix.</returns>
        /// <remarks>
        /// This function uses the following formula to compute the returned matrix. 
        ///
        /// 2*zn/w  0       0              0
        /// 0       2*zn/h  0              0
        /// 0       0       zf/(zf-zn)     1
        /// 0       0       zn*zf/(zn-zf)  0
        /// </remarks>
        public static Matrix PerspectiveLH(float width, float height, float nearPlaneDistance, float farPlaneDistance)
        {
            Matrix matrix;
            if (nearPlaneDistance <= 0f)
            {
                throw new ArgumentOutOfRangeException("nearPlaneDistance", string.Format(CultureInfo.CurrentCulture, "BaseTexts.NegativePlaneDistance", new object[] { "nearPlaneDistance" }));
            }
            if (farPlaneDistance <= 0f)
            {
                throw new ArgumentOutOfRangeException("farPlaneDistance", string.Format(CultureInfo.CurrentCulture, "BaseTexts.NegativePlaneDistance", new object[] { "farPlaneDistance" }));
            }
            if (nearPlaneDistance >= farPlaneDistance)
            {
                throw new ArgumentOutOfRangeException("nearPlaneDistance", "BaseTexts.OppositePlanes");
            }
            matrix.M11 = (2f * nearPlaneDistance) / width;
            matrix.M22 = (2f * nearPlaneDistance) / height;

            matrix.M12 = matrix.M13 = matrix.M14 = 0f;
            matrix.M21 = matrix.M23 = matrix.M24 = 0f;
            matrix.M41 = matrix.M42 = matrix.M44 = 0f;
            matrix.M31 = matrix.M32 = 0f;

            matrix.M33 = farPlaneDistance / (farPlaneDistance - nearPlaneDistance);
            matrix.M34 = 1f;
            matrix.M43 = (nearPlaneDistance * farPlaneDistance) / (nearPlaneDistance - farPlaneDistance);
            return matrix;
        }

        /// <summary>
        /// Creates a left-handed, perspective projection matrix.
        /// </summary>
        /// <param name="width">Width of the viewing volume.</param>
        /// <param name="height">Height of the viewing volume.</param>
        /// <param name="nearPlaneDistance">Minimum z-value of the viewing volume.</param>
        /// <param name="farPlaneDistance">Maximum z-value of the viewing volume.</param>
        /// <param name="result">When the method completes, contains the created projection matrix.</param>
        /// <remarks>
        /// This function uses the following formula to compute the returned matrix. 
        ///
        /// 2*zn/w  0       0              0
        /// 0       2*zn/h  0              0
        /// 0       0       zf/(zf-zn)     1
        /// 0       0       zn*zf/(zn-zf)  0
        /// </remarks>
        public static void PerspectiveLH(float width, float height,
            float nearPlaneDistance, float farPlaneDistance, out Matrix result)
        {
            if (nearPlaneDistance <= 0f)
            {
                throw new ArgumentOutOfRangeException("nearPlaneDistance", string.Format(CultureInfo.CurrentCulture, "BaseTexts.NegativePlaneDistance", new object[] { "nearPlaneDistance" }));
            }
            if (farPlaneDistance <= 0f)
            {
                throw new ArgumentOutOfRangeException("farPlaneDistance", string.Format(CultureInfo.CurrentCulture, "BaseTexts.NegativePlaneDistance", new object[] { "farPlaneDistance" }));
            }
            if (nearPlaneDistance >= farPlaneDistance)
            {
                throw new ArgumentOutOfRangeException("nearPlaneDistance", "BaseTexts.OppositePlanes");
            }
            result.M11 = (2f * nearPlaneDistance) / width;
            result.M22 = (2f * nearPlaneDistance) / height;

            result.M12 = result.M13 = result.M14 = 0f;
            result.M21 = result.M23 = result.M24 = 0f;
            result.M41 = result.M42 = result.M44 = 0f;
            result.M31 = result.M32 = 0f;

            result.M33 = farPlaneDistance / (farPlaneDistance - nearPlaneDistance);
            result.M34 = 1f;
            result.M43 = (nearPlaneDistance * farPlaneDistance) / (nearPlaneDistance - farPlaneDistance);
        }

        /// <summary>
        /// Creates a right-handed, perspective projection matrix.
        /// </summary>
        /// <param name="width">Width of the viewing volume.</param>
        /// <param name="height">Height of the viewing volume.</param>
        /// <param name="nearPlaneDistance">Minimum z-value of the viewing volume.</param>
        /// <param name="farPlaneDistance">Maximum z-value of the viewing volume.</param>
        /// <returns>The created projection matrix.</returns>
        /// <remarks>
        /// This function uses the following formula to compute the returned matrix. 
        ///
        /// 2*zn/w  0       0              0
        /// 0       2*zn/h  0              0
        /// 0       0       zf/(zn-zf)    -1
        /// 0       0       zn*zf/(zn-zf)  0
        /// </remarks>
        public static Matrix PerspectiveRH(float width, float height, float nearPlaneDistance, float farPlaneDistance)
        {
            Matrix matrix;
            if (nearPlaneDistance <= 0f)
            {
                throw new ArgumentOutOfRangeException("nearPlaneDistance", string.Format(CultureInfo.CurrentCulture, "BaseTexts.NegativePlaneDistance", new object[] { "nearPlaneDistance" }));
            }
            if (farPlaneDistance <= 0f)
            {
                throw new ArgumentOutOfRangeException("farPlaneDistance", string.Format(CultureInfo.CurrentCulture, "BaseTexts.NegativePlaneDistance", new object[] { "farPlaneDistance" }));
            }
            if (nearPlaneDistance >= farPlaneDistance)
            {
                throw new ArgumentOutOfRangeException("nearPlaneDistance", "BaseTexts.OppositePlanes");
            }
            matrix.M11 = (2f * nearPlaneDistance) / width;
            matrix.M22 = (2f * nearPlaneDistance) / height;

            matrix.M12 = matrix.M13 = matrix.M14 = 0f;
            matrix.M21 = matrix.M23 = matrix.M24 = 0f;
            matrix.M41 = matrix.M42 = matrix.M44 = 0f;
            matrix.M31 = matrix.M32 = 0f;

            matrix.M33 = farPlaneDistance / (nearPlaneDistance - farPlaneDistance);
            matrix.M34 = -1f;
            matrix.M43 = (nearPlaneDistance * farPlaneDistance) / (nearPlaneDistance - farPlaneDistance);
            return matrix;
        }

        /// <summary>
        /// Creates a right-handed, perspective projection matrix.
        /// </summary>
        /// <param name="width">Width of the viewing volume.</param>
        /// <param name="height">Height of the viewing volume.</param>
        /// <param name="nearPlaneDistance">Minimum z-value of the viewing volume.</param>
        /// <param name="farPlaneDistance">Maximum z-value of the viewing volume.</param>
        /// <param name="result">When the method completes, contains the created projection matrix.</param>
        /// <remarks>
        /// This function uses the following formula to compute the returned matrix. 
        ///
        /// 2*zn/w  0       0              0
        /// 0       2*zn/h  0              0
        /// 0       0       zf/(zn-zf)    -1
        /// 0       0       zn*zf/(zn-zf)  0
        /// </remarks>
        public static void PerspectiveRH(float width, float height, float nearPlaneDistance, float farPlaneDistance, out Matrix result)
        {
            if (nearPlaneDistance <= 0f)
            {
                throw new ArgumentOutOfRangeException("nearPlaneDistance", string.Format(CultureInfo.CurrentCulture, "BaseTexts.NegativePlaneDistance", new object[] { "nearPlaneDistance" }));
            }
            if (farPlaneDistance <= 0f)
            {
                throw new ArgumentOutOfRangeException("farPlaneDistance", string.Format(CultureInfo.CurrentCulture, "BaseTexts.NegativePlaneDistance", new object[] { "farPlaneDistance" }));
            }
            if (nearPlaneDistance >= farPlaneDistance)
            {
                throw new ArgumentOutOfRangeException("nearPlaneDistance", "BaseTexts.OppositePlanes");
            }
            result.M11 = (2f * nearPlaneDistance) / width;
            result.M22 = (2f * nearPlaneDistance) / height;

            result.M12 = result.M13 = result.M14 = 0f;
            result.M21 = result.M23 = result.M24 = 0f;
            result.M41 = result.M42 = result.M44 = 0f;
            result.M31 = result.M32 = 0f;

            result.M33 = farPlaneDistance / (nearPlaneDistance - farPlaneDistance);
            result.M34 = -1f;
            result.M43 = (nearPlaneDistance * farPlaneDistance) / (nearPlaneDistance - farPlaneDistance);
        }

        /// <summary>
        /// Creates a left-handed, perspective projection matrix based on a field of view.
        /// </summary>
        /// <param name="fieldOfView">Field of view in the y direction, in radians.</param>
        /// <param name="aspectRatio">Aspect ratio, defined as view space width divided by height.</param>
        /// <param name="nearPlaneDistance">Minimum z-value of the viewing volume.</param>
        /// <param name="farPlaneDistance">Maximum z-value of the viewing volume.</param>
        /// <returns>The created projection matrix.</returns>
        /// <remarks>
        /// This function computes the returned matrix as shown:
        ///
        /// xScale     0          0               0
        /// 0        yScale       0               0
        /// 0          0       zf/(zf-zn)         1
        /// 0          0       -zn*zf/(zf-zn)     0
        /// where:
        /// yScale = cot(fovY/2)
        ///
        /// xScale = yScale / aspect ratio
        /// </remarks>
        public static Matrix PerspectiveFovLH(float fieldOfView, float aspectRatio, float nearPlaneDistance, float farPlaneDistance)
        {
            Matrix matrix;
            if ((fieldOfView <= 0f) || (fieldOfView >= 3.141593f))
            {
                throw new ArgumentOutOfRangeException(string.Format(CultureInfo.CurrentCulture, "OutRangeFieldOfView", new object[] { "fieldOfView" }));
            }
            if (nearPlaneDistance <= 0f)
            {
                throw new ArgumentOutOfRangeException(string.Format(CultureInfo.CurrentCulture, "NegativePlaneDistance", new object[] { "nearPlaneDistance" }));
            }
            if (farPlaneDistance <= 0f)
            {
                throw new ArgumentOutOfRangeException(string.Format(CultureInfo.CurrentCulture, "NegativePlaneDistance", new object[] { "farPlaneDistance" }));
            }
            if (nearPlaneDistance >= farPlaneDistance)
            {
                throw new ArgumentOutOfRangeException("OppositePlanes");
            }
            float yScale = 1f / ((float)Math.Tan(fieldOfView * 0.5f));
            float xScale = yScale / aspectRatio;
            matrix.M11 = xScale;
            matrix.M22 = yScale;

            matrix.M12 = matrix.M13 = matrix.M14 = 0f;
            matrix.M21 = matrix.M23 = matrix.M24 = 0f;
            matrix.M41 = matrix.M42 = matrix.M44 = 0f;
            matrix.M31 = matrix.M32 = 0f;

            matrix.M33 = farPlaneDistance / (farPlaneDistance - nearPlaneDistance);
            matrix.M34 = 1f;

            matrix.M43 = -(nearPlaneDistance * farPlaneDistance) / (farPlaneDistance - nearPlaneDistance);
            return matrix;
        }

        /// <summary>
        /// Creates a left-handed, perspective projection matrix based on a field of view.
        /// </summary>
        /// <param name="fieldOfView">Field of view in the y direction, in radians.</param>
        /// <param name="aspectRatio">Aspect ratio, defined as view space width divided by height.</param>
        /// <param name="nearPlaneDistance">Minimum z-value of the viewing volume.</param>
        /// <param name="farPlaneDistance">Maximum z-value of the viewing volume.</param>
        /// <param name="result">When the method completes, contains the created projection matrix.</param>
        /// <remarks>
        /// This function computes the returned matrix as shown:
        ///
        /// xScale     0          0               0
        /// 0        yScale       0               0
        /// 0          0       zf/(zf-zn)         1
        /// 0          0       -zn*zf/(zf-zn)     0
        /// where:
        /// yScale = cot(fovY/2)
        ///
        /// xScale = yScale / aspect ratio
        /// </remarks>
        public static void PerspectiveFovLH(float fieldOfView, float aspectRatio, float nearPlaneDistance, float farPlaneDistance,
            out Matrix result)
        {
            if ((fieldOfView <= 0f) || (fieldOfView >= 3.141593f))
            {
                throw new ArgumentOutOfRangeException(string.Format(CultureInfo.CurrentCulture, "OutRangeFieldOfView", new object[] { "fieldOfView" }));
            }
            if (nearPlaneDistance <= 0f)
            {
                throw new ArgumentOutOfRangeException(string.Format(CultureInfo.CurrentCulture, "NegativePlaneDistance", new object[] { "nearPlaneDistance" }));
            }
            if (farPlaneDistance <= 0f)
            {
                throw new ArgumentOutOfRangeException(string.Format(CultureInfo.CurrentCulture, "NegativePlaneDistance", new object[] { "farPlaneDistance" }));
            }
            if (nearPlaneDistance >= farPlaneDistance)
            {
                throw new ArgumentOutOfRangeException("OppositePlanes");
            }
            float yScale = 1f / ((float)Math.Tan(fieldOfView * 0.5f));
            float xScale = yScale / aspectRatio;
            result.M11 = xScale;
            result.M22 = yScale;

            result.M12 = result.M13 = result.M14 = 0f;
            result.M21 = result.M23 = result.M24 = 0f;
            result.M41 = result.M42 = result.M44 = 0f;
            result.M31 = result.M32 = 0f;

            result.M33 = farPlaneDistance / (farPlaneDistance - nearPlaneDistance);
            result.M34 = 1f;

            result.M43 = -(nearPlaneDistance * farPlaneDistance) / (farPlaneDistance - nearPlaneDistance);
        }

        /// <summary>
        /// Creates a right-handed, perspective projection matrix based on a field of view.
        /// </summary>
        /// <param name="fieldOfView">Field of view in the y direction, in radians.</param>
        /// <param name="aspectRatio">Aspect ratio, defined as view space width divided by height.</param>
        /// <param name="nearPlaneDistance">Minimum z-value of the viewing volume.</param>
        /// <param name="farPlaneDistance">Maximum z-value of the viewing volume.</param>
        /// <returns>The created projection matrix.</returns>
        /// <remarks>
        /// This function computes the returned matrix as shown. 
        ///
        /// xScale     0          0              0
        /// 0        yScale       0              0
        /// 0        0        zf/(zn-zf)        -1
        /// 0        0        zn*zf/(zn-zf)      0
        /// where:
        /// yScale = cot(fovY/2)
        /// 
        /// xScale = yScale / aspect ratio
        /// </remarks>
        public static Matrix PerspectiveFovRH(float fieldOfView, float aspectRatio, float nearPlaneDistance, float farPlaneDistance)
        {
            Matrix matrix;
            if ((fieldOfView <= 0f) || (fieldOfView >= 3.141593f))
            {
                throw new ArgumentOutOfRangeException(string.Format(CultureInfo.CurrentCulture, "OutRangeFieldOfView", new object[] { "fieldOfView" }));
            }
            if (nearPlaneDistance <= 0f)
            {
                throw new ArgumentOutOfRangeException(string.Format(CultureInfo.CurrentCulture, "NegativePlaneDistance", new object[] { "nearPlaneDistance" }));
            }
            if (farPlaneDistance <= 0f)
            {
                throw new ArgumentOutOfRangeException(string.Format(CultureInfo.CurrentCulture, "NegativePlaneDistance", new object[] { "farPlaneDistance" }));
            }
            if (nearPlaneDistance >= farPlaneDistance)
            {
                throw new ArgumentOutOfRangeException("OppositePlanes");
            }
            float yScale = 1f / ((float)Math.Tan(fieldOfView * 0.5f));
            float xScale = yScale / aspectRatio;
            matrix.M11 = xScale;
            matrix.M22 = yScale;

            matrix.M12 = matrix.M13 = matrix.M14 = 0f;
            matrix.M21 = matrix.M23 = matrix.M24 = 0f;
            matrix.M41 = matrix.M42 = matrix.M44 = 0f;
            matrix.M31 = matrix.M32 = 0f;

            matrix.M33 = farPlaneDistance / (nearPlaneDistance - farPlaneDistance);
            matrix.M34 = -1f;

            matrix.M43 = (nearPlaneDistance * farPlaneDistance) / (nearPlaneDistance - farPlaneDistance);
            return matrix;
        }

        /// <summary>
        /// Creates a right-handed, perspective projection matrix based on a field of view.
        /// </summary>
        /// <param name="fieldOfView">Field of view in the y direction, in radians.</param>
        /// <param name="aspectRatio">Aspect ratio, defined as view space width divided by height.</param>
        /// <param name="nearPlaneDistance">Minimum z-value of the viewing volume.</param>
        /// <param name="farPlaneDistance">Maximum z-value of the viewing volume.</param>
        /// <param name="result">When the method completes, contains the created projection matrix.</param>
        /// <remarks>
        /// This function computes the returned matrix as shown. 
        ///
        /// xScale     0          0              0
        /// 0        yScale       0              0
        /// 0        0        zf/(zn-zf)        -1
        /// 0        0        zn*zf/(zn-zf)      0
        /// where:
        /// yScale = cot(fovY/2)
        /// 
        /// xScale = yScale / aspect ratio
        /// </remarks>
        public static void PerspectiveFovRH(float fieldOfView, float aspectRatio, float nearPlaneDistance, float farPlaneDistance,
            out Matrix result)
        {
            if ((fieldOfView <= 0f) || (fieldOfView >= 3.141593f))
            {
                throw new ArgumentOutOfRangeException(string.Format(CultureInfo.CurrentCulture, "OutRangeFieldOfView", new object[] { "fieldOfView" }));
            }
            if (nearPlaneDistance <= 0f)
            {
                throw new ArgumentOutOfRangeException(string.Format(CultureInfo.CurrentCulture, "NegativePlaneDistance", new object[] { "nearPlaneDistance" }));
            }
            if (farPlaneDistance <= 0f)
            {
                throw new ArgumentOutOfRangeException(string.Format(CultureInfo.CurrentCulture, "NegativePlaneDistance", new object[] { "farPlaneDistance" }));
            }
            if (nearPlaneDistance >= farPlaneDistance)
            {
                throw new ArgumentOutOfRangeException("OppositePlanes");
            }
            float yScale = 1f / ((float)Math.Tan(fieldOfView * 0.5f));
            float xScale = yScale / aspectRatio;
            result.M11 = xScale;
            result.M22 = yScale;

            result.M12 = result.M13 = result.M14 = 0f;
            result.M21 = result.M23 = result.M24 = 0f;
            result.M41 = result.M42 = result.M44 = 0f;
            result.M31 = result.M32 = 0f;

            result.M33 = farPlaneDistance / (nearPlaneDistance - farPlaneDistance);
            result.M34 = -1f;

            result.M43 = (nearPlaneDistance * farPlaneDistance) / (nearPlaneDistance - farPlaneDistance);
        }


        /// <summary>
        /// Creates a left-handed, customized perspective projection matrix.
        /// </summary>
        /// <param name="left">Minimum x-value of the viewing volume.</param>
        /// <param name="right">Maximum x-value of the viewing volume.</param>
        /// <param name="bottom">Minimum y-value of the viewing volume.</param>
        /// <param name="top">Maximum y-value of the viewing volume.</param>
        /// <param name="znear">Minimum z-value of the viewing volume.</param>
        /// <param name="zfar">Maximum z-value of the viewing volume.</param>
        /// <returns>The created projection matrix.</returns>
        /// <remarks>
        /// This function uses the following formula to compute the returned matrix.
        ///
        /// 2*zn/(r-l)   0            0              0
        /// 0            2*zn/(t-b)   0              0
        /// (l+r)/(l-r)  (t+b)/(b-t)  zf/(zf-zn)     1
        /// 0            0            zn*zf/(zn-zf)  0
        /// </remarks>
        public static Matrix PerspectiveOffCenterLH(float left, float right, float bottom, float top,
            float nearPlaneDistance, float farPlaneDistance)
        {
            Matrix result;
            if (nearPlaneDistance <= 0f)
            {
                throw new ArgumentOutOfRangeException("nearPlaneDistance", string.Format(CultureInfo.CurrentCulture, "BaseTexts.NegativePlaneDistance", new object[] { "nearPlaneDistance" }));
            }
            if (farPlaneDistance <= 0f)
            {
                throw new ArgumentOutOfRangeException("farPlaneDistance", string.Format(CultureInfo.CurrentCulture, "BaseTexts.NegativePlaneDistance", new object[] { "farPlaneDistance" }));
            }
            if (nearPlaneDistance >= farPlaneDistance)
            {
                throw new ArgumentOutOfRangeException("nearPlaneDistance", "BaseTexts.OppositePlanes");
            }
            result.M11 = (2f * nearPlaneDistance) / (right - left);
            result.M22 = (2f * nearPlaneDistance) / (top - bottom);

            result.M12 = result.M13 = result.M14 = 0f;
            result.M21 = result.M23 = result.M24 = 0f;
            result.M41 = result.M42 = result.M44 = 0f;

            result.M31 = (left + right) / (left - right);
            result.M32 = (top + bottom) / (bottom - top);
            result.M33 = farPlaneDistance / (farPlaneDistance - nearPlaneDistance);
            result.M34 = -1f;
            result.M43 = (nearPlaneDistance * farPlaneDistance) / (nearPlaneDistance - farPlaneDistance);
            return result;
        }

        /// <summary>
        /// Creates a left-handed, customized perspective projection matrix.
        /// </summary>
        /// <param name="left">Minimum x-value of the viewing volume.</param>
        /// <param name="right">Maximum x-value of the viewing volume.</param>
        /// <param name="bottom">Minimum y-value of the viewing volume.</param>
        /// <param name="top">Maximum y-value of the viewing volume.</param>
        /// <param name="znear">Minimum z-value of the viewing volume.</param>
        /// <param name="zfar">Maximum z-value of the viewing volume.</param>
        /// <param name="result">When the method completes, contains the created projection matrix.</param>
        /// <remarks>
        /// This function uses the following formula to compute the returned matrix.
        ///
        /// 2*zn/(r-l)   0            0              0
        /// 0            2*zn/(t-b)   0              0
        /// (l+r)/(l-r)  (t+b)/(b-t)  zf/(zf-zn)     1
        /// 0            0            zn*zf/(zn-zf)  0
        /// </remarks>
        public static void PerspectiveOffCenterLH(float left, float right, float bottom, float top,
            float nearPlaneDistance, float farPlaneDistance, out Matrix result)
        {
            if (nearPlaneDistance <= 0f)
            {
                throw new ArgumentOutOfRangeException("nearPlaneDistance", string.Format(CultureInfo.CurrentCulture, "BaseTexts.NegativePlaneDistance", new object[] { "nearPlaneDistance" }));
            }
            if (farPlaneDistance <= 0f)
            {
                throw new ArgumentOutOfRangeException("farPlaneDistance", string.Format(CultureInfo.CurrentCulture, "BaseTexts.NegativePlaneDistance", new object[] { "farPlaneDistance" }));
            }
            if (nearPlaneDistance >= farPlaneDistance)
            {
                throw new ArgumentOutOfRangeException("nearPlaneDistance", "BaseTexts.OppositePlanes");
            }
            result.M11 = (2f * nearPlaneDistance) / (right - left);
            result.M22 = (2f * nearPlaneDistance) / (top - bottom);

            result.M12 = result.M13 = result.M14 = 0f;
            result.M21 = result.M23 = result.M24 = 0f;
            result.M41 = result.M42 = result.M44 = 0f;

            result.M31 = (left + right) / (left - right);
            result.M32 = (top + bottom) / (bottom - top);
            result.M33 = farPlaneDistance / (farPlaneDistance - nearPlaneDistance);
            result.M34 = -1f;
            result.M43 = (nearPlaneDistance * farPlaneDistance) / (nearPlaneDistance - farPlaneDistance);
        }

        /// <summary>
        /// Creates a right-handed, customized perspective projection matrix.
        /// </summary>
        /// <param name="left">Minimum x-value of the viewing volume.</param>
        /// <param name="right">Maximum x-value of the viewing volume.</param>
        /// <param name="bottom">Minimum y-value of the viewing volume.</param>
        /// <param name="top">Maximum y-value of the viewing volume.</param>
        /// <param name="nearPlaneDistance">Minimum z-value of the viewing volume.</param>
        /// <param name="farPlaneDistance">Maximum z-value of the viewing volume.</param>
        /// <returns>The created projection matrix.</returns>
        /// <remarks>
        /// This function uses the following formula to compute the returned matrix.
        ///
        /// 2*zn/(r-l)   0            0                0
        /// 0            2*zn/(t-b)   0                0
        /// (l+r)/(r-l)  (t+b)/(t-b)  zf/(zn-zf)      -1
        /// 0            0            zn*zf/(zn-zf)    0
        /// </remarks>
        public static Matrix PerspectiveOffCenterRH(float left, float right, float bottom, float top,
            float nearPlaneDistance, float farPlaneDistance)
        {
            Matrix matrix;
            if (nearPlaneDistance <= 0f)
            {
                throw new ArgumentOutOfRangeException("nearPlaneDistance", string.Format(CultureInfo.CurrentCulture, "BaseTexts.NegativePlaneDistance", new object[] { "nearPlaneDistance" }));
            }
            if (farPlaneDistance <= 0f)
            {
                throw new ArgumentOutOfRangeException("farPlaneDistance", string.Format(CultureInfo.CurrentCulture, "BaseTexts.NegativePlaneDistance", new object[] { "farPlaneDistance" }));
            }
            if (nearPlaneDistance >= farPlaneDistance)
            {
                throw new ArgumentOutOfRangeException("nearPlaneDistance", "BaseTexts.OppositePlanes");
            }
            matrix.M11 = (2f * nearPlaneDistance) / (right - left);
            matrix.M22 = (2f * nearPlaneDistance) / (top - bottom);

            matrix.M12 = matrix.M13 = matrix.M14 = 0f;
            matrix.M21 = matrix.M23 = matrix.M24 = 0f;
            matrix.M41 = matrix.M42 = matrix.M44 = 0f;

            matrix.M31 = (left + right) / (right - left);
            matrix.M32 = (top + bottom) / (top - bottom);
            matrix.M33 = farPlaneDistance / (nearPlaneDistance - farPlaneDistance);
            matrix.M34 = -1f;
            matrix.M43 = (nearPlaneDistance * farPlaneDistance) / (nearPlaneDistance - farPlaneDistance);

            return matrix;
        }

        /// <summary>
        /// Creates a right-handed, customized perspective projection matrix.
        /// </summary>
        /// <param name="left">Minimum x-value of the viewing volume.</param>
        /// <param name="right">Maximum x-value of the viewing volume.</param>
        /// <param name="bottom">Minimum y-value of the viewing volume.</param>
        /// <param name="top">Maximum y-value of the viewing volume.</param>
        /// <param name="nearPlaneDistance">Minimum z-value of the viewing volume.</param>
        /// <param name="farPlaneDistance">Maximum z-value of the viewing volume.</param>
        /// <param name="result">When the method completes, contains the created projection matrix.</param>
        /// <remarks>
        /// This function uses the following formula to compute the returned matrix.
        ///
        /// 2*zn/(r-l)   0            0                0
        /// 0            2*zn/(t-b)   0                0
        /// (l+r)/(r-l)  (t+b)/(t-b)  zf/(zn-zf)      -1
        /// 0            0            zn*zf/(zn-zf)    0
        /// </remarks>
        public static void PerspectiveOffCenterRH(float left, float right, float bottom, float top,
            float nearPlaneDistance, float farPlaneDistance, out Matrix result)
        {
            if (nearPlaneDistance <= 0f)
            {
                throw new ArgumentOutOfRangeException("nearPlaneDistance", string.Format(CultureInfo.CurrentCulture, "BaseTexts.NegativePlaneDistance", new object[] { "nearPlaneDistance" }));
            }
            if (farPlaneDistance <= 0f)
            {
                throw new ArgumentOutOfRangeException("farPlaneDistance", string.Format(CultureInfo.CurrentCulture, "BaseTexts.NegativePlaneDistance", new object[] { "farPlaneDistance" }));
            }
            if (nearPlaneDistance >= farPlaneDistance)
            {
                throw new ArgumentOutOfRangeException("nearPlaneDistance", "BaseTexts.OppositePlanes");
            }
            result.M11 = (2f * nearPlaneDistance) / (right - left);
            result.M22 = (2f * nearPlaneDistance) / (top - bottom);

            result.M12 = result.M13 = result.M14 = 0f;
            result.M21 = result.M23 = result.M24 = 0f;
            result.M41 = result.M42 = result.M44 = 0f;

            result.M31 = (left + right) / (right - left);
            result.M32 = (top + bottom) / (top - bottom);
            result.M33 = farPlaneDistance / (nearPlaneDistance - farPlaneDistance);
            result.M34 = -1f;
            result.M43 = (nearPlaneDistance * farPlaneDistance) / (nearPlaneDistance - farPlaneDistance);
        }

        /// <summary>
        /// Creates a matrix that reflects the coordinate system about a plane.
        /// </summary>
        /// <param name="plane">The plane about which the coordinate system will be reflected.</param>
        /// <returns>The created reflection matrix.</returns>
        /// <remarks>
        /// This function uses the following formula to compute the returned matrix. 
        ///
        /// P = normalize(Plane);
        ///
        /// -2 * P.a * P.a + 1  -2 * P.b * P.a      -2 * P.c * P.a        0
        /// -2 * P.a * P.b      -2 * P.b * P.b + 1  -2 * P.c * P.b        0
        /// -2 * P.a * P.c      -2 * P.b * P.c      -2 * P.c * P.c + 1    0
        /// -2 * P.a * P.d      -2 * P.b * P.d      -2 * P.c * P.d
        ///
        /// </remarks>
        public static Matrix Reflection(Plane plane)
        {
            Matrix result;
            plane.Normalize();
            float x = plane.Normal.X;
            float y = plane.Normal.Y;
            float z = plane.Normal.Z;
            float x2 = -2.0f * x;
            float y2 = -2.0f * y;
            float z2 = -2.0f * z;
            result.M11 = (x2 * x) + 1.0f;
            result.M12 = y2 * x;
            result.M13 = z2 * x;
            result.M14 = 0.0f;
            result.M21 = x2 * y;
            result.M22 = (y2 * y) + 1.0f;
            result.M23 = z2 * y;
            result.M24 = 0.0f;
            result.M31 = x2 * z;
            result.M32 = y2 * z;
            result.M33 = (z2 * z) + 1.0f;
            result.M34 = 0.0f;
            result.M41 = x2 * plane.D;
            result.M42 = y2 * plane.D;
            result.M43 = z2 * plane.D;
            result.M44 = 1.0f;
            return result;
        }

        /// <summary>
        /// Creates a matrix that reflects the coordinate system about a plane.
        /// </summary>
        /// <param name="plane">The plane about which the coordinate system will be reflected.</param>
        /// <param name="result">When the method completes, contains the created reflection matrix.</param>
        /// <remarks>
        /// This function uses the following formula to compute the returned matrix. 
        ///
        /// P = normalize(Plane);
        ///
        /// -2 * P.a * P.a + 1  -2 * P.b * P.a      -2 * P.c * P.a        0
        /// -2 * P.a * P.b      -2 * P.b * P.b + 1  -2 * P.c * P.b        0
        /// -2 * P.a * P.c      -2 * P.b * P.c      -2 * P.c * P.c + 1    0
        /// -2 * P.a * P.d      -2 * P.b * P.d      -2 * P.c * P.d
        ///
        /// </remarks>
        public static void Reflection(ref Plane plane, out Matrix result)
        {
            plane.Normalize();
            float x = plane.Normal.X;
            float y = plane.Normal.Y;
            float z = plane.Normal.Z;
            float x2 = -2.0f * x;
            float y2 = -2.0f * y;
            float z2 = -2.0f * z;
            result.M11 = (x2 * x) + 1.0f;
            result.M12 = y2 * x;
            result.M13 = z2 * x;
            result.M14 = 0.0f;
            result.M21 = x2 * y;
            result.M22 = (y2 * y) + 1.0f;
            result.M23 = z2 * y;
            result.M24 = 0.0f;
            result.M31 = x2 * z;
            result.M32 = y2 * z;
            result.M33 = (z2 * z) + 1.0f;
            result.M34 = 0.0f;
            result.M41 = x2 * plane.D;
            result.M42 = y2 * plane.D;
            result.M43 = z2 * plane.D;
            result.M44 = 1.0f;
        }

        /// <summary>
        /// Creates a matrix that scales along the x-axis, y-axis, and y-axis.
        /// </summary>
        /// <param name="x">Scaling factor that is applied along the x-axis.</param>
        /// <param name="y">Scaling factor that is applied along the y-axis.</param>
        /// <param name="z">Scaling factor that is applied along the z-axis.</param>
        /// <returns>The created scaling matrix.</returns>
        public static Matrix Scaling(float x, float y, float z)
        {
            Matrix result;
            result.M11 = x;
            result.M12 = 0.0f;
            result.M13 = 0.0f;
            result.M14 = 0.0f;
            result.M21 = 0.0f;
            result.M22 = y;
            result.M23 = 0.0f;
            result.M24 = 0.0f;
            result.M31 = 0.0f;
            result.M32 = 0.0f;
            result.M33 = z;
            result.M34 = 0.0f;
            result.M41 = 0.0f;
            result.M42 = 0.0f;
            result.M43 = 0.0f;
            result.M44 = 1.0f;
            return result;
        }

        /// <summary>
        /// Creates a matrix that scales along the x-axis, y-axis, and y-axis.
        /// </summary>
        /// <param name="x">Scaling factor that is applied along the x-axis.</param>
        /// <param name="y">Scaling factor that is applied along the y-axis.</param>
        /// <param name="z">Scaling factor that is applied along the z-axis.</param>
        /// <param name="result">When the method completes, contains the created scaling matrix.</param>
        public static void Scaling(float x, float y, float z, out Matrix result)
        {
            result.M11 = x;
            result.M12 = 0.0f;
            result.M13 = 0.0f;
            result.M14 = 0.0f;
            result.M21 = 0.0f;
            result.M22 = y;
            result.M23 = 0.0f;
            result.M24 = 0.0f;
            result.M31 = 0.0f;
            result.M32 = 0.0f;
            result.M33 = z;
            result.M34 = 0.0f;
            result.M41 = 0.0f;
            result.M42 = 0.0f;
            result.M43 = 0.0f;
            result.M44 = 1.0f;
        }

        /// <summary>
        /// Creates a matrix that scales along the x-axis, y-axis, and y-axis.
        /// </summary>
        /// <param name="scale">Scaling factor for all three axes.</param>
        /// <returns>The created scaling matrix.</returns>
        public static Matrix Scaling(Vector3 scaling)
        {
            Matrix result;
            result.M11 = scaling.X;
            result.M12 = 0.0f;
            result.M13 = 0.0f;
            result.M14 = 0.0f;
            result.M21 = 0.0f;
            result.M22 = scaling.Y;
            result.M23 = 0.0f;
            result.M24 = 0.0f;
            result.M31 = 0.0f;
            result.M32 = 0.0f;
            result.M33 = scaling.Z;
            result.M34 = 0.0f;
            result.M41 = 0.0f;
            result.M42 = 0.0f;
            result.M43 = 0.0f;
            result.M44 = 1.0f;
            return result;
        }

        /// <summary>
        /// Creates a matrix that scales along the x-axis, y-axis, and y-axis.
        /// </summary>
        /// <param name="scale">Scaling factor for all three axes.</param>
        /// <param name="result">When the method completes, contains the created scaling matrix.</param>
        public static void Scaling(ref Vector3 scaling, out Matrix result)
        {
            result.M11 = scaling.X;
            result.M12 = 0.0f;
            result.M13 = 0.0f;
            result.M14 = 0.0f;
            result.M21 = 0.0f;
            result.M22 = scaling.Y;
            result.M23 = 0.0f;
            result.M24 = 0.0f;
            result.M31 = 0.0f;
            result.M32 = 0.0f;
            result.M33 = scaling.Z;
            result.M34 = 0.0f;
            result.M41 = 0.0f;
            result.M42 = 0.0f;
            result.M43 = 0.0f;
            result.M44 = 1.0f;
        }

        /// <summary>
        /// Creates a matrix that flattens geometry into a plane.
        /// </summary>
        /// <param name="light">Position of the shadow light.</param>
        /// <param name="plane">The plane into which the geometry will be flattened.</param>
        /// <returns>The created shadow matrix.</returns>
        /// <remarks>
        /// This function uses the following formula to compute the returned matrix.
        ///
        /// P = normalize(Plane);
        /// L = Light;
        /// d = dot(P, L)
        ///
        /// P.a * L.x + d  P.a * L.y      P.a * L.z      P.a * L.w  
        /// P.b * L.x      P.b * L.y + d  P.b * L.z      P.b * L.w  
        /// P.c * L.x      P.c * L.y      P.c * L.z + d  P.c * L.w  
        /// P.d * L.x      P.d * L.y      P.d * L.z      P.d * L.w + d
        ///
        /// If the light's w-component is 0, the ray from the origin to the light represents a directional light. If it is 1, the light is a point light.
        ///
        /// </remarks>
        public static Matrix Shadow(Vector4 light, Plane plane)
        {
            Matrix result;
            plane.Normalize();
            float dot = ((plane.Normal.X * light.X) + (plane.Normal.Y * light.Y)) + (plane.Normal.Z * light.Z);
            float x = -plane.Normal.X;
            float y = -plane.Normal.Y;
            float z = -plane.Normal.Z;
            float d = -plane.D;
            result.M11 = (x * light.X) + dot;
            result.M21 = y * light.X;
            result.M31 = z * light.X;
            result.M41 = d * light.X;
            result.M12 = x * light.Y;
            result.M22 = (y * light.Y) + dot;
            result.M32 = z * light.Y;
            result.M42 = d * light.Y;
            result.M13 = x * light.Z;
            result.M23 = y * light.Z;
            result.M33 = (z * light.Z) + dot;
            result.M43 = d * light.Z;
            result.M14 = 0.0f;
            result.M24 = 0.0f;
            result.M34 = 0.0f;
            result.M44 = dot;
            return result;
        }

        /// <summary>
        /// Creates a matrix that flattens geometry into a plane.
        /// </summary>
        /// <param name="light">Position of the shadow light.</param>
        /// <param name="plane">The plane into which the geometry will be flattened.</param>
        /// <param name="result">When the method completes, contains the created shadow matrix.</param>
        /// <remarks>
        /// This function uses the following formula to compute the returned matrix.
        ///
        /// P = normalize(Plane);
        /// L = Light;
        /// d = dot(P, L)
        ///
        /// P.a * L.x + d  P.a * L.y      P.a * L.z      P.a * L.w  
        /// P.b * L.x      P.b * L.y + d  P.b * L.z      P.b * L.w  
        /// P.c * L.x      P.c * L.y      P.c * L.z + d  P.c * L.w  
        /// P.d * L.x      P.d * L.y      P.d * L.z      P.d * L.w + d
        ///
        /// If the light's w-component is 0, the ray from the origin to the light represents a directional light. If it is 1, the light is a point light.
        /// </remarks>
        public static void Shadow(ref Vector4 light, ref Plane plane, out Matrix result)
        {
            plane.Normalize();
            float dot = ((plane.Normal.X * light.X) + (plane.Normal.Y * light.Y)) + (plane.Normal.Z * light.Z);
            float x = -plane.Normal.X;
            float y = -plane.Normal.Y;
            float z = -plane.Normal.Z;
            float d = -plane.D;
            result.M11 = (x * light.X) + dot;
            result.M21 = y * light.X;
            result.M31 = z * light.X;
            result.M41 = d * light.X;
            result.M12 = x * light.Y;
            result.M22 = (y * light.Y) + dot;
            result.M32 = z * light.Y;
            result.M42 = d * light.Y;
            result.M13 = x * light.Z;
            result.M23 = y * light.Z;
            result.M33 = (z * light.Z) + dot;
            result.M43 = d * light.Z;
            result.M14 = 0.0f;
            result.M24 = 0.0f;
            result.M34 = 0.0f;
            result.M44 = dot;
        }

        /// <summary>
        /// Creates a translation matrix using the specified offsets.
        /// </summary>
        /// <param name="x">X-coordinate offset.</param>
        /// <param name="y">Y-coordinate offset.</param>
        /// <param name="z">Z-coordinate offset.</param>
        /// <returns>The created translation matrix.</returns>
        public static Matrix Translation(float x, float y, float z)
        {
            Matrix result;
            result.M11 = 1.0f;
            result.M12 = 0.0f;
            result.M13 = 0.0f;
            result.M14 = 0.0f;
            result.M21 = 0.0f;
            result.M22 = 1.0f;
            result.M23 = 0.0f;
            result.M24 = 0.0f;
            result.M31 = 0.0f;
            result.M32 = 0.0f;
            result.M33 = 1.0f;
            result.M34 = 0.0f;
            result.M41 = x;
            result.M42 = y;
            result.M43 = z;
            result.M44 = 1.0f;
            return result;
        }

        /// <summary>
        /// Creates a translation matrix using the specified offsets.
        /// </summary>
        /// <param name="x">X-coordinate offset.</param>
        /// <param name="y">Y-coordinate offset.</param>
        /// <param name="z">Z-coordinate offset.</param>
        /// <param name="result">When the method completes, contains the created translation matrix.</param>
        public static void Translation(float x, float y, float z, out Matrix result)
        {
            result.M11 = 1.0f;
            result.M12 = 0.0f;
            result.M13 = 0.0f;
            result.M14 = 0.0f;
            result.M21 = 0.0f;
            result.M22 = 1.0f;
            result.M23 = 0.0f;
            result.M24 = 0.0f;
            result.M31 = 0.0f;
            result.M32 = 0.0f;
            result.M33 = 1.0f;
            result.M34 = 0.0f;
            result.M41 = x;
            result.M42 = y;
            result.M43 = z;
            result.M44 = 1.0f;
        }

        /// <summary>
        /// Creates a translation matrix using the specified offsets.
        /// </summary>
        /// <param name="amount">The offset for all three coordinate planes.</param>
        /// <returns>The created translation matrix.</returns>
        public static Matrix Translation(Vector3 amount)
        {
            Matrix result;
            result.M11 = 1.0f;
            result.M12 = 0.0f;
            result.M13 = 0.0f;
            result.M14 = 0.0f;
            result.M21 = 0.0f;
            result.M22 = 1.0f;
            result.M23 = 0.0f;
            result.M24 = 0.0f;
            result.M31 = 0.0f;
            result.M32 = 0.0f;
            result.M33 = 1.0f;
            result.M34 = 0.0f;
            result.M41 = amount.X;
            result.M42 = amount.Y;
            result.M43 = amount.Z;
            result.M44 = 1.0f;
            return result;
        }

        /// <summary>
        /// Creates a translation matrix using the specified offsets.
        /// </summary>
        /// <param name="amount">The offset for all three coordinate planes.</param>
        /// <param name="result">When the method completes, contains the created translation matrix.</param>
        public static void Translation(ref Vector3 amount, out Matrix result)
        {
            result.M11 = 1.0f;
            result.M12 = 0.0f;
            result.M13 = 0.0f;
            result.M14 = 0.0f;
            result.M21 = 0.0f;
            result.M22 = 1.0f;
            result.M23 = 0.0f;
            result.M24 = 0.0f;
            result.M31 = 0.0f;
            result.M32 = 0.0f;
            result.M33 = 1.0f;
            result.M34 = 0.0f;
            result.M41 = amount.X;
            result.M42 = amount.Y;
            result.M43 = amount.Z;
            result.M44 = 1.0f;
        }

        /// <summary>
        /// Calculates the inverse of the specified matrix.
        /// </summary>
        /// <param name="matrix">The matrix whose inverse is to be calculated.</param>
        /// <returns>The inverse of the specified matrix.</returns>
        public static Matrix Invert(Matrix matrix)
        {
            Matrix result;
            float m11 = matrix.M11;
            float m12 = matrix.M12;
            float m13 = matrix.M13;
            float m14 = matrix.M14;
            float m21 = matrix.M21;
            float m22 = matrix.M22;
            float m23 = matrix.M23;
            float m24 = matrix.M24;
            float m31 = matrix.M31;
            float m32 = matrix.M32;
            float m33 = matrix.M33;
            float m34 = matrix.M34;
            float m41 = matrix.M41;
            float m42 = matrix.M42;
            float m43 = matrix.M43;
            float m44 = matrix.M44;
            float num23 = m33 * m44 - m34 * m43;
            float num22 = m32 * m44 - m34 * m42;
            float num21 = m32 * m43 - m33 * m42;
            float num20 = m31 * m44 - m34 * m41;
            float num19 = m31 * m43 - m33 * m41;
            float num18 = m31 * m42 - m32 * m41;
            float num39 = m22 * num23 - m23 * num22 + m24 * num21;
            float num38 = -(m21 * num23 - m23 * num20 + m24 * num19);
            float num37 = m21 * num22 - m22 * num20 + m24 * num18;
            float num36 = -(m21 * num21 - m22 * num19 + m23 * num18);
            float num = 1f / (m11 * num39 + m12 * num38 + m13 * num37 + m14 * num36);
            result.M11 = num39 * num;
            result.M21 = num38 * num;
            result.M31 = num37 * num;
            result.M41 = num36 * num;
            result.M12 = -(m12 * num23 - m13 * num22 + m14 * num21) * num;
            result.M22 = (m11 * num23 - m13 * num20 + m14 * num19) * num;
            result.M32 = -(m11 * num22 - m12 * num20 + m14 * num18) * num;
            result.M42 = (m11 * num21 - m12 * num19 + m13 * num18) * num;
            float num35 = m23 * m44 - m24 * m43;
            float num34 = m22 * m44 - m24 * m42;
            float num33 = m22 * m43 - m23 * m42;
            float num32 = m21 * m44 - m24 * m41;
            float num31 = m21 * m43 - m23 * m41;
            float num30 = m21 * m42 - m22 * m41;
            result.M13 = (m12 * num35 - m13 * num34 + m14 * num33) * num;
            result.M23 = -(m11 * num35 - m13 * num32 + m14 * num31) * num;
            result.M33 = (m11 * num34 - m12 * num32 + m14 * num30) * num;
            result.M43 = -(m11 * num33 - m12 * num31 + m13 * num30) * num;
            float num29 = m23 * m34 - m24 * m33;
            float num28 = m22 * m34 - m24 * m32;
            float num27 = m22 * m33 - m23 * m32;
            float num26 = m21 * m34 - m24 * m31;
            float num25 = m21 * m33 - m23 * m31;
            float num24 = m21 * m32 - m22 * m31;
            result.M14 = -(m12 * num29 - m13 * num28 + m14 * num27) * num;
            result.M24 = (m11 * num29 - m13 * num26 + m14 * num25) * num;
            result.M34 = -(m11 * num28 - m12 * num26 + m14 * num24) * num;
            result.M44 = (m11 * num27 - m12 * num25 + m13 * num24) * num;
            return result;
        }

        /// <summary>
        /// Calculates the inverse of the specified matrix.
        /// </summary>
        /// <param name="matrix">The matrix whose inverse is to be calculated.</param>
        /// <param name="result">When the method completes, contains the inverse of the specified matrix.</param>
        public static void Invert(ref Matrix matrix, out Matrix result)
        {
            float m11 = matrix.M11;
            float m12 = matrix.M12;
            float m13 = matrix.M13;
            float m14 = matrix.M14;
            float m21 = matrix.M21;
            float m22 = matrix.M22;
            float m23 = matrix.M23;
            float m24 = matrix.M24;
            float m31 = matrix.M31;
            float m32 = matrix.M32;
            float m33 = matrix.M33;
            float m34 = matrix.M34;
            float m41 = matrix.M41;
            float m42 = matrix.M42;
            float m43 = matrix.M43;
            float m44 = matrix.M44;
            float num23 = m33 * m44 - m34 * m43;
            float num22 = m32 * m44 - m34 * m42;
            float num21 = m32 * m43 - m33 * m42;
            float num20 = m31 * m44 - m34 * m41;
            float num19 = m31 * m43 - m33 * m41;
            float num18 = m31 * m42 - m32 * m41;
            float num39 = m22 * num23 - m23 * num22 + m24 * num21;
            float num38 = -(m21 * num23 - m23 * num20 + m24 * num19);
            float num37 = m21 * num22 - m22 * num20 + m24 * num18;
            float num36 = -(m21 * num21 - m22 * num19 + m23 * num18);
            float num = 1f / (m11 * num39 + m12 * num38 + m13 * num37 + m14 * num36);
            result.M11 = num39 * num;
            result.M21 = num38 * num;
            result.M31 = num37 * num;
            result.M41 = num36 * num;
            result.M12 = -(m12 * num23 - m13 * num22 + m14 * num21) * num;
            result.M22 = (m11 * num23 - m13 * num20 + m14 * num19) * num;
            result.M32 = -(m11 * num22 - m12 * num20 + m14 * num18) * num;
            result.M42 = (m11 * num21 - m12 * num19 + m13 * num18) * num;
            float num35 = m23 * m44 - m24 * m43;
            float num34 = m22 * m44 - m24 * m42;
            float num33 = m22 * m43 - m23 * m42;
            float num32 = m21 * m44 - m24 * m41;
            float num31 = m21 * m43 - m23 * m41;
            float num30 = m21 * m42 - m22 * m41;
            result.M13 = (m12 * num35 - m13 * num34 + m14 * num33) * num;
            result.M23 = -(m11 * num35 - m13 * num32 + m14 * num31) * num;
            result.M33 = (m11 * num34 - m12 * num32 + m14 * num30) * num;
            result.M43 = -(m11 * num33 - m12 * num31 + m13 * num30) * num;
            float num29 = m23 * m34 - m24 * m33;
            float num28 = m22 * m34 - m24 * m32;
            float num27 = m22 * m33 - m23 * m32;
            float num26 = m21 * m34 - m24 * m31;
            float num25 = m21 * m33 - m23 * m31;
            float num24 = m21 * m32 - m22 * m31;
            result.M14 = -(m12 * num29 - m13 * num28 + m14 * num27) * num;
            result.M24 = (m11 * num29 - m13 * num26 + m14 * num25) * num;
            result.M34 = -(m11 * num28 - m12 * num26 + m14 * num24) * num;
            result.M44 = (m11 * num27 - m12 * num25 + m13 * num24) * num;
        }

        /// <summary>
        /// Calculates the transpose of the specified matrix.
        /// </summary>
        /// <param name="matrix">The matrix whose transpose is to be calculated.</param>
        /// <returns>The transpose of the specified matrix.</returns>
        public static Matrix Transpose(Matrix mat)
        {
            Matrix result;
            result.M11 = mat.M11;
            result.M12 = mat.M21;
            result.M13 = mat.M31;
            result.M14 = mat.M41;
            result.M21 = mat.M12;
            result.M22 = mat.M22;
            result.M23 = mat.M32;
            result.M24 = mat.M42;
            result.M31 = mat.M13;
            result.M32 = mat.M23;
            result.M33 = mat.M33;
            result.M34 = mat.M43;
            result.M41 = mat.M14;
            result.M42 = mat.M24;
            result.M43 = mat.M34;
            result.M44 = mat.M44;
            return result;
        }

        /// <summary>
        /// Calculates the transpose of the specified matrix.
        /// </summary>
        /// <param name="matrix">The matrix whose transpose is to be calculated.</param>
        /// <param name="result">When the method completes, contains the transpose of the specified matrix.</param>
        public static void Transpose(ref Matrix mat, out Matrix r)
        {
            r.M11 = mat.M11;
            r.M12 = mat.M21;
            r.M13 = mat.M31;
            r.M14 = mat.M41;
            r.M21 = mat.M12;
            r.M22 = mat.M22;
            r.M23 = mat.M32;
            r.M24 = mat.M42;
            r.M31 = mat.M13;
            r.M32 = mat.M23;
            r.M33 = mat.M33;
            r.M34 = mat.M43;
            r.M41 = mat.M14;
            r.M42 = mat.M24;
            r.M43 = mat.M34;
            r.M44 = mat.M44;
        }

        /// <summary>
        /// Creates a 3D affine transformation matrix.
        /// </summary>
        /// <param name="scaling">Scaling factor.</param>
        /// <param name="rotationCenter">The center of the rotation.</param>
        /// <param name="rotation">The rotation of the transformation.</param>
        /// <param name="translation">The translation factor of the transformation.</param>
        /// <returns>The created affine transformation matrix.</returns>
        /// <remarks>
        /// This function calculates the affine transformation matrix with the following formula, with matrix concatenation evaluated in left-to-right order:
        ///
        /// Mout = Ms * (Mrc)-1 * Mr * Mrc * Mt
        ///
        /// where:
        ///
        /// Mout = output matrix (pOut)
        ///
        /// Ms = scaling matrix (Scaling)
        ///
        /// Mrc = center of rotation matrix (pRotationCenter)
        ///
        /// Mr = rotation matrix (pRotation)
        ///
        /// Mt = translation matrix (pTranslation)
        ///
        /// </remarks>
        public static Matrix AffineTransformation(float scaling, Vector3 rotationCenter, Quaternion rotation, Vector3 translation)
        {
            Matrix mul;
            // Ms
            Matrix result;
            Matrix.Scaling(scaling, scaling, scaling, out result);

            // (Mrc)-1
            Matrix.Translation(-rotationCenter.X, -rotationCenter.Y, -rotationCenter.Z, out mul);
            Matrix.Multiply(ref result, ref mul, out result);

            // Mr
            Matrix.RotationQuaternion(ref rotation, out mul);
            Matrix.Multiply(ref result, ref mul, out result);

            // Mrc
            Matrix.Translation(rotationCenter.X, rotationCenter.Y, rotationCenter.Z, out mul);
            Matrix.Multiply(ref result, ref mul, out result);

            // Mt
            Matrix.Translation(ref translation, out mul);
            Matrix.Multiply(ref result, ref mul, out result);
            return result;
        }

        /// <summary>
        /// Creates a 3D affine transformation matrix.
        /// </summary>
        /// <param name="scaling">Scaling factor.</param>
        /// <param name="rotationCenter">The center of the rotation.</param>
        /// <param name="rotation">The rotation of the transformation.</param>
        /// <param name="translation">The translation factor of the transformation.</param>
        /// <param name="result">When the method completes, contains the created affine transformation matrix.</param>
        /// <remarks>
        /// This function calculates the affine transformation matrix with the following formula, with matrix concatenation evaluated in left-to-right order:
        ///
        /// Mout = Ms * (Mrc)-1 * Mr * Mrc * Mt
        ///
        /// where:
        ///
        /// Mout = output matrix (pOut)
        ///
        /// Ms = scaling matrix (Scaling)
        ///
        /// Mrc = center of rotation matrix (pRotationCenter)
        ///
        /// Mr = rotation matrix (pRotation)
        ///
        /// Mt = translation matrix (pTranslation)
        ///
        /// </remarks>
        public static void AffineTransformation(float scaling, ref Vector3 rotationCenter, ref Quaternion rotation, ref Vector3 translation,
            out Matrix result)
        {
            // Ms
            Matrix mul;
            Matrix.Scaling(scaling, scaling, scaling, out result);

            // (Mrc)-1
            Matrix.Translation(-rotationCenter.X, -rotationCenter.Y, -rotationCenter.Z, out mul);
            Matrix.Multiply(ref result, ref mul, out result);

            // Mr
            Matrix.RotationQuaternion(ref rotation, out mul);
            Matrix.Multiply(ref result, ref mul, out result);

            // Mrc
            Matrix.Translation(rotationCenter.X, rotationCenter.Y, rotationCenter.Z, out mul);
            Matrix.Multiply(ref result, ref mul, out result);

            // Mt
            Matrix.Translation(ref translation, out mul);
            Matrix.Multiply(ref result, ref mul, out result);
        }

        /// <summary>
        /// Creates a 2D affine transformation matrix.
        /// </summary>
        /// <param name="scaling">Scaling factor.</param>
        /// <param name="rotationCenter">The center of the rotation.</param>
        /// <param name="rotation">The rotation of the transformation.</param>
        /// <param name="translation">The translation factor of the transformation.</param>
        /// <returns>The created affine transformation matrix.</returns>
        /// <remarks>
        /// This function calculates the affine transformation matrix with the following formula, with matrix concatenation evaluated in left-to-right order:
        ///
        /// Mout = Ms * (Mrc)-1 * Mr * Mrc * Mt
        ///
        /// where:
        ///
        /// Mout = output matrix (pOut)
        ///
        /// Ms = scaling matrix (Scaling)
        ///
        /// Mrc = center of rotation matrix (pRotationCenter)
        ///
        /// Mr = rotation matrix (Rotation)
        ///
        /// Mt = translation matrix (pTranslation)
        ///
        /// </remarks>
        public static Matrix AffineTransformation2D(float scaling, Vector2 rotationCenter, float rotation, Vector2 translation)
        {
            Matrix mul;

            // Ms
            Matrix result;
            Matrix.Scaling(scaling, scaling, 1, out result);

            // (Mrc)-1
            Matrix.Translation(-rotationCenter.X, -rotationCenter.Y, 0, out mul);
            Matrix.Multiply(ref result, ref mul, out result);

            // Mr
            Matrix.RotationZ(rotation, out mul);
            Matrix.Multiply(ref result, ref mul, out result);

            // Mrc
            Matrix.Translation(rotationCenter.X, rotationCenter.Y, 0, out mul);
            Matrix.Multiply(ref result, ref mul, out result);

            // Mt
            Matrix.Translation(translation.X, translation.Y, 0, out mul);
            Matrix.Multiply(ref result, ref mul, out result);

            return result;
        }

        /// <summary>
        /// Creates a 2D affine transformation matrix.
        /// </summary>
        /// <param name="scaling">Scaling factor.</param>
        /// <param name="rotationCenter">The center of the rotation.</param>
        /// <param name="rotation">The rotation of the transformation.</param>
        /// <param name="translation">The translation factor of the transformation.</param>
        /// <param name="result">When the method completes, contains the created affine transformation matrix.</param>
        /// <remarks>
        /// This function calculates the affine transformation matrix with the following formula, with matrix concatenation evaluated in left-to-right order:
        ///
        /// Mout = Ms * (Mrc)-1 * Mr * Mrc * Mt
        ///
        /// where:
        ///
        /// Mout = output matrix (pOut)
        ///
        /// Ms = scaling matrix (Scaling)
        ///
        /// Mrc = center of rotation matrix (pRotationCenter)
        ///
        /// Mr = rotation matrix (Rotation)
        ///
        /// Mt = translation matrix (pTranslation)
        ///
        /// </remarks>
        public static void AffineTransformation2D(float scaling, ref Vector2 rotationCenter,
            float rotation, ref Vector2 translation, out Matrix result)
        {
            Matrix mul;
            // Ms
            Matrix.Scaling(scaling, scaling, 1, out result);

            // (Mrc)-1
            Matrix.Translation(-rotationCenter.X, -rotationCenter.Y, 0, out mul);
            Matrix.Multiply(ref result, ref mul, out result);

            // Mr
            Matrix.RotationZ(rotation, out mul);
            Matrix.Multiply(ref result, ref mul, out result);

            // Mrc
            Matrix.Translation(rotationCenter.X, rotationCenter.Y, 0, out mul);
            Matrix.Multiply(ref result, ref mul, out result);

            // Mt
            Matrix.Translation(translation.X, translation.Y, 0, out mul);
            Matrix.Multiply(ref result, ref mul, out result);
        }

        /// <summary>
        /// Creates a transformation matrix.
        /// </summary>
        /// <param name="scalingCenter">Center point of the scaling operation.</param>
        /// <param name="scalingRotation">Scaling rotation amount.</param>
        /// <param name="scaling">Scaling factor.</param>
        /// <param name="rotationCenter">The center of the rotation.</param>
        /// <param name="rotation">The rotation of the transformation.</param>
        /// <param name="translation">The translation factor of the transformation.</param>
        /// <returns>The created transformation matrix.</returns>
        /// <remarks>
        /// This function calculates the transformation matrix with the following formula, with matrix concatenation evaluated in left-to-right order:
        ///
        /// Mout = (Msc)-1 * (Msr)-1* Ms * Msr * Msc * (Mrc)-1* Mr * Mrc * Mt
        ///
        /// where:
        ///
        /// Mout = output matrix (pOut)
        ///
        /// Msc = scaling center matrix (pScalingCenter)
        ///
        /// Msr = scaling rotation matrix (pScalingRotation)
        ///
        /// Ms = scaling matrix (pScaling)
        ///
        /// Mrc = center of rotation matrix (pRotationCenter)
        ///
        /// Mr = rotation matrix (pRotation)
        ///
        /// Mt = translation matrix (pTranslation)
        /// </remarks>
        public static Matrix Transformation(Vector3 scalingCenter, Quaternion scalingRotation, Vector3 scaling,
            Vector3 rotationCenter, Quaternion rotation, Vector3 translation)
        {
            Matrix mul;

            // (Msc)-1
            Matrix result;
            Matrix.Translation(-scalingCenter.X, -scalingCenter.Y, -scalingCenter.Z, out result);

            // (Msr)-1
            Quaternion temp = scalingRotation;
            temp.Invert();
            Matrix.RotationQuaternion(ref scalingRotation, out mul);
            Matrix.Multiply(ref result, ref mul, out result);

            // Ms
            Matrix.Scaling(ref scaling, out mul);
            Matrix.Multiply(ref result, ref mul, out result);

            // Msr
            Matrix.RotationQuaternion(ref scalingRotation, out mul);
            Matrix.Multiply(ref result, ref mul, out result);

            // Msc
            Matrix.Translation(scalingCenter.X, scalingCenter.Y, scalingCenter.Z, out mul);
            Matrix.Multiply(ref result, ref mul, out result);

            // (Mrc)-1
            Matrix.Translation(-rotationCenter.X, -rotationCenter.Y, -rotationCenter.Z, out mul);
            Matrix.Multiply(ref result, ref mul, out result);

            // Mr
            Matrix.RotationQuaternion(ref rotation, out mul);
            Matrix.Multiply(ref result, ref mul, out result);

            // Mrc
            Matrix.Translation(ref rotationCenter, out mul);
            Matrix.Multiply(ref result, ref mul, out result);

            // Mt
            Matrix.Translation(ref translation, out mul);
            Matrix.Multiply(ref result, ref mul, out result);
            return result;
        }

        /// <summary>
        /// Creates a transformation matrix.
        /// </summary>
        /// <param name="scalingCenter">Center point of the scaling operation.</param>
        /// <param name="scalingRotation">Scaling rotation amount.</param>
        /// <param name="scaling">Scaling factor.</param>
        /// <param name="rotationCenter">The center of the rotation.</param>
        /// <param name="rotation">The rotation of the transformation.</param>
        /// <param name="translation">The translation factor of the transformation.</param>
        /// <param name="result">When the method completes, contains the created transformation matrix.</param>
        /// <remarks>
        /// This function calculates the transformation matrix with the following formula, with matrix concatenation evaluated in left-to-right order:
        ///
        /// Mout = (Msc)-1 * (Msr)-1* Ms * Msr * Msc * (Mrc)-1* Mr * Mrc * Mt
        ///
        /// where:
        ///
        /// Mout = output matrix (pOut)
        ///
        /// Msc = scaling center matrix (pScalingCenter)
        ///
        /// Msr = scaling rotation matrix (pScalingRotation)
        ///
        /// Ms = scaling matrix (pScaling)
        ///
        /// Mrc = center of rotation matrix (pRotationCenter)
        ///
        /// Mr = rotation matrix (pRotation)
        ///
        /// Mt = translation matrix (pTranslation)
        /// </remarks>
        public static void Transformation(ref Vector3 scalingCenter, ref Quaternion scalingRotation, ref Vector3 scaling,
            ref Vector3 rotationCenter, ref Quaternion rotation, ref Vector3 translation, out Matrix result)
        {
            Matrix mul;
            Matrix.Translation(-scalingCenter.X, -scalingCenter.Y, -scalingCenter.Z, out result);

            // (Msr)-1
            Quaternion temp = scalingRotation;
            temp.Invert();
            Matrix.RotationQuaternion(ref scalingRotation, out mul);
            Matrix.Multiply(ref result, ref mul, out result);

            // Ms
            Matrix.Scaling(ref scaling, out mul);
            Matrix.Multiply(ref result, ref mul, out result);

            // Msr
            Matrix.RotationQuaternion(ref scalingRotation, out mul);
            Matrix.Multiply(ref result, ref mul, out result);

            // Msc
            Matrix.Translation(scalingCenter.X, scalingCenter.Y, scalingCenter.Z, out mul);
            Matrix.Multiply(ref result, ref mul, out result);

            // (Mrc)-1
            Matrix.Translation(-rotationCenter.X, -rotationCenter.Y, -rotationCenter.Z, out mul);
            Matrix.Multiply(ref result, ref mul, out result);

            // Mr
            Matrix.RotationQuaternion(ref rotation, out mul);
            Matrix.Multiply(ref result, ref mul, out result);

            // Mrc
            Matrix.Translation(ref rotationCenter, out mul);
            Matrix.Multiply(ref result, ref mul, out result);

            // Mt
            Matrix.Translation(ref translation, out mul);
            Matrix.Multiply(ref result, ref mul, out result);
        }

        /// <summary>
        /// Creates a 2D transformation matrix.
        /// </summary>
        /// <param name="scalingCenter">Center point of the scaling operation.</param>
        /// <param name="scalingRotation">Scaling rotation amount.</param>
        /// <param name="scaling">Scaling factor.</param>
        /// <param name="rotationCenter">The center of the rotation.</param>
        /// <param name="rotation">The rotation of the transformation.</param>
        /// <param name="translation">The translation factor of the transformation.</param>
        /// <returns>The created transformation matrix.</returns>
        /// <remarks>
        /// This function calculates the transformation matrix with the following formula, with matrix concatenation evaluated in left-to-right order:
        ///
        /// Mout = (Msc)-1* (Msr)-1* Ms * Msr * Msc * (Mrc)-1* Mr * Mrc * Mt
        ///
        /// where:
        ///
        /// Mout = output matrix (pOut)
        ///
        /// Msc = scaling center matrix (pScalingCenter)
        ///
        /// Msr = scaling rotation matrix (pScalingRotation)
        ///
        /// Ms = scaling matrix (pScaling)
        ///
        /// Mrc = center of rotation matrix (pRotationCenter)
        ///
        /// Mr = rotation matrix (Rotation)
        ///
        /// Mt = translation matrix (pTranslation)
        /// </remarks>
        public static Matrix Transformation2D(Vector2 scalingCenter, float scalingRotation, Vector2 scaling,
            Vector2 rotationCenter, float rotation, Vector2 translation)
        {
            Matrix mul;

            // (Msc)-1
            Matrix result;
            Matrix.Translation(-scalingCenter.X, -scalingCenter.Y, 0, out result);

            // (Msr)-1
            Matrix.RotationZ(scalingRotation, out mul);
            Matrix.Multiply(ref result, ref mul, out result);

            // Ms
            Matrix.Scaling(scalingCenter.X, scalingCenter.Y, 1, out mul);
            Matrix.Multiply(ref result, ref mul, out result);

            // Msr
            Matrix.RotationZ(scalingRotation, out mul);
            Matrix.Multiply(ref result, ref mul, out result);

            // Msc
            Matrix.Translation(scalingCenter.X, scalingCenter.Y, 0, out mul);
            Matrix.Multiply(ref result, ref mul, out result);



            // (Mrc)-1
            Matrix.Translation(-rotationCenter.X, -rotationCenter.Y, 0, out mul);
            Matrix.Multiply(ref result, ref mul, out result);

            // Mr
            Matrix.RotationZ(rotation, out mul);
            Matrix.Multiply(ref result, ref mul, out result);

            // Mrc
            Matrix.Translation(rotationCenter.X, rotationCenter.Y, 0, out mul);
            Matrix.Multiply(ref result, ref mul, out result);

            // Mt
            Matrix.Translation(translation.X, translation.Y, 0, out mul);
            Matrix.Multiply(ref result, ref mul, out result);
            return result;
        }

        /// <summary>
        /// Creates a 2D transformation matrix.
        /// </summary>
        /// <param name="scalingCenter">Center point of the scaling operation.</param>
        /// <param name="scalingRotation">Scaling rotation amount.</param>
        /// <param name="scaling">Scaling factor.</param>
        /// <param name="rotationCenter">The center of the rotation.</param>
        /// <param name="rotation">The rotation of the transformation.</param>
        /// <param name="translation">The translation factor of the transformation.</param>
        /// <param name="result">When the method completes, contains the created transformation matrix.</param>
        /// <remarks>
        /// This function calculates the transformation matrix with the following formula, with matrix concatenation evaluated in left-to-right order:
        ///
        /// Mout = (Msc)-1* (Msr)-1* Ms * Msr * Msc * (Mrc)-1* Mr * Mrc * Mt
        ///
        /// where:
        ///
        /// Mout = output matrix (pOut)
        ///
        /// Msc = scaling center matrix (pScalingCenter)
        ///
        /// Msr = scaling rotation matrix (pScalingRotation)
        ///
        /// Ms = scaling matrix (pScaling)
        ///
        /// Mrc = center of rotation matrix (pRotationCenter)
        ///
        /// Mr = rotation matrix (Rotation)
        ///
        /// Mt = translation matrix (pTranslation)
        /// </remarks>
        public static void Transformation2D(ref Vector2 scalingCenter, float scalingRotation, ref Vector2 scaling,
            ref Vector2 rotationCenter, float rotation, ref Vector2 translation, out Matrix result)
        {
            Matrix mul;

            // (Msc)-1
            Matrix.Translation(-scalingCenter.X, -scalingCenter.Y, 0, out result);

            // (Msr)-1
            Matrix.RotationZ(scalingRotation, out mul);
            Matrix.Multiply(ref result, ref mul, out result);

            // Ms
            Matrix.Scaling(scalingCenter.X, scalingCenter.Y, 1, out mul);
            Matrix.Multiply(ref result, ref mul, out result);

            // Msr
            Matrix.RotationZ(scalingRotation, out mul);
            Matrix.Multiply(ref result, ref mul, out result);

            // Msc
            Matrix.Translation(scalingCenter.X, scalingCenter.Y, 0, out mul);
            Matrix.Multiply(ref result, ref mul, out result);



            // (Mrc)-1
            Matrix.Translation(-rotationCenter.X, -rotationCenter.Y, 0, out mul);
            Matrix.Multiply(ref result, ref mul, out result);

            // Mr
            Matrix.RotationZ(rotation, out mul);
            Matrix.Multiply(ref result, ref mul, out result);

            // Mrc
            Matrix.Translation(rotationCenter.X, rotationCenter.Y, 0, out mul);
            Matrix.Multiply(ref result, ref mul, out result);

            // Mt
            Matrix.Translation(translation.X, translation.Y, 0, out mul);
            Matrix.Multiply(ref result, ref mul, out result);
        }

        #endregion

        #region Operators(运算符)
        /// <summary>
        /// Negates a matrix.
        /// </summary>
        /// <param name="matrix">The matrix to negate.</param>
        /// <returns>The negated matrix.</returns>
        public static Matrix operator -(Matrix matrix)
        {
            Matrix result;
            result.M11 = -matrix.M11;
            result.M12 = -matrix.M12;
            result.M13 = -matrix.M13;
            result.M14 = -matrix.M14;
            result.M21 = -matrix.M21;
            result.M22 = -matrix.M22;
            result.M23 = -matrix.M23;
            result.M24 = -matrix.M24;
            result.M31 = -matrix.M31;
            result.M32 = -matrix.M32;
            result.M33 = -matrix.M33;
            result.M34 = -matrix.M34;
            result.M41 = -matrix.M41;
            result.M42 = -matrix.M42;
            result.M43 = -matrix.M43;
            result.M44 = -matrix.M44;
            return result;
        }

        /// <summary>
        /// Adds two matricies.
        /// </summary>
        /// <param name="left">The first matrix to add.</param>
        /// <param name="right">The second matrix to add.</param>
        /// <returns>The sum of the two matricies.</returns>
        public static Matrix operator +(Matrix left, Matrix right)
        {
            Matrix result;
            result.M11 = left.M11 + right.M11;
            result.M12 = left.M12 + right.M12;
            result.M13 = left.M13 + right.M13;
            result.M14 = left.M14 + right.M14;
            result.M21 = left.M21 + right.M21;
            result.M22 = left.M22 + right.M22;
            result.M23 = left.M23 + right.M23;
            result.M24 = left.M24 + right.M24;
            result.M31 = left.M31 + right.M31;
            result.M32 = left.M32 + right.M32;
            result.M33 = left.M33 + right.M33;
            result.M34 = left.M34 + right.M34;
            result.M41 = left.M41 + right.M41;
            result.M42 = left.M42 + right.M42;
            result.M43 = left.M43 + right.M43;
            result.M44 = left.M44 + right.M44;
            return result;
        }

        /// <summary>
        /// Subtracts two matricies.
        /// </summary>
        /// <param name="left">The first matrix to subtract.</param>
        /// <param name="right">The second matrix to subtract.</param>
        /// <returns>The difference between the two matricies.</returns>
        public static Matrix operator -(Matrix left, Matrix right)
        {
            Matrix result;
            result.M11 = left.M11 - right.M11;
            result.M12 = left.M12 - right.M12;
            result.M13 = left.M13 - right.M13;
            result.M14 = left.M14 - right.M14;
            result.M21 = left.M21 - right.M21;
            result.M22 = left.M22 - right.M22;
            result.M23 = left.M23 - right.M23;
            result.M24 = left.M24 - right.M24;
            result.M31 = left.M31 - right.M31;
            result.M32 = left.M32 - right.M32;
            result.M33 = left.M33 - right.M33;
            result.M34 = left.M34 - right.M34;
            result.M41 = left.M41 - right.M41;
            result.M42 = left.M42 - right.M42;
            result.M43 = left.M43 - right.M43;
            result.M44 = left.M44 - right.M44;
            return result;
        }

        /// <summary>
        /// Divides two matricies.
        /// </summary>
        /// <param name="left">The first matrix to divide.</param>
        /// <param name="right">The second matrix to divide.</param>
        /// <returns>The quotient of the two matricies.</returns>
        public static Matrix operator /(Matrix left, Matrix right)
        {
            Matrix result;
            result.M11 = left.M11 / right.M11;
            result.M12 = left.M12 / right.M12;
            result.M13 = left.M13 / right.M13;
            result.M14 = left.M14 / right.M14;
            result.M21 = left.M21 / right.M21;
            result.M22 = left.M22 / right.M22;
            result.M23 = left.M23 / right.M23;
            result.M24 = left.M24 / right.M24;
            result.M31 = left.M31 / right.M31;
            result.M32 = left.M32 / right.M32;
            result.M33 = left.M33 / right.M33;
            result.M34 = left.M34 / right.M34;
            result.M41 = left.M41 / right.M41;
            result.M42 = left.M42 / right.M42;
            result.M43 = left.M43 / right.M43;
            result.M44 = left.M44 / right.M44;
            return result;
        }

        /// <summary>
        /// Scales a matrix by a given value.
        /// </summary>
        /// <param name="left">The matrix to scale.</param>
        /// <param name="right">The amount by which to scale.</param>
        /// <returns>The scaled matrix.</returns>
        public static Matrix operator /(Matrix left, float right)
        {
            Matrix result;
            result.M11 = left.M11 / right;
            result.M12 = left.M12 / right;
            result.M13 = left.M13 / right;
            result.M14 = left.M14 / right;
            result.M21 = left.M21 / right;
            result.M22 = left.M22 / right;
            result.M23 = left.M23 / right;
            result.M24 = left.M24 / right;
            result.M31 = left.M31 / right;
            result.M32 = left.M32 / right;
            result.M33 = left.M33 / right;
            result.M34 = left.M34 / right;
            result.M41 = left.M41 / right;
            result.M42 = left.M42 / right;
            result.M43 = left.M43 / right;
            result.M44 = left.M44 / right;
            return result;
        }

        /// <summary>
        /// Multiplies two matricies.
        /// </summary>
        /// <param name="left">The first matrix to multiply.</param>
        /// <param name="right">The second matrix to multiply.</param>
        /// <returns>The product of the two matricies.</returns>
        /// <remarks>The result represents the transformation M1 followed by the transformation M2 (Out = M1 * M2).</remarks>
        public static Matrix operator *(Matrix left, Matrix right)
        {
            Matrix result;
            result.M11 = (left.M11 * right.M11) + (left.M12 * right.M21) + (left.M13 * right.M31) + (left.M14 * right.M41);
            result.M12 = (left.M11 * right.M12) + (left.M12 * right.M22) + (left.M13 * right.M32) + (left.M14 * right.M42);
            result.M13 = (left.M11 * right.M13) + (left.M12 * right.M23) + (left.M13 * right.M33) + (left.M14 * right.M43);
            result.M14 = (left.M11 * right.M14) + (left.M12 * right.M24) + (left.M13 * right.M34) + (left.M14 * right.M44);
            result.M21 = (left.M21 * right.M11) + (left.M22 * right.M21) + (left.M23 * right.M31) + (left.M24 * right.M41);
            result.M22 = (left.M21 * right.M12) + (left.M22 * right.M22) + (left.M23 * right.M32) + (left.M24 * right.M42);
            result.M23 = (left.M21 * right.M13) + (left.M22 * right.M23) + (left.M23 * right.M33) + (left.M24 * right.M43);
            result.M24 = (left.M21 * right.M14) + (left.M22 * right.M24) + (left.M23 * right.M34) + (left.M24 * right.M44);
            result.M31 = (left.M31 * right.M11) + (left.M32 * right.M21) + (left.M33 * right.M31) + (left.M34 * right.M41);
            result.M32 = (left.M31 * right.M12) + (left.M32 * right.M22) + (left.M33 * right.M32) + (left.M34 * right.M42);
            result.M33 = (left.M31 * right.M13) + (left.M32 * right.M23) + (left.M33 * right.M33) + (left.M34 * right.M43);
            result.M34 = (left.M31 * right.M14) + (left.M32 * right.M24) + (left.M33 * right.M34) + (left.M34 * right.M44);
            result.M41 = (left.M41 * right.M11) + (left.M42 * right.M21) + (left.M43 * right.M31) + (left.M44 * right.M41);
            result.M42 = (left.M41 * right.M12) + (left.M42 * right.M22) + (left.M43 * right.M32) + (left.M44 * right.M42);
            result.M43 = (left.M41 * right.M13) + (left.M42 * right.M23) + (left.M43 * right.M33) + (left.M44 * right.M43);
            result.M44 = (left.M41 * right.M14) + (left.M42 * right.M24) + (left.M43 * right.M34) + (left.M44 * right.M44);
            return result;
        }

        /// <summary>
        /// Scales a matrix by a given value.
        /// </summary>
        /// <param name="left">The matrix to scale.</param>
        /// <param name="right">The amount by which to scale.</param>
        /// <returns>The scaled matrix.</returns>
        public static Matrix operator *(Matrix left, float right)
        {
            Matrix result;
            result.M11 = left.M11 * right;
            result.M12 = left.M12 * right;
            result.M13 = left.M13 * right;
            result.M14 = left.M14 * right;
            result.M21 = left.M21 * right;
            result.M22 = left.M22 * right;
            result.M23 = left.M23 * right;
            result.M24 = left.M24 * right;
            result.M31 = left.M31 * right;
            result.M32 = left.M32 * right;
            result.M33 = left.M33 * right;
            result.M34 = left.M34 * right;
            result.M41 = left.M41 * right;
            result.M42 = left.M42 * right;
            result.M43 = left.M43 * right;
            result.M44 = left.M44 * right;
            return result;
        }

        /// <summary>
        /// Scales a matrix by a given value.
        /// </summary>
        /// <param name="right">The matrix to scale.</param>
        /// <param name="left">The amount by which to scale.</param>
        /// <returns>The scaled matrix.</returns>
        public static Matrix operator *(float left, Matrix right)
        {
            return left * right;
        }

        /// <summary>
        /// Tests for equality between two objects.
        /// </summary>
        /// <param name="left">The first value to compare.</param>
        /// <param name="right">The second value to compare.</param>
        /// <returns><c>true</c> if <paramref name="left"/> has the same value as <paramref name="right"/>; otherwise, <c>false</c>.</returns>
        public static bool operator ==(Matrix left, Matrix right)
        {
            return Matrix.Equals(ref left, ref right);
        }

        /// <summary>
        /// Tests for inequality between two objects.
        /// </summary>
        /// <param name="left">The first value to compare.</param>
        /// <param name="right">The second value to compare.</param>
        /// <returns><c>true</c> if <paramref name="left"/> has a different value than <paramref name="right"/>; otherwise, <c>false</c>.</returns>
        public static bool operator !=(Matrix left, Matrix right)
        {
            return !Matrix.Equals(ref left, ref right);
        }
        
        #endregion

        /// <summary>
        /// Converts the value of the object to its equivalent string representation.
        /// </summary>
        /// <returns>The string representation of the value of this instance.</returns>
        public override string ToString()
        {
            return string.Format(CultureInfo.CurrentCulture, "[[M11:{0} M12:{1} M13:{2} M14:{3}] [M21:{4} M22:{5} M23:{6} M24:{7}] [M31:{8} M32:{9} M33:{10} M34:{11}] [M41:{12} M42:{13} M43:{14} M44:{15}]]",
                M11.ToString(CultureInfo.CurrentCulture), M12.ToString(CultureInfo.CurrentCulture), M13.ToString(CultureInfo.CurrentCulture), M14.ToString(CultureInfo.CurrentCulture),
                M21.ToString(CultureInfo.CurrentCulture), M22.ToString(CultureInfo.CurrentCulture), M23.ToString(CultureInfo.CurrentCulture), M24.ToString(CultureInfo.CurrentCulture),
                M31.ToString(CultureInfo.CurrentCulture), M32.ToString(CultureInfo.CurrentCulture), M33.ToString(CultureInfo.CurrentCulture), M34.ToString(CultureInfo.CurrentCulture),
                M41.ToString(CultureInfo.CurrentCulture), M42.ToString(CultureInfo.CurrentCulture), M43.ToString(CultureInfo.CurrentCulture), M44.ToString(CultureInfo.CurrentCulture));
        }

        /// <summary>
        /// Returns the hash code for this instance.
        /// </summary>
        /// <returns>A 32-bit signed integer hash code.</returns>
        public override int GetHashCode()
        {
            return M11.GetHashCode() + M12.GetHashCode() + M13.GetHashCode() + M14.GetHashCode() +
                   M21.GetHashCode() + M22.GetHashCode() + M23.GetHashCode() + M24.GetHashCode() +
                   M31.GetHashCode() + M32.GetHashCode() + M33.GetHashCode() + M34.GetHashCode() +
                   M41.GetHashCode() + M42.GetHashCode() + M43.GetHashCode() + M44.GetHashCode();
        }

        /// <summary>
        /// Returns a value that indicates whether the current instance is equal to a specified object. 
        /// </summary>
        /// <param name="value">Object to make the comparison with.</param>
        /// <returns><c>true</c> if the current instance is equal to the specified object; <c>false</c> otherwise.</returns>
        public override bool Equals(object value)
        {
            if (value == null)
                return false;

            if (value.GetType() != GetType())
                return false;

            return Equals((Matrix)value);
        }

        /// <summary>
        /// Returns a value that indicates whether the current instance is equal to the specified object. 
        /// </summary>
        /// <param name="other">Object to make the comparison with.</param>
        /// <returns><c>true</c> if the current instance is equal to the specified object; <c>false</c> otherwise.</returns>
        public bool Equals(Matrix value)
        {
            return (M11 == value.M11 && M12 == value.M12 && M13 == value.M13 && M14 == value.M14 &&
                    M21 == value.M21 && M22 == value.M22 && M23 == value.M23 && M24 == value.M24 &&
                    M31 == value.M31 && M32 == value.M32 && M33 == value.M33 && M34 == value.M34 &&
                    M41 == value.M41 && M42 == value.M42 && M43 == value.M43 && M44 == value.M44);
        }

        /// <summary>
        /// Determines whether the specified object instances are considered equal. 
        /// </summary>
        /// <param name="value1"></param>
        /// <param name="value2"></param>
        /// <returns><c>true</c> if <paramref name="value1"/> is the same instance as <paramref name="value2"/> or 
        /// if both are <c>null</c> references or if <c>value1.Equals(value2)</c> returns <c>true</c>; otherwise, <c>false</c>.</returns>
        public static bool Equals(ref Matrix value1, ref Matrix value2)
        {
            return (value1.M11 == value2.M11 && value1.M12 == value2.M12 && value1.M13 == value2.M13 && value1.M14 == value2.M14 &&
                    value1.M21 == value2.M21 && value1.M22 == value2.M22 && value1.M23 == value2.M23 && value1.M24 == value2.M24 &&
                    value1.M31 == value2.M31 && value1.M32 == value2.M32 && value1.M33 == value2.M33 && value1.M34 == value2.M34 &&
                    value1.M41 == value2.M41 && value1.M42 == value2.M42 && value1.M43 == value2.M43 && value1.M44 == value2.M44);
        }

        #region Helper
        [StructLayout(LayoutKind.Sequential)]
        private struct CanonicalBasis
        {
            public Vector3 Row0;
            public Vector3 Row1;
            public Vector3 Row2;
        }

        [StructLayout(LayoutKind.Sequential)]
        private struct VectorBasis
        {
            public unsafe Vector3* Element0;
            public unsafe Vector3* Element1;
            public unsafe Vector3* Element2;
        }
        #endregion
    }
}
