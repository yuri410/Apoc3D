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
    /// Defines a three component vector.
    /// </summary>
    [Serializable, StructLayout(LayoutKind.Sequential), TypeConverter(typeof(Vector3Converter))]
    public unsafe struct Vector3 : IEquatable<Vector3>
    {
        #region Fields(字段)
        /// <summary>
        /// Gets or sets the X component of the vector.
        /// </summary>
        /// <value>The X component of the vector.</value>
        public float X;

        /// <summary>
        /// Gets or sets the Y component of the vector.
        /// </summary>
        /// <value>The Y component of the vector.</value>
        public float Y;

        /// <summary>
        /// Gets or sets the Z component of the vector.
        /// </summary>
        /// <value>The Z component of the vector.</value>
        public float Z;
        #endregion

        #region Properties(属性)

        public float this[int index]
        {
            get
            {
                switch (index)
                {
                    case 0:
                        return X;

                    case 1:
                        return Y;

                    case 2:
                        return Z;

                    default:
                        throw new IndexOutOfRangeException("Indices for Vector3 run from 0 to 2, inclusive.");
                }
            }
            set
            {
                switch (index)
                {
                    case 0:
                        X = value;
                        break;

                    case 1:
                        Y = value;
                        break;

                    case 2:
                        Z = value;
                        break;

                    default:
                        throw new IndexOutOfRangeException("Indices for Vector3 run from 0 to 2, inclusive.");
                }
            }
        }

        /// <summary>
        /// Gets a <see cref="Vector3"/> with all of its components set to zero.
        /// </summary>
        /// <value>A <see cref="Vector3"/> that has all of its components set to zero.</value>
        public static Vector3 Zero
        {
            get { return new Vector3(0, 0, 0); }
        }

        /// <summary>
        /// Gets the X unit <see cref="Vector3"/> (1, 0, 0).
        /// </summary>
        /// <value>A <see cref="Vector3"/> that has a value of (1, 0, 0).</value>
        public static Vector3 UnitX
        {
            get { return new Vector3(1, 0, 0); }
        }

        /// <summary>
        /// Gets the Y unit <see cref="Vector3"/> (0, 1, 0).
        /// </summary>
        /// <value>A <see cref="Vector3"/> that has a value of (0, 1, 0).</value>
        public static Vector3 UnitY
        {
            get { return new Vector3(0, 1, 0); }
        }

        /// <summary>
        /// Gets the Z unit <see cref="Vector3"/> (0, 0, 1).
        /// </summary>
        /// <value>A <see cref="Vector3"/> that has a value of (0, 0, 1).</value>
        public static Vector3 UnitZ
        {
            get { return new Vector3(0, 0, 1); }
        }

        /// <summary>
        /// Gets the size of the <see cref="Vector3"/> type, in bytes.
        /// </summary>
        public static int SizeInBytes
        {
            get { return sizeof(Vector3); }
        }

        #endregion

        #region Constructor(构造函数)
        /// <summary>
        /// Initializes a new instance of the <see cref="Vector3"/> class.
        /// </summary>
        /// <param name="value">The value that will be assigned to all components.</param>
        public Vector3(float value)
        {
            X = value;
            Y = value;
            Z = value;
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="Vector3"/> class.
        /// </summary>
        /// <param name="value">A vector containing the values with which to initialize the X and Y components</param>
        /// <param name="z">Initial value for the Z component of the vector.</param>
        public Vector3(Vector2 value, float z)
        {
            X = value.X;
            Y = value.Y;
            Z = z;
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="Vector3"/> class.
        /// </summary>
        /// <param name="x">Initial value for the X component of the vector.</param>
        /// <param name="y">Initial value for the Y component of the vector.</param>
        /// <param name="z">Initial value for the Z component of the vector.</param>
        public Vector3(float x, float y, float z)
        {
            X = x;
            Y = y;
            Z = z;
        }

        #endregion

        #region Methods(方法)

        /// <summary>
        /// Calculates the length of the vector.
        /// </summary>
        /// <returns>The length of the vector.</returns>
        public float Length()
        {
            return (float)Math.Sqrt(X * X + Y * Y + Z * Z);
        }

        /// <summary>
        /// Calculates the squared length of the vector.
        /// </summary>
        /// <returns>The squared length of the vector.</returns>
        public float LengthSquared()
        {
            return X * X + Y * Y + Z * Z;
        }

        /// <summary>
        /// Converts the vector into a unit vector.
        /// </summary>
        public void Normalize()
        {
            float length = Length();
            if (length == 0)
                return;
            float num = 1.0f / length;
            X *= num;
            Y *= num;
            Z *= num;
        }

        #endregion


        /// <summary>
        /// Adds two vectors.
        /// </summary>
        /// <param name="left">The first vector to add.</param>
        /// <param name="right">The second vector to add.</param>
        /// <returns>The sum of the two vectors.</returns>
        public static Vector3 Add(Vector3 left, Vector3 right)
        {
            return new Vector3(left.X + right.X, left.Y + right.Y, left.Z + right.Z);
        }

        /// <summary>
        /// Adds two vectors.
        /// </summary>
        /// <param name="left">The first vector to add.</param>
        /// <param name="right">The second vector to add.</param>
        /// <param name="result">When the method completes, contains the sum of the two vectors.</param>
        public static void Add(ref Vector3 left, ref Vector3 right, out Vector3 result)
        {
            result = new Vector3(left.X + right.X, left.Y + right.Y, left.Z + right.Z);
        }

        /// <summary>
        /// Subtracts two vectors.
        /// </summary>
        /// <param name="left">The first vector to subtract.</param>
        /// <param name="right">The second vector to subtract.</param>
        /// <returns>The difference of the two vectors.</returns>
        public static Vector3 Subtract(Vector3 left, Vector3 right)
        {
            return new Vector3(left.X - right.X, left.Y - right.Y, left.Z - right.Z);
        }

        /// <summary>
        /// Subtracts two vectors.
        /// </summary>
        /// <param name="left">The first vector to subtract.</param>
        /// <param name="right">The second vector to subtract.</param>
        /// <param name="result">When the method completes, contains the difference of the two vectors.</param>
        public static void Subtract(ref Vector3 left, ref Vector3 right, out Vector3 result)
        {
            result = new Vector3(left.X - right.X, left.Y - right.Y, left.Z - right.Z);
        }

        /// <summary>
        /// Scales a vector by the given value.
        /// </summary>
        /// <param name="value">The vector to scale.</param>
        /// <param name="scale">The amount by which to scale the vector.</param>
        /// <returns>The scaled vector.</returns>
        public static Vector3 Multiply(Vector3 value, float scale)
        {
            return new Vector3(value.X * scale, value.Y * scale, value.Z * scale);
        }

        /// <summary>
        /// Scales a vector by the given value.
        /// </summary>
        /// <param name="vector">The vector to scale.</param>
        /// <param name="scale">The amount by which to scale the vector.</param>
        /// <param name="result">When the method completes, contains the scaled vector.</param>
        public static void Multiply(ref Vector3 vector, float scale, out Vector3 result)
        {
            result = new Vector3(vector.X * scale, vector.Y * scale, vector.Z * scale);
        }

        /// <summary>
        /// Modulates a vector by another.
        /// </summary>
        /// <param name="left">The first vector to modulate.</param>
        /// <param name="right">The second vector to modulate.</param>
        /// <returns>The modulated vector.</returns>
        public static Vector3 Modulate(Vector3 left, Vector3 right)
        {
            return new Vector3(left.X * right.X, left.Y * right.Y, left.Z * right.Z);
        }

        /// <summary>
        /// Modulates a vector by another.
        /// </summary>
        /// <param name="left">The first vector to modulate.</param>
        /// <param name="right">The second vector to modulate.</param>
        /// <param name="result">When the moethod completes, contains the modulated vector.</param>
        public static void Modulate(ref Vector3 left, ref Vector3 right, out Vector3 result)
        {
            result = new Vector3(left.X * right.X, left.Y * right.Y, left.Z * right.Z);
        }

        /// <summary>
        /// Scales a vector by the given value.
        /// </summary>
        /// <param name="value">The vector to scale.</param>
        /// <param name="scale">The amount by which to scale the vector.</param>
        /// <returns>The scaled vector.</returns>
        public static Vector3 Divide(Vector3 value, float scale)
        {
            return new Vector3(value.X / scale, value.Y / scale, value.Z / scale);
        }

        /// <summary>
        /// Scales a vector by the given value.
        /// </summary>
        /// <param name="vector">The vector to scale.</param>
        /// <param name="scale">The amount by which to scale the vector.</param>
        /// <param name="result">When the method completes, contains the scaled vector.</param>
        public static void Divide(ref Vector3 vector, float scale, out Vector3 result)
        {
            result = new Vector3(vector.X / scale, vector.Y / scale, vector.Z / scale);
        }

        /// <summary>
        /// Reverses the direction of a given vector.
        /// </summary>
        /// <param name="value">The vector to negate.</param>
        /// <returns>A vector facing in the opposite direction.</returns>
        public static Vector3 Negate(Vector3 value)
        {
            return new Vector3(-value.X, -value.Y, -value.Z);
        }

        /// <summary>
        /// Reverses the direction of a given vector.
        /// </summary>
        /// <param name="value">The vector to negate.</param>
        /// <param name="result">When the method completes, contains a vector facing in the opposite direction.</param>
        public static void Negate(ref Vector3 value, out Vector3 result)
        {
            result = new Vector3(-value.X, -value.Y, -value.Z);
        }

        /// <summary>
        /// Returns a <see cref="Vector3"/> containing the 3D Cartesian coordinates of a point specified in Barycentric coordinates relative to a 3D triangle.
        /// </summary>
        /// <param name="value1">A <see cref="Vector3"/> containing the 3D Cartesian coordinates of vertex 1 of the triangle.</param>
        /// <param name="value2">A <see cref="Vector3"/> containing the 3D Cartesian coordinates of vertex 2 of the triangle.</param>
        /// <param name="value3">A <see cref="Vector3"/> containing the 3D Cartesian coordinates of vertex 3 of the triangle.</param>
        /// <param name="amount1">Barycentric coordinate b2, which expresses the weighting factor toward vertex 2 (specified in <paramref name="value2"/>).</param>
        /// <param name="amount2">Barycentric coordinate b3, which expresses the weighting factor toward vertex 3 (specified in <paramref name="value3"/>).</param>
        /// <returns>A new <see cref="Vector3"/> containing the 3D Cartesian coordinates of the specified point.</returns>
        public static Vector3 Barycentric(Vector3 value1, Vector3 value2, Vector3 value3, float amount1, float amount2)
        {
            Vector3 vector;
            vector.X = (value1.X + (amount1 * (value2.X - value1.X))) + (amount2 * (value3.X - value1.X));
            vector.Y = (value1.Y + (amount1 * (value2.Y - value1.Y))) + (amount2 * (value3.Y - value1.Y));
            vector.Z = (value1.Z + (amount1 * (value2.Z - value1.Z))) + (amount2 * (value3.Z - value1.Z));
            return vector;
        }

        /// <summary>
        /// Returns a <see cref="Vector3"/> containing the 3D Cartesian coordinates of a point specified in Barycentric coordinates relative to a 3D triangle.
        /// </summary>
        /// <param name="value1">A <see cref="Vector3"/> containing the 3D Cartesian coordinates of vertex 1 of the triangle.</param>
        /// <param name="value2">A <see cref="Vector3"/> containing the 3D Cartesian coordinates of vertex 2 of the triangle.</param>
        /// <param name="value3">A <see cref="Vector3"/> containing the 3D Cartesian coordinates of vertex 3 of the triangle.</param>
        /// <param name="amount1">Barycentric coordinate b2, which expresses the weighting factor toward vertex 2 (specified in <paramref name="value2"/>).</param>
        /// <param name="amount2">Barycentric coordinate b3, which expresses the weighting factor toward vertex 3 (specified in <paramref name="value3"/>).</param>
        /// <param name="result">When the method completes, contains the 3D Cartesian coordinates of the specified point.</param>
        public static void Barycentric(ref Vector3 value1, ref Vector3 value2, ref Vector3 value3, float amount1, float amount2, out Vector3 result)
        {
            result = new Vector3((value1.X + (amount1 * (value2.X - value1.X))) + (amount2 * (value3.X - value1.X)),
                (value1.Y + (amount1 * (value2.Y - value1.Y))) + (amount2 * (value3.Y - value1.Y)),
                (value1.Z + (amount1 * (value2.Z - value1.Z))) + (amount2 * (value3.Z - value1.Z)));
        }

        /// <summary>
        /// Performs a Catmull-Rom interpolation using the specified positions.
        /// </summary>
        /// <param name="value1">The first position in the interpolation.</param>
        /// <param name="value2">The second position in the interpolation.</param>
        /// <param name="value3">The third position in the interpolation.</param>
        /// <param name="value4">The fourth position in the interpolation.</param>
        /// <param name="amount">Weighting factor.</param>
        /// <returns>A vector that is the result of the Catmull-Rom interpolation.</returns>
        public static Vector3 CatmullRom(Vector3 value1, Vector3 value2, Vector3 value3, Vector3 value4, float amount)
        {
            Vector3 vector;
            float squared = amount * amount;
            float cubed = amount * squared;

            vector.X = 0.5f * ((((2.0f * value2.X) + ((-value1.X + value3.X) * amount)) +
                (((((2.0f * value1.X) - (5.0f * value2.X)) + (4.0f * value3.X)) - value4.X) * squared)) +
                ((((-value1.X + (3.0f * value2.X)) - (3.0f * value3.X)) + value4.X) * cubed));

            vector.Y = 0.5f * ((((2.0f * value2.Y) + ((-value1.Y + value3.Y) * amount)) +
                (((((2.0f * value1.Y) - (5.0f * value2.Y)) + (4.0f * value3.Y)) - value4.Y) * squared)) +
                ((((-value1.Y + (3.0f * value2.Y)) - (3.0f * value3.Y)) + value4.Y) * cubed));

            vector.Z = 0.5f * ((((2.0f * value2.Z) + ((-value1.Z + value3.Z) * amount)) +
                (((((2.0f * value1.Z) - (5.0f * value2.Z)) + (4.0f * value3.Z)) - value4.Z) * squared)) +
                ((((-value1.Z + (3.0f * value2.Z)) - (3.0f * value3.Z)) + value4.Z) * cubed));

            return vector;
        }

        /// <summary>
        /// Performs a Catmull-Rom interpolation using the specified positions.
        /// </summary>
        /// <param name="value1">The first position in the interpolation.</param>
        /// <param name="value2">The second position in the interpolation.</param>
        /// <param name="value3">The third position in the interpolation.</param>
        /// <param name="value4">The fourth position in the interpolation.</param>
        /// <param name="amount">Weighting factor.</param>
        /// <param name="result">When the method completes, contains the result of the Catmull-Rom interpolation.</param>
        public static void CatmullRom(ref Vector3 value1, ref Vector3 value2, ref Vector3 value3, ref Vector3 value4, float amount, out Vector3 result)
        {
            float squared = amount * amount;
            float cubed = amount * squared;

            Vector3 r;

            r.X = 0.5f * ((((2.0f * value2.X) + ((-value1.X + value3.X) * amount)) +
                (((((2.0f * value1.X) - (5.0f * value2.X)) + (4.0f * value3.X)) - value4.X) * squared)) +
                ((((-value1.X + (3.0f * value2.X)) - (3.0f * value3.X)) + value4.X) * cubed));

            r.Y = 0.5f * ((((2.0f * value2.Y) + ((-value1.Y + value3.Y) * amount)) +
                (((((2.0f * value1.Y) - (5.0f * value2.Y)) + (4.0f * value3.Y)) - value4.Y) * squared)) +
                ((((-value1.Y + (3.0f * value2.Y)) - (3.0f * value3.Y)) + value4.Y) * cubed));

            r.Z = 0.5f * ((((2.0f * value2.Z) + ((-value1.Z + value3.Z) * amount)) +
                (((((2.0f * value1.Z) - (5.0f * value2.Z)) + (4.0f * value3.Z)) - value4.Z) * squared)) +
                ((((-value1.Z + (3.0f * value2.Z)) - (3.0f * value3.Z)) + value4.Z) * cubed));

            result = r;
        }

        /// <summary>
        /// Restricts a value to be within a specified range.
        /// </summary>
        /// <param name="value">The value to clamp.</param>
        /// <param name="min">The minimum value.</param>
        /// <param name="max">The maximum value.</param>
        /// <returns>The clamped value.</returns>
        public static Vector3 Clamp(Vector3 value, Vector3 min, Vector3 max)
        {
            float x = value.X;
            x = (x > max.X) ? max.X : x;
            x = (x < min.X) ? min.X : x;

            float y = value.Y;
            y = (y > max.Y) ? max.Y : y;
            y = (y < min.Y) ? min.Y : y;

            float z = value.Z;
            z = (z > max.Z) ? max.Z : z;
            z = (z < min.Z) ? min.Z : z;

            return new Vector3(x, y, z);
        }

        /// <summary>
        /// Restricts a value to be within a specified range.
        /// </summary>
        /// <param name="value">The value to clamp.</param>
        /// <param name="min">The minimum value.</param>
        /// <param name="max">The maximum value.</param>
        /// <param name="result">When the method completes, contains the clamped value.</param>
        public static void Clamp(ref Vector3 value, ref Vector3 min, ref Vector3 max, out Vector3 result)
        {
            float x = value.X;
            x = (x > max.X) ? max.X : x;
            x = (x < min.X) ? min.X : x;

            float y = value.Y;
            y = (y > max.Y) ? max.Y : y;
            y = (y < min.Y) ? min.Y : y;

            float z = value.Z;
            z = (z > max.Z) ? max.Z : z;
            z = (z < min.Z) ? min.Z : z;

            result = new Vector3(x, y, z);
        }

        /// <summary>
        /// Performs a Hermite spline interpolation.
        /// </summary>
        /// <param name="value1">First source position vector.</param>
        /// <param name="tangent1">First source tangent vector.</param>
        /// <param name="value2">Second source position vector.</param>
        /// <param name="tangent2">Second source tangent vector.</param>
        /// <param name="amount">Weighting factor.</param>
        /// <returns>The result of the Hermite spline interpolation.</returns>
        public static Vector3 Hermite(Vector3 value1, Vector3 tangent1, Vector3 value2, Vector3 tangent2, float amount)
        {
            Vector3 vector;
            float squared = amount * amount;
            float cubed = amount * squared;
            float part1 = ((2.0f * cubed) - (3.0f * squared)) + 1.0f;
            float part2 = (-2.0f * cubed) + (3.0f * squared);
            float part3 = (cubed - (2.0f * squared)) + amount;
            float part4 = cubed - squared;

            vector.X = (((value1.X * part1) + (value2.X * part2)) + (tangent1.X * part3)) + (tangent2.X * part4);
            vector.Y = (((value1.Y * part1) + (value2.Y * part2)) + (tangent1.Y * part3)) + (tangent2.Y * part4);
            vector.Z = (((value1.Z * part1) + (value2.Z * part2)) + (tangent1.Z * part3)) + (tangent2.Z * part4);

            return vector;
        }

        /// <summary>
        /// Performs a Hermite spline interpolation.
        /// </summary>
        /// <param name="value1">First source position vector.</param>
        /// <param name="tangent1">First source tangent vector.</param>
        /// <param name="value2">Second source position vector.</param>
        /// <param name="tangent2">Second source tangent vector.</param>
        /// <param name="amount">Weighting factor.</param>
        /// <param name="result">When the method completes, contains the result of the Hermite spline interpolation.</param>
        public static void Hermite(ref Vector3 value1, ref Vector3 tangent1, ref Vector3 value2, ref Vector3 tangent2, float amount,
            out Vector3 result)
        {
            float squared = amount * amount;
            float cubed = amount * squared;
            float part1 = ((2.0f * cubed) - (3.0f * squared)) + 1.0f;
            float part2 = (-2.0f * cubed) + (3.0f * squared);
            float part3 = (cubed - (2.0f * squared)) + amount;
            float part4 = cubed - squared;

            result.X = (((value1.X * part1) + (value2.X * part2)) + (tangent1.X * part3)) + (tangent2.X * part4);
            result.Y = (((value1.Y * part1) + (value2.Y * part2)) + (tangent1.Y * part3)) + (tangent2.Y * part4);
            result.Z = (((value1.Z * part1) + (value2.Z * part2)) + (tangent1.Z * part3)) + (tangent2.Z * part4);
        }

        /// <summary>
        /// Performs a linear interpolation between two vectors.
        /// </summary>
        /// <param name="start">Start vector.</param>
        /// <param name="end">End vector.</param>
        /// <param name="amount">Value between 0 and 1 indicating the weight of <paramref name="end"/>.</param>
        /// <returns>The linear interpolation of the two vectors.</returns>
        /// <remarks>
        /// This method performs the linear interpolation based on the following formula.
        /// <code>start + (end - start) * amount</code>
        /// Passing <paramref name="amount"/> a value of 0 will cause <paramref name="start"/> to be returned; a value of 1 will cause <paramref name="end"/> to be returned. 
        /// </remarks>
        public static Vector3 Lerp(Vector3 start, Vector3 end, float amount)
        {
            Vector3 vector;

            vector.X = start.X + ((end.X - start.X) * amount);
            vector.Y = start.Y + ((end.Y - start.Y) * amount);
            vector.Z = start.Z + ((end.Z - start.Z) * amount);

            return vector;
        }

        /// <summary>
        /// Performs a linear interpolation between two vectors.
        /// </summary>
        /// <param name="start">Start vector.</param>
        /// <param name="end">End vector.</param>
        /// <param name="amount">Value between 0 and 1 indicating the weight of <paramref name="end"/>.</param>
        /// <param name="result">When the method completes, contains the linear interpolation of the two vectors.</param>
        /// <remarks>
        /// This method performs the linear interpolation based on the following formula.
        /// <code>start + (end - start) * amount</code>
        /// Passing <paramref name="amount"/> a value of 0 will cause <paramref name="start"/> to be returned; a value of 1 will cause <paramref name="end"/> to be returned. 
        /// </remarks>
        public static void Lerp(ref Vector3 start, ref Vector3 end, float amount, out Vector3 result)
        {
            result.X = start.X + ((end.X - start.X) * amount);
            result.Y = start.Y + ((end.Y - start.Y) * amount);
            result.Z = start.Z + ((end.Z - start.Z) * amount);
        }

        /// <summary>
        /// Performs a cubic interpolation between two vectors.
        /// </summary>
        /// <param name="start">Start vector.</param>
        /// <param name="end">End vector.</param>
        /// <param name="amount">Value between 0 and 1 indicating the weight of <paramref name="end"/>.</param>
        /// <returns>The cubic interpolation of the two vectors.</returns>
        public static Vector3 SmoothStep(Vector3 start, Vector3 end, float amount)
        {
            Vector3 vector;

            amount = (amount > 1.0f) ? 1.0f : ((amount < 0.0f) ? 0.0f : amount);
            amount = (amount * amount) * (3.0f - (2.0f * amount));

            vector.X = start.X + ((end.X - start.X) * amount);
            vector.Y = start.Y + ((end.Y - start.Y) * amount);
            vector.Z = start.Z + ((end.Z - start.Z) * amount);

            return vector;
        }

        /// <summary>
        /// Performs a cubic interpolation between two vectors.
        /// </summary>
        /// <param name="start">Start vector.</param>
        /// <param name="end">End vector.</param>
        /// <param name="amount">Value between 0 and 1 indicating the weight of <paramref name="end"/>.</param>
        /// <param name="result">When the method completes, contains the cubic interpolation of the two vectors.</param>
        public static void SmoothStep(ref Vector3 start, ref Vector3 end, float amount, out Vector3 result)
        {
            amount = (amount > 1.0f) ? 1.0f : ((amount < 0.0f) ? 0.0f : amount);
            amount = (amount * amount) * (3.0f - (2.0f * amount));

            result.X = start.X + ((end.X - start.X) * amount);
            result.Y = start.Y + ((end.Y - start.Y) * amount);
            result.Z = start.Z + ((end.Z - start.Z) * amount);
        }

        /// <summary>
        /// Calculates the distance between two vectors.
        /// </summary>
        /// <param name="value1">The first vector.</param>
        /// <param name="value2">The second vector.</param>
        /// <returns>The distance between the two vectors.</returns>
        public static float Distance(Vector3 value1, Vector3 value2)
        {
            float x = value1.X - value2.X;
            float y = value1.Y - value2.Y;
            float z = value1.Z - value2.Z;

            return (float)Math.Sqrt(x * x + y * y + z * z);
        }
       
        /// <summary>
        /// Calculates the distance between two vectors.
        /// </summary>
        /// <param name="value1">The first vector.</param>
        /// <param name="value2">The second vector.</param>
        /// <returns>The distance between the two vectors.</returns>
        public static float Distance(ref Vector3 value1, ref Vector3 value2)
        {
            float x = value1.X - value2.X;
            float y = value1.Y - value2.Y;
            float z = value1.Z - value2.Z;

            return (float)Math.Sqrt(x * x + y * y + z * z);
        }

        /// <summary>
        /// Calculates the squared distance between two vectors.
        /// </summary>
        /// <param name="value1">The first vector.</param>
        /// <param name="value2">The second vector.</param>
        /// <returns>The squared distance between the two vectors.</returns>
        /// <remarks>Distance squared is the value before taking the square root. 
        /// Distance squared can often be used in place of distance if relative comparisons are being made. 
        /// For example, consider three points A, B, and C. To determine whether B or C is further from A, 
        /// compare the distance between A and B to the distance between A and C. Calculating the two distances 
        /// involves two square roots, which are computationally expensive. However, using distance squared 
        /// provides the same information and avoids calculating two square roots.
        /// </remarks>
        public static float DistanceSquared(Vector3 value1, Vector3 value2)
        {
            float x = value1.X - value2.X;
            float y = value1.Y - value2.Y;
            float z = value1.Z - value2.Z;

            return (x * x) + (y * y) + (z * z);
        }

        /// <summary>
        /// Calculates the squared distance between two vectors.
        /// </summary>
        /// <param name="value1">The first vector.</param>
        /// <param name="value2">The second vector.</param>
        /// <returns>The squared distance between the two vectors.</returns>
        /// <remarks>Distance squared is the value before taking the square root. 
        /// Distance squared can often be used in place of distance if relative comparisons are being made. 
        /// For example, consider three points A, B, and C. To determine whether B or C is further from A, 
        /// compare the distance between A and B to the distance between A and C. Calculating the two distances 
        /// involves two square roots, which are computationally expensive. However, using distance squared 
        /// provides the same information and avoids calculating two square roots.
        /// </remarks>
        public static float DistanceSquared(ref Vector3 value1, ref Vector3 value2)
        {
            float x = value1.X - value2.X;
            float y = value1.Y - value2.Y;
            float z = value1.Z - value2.Z;

            return (x * x) + (y * y) + (z * z);
        }

        /// <summary>
        /// Calculates the dot product of two vectors.
        /// </summary>
        /// <param name="left">First source vector.</param>
        /// <param name="right">Second source vector.</param>
        /// <returns>The dot product of the two vectors.</returns>
        public static float Dot(Vector3 left, Vector3 right)
        {
            return left.X * right.X + left.Y * right.Y + left.Z * right.Z;
        }

        /// <summary>
        /// Calculates the dot product of two vectors.
        /// </summary>
        /// <param name="left">First source vector.</param>
        /// <param name="right">Second source vector.</param>
        /// <returns>The dot product of the two vectors.</returns>
        public static float Dot(ref Vector3 left, ref Vector3 right)
        {
            return left.X * right.X + left.Y * right.Y + left.Z * right.Z;
        }

        /// <summary>
        /// Calculates the cross product of two vectors.
        /// </summary>
        /// <param name="left">First source vector.</param>
        /// <param name="right">Second source vector.</param>
        /// <returns>The cross product of the two vectors.</returns>
        public static Vector3 Cross(Vector3 left, Vector3 right)
        {
            Vector3 result;
            result.X = left.Y * right.Z - left.Z * right.Y;
            result.Y = left.Z * right.X - left.X * right.Z;
            result.Z = left.X * right.Y - left.Y * right.X;
            return result;
        }

        /// <summary>
        /// Calculates the cross product of two vectors.
        /// </summary>
        /// <param name="left">First source vector.</param>
        /// <param name="right">Second source vector.</param>
        /// <param name="result">The cross product of the two vectors.</param>
        public static void Cross(ref Vector3 left, ref Vector3 right, out Vector3 result)
        {
            Vector3 r;
            r.X = left.Y * right.Z - left.Z * right.Y;
            r.Y = left.Z * right.X - left.X * right.Z;
            r.Z = left.X * right.Y - left.Y * right.X;

            result = r;
        }

        /// <summary>
        /// Returns the reflection of a vector off a surface that has the specified normal. 
        /// </summary>
        /// <param name="vector">The source vector.</param>
        /// <param name="normal">Normal of the surface.</param>
        /// <returns>The reflected vector.</returns>
        /// <remarks>Reflect only gives the direction of a reflection off a surface, it does not determine 
        /// whether the original vector was close enough to the surface to hit it.</remarks>
        public static Vector3 Reflect(Vector3 vector, Vector3 normal)
        {
            Vector3 result;
            float dot = ((vector.X * normal.X) + (vector.Y * normal.Y)) + (vector.Z * normal.Z);

            result.X = vector.X - ((2.0f * dot) * normal.X);
            result.Y = vector.Y - ((2.0f * dot) * normal.Y);
            result.Z = vector.Z - ((2.0f * dot) * normal.Z);

            return result;
        }

        /// <summary>
        /// Returns the reflection of a vector off a surface that has the specified normal. 
        /// </summary>
        /// <param name="vector">The source vector.</param>
        /// <param name="normal">Normal of the surface.</param>
        /// <param name="result">When the method completes, contains the reflected vector.</param>
        /// <remarks>Reflect only gives the direction of a reflection off a surface, it does not determine 
        /// whether the original vector was close enough to the surface to hit it.</remarks>
        public static void Reflect(ref Vector3 vector, ref Vector3 normal, out Vector3 result)
        {
            float dot = ((vector.X * normal.X) + (vector.Y * normal.Y)) + (vector.Z * normal.Z);

            result.X = vector.X - ((2.0f * dot) * normal.X);
            result.Y = vector.Y - ((2.0f * dot) * normal.Y);
            result.Z = vector.Z - ((2.0f * dot) * normal.Z);
        }

        /// <summary>
        /// Converts the vector into a unit vector.
        /// </summary>
        /// <param name="vector">The vector to normalize.</param>
        /// <returns>The normalized vector.</returns>
        public static Vector3 Normalize(Vector3 vector)
        {
            vector.Normalize();
            return vector;
        }

        /// <summary>
        /// Converts the vector into a unit vector.
        /// </summary>
        /// <param name="vector">The vector to normalize.</param>
        /// <param name="result">When the method completes, contains the normalized vector.</param>
        public static void Normalize(ref Vector3 vector, out Vector3 result)
        {
            result = vector;
            result.Normalize();
        }

        /// <summary>
        /// Transforms a 3D vector by the given <see cref="Matrix"/>.
        /// </summary>
        /// <param name="vector">The source vector.</param>
        /// <param name="transformation">The transformation <see cref="Matrix"/>.</param>
        /// <returns>The transformed <see cref="Vector4"/>.</returns>
        public static Vector4 Transform(Vector3 vector, Matrix transformation)
        {
            Vector4 result;

            result.X = vector.X * transformation.M11 + vector.Y * transformation.M21 + vector.Z * transformation.M31 + transformation.M41;
            result.Y = vector.X * transformation.M12 + vector.Y * transformation.M22 + vector.Z * transformation.M32 + transformation.M42;
            result.Z = vector.X * transformation.M13 + vector.Y * transformation.M23 + vector.Z * transformation.M33 + transformation.M43;
            result.W = vector.X * transformation.M14 + vector.Y * transformation.M24 + vector.Z * transformation.M34 + transformation.M44;

            return result;
        }

        /// <summary>
        /// Transforms a 3D vector by the given <see cref="Matrix"/>.
        /// </summary>
        /// <param name="vector">The source vector.</param>
        /// <param name="transformation">The transformation <see cref="Matrix"/>.</param>
        /// <param name="result">When the method completes, contains the transformed <see cref="Vector4"/>.</param>
        public static void Transform(ref Vector3 vector, ref Matrix transformation, out Vector4 result)
        {
            result.X = vector.X * transformation.M11 + vector.Y * transformation.M21 + vector.Z * transformation.M31 + transformation.M41;
            result.Y = vector.X * transformation.M12 + vector.Y * transformation.M22 + vector.Z * transformation.M32 + transformation.M42;
            result.Z = vector.X * transformation.M13 + vector.Y * transformation.M23 + vector.Z * transformation.M33 + transformation.M43;
            result.W = vector.X * transformation.M14 + vector.Y * transformation.M24 + vector.Z * transformation.M34 + transformation.M44;
        }

        /// <summary>
        /// Transforms an array of 3D vectors by the given <see cref="Matrix"/>.
        /// </summary>
        /// <param name="vectorsIn">The source vectors.</param>
        /// <param name="inputStride">The stride in bytes between vectors in the input.</param>
        /// <param name="transformation">The transformation <see cref="Matrix"/>.</param>
        /// <param name="vectorsOut">The transformed <see cref="Vector4"/>s.</param>
        /// <param name="outputStride">The stride in bytes between vectors in the output.</param>
        /// <param name="count">The number of vectors to transform.</param>
        public static void Transform(Vector3* vectorsIn, int inputStride, Matrix* transformation, Vector4* vectorsOut, int outputStride, int count)
        {
            Matrix t = *transformation;
            for (int i = 0; i < count; i++)
            {
                float x = vectorsIn->X;
                float y = vectorsIn->Y;
                float z = vectorsIn->Z;

                vectorsOut->X = x * t.M11 + y * t.M21 + z * t.M31 + t.M41;
                vectorsOut->Y = x * t.M12 + y * t.M22 + z * t.M32 + t.M42;
                vectorsOut->Z = x * t.M13 + y * t.M23 + z * t.M33 + t.M43;
                vectorsOut->W = x * t.M14 + y * t.M24 + z * t.M34 + t.M44;

                vectorsIn = (Vector3*)((byte*)vectorsIn + inputStride);
                vectorsOut = (Vector4*)((byte*)vectorsOut + outputStride);
            }
        }

        /// <summary>
        /// Transforms an array of 3D vectors by the given <see cref="Matrix"/>.
        /// </summary>
        /// <param name="vectorsIn">The source vectors.</param>
        /// <param name="transformation">The transformation <see cref="Matrix"/>.</param>
        /// <param name="vectorsOut">The transformed <see cref="Vector4"/>s.</param>
        /// <param name="count">The number of vectors to transform.</param>
        public static void Transform(Vector3* vectorsIn, Matrix* transformation, Vector4* vectorsOut, int count)
        {
            Transform(vectorsIn, (int)sizeof(Vector3), transformation, vectorsOut, (int)sizeof(Vector4), count);
        }

        /// <summary>
        /// Transforms an array of 3D vectors by the given <see cref="Matrix"/>.
        /// </summary>
        /// <param name="vectorsIn">The source vectors.</param>
        /// <param name="transformation">The transformation <see cref="Matrix"/>.</param>
        /// <param name="vectorsOut">The transformed <see cref="Vector4"/>s.</param>
        /// <param name="offset">The offset at which to begin transforming.</param>
        /// <param name="count">The number of vectors to transform, or 0 to process the whole array.</param>
        public static void Transform(Vector3[] vectorsIn, ref Matrix transformation, Vector4[] vectorsOut, int offset, int count)
        {
            if (vectorsIn == null)
            {
                throw new ArgumentNullException("vectorsIn");
            }

            if (vectorsOut == null)
            {
                throw new ArgumentNullException("destinationArray");
            }

            if (count == 0)
            {
                count = vectorsIn.Length - offset;
            }
            if (vectorsOut.Length < count)
            {
                throw new ArgumentException("NotEnoughTargetSize");
            }

            for (int i = 0; i < count; i++)
            {
                float x = vectorsIn[i + offset].X;
                float y = vectorsIn[i + offset].Y;
                float z = vectorsIn[i + offset].Z;
                vectorsOut[i].X = (((x * transformation.M11) + (y * transformation.M21)) + (z * transformation.M31)) + transformation.M41;
                vectorsOut[i].Y = (((x * transformation.M12) + (y * transformation.M22)) + (z * transformation.M32)) + transformation.M42;
                vectorsOut[i].Z = (((x * transformation.M13) + (y * transformation.M23)) + (z * transformation.M33)) + transformation.M43;
                vectorsOut[i].W = (((x * transformation.M13) + (y * transformation.M23)) + (z * transformation.M33)) + transformation.M43;

            }
        }

        /// <summary>
        /// Transforms an array of 3D vectors by the given <see cref="Matrix"/>.
        /// </summary>
        /// <param name="vectorsIn">The source vectors.</param>
        /// <param name="transformation">The transformation <see cref="Matrix"/>.</param>
        /// <param name="vectorsOut">The transformed <see cref="Vector4"/>s.</param>
        public static void Transform(Vector3[] vectorsIn, ref Matrix transformation, Vector4[] vectorsOut)
        {
            Transform(vectorsIn, ref transformation, vectorsOut, 0, 0);
        }

        /// <summary>
        /// Transforms an array of 3D vectors by the given <see cref="Matrix"/>.
        /// </summary>
        /// <param name="vectors">The source vectors.</param>
        /// <param name="transformation">The transformation <see cref="Matrix"/>.</param>
        /// <returns>The transformed <see cref="Vector4"/>s.</returns>
        public static Vector4[] Transform(Vector3[] vectors, ref Matrix transformation)
        {
            if (vectors == null)
            {
                throw new ArgumentNullException("vectors");
            }

            Vector4[] result = new Vector4[vectors.Length];

            for (int i = 0; i < vectors.Length; i++)
            {
                float x = vectors[i].X;
                float y = vectors[i].Y;
                float z = vectors[i].Z;
                result[i].X = (((x * transformation.M11) + (y * transformation.M21)) + (z * transformation.M31)) + transformation.M41;
                result[i].Y = (((x * transformation.M12) + (y * transformation.M22)) + (z * transformation.M32)) + transformation.M42;
                result[i].Z = (((x * transformation.M13) + (y * transformation.M23)) + (z * transformation.M33)) + transformation.M43;
                result[i].W = (((x * transformation.M13) + (y * transformation.M23)) + (z * transformation.M33)) + transformation.M43;
            }
            return result;

        }

        /// <summary>
        /// Transforms a 3D vector by the given <see cref="Quaternion"/> rotation.
        /// </summary>
        /// <param name="vector">The vector to rotate.</param>
        /// <param name="rotation">The <see cref="Quaternion"/> rotation to apply.</param>
        /// <returns>The transformed <see cref="Vector4"/>.</returns>
        public static Vector4 Transform(Vector3 vector, Quaternion rotation)
        {
            Vector4 result;
            float x = rotation.X + rotation.X;
            float y = rotation.Y + rotation.Y;
            float z = rotation.Z + rotation.Z;
            float wx = rotation.W * x;
            float wy = rotation.W * y;
            float wz = rotation.W * z;
            float xx = rotation.X * x;
            float xy = rotation.X * y;
            float xz = rotation.X * z;
            float yy = rotation.Y * y;
            float yz = rotation.Y * z;
            float zz = rotation.Z * z;

            result.X = ((vector.X * ((1.0f - yy) - zz)) + (vector.Y * (xy - wz))) + (vector.Z * (xz + wy));
            result.Y = ((vector.X * (xy + wz)) + (vector.Y * ((1.0f - xx) - zz))) + (vector.Z * (yz - wx));
            result.Z = ((vector.X * (xz - wy)) + (vector.Y * (yz + wx))) + (vector.Z * ((1.0f - xx) - yy));
            result.W = 1.0f;

            return result;
        }

        /// <summary>
        /// Transforms a 3D vector by the given <see cref="Quaternion"/> rotation.
        /// </summary>
        /// <param name="vector">The vector to rotate.</param>
        /// <param name="rotation">The <see cref="Quaternion"/> rotation to apply.</param>
        /// <param name="result">When the method completes, contains the transformed <see cref="Vector4"/>.</param>
        public static void Transform(ref Vector3 vector, ref Quaternion rotation, out Vector4 result)
        {
            float x = rotation.X + rotation.X;
            float y = rotation.Y + rotation.Y;
            float z = rotation.Z + rotation.Z;
            float wx = rotation.W * x;
            float wy = rotation.W * y;
            float wz = rotation.W * z;
            float xx = rotation.X * x;
            float xy = rotation.X * y;
            float xz = rotation.X * z;
            float yy = rotation.Y * y;
            float yz = rotation.Y * z;
            float zz = rotation.Z * z;

            result.X = ((vector.X * ((1.0f - yy) - zz)) + (vector.Y * (xy - wz))) + (vector.Z * (xz + wy));
            result.Y = ((vector.X * (xy + wz)) + (vector.Y * ((1.0f - xx) - zz))) + (vector.Z * (yz - wx));
            result.Z = ((vector.X * (xz - wy)) + (vector.Y * (yz + wx))) + (vector.Z * ((1.0f - xx) - yy));
            result.W = 1.0f;
        }

        /// <summary>
        /// Transforms an array of 3D vectors by the given <see cref="Quaternion"/> rotation.
        /// </summary>
        /// <param name="vectors">The vectors to rotate.</param>
        /// <param name="rotation">The <see cref="Quaternion"/> rotation to apply.</param>
        /// <returns>The transformed <see cref="Vector4"/>.</returns>
        public static Vector4[] Transform(Vector3[] vectors, ref Quaternion rotation)
        {
            if (vectors == null)
                throw new ArgumentNullException("vectors");

            int count = vectors.Length;
            Vector4[] results = new Vector4[count];

            float x = rotation.X + rotation.X;
            float y = rotation.Y + rotation.Y;
            float z = rotation.Z + rotation.Z;
            float wx = rotation.W * x;
            float wy = rotation.W * y;
            float wz = rotation.W * z;
            float xx = rotation.X * x;
            float xy = rotation.X * y;
            float xz = rotation.X * z;
            float yy = rotation.Y * y;
            float yz = rotation.Y * z;
            float zz = rotation.Z * z;

            for (int i = 0; i < count; i++)
            {
                Vector4 r;
                r.X = ((vectors[i].X * ((1.0f - yy) - zz)) + (vectors[i].Y * (xy - wz))) + (vectors[i].Z * (xz + wy));
                r.Y = ((vectors[i].X * (xy + wz)) + (vectors[i].Y * ((1.0f - xx) - zz))) + (vectors[i].Z * (yz - wx));
                r.Z = ((vectors[i].X * (xz - wy)) + (vectors[i].Y * (yz + wx))) + (vectors[i].Z * ((1.0f - xx) - yy));
                r.W = 1.0f;

                results[i] = r;
            }

            return results;
        }

        /// <summary>
        /// Performs a coordinate transformation using the given <see cref="Matrix"/>.
        /// </summary>
        /// <param name="coordinate">The coordinate vector to transform.</param>
        /// <param name="transformation">The transformation <see cref="Matrix"/>.</param>
        /// <returns>The transformed coordinates.</returns>
        public static Vector3 TransformCoordinate(Vector3 coordinate, Matrix transformation)
        {
            Vector4 vector;

            vector.X = coordinate.X * transformation.M11 + coordinate.Y * transformation.M21 + coordinate.Z * transformation.M31 + transformation.M41;
            vector.Y = coordinate.X * transformation.M12 + coordinate.Y * transformation.M22 + coordinate.Z * transformation.M32 + transformation.M42;
            vector.Z = coordinate.X * transformation.M13 + coordinate.Y * transformation.M23 + coordinate.Z * transformation.M33 + transformation.M43;
            vector.W = 1 / (coordinate.X * transformation.M14 + coordinate.Y * transformation.M24 + coordinate.Z * transformation.M34 + transformation.M44);

            return new Vector3(vector.X * vector.W, vector.Y * vector.W, vector.Z * vector.W);
        }

        /// <summary>
        /// Performs a coordinate transformation using the given <see cref="Matrix"/>.
        /// </summary>
        /// <param name="coordinate">The coordinate vector to transform.</param>
        /// <param name="transformation">The transformation <see cref="Matrix"/>.</param>
        /// <param name="result">When the method completes, contains the transformed coordinates.</param>
        public static void TransformCoordinate(ref Vector3 coordinate, ref Matrix transformation, out Vector3 result)
        {
            Vector4 vector;

            vector.X = coordinate.X * transformation.M11 + coordinate.Y * transformation.M21 + coordinate.Z * transformation.M31 + transformation.M41;
            vector.Y = coordinate.X * transformation.M12 + coordinate.Y * transformation.M22 + coordinate.Z * transformation.M32 + transformation.M42;
            vector.Z = coordinate.X * transformation.M13 + coordinate.Y * transformation.M23 + coordinate.Z * transformation.M33 + transformation.M43;
            vector.W = 1 / (coordinate.X * transformation.M14 + coordinate.Y * transformation.M24 + coordinate.Z * transformation.M34 + transformation.M44);

            result = new Vector3(vector.X * vector.W, vector.Y * vector.W, vector.Z * vector.W);
        }

        /// <summary>
        /// Performs a coordinate transformation using the given <see cref="Matrix"/>.
        /// </summary>
        /// <param name="coordsIn">The source coordinate vectors.</param>
        /// <param name="inputStride">The stride in bytes between vectors in the input.</param>
        /// <param name="transformation">The transformation <see cref="Matrix"/>.</param>
        /// <param name="coordsOut">The transformed coordinate <see cref="Vector3"/>s.</param>
        /// <param name="outputStride">The stride in bytes between vectors in the output.</param>
        /// <param name="count">The number of coordinate vectors to transform.</param>
        public static void TransformCoordinate(Vector3* coordsIn, int inputStride, Matrix* transformation, Vector3* coordsOut, int outputStride, int count)
        {
            Matrix t = *transformation;
            byte* input = (byte*)coordsIn;
            byte* output = (byte*)coordsOut;

            for (int i = 0; i < count; i++)
            {
                Vector3* coordIn = (Vector3*)input;

                float x = coordIn->X * t.M11 + coordIn->Y * t.M21 + coordIn->Z * t.M31 + t.M41;
                float y = coordIn->X * t.M12 + coordIn->Y * t.M22 + coordIn->Z * t.M32 + t.M42;
                float z = coordIn->X * t.M13 + coordIn->Y * t.M23 + coordIn->Z * t.M33 + t.M43;
                float w = 1 / (coordIn->X * t.M14 + coordIn->Y * t.M24 + coordIn->Z * t.M34 + t.M44);

                *(Vector3*)output = new Vector3(x * w, y * w, z * w);

                input += inputStride;
                output += outputStride;
            }
        }

        /// <summary>
        /// Performs a coordinate transformation using the given <see cref="Matrix"/>.
        /// </summary>
        /// <param name="coordsIn">The source coordinate vectors.</param>
        /// <param name="transformation">The transformation <see cref="Matrix"/>.</param>
        /// <param name="coordsOut">The transformed coordinate <see cref="Vector3"/>s.</param>
        /// <param name="count">The number of coordinate vectors to transform.</param>
        public static void TransformCoordinate(Vector3* coordsIn, Matrix* transformation, Vector3* coordsOut, int count)
        {
            TransformCoordinate(coordsIn, (int)sizeof(Vector3), transformation, coordsOut, (int)sizeof(Vector3), count);
        }

        /// <summary>
        /// Performs a coordinate transformation using the given <see cref="Matrix"/>.
        /// </summary>
        /// <param name="coordsIn">The source coordinate vectors.</param>
        /// <param name="transformation">The transformation <see cref="Matrix"/>.</param>
        /// <param name="coordsOut">The transformed coordinate <see cref="Vector3"/>s.</param>
        /// <param name="offset">The offset at which to begin transforming.</param>
        /// <param name="count">The number of coordinate vectors to transform, or 0 to process the whole array.</param>
        public static void TransformCoordinate(Vector3[] coordsIn, ref Matrix transformation, Vector3[] coordsOut, int offset, int count)
        {
            fixed (Vector3* lp = &coordsIn[offset], resp = &coordsOut[offset])
            {
                TransformCoordinate(lp, Vector3.SizeInBytes, &transformation, resp, Vector3.SizeInBytes, count);
            }
        }

        /// <summary>
        /// Performs a coordinate transformation using the given <see cref="Matrix"/>.
        /// </summary>
        /// <param name="coordsIn">The source coordinate vectors.</param>
        /// <param name="transformation">The transformation <see cref="Matrix"/>.</param>
        /// <param name="coordsOut">The transformed coordinate <see cref="Vector3"/>s.</param>
        public static void TransformCoordinate(Vector3[] coordsIn, ref Matrix transformation, Vector3[] coordsOut)
        {
            TransformCoordinate(coordsIn, ref transformation, coordsOut, 0, 0);
        }

        /// <summary>
        /// Performs a coordinate transformation using the given <see cref="Matrix"/>.
        /// </summary>
        /// <param name="coordinates">The coordinate vectors to transform.</param>
        /// <param name="transformation">The transformation <see cref="Matrix"/>.</param>
        /// <returns>The transformed coordinates.</returns>
        public static Vector3[] TransformCoordinate(Vector3[] coordinates, ref Matrix transformation)
        {
            Vector3[] result = new Vector3[coordinates.Length];

            TransformCoordinate(coordinates, ref transformation, result);

            return result;
        }


        /// <summary>
        /// Performs a normal transformation using the given <see cref="Matrix"/>.
        /// </summary>
        /// <param name="normal">The normal vector to transform.</param>
        /// <param name="transformation">The transformation <see cref="Matrix"/>.</param>
        /// <returns>The transformed normal.</returns>
        public static Vector3 TransformNormal(Vector3 normal, Matrix transformation)
        {
            Vector3 vector;

            vector.X = normal.X * transformation.M11 + normal.Y * transformation.M21 + normal.Z * transformation.M31;
            vector.Y = normal.X * transformation.M12 + normal.Y * transformation.M22 + normal.Z * transformation.M32;
            vector.Z = normal.X * transformation.M13 + normal.Y * transformation.M23 + normal.Z * transformation.M33;

            return vector;
        }

        /// <summary>
        /// Performs a normal transformation using the given <see cref="Matrix"/>.
        /// </summary>
        /// <param name="normal">The normal vector to transform.</param>
        /// <param name="transformation">The transformation <see cref="Matrix"/>.</param>
        /// <param name="result">When the method completes, contains the transformed normal.</param>
        public static void TransformNormal(ref Vector3 normal, ref Matrix transformation, out Vector3 result)
        {
            result.X = normal.X * transformation.M11 + normal.Y * transformation.M21 + normal.Z * transformation.M31;
            result.Y = normal.X * transformation.M12 + normal.Y * transformation.M22 + normal.Z * transformation.M32;
            result.Z = normal.X * transformation.M13 + normal.Y * transformation.M23 + normal.Z * transformation.M33;
        }

        /// <summary>
        /// Performs a normal transformation using the given <see cref="Matrix"/>.
        /// </summary>
        /// <param name="normalsIn">The source normals to transform.</param>
        /// <param name="inputStride">The stride in bytes between normals in the input.</param>
        /// <param name="transformation">The transformation <see cref="Matrix"/>.</param>
        /// <param name="normalsOut">The transformed <see cref="Vector3"/>s.</param>
        /// <param name="outputStride">The stride in bytes between vectors in the output.</param>
        /// <param name="count">The number of vectors to transform.</param>
        public static void TransformNormal(Vector3* normalsIn, int inputStride, Matrix* transformation, Vector3* normalsOut, 
            int outputStride, int count) 
        {
            Matrix t = *transformation;
            byte* input = (byte*)normalsIn;
            byte* output = (byte*)normalsOut;

            for (int i = 0; i < count; i++)
            {
                Vector3* normalIn = (Vector3*)input;
                Vector3* normalOut = (Vector3*)output;

                normalOut->X = normalIn->X * t.M11 + normalIn->Y * t.M21 + normalIn->Z * t.M31;
                normalOut->Y = normalIn->X * t.M12 + normalIn->Y * t.M22 + normalIn->Z * t.M32;
                normalOut->Z = normalIn->X * t.M13 + normalIn->Y * t.M23 + normalIn->Z * t.M33;


                input += inputStride;
                output += outputStride;
            }
        }

        /// <summary>
        /// Performs a normal transformation using the given <see cref="Matrix"/>.
        /// </summary>
        /// <param name="normalsIn">The source normals to transform.</param>
        /// <param name="transformation">The transformation <see cref="Matrix"/>.</param>
        /// <param name="normalsOut">The transformed <see cref="Vector3"/>s.</param>
        /// <param name="count">The number of vectors to transform.</param>
        public static void TransformNormal(Vector3* normalsIn, Matrix* transformation, Vector3* normalsOut, int count)
        {
            TransformNormal(normalsIn, (int)sizeof(Vector3), transformation, normalsOut, (int)sizeof(Vector3), count);
        }

        /// <summary>
        /// Performs a normal transformation using the given <see cref="Matrix"/>.
        /// </summary>
        /// <param name="normalsIn">The source vectors.</param>
        /// <param name="transformation">The transformation <see cref="Matrix"/>.</param>
        /// <param name="normalsOut">The transformed <see cref="Vector3"/>s.</param>
        /// <param name="offset">The offset at which to begin transforming.</param>
        /// <param name="count">The number of vectors to transform, or 0 to process the whole array.</param>
        public static void TransformNormal(Vector3[] normalsIn, ref Matrix transformation, Vector3[] normalsOut, int offset, int count)
        {
            fixed (Vector3* lp = &normalsIn[offset], resp = &normalsOut[offset])
            {
                TransformNormal(lp, Vector3.SizeInBytes, &transformation, resp, Vector3.SizeInBytes, count);
            }
        }

        /// <summary>
        /// Performs a normal transformation using the given <see cref="Matrix"/>.
        /// </summary>
        /// <param name="normalsIn">The source vectors.</param>
        /// <param name="transformation">The transformation <see cref="Matrix"/>.</param>
        /// <param name="normalsOut">The transformed <see cref="Vector3"/>s.</param>
        public static void TransformNormal(Vector3[] normalsIn, ref Matrix transformation, Vector3[] normalsOut)
        {
            TransformNormal(normalsIn, ref transformation, normalsOut, 0, 0);
        }

        /// <summary>
        /// Performs a normal transformation using the given <see cref="Matrix"/>.
        /// </summary>
        /// <param name="normals">The normal vectors to transform.</param>
        /// <param name="transformation">The transformation <see cref="Matrix"/>.</param>
        /// <returns>The transformed normals.</returns>
        public static Vector3[] TransformNormal(Vector3[] normals, ref Matrix transformation)
        {
            Vector3[] result = new Vector3[normals.Length];

            TransformNormal(normals, ref transformation, result);

            return result;
        }

        /// <summary>
        /// Projects a 3D vector from object space into screen space. 
        /// </summary>
        /// <param name="vector">The vector to project.</param>
        /// <param name="viewport">The viewport representing screen space.</param>
        /// <param name="projection">The projection matrix.</param>
        /// <param name="view">The view matrix.</param>
        /// <param name="world">The world matrix.</param>
        /// <returns>The vector in screen space.</returns>
        public static Vector3 Project(Vector3 vector, Viewport viewport, Matrix projection, Matrix view, Matrix world) 
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Projects a 3D vector from object space into screen space. 
        /// </summary>
        /// <param name="vector">The vector to project.</param>
        /// <param name="viewport">The viewport representing screen space.</param>
        /// <param name="projection">The projection matrix.</param>
        /// <param name="view">The view matrix.</param>
        /// <param name="world">The world matrix.</param>
        /// <param name="result">When the method completes, contains the vector in screen space.</param>
        public static void Project(ref Vector3 vector, ref Viewport viewport, ref Matrix projection, ref Matrix view, ref Matrix world, out Vector3 result)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Projects a 3D vector from screen space into object space. 
        /// </summary>
        /// <param name="vector">The vector to project.</param>
        /// <param name="viewport">The viewport representing screen space.</param>
        /// <param name="projection">The projection matrix.</param>
        /// <param name="view">The view matrix.</param>
        /// <param name="world">The world matrix.</param>
        /// <returns>The vector in object space.</returns>
        public static Vector3 Unproject(Vector3 vector, Viewport viewport, Matrix projection, Matrix view, Matrix world)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Projects a 3D vector from screen space into object space. 
        /// </summary>
        /// <param name="vector">The vector to project.</param>
        /// <param name="viewport">The viewport representing screen space.</param>
        /// <param name="projection">The projection matrix.</param>
        /// <param name="view">The view matrix.</param>
        /// <param name="world">The world matrix.</param>
        /// <param name="result">When the method completes, contains the vector in object space.</param>
        public static void Unproject(ref Vector3 vector, ref Viewport viewport, ref Matrix projection, ref Matrix view, ref Matrix world, out Vector3 result)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Returns a vector containing the smallest components of the specified vectors.
        /// </summary>
        /// <param name="left">The first source vector.</param>
        /// <param name="right">The second source vector.</param>
        /// <returns>A vector containing the smallest components of the source vectors.</returns>
        public static Vector3 Minimize(Vector3 left, Vector3 right)
        {
            Vector3 vector;
            vector.X = (left.X < right.X) ? left.X : right.X;
            vector.Y = (left.Y < right.Y) ? left.Y : right.Y;
            vector.Z = (left.Z < right.Z) ? left.Z : right.Z;
            return vector;
        }

        /// <summary>
        /// Returns a vector containing the smallest components of the specified vectors.
        /// </summary>
        /// <param name="left">The first source vector.</param>
        /// <param name="right">The second source vector.</param>
        /// <param name="result">When the method completes, contains an new vector composed of the smallest components of the source vectors.</param>
        public static void Minimize(ref Vector3 left, ref Vector3 right, out Vector3 result)
        {
            result.X = (left.X < right.X) ? left.X : right.X;
            result.Y = (left.Y < right.Y) ? left.Y : right.Y;
            result.Z = (left.Z < right.Z) ? left.Z : right.Z;
        }

        /// <summary>
        /// Returns a vector containing the largest components of the specified vectors.
        /// </summary>
        /// <param name="left">The first source vector.</param>
        /// <param name="right">The second source vector.</param>
        /// <returns>A vector containing the largest components of the source vectors.</returns>
        public static Vector3 Maximize(Vector3 left, Vector3 right)
        {
            Vector3 vector;
            vector.X = (left.X > right.X) ? left.X : right.X;
            vector.Y = (left.Y > right.Y) ? left.Y : right.Y;
            vector.Z = (left.Z > right.Z) ? left.Z : right.Z;
            return vector;
        }

        /// <summary>
        /// Returns a vector containing the smallest components of the specified vectors.
        /// </summary>
        /// <param name="left">The first source vector.</param>
        /// <param name="right">The second source vector.</param>
        /// <param name="result">When the method completes, contains an new vector composed of the largest components of the source vectors.</param>
        public static void Maximize(ref Vector3 left, ref Vector3 right, out Vector3 result)
        {
            result.X = (left.X > right.X) ? left.X : right.X;
            result.Y = (left.Y > right.Y) ? left.Y : right.Y;
            result.Z = (left.Z > right.Z) ? left.Z : right.Z;
        }

        /// <summary>
        /// Adds two vectors.
        /// </summary>
        /// <param name="left">The first vector to add.</param>
        /// <param name="right">The second vector to add.</param>
        /// <returns>The sum of the two vectors.</returns>
        public static Vector3 operator +(Vector3 left, Vector3 right)
        {
            return new Vector3(left.X + right.X, left.Y + right.Y, left.Z + right.Z);
        }

        /// <summary>
        /// Subtracts two vectors.
        /// </summary>
        /// <param name="left">The first vector to subtract.</param>
        /// <param name="right">The second vector to subtract.</param>
        /// <returns>The difference of the two vectors.</returns>
        public static Vector3 operator -(Vector3 left, Vector3 right)
        {
            return new Vector3(left.X - right.X, left.Y - right.Y, left.Z - right.Z);
        }

        /// <summary>
        /// Reverses the direction of a given vector.
        /// </summary>
        /// <param name="value">The vector to negate.</param>
        /// <returns>A vector facing in the opposite direction.</returns>
        public static Vector3 operator -(Vector3 value)
        {
            return new Vector3(-value.X, -value.Y, -value.Z);
        }

        /// <summary>
        /// Scales a vector by the given value.
        /// </summary>
        /// <param name="vector">The vector to scale.</param>
        /// <param name="scale">The amount by which to scale the vector.</param>
        /// <returns>The scaled vector.</returns>
        public static Vector3 operator *(Vector3 vector, float scale)
        {
            return new Vector3(vector.X * scale, vector.Y * scale, vector.Z * scale);
        }

        /// <summary>
        /// Scales a vector by the given value.
        /// </summary>
        /// <param name="vector">The vector to scale.</param>
        /// <param name="scale">The amount by which to scale the vector.</param>
        /// <returns>The scaled vector.</returns>
        public static Vector3 operator *(float scale, Vector3 vector)
        {
            return vector * scale;
        }

        /// <summary>
        /// Scales a vector by the given value.
        /// </summary>
        /// <param name="vector">The vector to scale.</param>
        /// <param name="scale">The amount by which to scale the vector.</param>
        /// <returns>The scaled vector.</returns>
        public static Vector3 operator /(Vector3 vector, float scale)
        {
            return new Vector3(vector.X / scale, vector.Y / scale, vector.Z / scale);
        }

        /// <summary>
        /// Tests for equality between two objects.
        /// </summary>
        /// <param name="left">The first value to compare.</param>
        /// <param name="right">The second value to compare.</param>
        /// <returns><c>true</c> if <paramref name="left"/> has the same value as <paramref name="right"/>; otherwise, <c>false</c>.</returns>
        public static bool operator ==(Vector3 left, Vector3 right)
        {
            return Vector3.Equals(ref left, ref right);
        }

        /// <summary>
        /// Tests for inequality between two objects.
        /// </summary>
        /// <param name="left">The first value to compare.</param>
        /// <param name="right">The second value to compare.</param>
        /// <returns><c>true</c> if <paramref name="left"/> has a different value than <paramref name="right"/>; otherwise, <c>false</c>.</returns>
        public static bool operator !=(Vector3 left, Vector3 right)
        {
            return !Vector3.Equals(ref left, ref right);
        }

        /// <summary>
        /// Converts the value of the object to its equivalent string representation.
        /// </summary>
        /// <returns>The string representation of the value of this instance.</returns>
        public override string ToString()
        {
            return string.Format(CultureInfo.CurrentCulture, "X:{0} Y:{1} Z:{2}", X.ToString(CultureInfo.CurrentCulture), Y.ToString(CultureInfo.CurrentCulture), Z.ToString(CultureInfo.CurrentCulture));
        }

        /// <summary>
        /// Returns the hash code for this instance.
        /// </summary>
        /// <returns>A 32-bit signed integer hash code.</returns>
        public override int GetHashCode()
        {
            return X.GetHashCode() + Y.GetHashCode() + Z.GetHashCode();
        }

        /// <summary>
        /// Returns a value that indicates whether the current instance is equal to a specified object. 
        /// </summary>
        /// <param name="obj">Object to make the comparison with.</param>
        /// <returns><c>true</c> if the current instance is equal to the specified object; <c>false</c> otherwise.</returns>
        public override bool Equals(object obj)
        {
            if (obj == null)
                return false;

            if (obj.GetType() != GetType())
                return false;

            return Equals((Vector3)obj);
        }

        /// <summary>
        /// Returns a value that indicates whether the current instance is equal to the specified object. 
        /// </summary>
        /// <param name="other">Object to make the comparison with.</param>
        /// <returns><c>true</c> if the current instance is equal to the specified object; <c>false</c> otherwise.</returns>
        public bool Equals(Vector3 other)
        {
            return (X == other.X && Y == other.Y && Z == other.Z);
        }

        /// <summary>
        /// Determines whether the specified object instances are considered equal. 
        /// </summary>
        /// <param name="value1">The first value to compare.</param>
        /// <param name="value2">The second value to compare.</param>
        /// <returns><c>true</c> if <paramref name="value1"/> is the same instance as <paramref name="value2"/> or 
        /// if both are <c>null</c> references or if <c>value1.Equals(value2)</c> returns <c>true</c>; otherwise, <c>false</c>.</returns>
        public static bool Equals(ref Vector3 value1, ref Vector3 value2)
        {
            return (value1.X == value2.X && value1.Y == value2.Y && value1.Z == value2.Z);
        }
    }
}
