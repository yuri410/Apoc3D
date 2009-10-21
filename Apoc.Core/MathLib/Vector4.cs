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
    /// Defines a four component vector.
    /// </summary>
    [Serializable, StructLayout(LayoutKind.Sequential), TypeConverter(typeof(Vector4Converter))]
    public unsafe struct Vector4 : IEquatable<Vector4>
    {
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

        /// <summary>
        /// Gets or sets the W component of the vector.
        /// </summary>
        /// <value>The W component of the vector.</value>
        public float W;



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

                    case 3:
                        return W;

                    default:
                        throw new IndexOutOfRangeException("Indices for Vector4 run from 0 to 3, inclusive.");
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

                    case 3:
                        W = value;
                        break;

                    default:
                        throw new IndexOutOfRangeException("Indices for Vector4 run from 0 to 3, inclusive.");
                }
            }
        }

        /// <summary>
        /// Gets a <see cref="Vector4"/> with all of its components set to zero.
        /// </summary>
        /// <value>A <see cref="Vector4"/> that has all of its components set to zero.</value>
        public static Vector4 Zero
        {
            get { return new Vector4(0, 0, 0, 0); }
        }

        /// <summary>
        /// Gets the X unit <see cref="Vector4"/> (1, 0, 0, 0).
        /// </summary>
        /// <value>A <see cref="Vector4"/> that has a value of (1, 0, 0, 0).</value>
        public static Vector4 UnitX
        {
            get { return new Vector4(1, 0, 0, 0); }
        }

        /// <summary>
        /// Gets the Y unit <see cref="Vector4"/> (0, 1, 0, 0).
        /// </summary>
        /// <value>A <see cref="Vector4"/> that has a value of (0, 1, 0, 0).</value>
        public static Vector4 UnitY
        {
            get { return new Vector4(0, 1, 0, 0); }
        }

        /// <summary>
        /// Gets the Z unit <see cref="Vector4"/> (0, 0, 1, 0).
        /// </summary>
        /// <value>A <see cref="Vector4"/> that has a value of (0, 0, 1, 0).</value>
        public static Vector4 UnitZ
        {
            get { return new Vector4(0, 0, 1, 0); }
        }

        /// <summary>
        /// Gets the W unit <see cref="Vector4"/> (0, 0, 0, 1).
        /// </summary>
        /// <value>A <see cref="Vector4"/> that has a value of (0, 0, 0, 1).</value>
        public static Vector4 UnitW
        {
            get { return new Vector4(0, 0, 0, 1); }
        }

        /// <summary>
        /// Gets the size of the <see cref="Vector4"/> type, in bytes.
        /// </summary>
        public static int SizeInBytes
        {
            get { return sizeof(Vector4); }
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="Vector4"/> class.
        /// </summary>
        /// <param name="value">The value that will be assigned to all components.</param>
        public Vector4(float value)
        {
            X = value;
            Y = value;
            Z = value;
            W = value;
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="Vector4"/> class.
        /// </summary>
        /// <param name="value">A vector containing the values with which to initialize the X and Y components</param>
        /// <param name="z">Initial value for the Z component of the vector.</param>
        /// <param name="w">Initial value for the W component of the vector.</param>
        public Vector4(Vector2 value, float z, float w)
        {
            X = value.X;
            Y = value.Y;
            Z = z;
            W = w;
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="Vector4"/> class.
        /// </summary>
        /// <param name="value">A vector containing the values with which to initialize the X, Y, and Z components</param>
        /// <param name="w">Initial value for the W component of the vector.</param>
        public Vector4(Vector3 value, float w)
        {
            X = value.X;
            Y = value.Y;
            Z = value.Z;
            W = w;
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="Vector4"/> class.
        /// </summary>
        /// <param name="x">Initial value for the X component of the vector.</param>
        /// <param name="y">Initial value for the Y component of the vector.</param>
        /// <param name="z">Initial value for the Z component of the vector.</param>
        /// <param name="w">Initial value for the W component of the vector.</param>
        public Vector4(float x, float y, float z, float w)
        {
            X = x;
            Y = y;
            Z = z;
            W = w;
        }

        /// <summary>
        /// Calculates the length of the vector.
        /// </summary>
        /// <returns>The length of the vector.</returns>
        public float Length()
        {
            return (float) Math.Sqrt(X * X + Y * Y + Z * Z + W * W);
        }

        /// <summary>
        /// Calculates the squared length of the vector.
        /// </summary>
        /// <returns>The squared length of the vector.</returns>
        public float LengthSquared()
        {
            return X * X + Y * Y + Z * Z + W * W;
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
            W *= num;
        }

        /// <summary>
        /// Adds two vectors.
        /// </summary>
        /// <param name="left">The first vector to add.</param>
        /// <param name="right">The second vector to add.</param>
        /// <returns>The sum of the two vectors.</returns>
        public static Vector4 Add(Vector4 left, Vector4 right)
        {
            return new Vector4(left.X + right.X, left.Y + right.Y, left.Z + right.Z, left.W + right.W);
        }

        /// <summary>
        /// Adds two vectors.
        /// </summary>
        /// <param name="left">The first vector to add.</param>
        /// <param name="right">The second vector to add.</param>
        /// <param name="result">When the method completes, contains the sum of the two vectors.</param>
        public static void Add(ref Vector4 left, ref Vector4 right, out Vector4 result)
        {
            result = new Vector4(left.X + right.X, left.Y + right.Y, left.Z + right.Z, left.W + right.W);
        }

        /// <summary>
        /// Subtracts two vectors.
        /// </summary>
        /// <param name="left">The first vector to subtract.</param>
        /// <param name="right">The second vector to subtract.</param>
        /// <returns>The difference of the two vectors.</returns>
        public static Vector4 Subtract(Vector4 left, Vector4 right)
        {
            return new Vector4(left.X - right.X, left.Y - right.Y, left.Z - right.Z, left.W - right.W);
        }

        /// <summary>
        /// Subtracts two vectors.
        /// </summary>
        /// <param name="left">The first vector to subtract.</param>
        /// <param name="right">The second vector to subtract.</param>
        /// <param name="result">When the method completes, contains the difference of the two vectors.</param>
        public static void Subtract(ref Vector4 left, ref Vector4 right, out Vector4 result)
        {
            result = new Vector4(left.X - right.X, left.Y - right.Y, left.Z - right.Z, left.W - right.W);
        }

        /// <summary>
        /// Scales a vector by the given value.
        /// </summary>
        /// <param name="value">The vector to scale.</param>
        /// <param name="scale">The amount by which to scale the vector.</param>
        /// <returns>The scaled vector.</returns>
        public static Vector4 Multiply(Vector4 value, float scale)
        {
            return new Vector4(value.X * scale, value.Y * scale, value.Z * scale, value.W * scale);
        }

        /// <summary>
        /// Scales a vector by the given value.
        /// </summary>
        /// <param name="vector">The vector to scale.</param>
        /// <param name="scale">The amount by which to scale the vector.</param>
        /// <param name="result">When the method completes, contains the scaled vector.</param>
        public static void Multiply(ref Vector4 vector, float scale, out Vector4 result)
        {
            result = new Vector4(vector.X * scale, vector.Y * scale, vector.Z * scale, vector.W * scale);
        }

        /// <summary>
        /// Modulates a vector by another.
        /// </summary>
        /// <param name="left">The first vector to modulate.</param>
        /// <param name="right">The second vector to modulate.</param>
        /// <returns>The modulated vector.</returns>
        public static Vector4 Modulate(Vector4 left, Vector4 right)
        {
            return new Vector4(left.X * right.X, left.Y * right.Y, left.Z * right.Z, left.W * right.W);
        }

        /// <summary>
        /// Modulates a vector by another.
        /// </summary>
        /// <param name="left">The first vector to modulate.</param>
        /// <param name="right">The second vector to modulate.</param>
        /// <param name="result">When the moethod completes, contains the modulated vector.</param>
        public static void Modulate(ref Vector4 left, ref Vector4 right, out Vector4 result)
        {
            result = new Vector4(left.X * right.X, left.Y * right.Y, left.Z * right.Z, left.W * right.W);
        }

        /// <summary>
        /// Scales a vector by the given value.
        /// </summary>
        /// <param name="value">The vector to scale.</param>
        /// <param name="scale">The amount by which to scale the vector.</param>
        /// <returns>The scaled vector.</returns>
        public static Vector4 Divide(Vector4 value, float scale)
        {
            return new Vector4(value.X / scale, value.Y / scale, value.Z / scale, value.W / scale);
        }

        /// <summary>
        /// Scales a vector by the given value.
        /// </summary>
        /// <param name="vector">The vector to scale.</param>
        /// <param name="scale">The amount by which to scale the vector.</param>
        /// <param name="result">When the method completes, contains the scaled vector.</param>
        public static void Divide(ref Vector4 vector, float scale, out Vector4 result)
        {
            result = new Vector4(vector.X / scale, vector.Y / scale, vector.Z / scale, vector.W / scale);
        }

        /// <summary>
        /// Reverses the direction of a given vector.
        /// </summary>
        /// <param name="value">The vector to negate.</param>
        /// <returns>A vector facing in the opposite direction.</returns>
        public static Vector4 Negate(Vector4 value)
        {
            return new Vector4(-value.X, -value.Y, -value.Z, -value.W);
        }

        /// <summary>
        /// Reverses the direction of a given vector.
        /// </summary>
        /// <param name="value">The vector to negate.</param>
        /// <param name="result">When the method completes, contains a vector facing in the opposite direction.</param>
        public static void Negate(ref Vector4 value, out Vector4 result)
        {
            result = new Vector4(-value.X, -value.Y, -value.Z, -value.W);
        }

        /// <summary>
        /// Returns a <see cref="Vector4"/> containing the 4D Cartesian coordinates of a point specified in Barycentric coordinates relative to a 4D triangle.
        /// </summary>
        /// <param name="value1">A <see cref="Vector4"/> containing the 4D Cartesian coordinates of vertex 1 of the triangle.</param>
        /// <param name="value2">A <see cref="Vector4"/> containing the 4D Cartesian coordinates of vertex 2 of the triangle.</param>
        /// <param name="value3">A <see cref="Vector4"/> containing the 4D Cartesian coordinates of vertex 3 of the triangle.</param>
        /// <param name="amount1">Barycentric coordinate b2, which expresses the weighting factor toward vertex 2 (specified in <paramref name="value2"/>).</param>
        /// <param name="amount2">Barycentric coordinate b3, which expresses the weighting factor toward vertex 3 (specified in <paramref name="value3"/>).</param>
        /// <returns>A new <see cref="Vector4"/> containing the 4D Cartesian coordinates of the specified point.</returns>
        public static Vector4 Barycentric(Vector4 value1, Vector4 value2, Vector4 value3, float amount1, float amount2)
        {
            Vector4 vector;
            vector.X = value1.X + amount1 * (value2.X - value1.X) + amount2 * (value3.X - value1.X);
            vector.Y = value1.Y + amount1 * (value2.Y - value1.Y) + amount2 * (value3.Y - value1.Y);
            vector.Z = value1.Z + amount1 * (value2.Z - value1.Z) + amount2 * (value3.Z - value1.Z);
            vector.W = value1.W + amount1 * (value2.W - value1.W) + amount2 * (value3.W - value1.W);
            return vector;
        }

        /// <summary>
        /// Returns a <see cref="Vector4"/> containing the 4D Cartesian coordinates of a point specified in Barycentric coordinates relative to a 4D triangle.
        /// </summary>
        /// <param name="value1">A <see cref="Vector4"/> containing the 4D Cartesian coordinates of vertex 1 of the triangle.</param>
        /// <param name="value2">A <see cref="Vector4"/> containing the 4D Cartesian coordinates of vertex 2 of the triangle.</param>
        /// <param name="value3">A <see cref="Vector4"/> containing the 4D Cartesian coordinates of vertex 3 of the triangle.</param>
        /// <param name="amount1">Barycentric coordinate b2, which expresses the weighting factor toward vertex 2 (specified in <paramref name="value2"/>).</param>
        /// <param name="amount2">Barycentric coordinate b3, which expresses the weighting factor toward vertex 3 (specified in <paramref name="value3"/>).</param>
        /// <param name="result">When the method completes, contains the 4D Cartesian coordinates of the specified point.</param>
        public static void Barycentric(ref Vector4 value1, ref Vector4 value2, ref Vector4 value3, float amount1, float amount2, out Vector4 result)
        {
            result = new Vector4(
                value1.X + amount1 * (value2.X - value1.X) + amount2 * (value3.X - value1.X),
                value1.Y + amount1 * (value2.Y - value1.Y) + amount2 * (value3.Y - value1.Y),
                value1.Z + amount1 * (value2.Z - value1.Z) + amount2 * (value3.Z - value1.Z),
                value1.W + amount1 * (value2.W - value1.W) + amount2 * (value3.W - value1.W));
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
        public static Vector4 CatmullRom(Vector4 value1, Vector4 value2, Vector4 value3, Vector4 value4, float amount)
        {
            Vector4 vector;
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

            vector.W = 0.5f * ((((2.0f * value2.W) + ((-value1.W + value3.W) * amount)) +
                (((((2.0f * value1.W) - (5.0f * value2.W)) + (4.0f * value3.W)) - value4.W) * squared)) +
                ((((-value1.W + (3.0f * value2.W)) - (3.0f * value3.W)) + value4.W) * cubed));

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
        public static void CatmullRom(ref Vector4 value1, ref Vector4 value2, ref Vector4 value3, ref Vector4 value4, float amount, out Vector4 result)
        {
            float squared = amount * amount;
            float cubed = amount * squared;

            Vector4 r;

            r.X = 0.5f * ((((2.0f * value2.X) + ((-value1.X + value3.X) * amount)) +
                (((((2.0f * value1.X) - (5.0f * value2.X)) + (4.0f * value3.X)) - value4.X) * squared)) +
                ((((-value1.X + (3.0f * value2.X)) - (3.0f * value3.X)) + value4.X) * cubed));

            r.Y = 0.5f * ((((2.0f * value2.Y) + ((-value1.Y + value3.Y) * amount)) +
                (((((2.0f * value1.Y) - (5.0f * value2.Y)) + (4.0f * value3.Y)) - value4.Y) * squared)) +
                ((((-value1.Y + (3.0f * value2.Y)) - (3.0f * value3.Y)) + value4.Y) * cubed));

            r.Z = 0.5f * ((((2.0f * value2.Z) + ((-value1.Z + value3.Z) * amount)) +
                (((((2.0f * value1.Z) - (5.0f * value2.Z)) + (4.0f * value3.Z)) - value4.Z) * squared)) +
                ((((-value1.Z + (3.0f * value2.Z)) - (3.0f * value3.Z)) + value4.Z) * cubed));

            r.W = 0.5f * ((((2.0f * value2.W) + ((-value1.W + value3.W) * amount)) +
                (((((2.0f * value1.W) - (5.0f * value2.W)) + (4.0f * value3.W)) - value4.W) * squared)) +
                ((((-value1.W + (3.0f * value2.W)) - (3.0f * value3.W)) + value4.W) * cubed));

            result = r;
        }

        /// <summary>
        /// Restricts a value to be within a specified range.
        /// </summary>
        /// <param name="value">The value to clamp.</param>
        /// <param name="min">The minimum value.</param>
        /// <param name="max">The maximum value.</param>
        /// <returns>The clamped value.</returns>
        public static Vector4 Clamp(Vector4 value, Vector4 min, Vector4 max)
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

            float w = value.W;
            w = (w > max.W) ? max.W : w;
            w = (w < min.W) ? min.W : w;

            return new Vector4(x, y, z, w);
        }

        /// <summary>
        /// Restricts a value to be within a specified range.
        /// </summary>
        /// <param name="value">The value to clamp.</param>
        /// <param name="min">The minimum value.</param>
        /// <param name="max">The maximum value.</param>
        /// <param name="result">When the method completes, contains the clamped value.</param>
        public static void Clamp(ref Vector4 value, ref Vector4 min, ref Vector4 max, out Vector4 result)
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

            float w = value.W;
            w = (w > max.W) ? max.W : w;
            w = (w < min.W) ? min.W : w;

            result = new Vector4(x, y, z, w);
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
        public static Vector4 Hermite(Vector4 value1, Vector4 tangent1, Vector4 value2, Vector4 tangent2, float amount)
        {
            Vector4 vector;
            float squared = amount * amount;
            float cubed = amount * squared;
            float part1 = ((2.0f * cubed) - (3.0f * squared)) + 1.0f;
            float part2 = (-2.0f * cubed) + (3.0f * squared);
            float part3 = (cubed - (2.0f * squared)) + amount;
            float part4 = cubed - squared;

            vector.X = (((value1.X * part1) + (value2.X * part2)) + (tangent1.X * part3)) + (tangent2.X * part4);
            vector.Y = (((value1.Y * part1) + (value2.Y * part2)) + (tangent1.Y * part3)) + (tangent2.Y * part4);
            vector.Z = (((value1.Z * part1) + (value2.Z * part2)) + (tangent1.Z * part3)) + (tangent2.Z * part4);
            vector.W = (((value1.W * part1) + (value2.W * part2)) + (tangent1.W * part3)) + (tangent2.W * part4);

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
        public static void Hermite(ref Vector4 value1, ref Vector4 tangent1, ref Vector4 value2, ref Vector4 tangent2, float amount, out Vector4 result)
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
            result.W = (((value1.W * part1) + (value2.W * part2)) + (tangent1.W * part3)) + (tangent2.W * part4);
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
        public static Vector4 Lerp(Vector4 start, Vector4 end, float amount)
        {
            Vector4 vector;

            vector.X = start.X + ((end.X - start.X) * amount);
            vector.Y = start.Y + ((end.Y - start.Y) * amount);
            vector.Z = start.Z + ((end.Z - start.Z) * amount);
            vector.W = start.W + ((end.W - start.W) * amount);

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
        public static void Lerp(ref Vector4 start, ref Vector4 end, float amount, out Vector4 result)
        {
            result.X = start.X + ((end.X - start.X) * amount);
            result.Y = start.Y + ((end.Y - start.Y) * amount);
            result.Z = start.Z + ((end.Z - start.Z) * amount);
            result.W = start.W + ((end.W - start.W) * amount);
        }

        /// <summary>
        /// Performs a cubic interpolation between two vectors.
        /// </summary>
        /// <param name="start">Start vector.</param>
        /// <param name="end">End vector.</param>
        /// <param name="amount">Value between 0 and 1 indicating the weight of <paramref name="end"/>.</param>
        /// <returns>The cubic interpolation of the two vectors.</returns>
        public static Vector4 SmoothStep(Vector4 start, Vector4 end, float amount)
        {
            Vector4 vector;

            amount = (amount > 1.0f) ? 1.0f : ((amount < 0.0f) ? 0.0f : amount);
            amount = (amount * amount) * (3.0f - (2.0f * amount));

            vector.X = start.X + ((end.X - start.X) * amount);
            vector.Y = start.Y + ((end.Y - start.Y) * amount);
            vector.Z = start.Z + ((end.Z - start.Z) * amount);
            vector.W = start.W + ((end.W - start.W) * amount);

            return vector;
        }

        /// <summary>
        /// Performs a cubic interpolation between two vectors.
        /// </summary>
        /// <param name="start">Start vector.</param>
        /// <param name="end">End vector.</param>
        /// <param name="amount">Value between 0 and 1 indicating the weight of <paramref name="end"/>.</param>
        /// <param name="result">When the method completes, contains the cubic interpolation of the two vectors.</param>
        public static void SmoothStep(ref Vector4 start, ref Vector4 end, float amount, out Vector4 result)
        {
            amount = (amount > 1.0f) ? 1.0f : ((amount < 0.0f) ? 0.0f : amount);
            amount = (amount * amount) * (3.0f - (2.0f * amount));

            result.X = start.X + ((end.X - start.X) * amount);
            result.Y = start.Y + ((end.Y - start.Y) * amount);
            result.Z = start.Z + ((end.Z - start.Z) * amount);
            result.W = start.W + ((end.W - start.W) * amount);
        }

        /// <summary>
        /// Calculates the distance between two vectors.
        /// </summary>
        /// <param name="value1">The first vector.</param>
        /// <param name="value2">The second vector.</param>
        /// <returns>The distance between the two vectors.</returns>
        public static float Distance(Vector4 value1, Vector4 value2)
        {
            float x = value1.X - value2.X;
            float y = value1.Y - value2.Y;
            float z = value1.Z - value2.Z;
            float w = value1.W - value2.W;

            return (float)Math.Sqrt(x * x + y * y + z * z + w * w);
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
        public static float DistanceSquared(Vector4 value1, Vector4 value2)
        {
            float x = value1.X - value2.X;
            float y = value1.Y - value2.Y;
            float z = value1.Z - value2.Z;
            float w = value1.W - value2.W;

            return x * x + y * y + z * z + w * w;
        }

        /// <summary>
        /// Calculates the dot product of two vectors.
        /// </summary>
        /// <param name="left">First source vector.</param>
        /// <param name="right">Second source vector.</param>
        /// <returns>The dot product of the two vectors.</returns>
        public static float Dot(Vector4 left, Vector4 right)
        {
            return left.X * right.X + left.Y * right.Y + left.Z * right.Z + left.W * right.W;
        }

        /// <summary>
        /// Converts the vector into a unit vector.
        /// </summary>
        /// <param name="vector">The vector to normalize.</param>
        /// <returns>The normalized vector.</returns>
        public static Vector4 Normalize(Vector4 vector)
        {
            vector.Normalize();
            return vector;
        }

        /// <summary>
        /// Converts the vector into a unit vector.
        /// </summary>
        /// <param name="vector">The vector to normalize.</param>
        /// <param name="result">When the method completes, contains the normalized vector.</param>
        public static void Normalize(ref Vector4 vector, out Vector4 result)
        {
            result = vector;
            result.Normalize();
        }

        /// <summary>
        /// Transforms a 4D vector by the given <see cref="Matrix"/>.
        /// </summary>
        /// <param name="vector">The source vector.</param>
        /// <param name="transformation">The transformation <see cref="Matrix"/>.</param>
        /// <returns>The transformed <see cref="Vector4"/>.</returns>
        public static Vector4 Transform(Vector4 vector, Matrix transformation)
        {
            Vector4 result;

            result.X = (vector.X * transformation.M11) + (vector.Y * transformation.M21) + (vector.Z * transformation.M31) + (vector.W * transformation.M41);
            result.Y = (vector.X * transformation.M12) + (vector.Y * transformation.M22) + (vector.Z * transformation.M32) + (vector.W * transformation.M42);
            result.Z = (vector.X * transformation.M13) + (vector.Y * transformation.M23) + (vector.Z * transformation.M33) + (vector.W * transformation.M43);
            result.W = (vector.X * transformation.M14) + (vector.Y * transformation.M24) + (vector.Z * transformation.M34) + (vector.W * transformation.M44);

            return result;
        }

        /// <summary>
        /// Transforms a 4D vector by the given <see cref="Matrix"/>.
        /// </summary>
        /// <param name="vector">The source vector.</param>
        /// <param name="transformation">The transformation <see cref="Matrix"/>.</param>
        /// <param name="result">When the method completes, contains the transformed <see cref="Vector4"/>.</param>
        public static void Transform(ref Vector4 vector, ref Matrix transformation, out Vector4 result)
        {
            Vector4 r;
            r.X = (vector.X * transformation.M11) + (vector.Y * transformation.M21) + (vector.Z * transformation.M31) + (vector.W * transformation.M41);
            r.Y = (vector.X * transformation.M12) + (vector.Y * transformation.M22) + (vector.Z * transformation.M32) + (vector.W * transformation.M42);
            r.Z = (vector.X * transformation.M13) + (vector.Y * transformation.M23) + (vector.Z * transformation.M33) + (vector.W * transformation.M43);
            r.W = (vector.X * transformation.M14) + (vector.Y * transformation.M24) + (vector.Z * transformation.M34) + (vector.W * transformation.M44);

            result = r;
        }

        /// <summary>
        /// Transforms an array of 4D vectors by the given <see cref="Matrix"/>.
        /// </summary>
        /// <param name="vectors">The source vectors.</param>
        /// <param name="transformation">The transformation <see cref="Matrix"/>.</param>
        /// <returns>The transformed <see cref="Vector4"/>s.</returns>
        public static Vector4[] Transform(Vector4[] vectors, ref Matrix transformation)
        {
            if (vectors == null)
                throw new ArgumentNullException("vectors");

            int count = vectors.Length;
            Vector4[] results = new Vector4[count];

            for (int i = 0; i < count; i++)
            {
                Vector4 r;
                r.X = (vectors[i].X * transformation.M11) + (vectors[i].Y * transformation.M21) + (vectors[i].Z * transformation.M31) + (vectors[i].W * transformation.M41);
                r.Y = (vectors[i].X * transformation.M12) + (vectors[i].Y * transformation.M22) + (vectors[i].Z * transformation.M32) + (vectors[i].W * transformation.M42);
                r.Z = (vectors[i].X * transformation.M13) + (vectors[i].Y * transformation.M23) + (vectors[i].Z * transformation.M33) + (vectors[i].W * transformation.M43);
                r.W = (vectors[i].X * transformation.M14) + (vectors[i].Y * transformation.M24) + (vectors[i].Z * transformation.M34) + (vectors[i].W * transformation.M44);

                results[i] = r;
            }

            return results;
        }

        /// <summary>
        /// Transforms a 4D vector by the given <see cref="Quaternion"/> rotation.
        /// </summary>
        /// <param name="vector">The vector to rotate.</param>
        /// <param name="rotation">The <see cref="Quaternion"/> rotation to apply.</param>
        /// <returns>The transformed <see cref="Vector4"/>.</returns>
        public static Vector4 Transform(Vector4 vector, Quaternion rotation)
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
            result.W = vector.W;

            return result;
        }

        /// <summary>
        /// Transforms a 4D vector by the given <see cref="Quaternion"/> rotation.
        /// </summary>
        /// <param name="vector">The vector to rotate.</param>
        /// <param name="rotation">The <see cref="Quaternion"/> rotation to apply.</param>
        /// <param name="result">When the method completes, contains the transformed <see cref="Vector4"/>.</param>
        public static void Transform(ref Vector4 vector, ref Quaternion rotation, out Vector4 result)
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

            Vector4 r;
            r.X = ((vector.X * ((1.0f - yy) - zz)) + (vector.Y * (xy - wz))) + (vector.Z * (xz + wy));
            r.Y = ((vector.X * (xy + wz)) + (vector.Y * ((1.0f - xx) - zz))) + (vector.Z * (yz - wx));
            r.Z = ((vector.X * (xz - wy)) + (vector.Y * (yz + wx))) + (vector.Z * ((1.0f - xx) - yy));
            r.W = vector.W;

            result = r;
        }

        /// <summary>
        /// Transforms an array of 4D vectors by the given <see cref="Quaternion"/> rotation.
        /// </summary>
        /// <param name="vectors">The vectors to rotate.</param>
        /// <param name="rotation">The <see cref="Quaternion"/> rotation to apply.</param>
        /// <returns>The transformed <see cref="Vector4"/>.</returns>
        public static Vector4[] Transform(Vector4[] vectors, ref Quaternion rotation)
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
                r.W = vectors[i].W;

                results[i] = r;
            }

            return results;
        }

        /// <summary>
        /// Returns a vector containing the smallest components of the specified vectors.
        /// </summary>
        /// <param name="left">The first source vector.</param>
        /// <param name="right">The second source vector.</param>
        /// <returns>A vector containing the smallest components of the source vectors.</returns>
        public static Vector4 Minimize(Vector4 left, Vector4 right)
        {
            Vector4 vector;
            vector.X = (left.X < right.X) ? left.X : right.X;
            vector.Y = (left.Y < right.Y) ? left.Y : right.Y;
            vector.Z = (left.Z < right.Z) ? left.Z : right.Z;
            vector.W = (left.W < right.W) ? left.W : right.W;
            return vector;
        }

        /// <summary>
        /// Returns a vector containing the smallest components of the specified vectors.
        /// </summary>
        /// <param name="left">The first source vector.</param>
        /// <param name="right">The second source vector.</param>
        /// <param name="result">When the method completes, contains an new vector composed of the smallest components of the source vectors.</param>
        public static void Minimize(ref Vector4 left, ref Vector4 right, out Vector4 result)
        {
            result.X = (left.X < right.X) ? left.X : right.X;
            result.Y = (left.Y < right.Y) ? left.Y : right.Y;
            result.Z = (left.Z < right.Z) ? left.Z : right.Z;
            result.W = (left.W < right.W) ? left.W : right.W;
        }

        /// <summary>
        /// Returns a vector containing the largest components of the specified vectors.
        /// </summary>
        /// <param name="left">The first source vector.</param>
        /// <param name="right">The second source vector.</param>
        /// <returns>A vector containing the largest components of the source vectors.</returns>
        public static Vector4 Maximize(Vector4 left, Vector4 right)
        {
            Vector4 vector;
            vector.X = (left.X > right.X) ? left.X : right.X;
            vector.Y = (left.Y > right.Y) ? left.Y : right.Y;
            vector.Z = (left.Z > right.Z) ? left.Z : right.Z;
            vector.W = (left.W > right.W) ? left.W : right.W;
            return vector;
        }

        /// <summary>
        /// Returns a vector containing the smallest components of the specified vectors.
        /// </summary>
        /// <param name="left">The first source vector.</param>
        /// <param name="right">The second source vector.</param>
        /// <param name="result">When the method completes, contains an new vector composed of the largest components of the source vectors.</param>
        public static void Maximize(ref Vector4 left, ref Vector4 right, out Vector4 result)
        {
            result.X = (left.X > right.X) ? left.X : right.X;
            result.Y = (left.Y > right.Y) ? left.Y : right.Y;
            result.Z = (left.Z > right.Z) ? left.Z : right.Z;
            result.W = (left.W > right.W) ? left.W : right.W;
        }

        /// <summary>
        /// Adds two vectors.
        /// </summary>
        /// <param name="left">The first vector to add.</param>
        /// <param name="right">The second vector to add.</param>
        /// <returns>The sum of the two vectors.</returns>
        public static Vector4 operator +(Vector4 left, Vector4 right)
        {
            return new Vector4(left.X + right.X, left.Y + right.Y, left.Z + right.Z, left.W + right.W);
        }

        /// <summary>
        /// Subtracts two vectors.
        /// </summary>
        /// <param name="left">The first vector to subtract.</param>
        /// <param name="right">The second vector to subtract.</param>
        /// <returns>The difference of the two vectors.</returns>
        public static Vector4 operator -(Vector4 left, Vector4 right)
        {
            return new Vector4(left.X - right.X, left.Y - right.Y, left.Z - right.Z, left.W - right.W);
        }

        /// <summary>
        /// Reverses the direction of a given vector.
        /// </summary>
        /// <param name="value">The vector to negate.</param>
        /// <returns>A vector facing in the opposite direction.</returns>
        public static Vector4 operator -(Vector4 value)
        {
            return new Vector4(-value.X, -value.Y, -value.Z, -value.W);
        }
	

        /// <summary>
        /// Scales a vector by the given value.
        /// </summary>
        /// <param name="vector">The vector to scale.</param>
        /// <param name="scale">The amount by which to scale the vector.</param>
        /// <returns>The scaled vector.</returns>
        public static Vector4 operator *(Vector4 vector, float scale)
        {
            return new Vector4(vector.X * scale, vector.Y * scale, vector.Z * scale, vector.W * scale);
        }

        /// <summary>
        /// Scales a vector by the given value.
        /// </summary>
        /// <param name="vector">The vector to scale.</param>
        /// <param name="scale">The amount by which to scale the vector.</param>
        /// <returns>The scaled vector.</returns>
        public static Vector4 operator *(float scale, Vector4 vector)
        {
            return vector * scale;
        }

        /// <summary>
        /// Scales a vector by the given value.
        /// </summary>
        /// <param name="vector">The vector to scale.</param>
        /// <param name="scale">The amount by which to scale the vector.</param>
        /// <returns>The scaled vector.</returns>
        public static Vector4 operator /(Vector4 vector, float scale)
        {
            return new Vector4(vector.X / scale, vector.Y / scale, vector.Z / scale, vector.W / scale);
        }

        /// <summary>
        /// Tests for equality between two objects.
        /// </summary>
        /// <param name="left">The first value to compare.</param>
        /// <param name="right">The second value to compare.</param>
        /// <returns><c>true</c> if <paramref name="left"/> has the same value as <paramref name="right"/>; otherwise, <c>false</c>.</returns>
        public static bool operator ==(Vector4 left, Vector4 right)
        {
            return Vector4.Equals(ref left, ref right);
        }

        /// <summary>
        /// Tests for inequality between two objects.
        /// </summary>
        /// <param name="left">The first value to compare.</param>
        /// <param name="right">The second value to compare.</param>
        /// <returns><c>true</c> if <paramref name="left"/> has a different value than <paramref name="right"/>; otherwise, <c>false</c>.</returns>
        public static bool operator !=(Vector4 left, Vector4 right)
        {
            return !Vector4.Equals(ref left, ref right);
        }

        /// <summary>
        /// Converts the value of the object to its equivalent string representation.
        /// </summary>
        /// <returns>The string representation of the value of this instance.</returns>
        public override string ToString()
        {
            return string.Format(CultureInfo.CurrentCulture, "X:{0} Y:{1} Z:{2} W:{3}", 
                X.ToString(CultureInfo.CurrentCulture),
                Y.ToString(CultureInfo.CurrentCulture), Z.ToString(CultureInfo.CurrentCulture),
                W.ToString(CultureInfo.CurrentCulture));
        }

        /// <summary>
        /// Returns the hash code for this instance.
        /// </summary>
        /// <returns>A 32-bit signed integer hash code.</returns>
        public override int GetHashCode()
        {
            return X.GetHashCode() + Y.GetHashCode() + Z.GetHashCode() + W.GetHashCode();
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

            return Equals((Vector4)obj);
        }

        /// <summary>
        /// Returns a value that indicates whether the current instance is equal to the specified object. 
        /// </summary>
        /// <param name="other">Object to make the comparison with.</param>
        /// <returns><c>true</c> if the current instance is equal to the specified object; <c>false</c> otherwise.</returns>
        public bool Equals(Vector4 other)
        {
            return (X == other.X && Y == other.Y && Z == other.Z && W == other.W);
        }

        /// <summary>
        /// Determines whether the specified object instances are considered equal. 
        /// </summary>
        /// <param name="value1">The first value to compare.</param>
        /// <param name="value2">The second value to compare.</param>
        /// <returns><c>true</c> if <paramref name="value1"/> is the same instance as <paramref name="value2"/> or 
        /// if both are <c>null</c> references or if <c>value1.Equals(value2)</c> returns <c>true</c>; otherwise, <c>false</c>.</returns>
        public static bool Equals(ref Vector4 value1, ref Vector4 value2)
        {
            return (value1.X == value2.X && value1.Y == value2.Y && value1.Z == value2.Z && value1.W == value2.W);
        }

    }
}