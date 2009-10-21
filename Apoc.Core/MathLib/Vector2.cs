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
    /// Defines a two component vector.
    /// </summary>
    [Serializable, StructLayout(LayoutKind.Sequential), TypeConverter(typeof(Vector2Converter))]
    public unsafe struct Vector2 : IEquatable<Vector2>
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

                    default:
                        throw new IndexOutOfRangeException("Indices for Vector2 run from 0 to 1, inclusive.");
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

                    default:
                        throw new IndexOutOfRangeException("Indices for Vector2 run from 0 to 1, inclusive.");
                }
            }
        }

        /// <summary>
        /// Gets a <see cref="Vector2"/> with all of its components set to zero.
        /// </summary>
        /// <value>A <see cref="Vector2"/> that has all of its components set to zero.</value>
        public static Vector2 Zero
        {
            get { return new Vector2(0, 0); }
        }

        /// <summary>
        /// Gets the X unit <see cref="Vector2"/> (1, 0).
        /// </summary>
        /// <value>A <see cref="Vector2"/> that has a value of (1, 0).</value>
        public static Vector2 UnitX
        {
            get { return new Vector2(1, 0); }
        }

        /// <summary>
        /// Gets the Y unit <see cref="Vector2"/> (0, 1).
        /// </summary>
        /// <value>A <see cref="Vector2"/> that has a value of (0, 1).</value>
        public static Vector2 UnitY
        {
            get { return new Vector2(0, 1); }
        }

        /// <summary>
        /// Gets the size of the <see cref="Vector2"/> type, in bytes.
        /// </summary>
        public static int SizeInBytes
        {
            get { return sizeof(Vector2); }
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="Vector2"/> class.
        /// </summary>
        /// <param name="value">The value that will be assigned to all components.</param>
        public Vector2(float value)
        {
            X = value;
            Y = value;
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="Vector2"/> class.
        /// </summary>
        /// <param name="x">Initial value for the X component of the vector.</param>
        /// <param name="y">Initial value for the Y component of the vector.</param>
        public Vector2(float x, float y)
        {
            X = x;
            Y = y;
        }

        /// <summary>
        /// Calculates the length of the vector.
        /// </summary>
        /// <returns>The length of the vector.</returns>
        public float Length() 
        {
            return (float)Math.Sqrt(X * X + Y * Y);
        }

        /// <summary>
        /// Calculates the squared length of the vector.
        /// </summary>
        /// <returns>The squared length of the vector.</returns>
        public float LengthSquared()
        {
            return X * X + Y * Y;
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
        }

        /// <summary>
        /// Adds two vectors.
        /// </summary>
        /// <param name="left">The first vector to add.</param>
        /// <param name="right">The second vector to add.</param>
        /// <returns>The sum of the two vectors.</returns>
        public static Vector2 Add(Vector2 left, Vector2 right)
        {
            return new Vector2(left.X + right.X, left.Y + right.Y);
        }

        /// <summary>
        /// Adds two vectors.
        /// </summary>
        /// <param name="left">The first vector to add.</param>
        /// <param name="right">The second vector to add.</param>
        /// <param name="result">When the method completes, contains the sum of the two vectors.</param>
        public static void Add(ref Vector2 left, ref Vector2 right, out Vector2 result)
        {
            result = new Vector2(left.X + right.X, left.Y + right.Y);
        }

        /// <summary>
        /// Subtracts two vectors.
        /// </summary>
        /// <param name="left">The first vector to subtract.</param>
        /// <param name="right">The second vector to subtract.</param>
        /// <returns>The difference of the two vectors.</returns>
        public static Vector2 Subtract(Vector2 left, Vector2 right)
        {
            return new Vector2(left.X - right.X, left.Y - right.Y);
        }

        /// <summary>
        /// Subtracts two vectors.
        /// </summary>
        /// <param name="left">The first vector to subtract.</param>
        /// <param name="right">The second vector to subtract.</param>
        /// <param name="result">When the method completes, contains the difference of the two vectors.</param>
        public static void Subtract(ref Vector2 left, ref Vector2 right, out Vector2 result)
        {
            result = new Vector2(left.X - right.X, left.Y - right.Y);
        }

        /// <summary>
        /// Scales a vector by the given value.
        /// </summary>
        /// <param name="value">The vector to scale.</param>
        /// <param name="scale">The amount by which to scale the vector.</param>
        /// <returns>The scaled vector.</returns>
        public static Vector2 Multiply(Vector2 value, float scale)
        {
            return new Vector2(value.X * scale, value.Y * scale);
        }

        /// <summary>
        /// Scales a vector by the given value.
        /// </summary>
        /// <param name="vector">The vector to scale.</param>
        /// <param name="scale">The amount by which to scale the vector.</param>
        /// <param name="result">When the method completes, contains the scaled vector.</param>
        public static void Multiply(ref Vector2 vector, float scale, out Vector2 result)
        {
            result = new Vector2(vector.X * scale, vector.Y * scale);
        }

        /// <summary>
        /// Modulates a vector by another.
        /// </summary>
        /// <param name="left">The first vector to modulate.</param>
        /// <param name="right">The second vector to modulate.</param>
        /// <returns>The modulated vector.</returns>
        public static Vector2 Modulate(Vector2 left, Vector2 right)
        {
            return new Vector2(left.X * right.X, left.Y * right.Y);
        }

        /// <summary>
        /// Modulates a vector by another.
        /// </summary>
        /// <param name="left">The first vector to modulate.</param>
        /// <param name="right">The second vector to modulate.</param>
        /// <param name="result">When the moethod completes, contains the modulated vector.</param>
        public static void Modulate(ref Vector2 left, ref Vector2 right, out Vector2 result)
        {
            result = new Vector2(left.X * right.X, left.Y * right.Y);
        }

        /// <summary>
        /// Scales a vector by the given value.
        /// </summary>
        /// <param name="value">The vector to scale.</param>
        /// <param name="scale">The amount by which to scale the vector.</param>
        /// <returns>The scaled vector.</returns>
        public static Vector2 Divide(Vector2 value, float scale)
        {
            return new Vector2(value.X / scale, value.Y / scale);
        }

        /// <summary>
        /// Scales a vector by the given value.
        /// </summary>
        /// <param name="vector">The vector to scale.</param>
        /// <param name="scale">The amount by which to scale the vector.</param>
        /// <param name="result">When the method completes, contains the scaled vector.</param>
        public static void Divide(ref Vector2 vector, float scale, out Vector2 result)
        {
            result = new Vector2(vector.X / scale, vector.Y / scale);
        }

        /// <summary>
        /// Reverses the direction of a given vector.
        /// </summary>
        /// <param name="value">The vector to negate.</param>
        /// <returns>A vector facing in the opposite direction.</returns>
        public static Vector2 Negate(Vector2 value)
        {
            return new Vector2(-value.X, -value.Y);
        }

        /// <summary>
        /// Reverses the direction of a given vector.
        /// </summary>
        /// <param name="value">The vector to negate.</param>
        /// <param name="result">When the method completes, contains a vector facing in the opposite direction.</param>
        public static void Negate(ref Vector2 value, out Vector2 result)
        {
            result = new Vector2(-value.X, -value.Y);
        }

        /// <summary>
        /// Returns a <see cref="Vector2"/> containing the 2D Cartesian coordinates of a point specified in Barycentric coordinates relative to a 2D triangle.
        /// </summary>
        /// <param name="value1">A <see cref="Vector2"/> containing the 2D Cartesian coordinates of vertex 1 of the triangle.</param>
        /// <param name="value2">A <see cref="Vector2"/> containing the 2D Cartesian coordinates of vertex 2 of the triangle.</param>
        /// <param name="value3">A <see cref="Vector2"/> containing the 2D Cartesian coordinates of vertex 3 of the triangle.</param>
        /// <param name="amount1">Barycentric coordinate b2, which expresses the weighting factor toward vertex 2 (specified in <paramref name="value2"/>).</param>
        /// <param name="amount2">Barycentric coordinate b3, which expresses the weighting factor toward vertex 3 (specified in <paramref name="value3"/>).</param>
        /// <returns>A new <see cref="Vector2"/> containing the 2D Cartesian coordinates of the specified point.</returns>
        public static Vector2 Barycentric(Vector2 value1, Vector2 value2, Vector2 value3, float amount1, float amount2)
        {
            Vector2 vector;
            vector.X = (value1.X + (amount1 * (value2.X - value1.X))) + (amount2 * (value3.X - value1.X));
            vector.Y = (value1.Y + (amount1 * (value2.Y - value1.Y))) + (amount2 * (value3.Y - value1.Y));
            return vector;
        }

        /// <summary>
        /// Returns a <see cref="Vector2"/> containing the 2D Cartesian coordinates of a point specified in Barycentric coordinates relative to a 2D triangle.
        /// </summary>
        /// <param name="value1">A <see cref="Vector2"/> containing the 2D Cartesian coordinates of vertex 1 of the triangle.</param>
        /// <param name="value2">A <see cref="Vector2"/> containing the 2D Cartesian coordinates of vertex 2 of the triangle.</param>
        /// <param name="value3">A <see cref="Vector2"/> containing the 2D Cartesian coordinates of vertex 3 of the triangle.</param>
        /// <param name="amount1">Barycentric coordinate b2, which expresses the weighting factor toward vertex 2 (specified in <paramref name="value2"/>).</param>
        /// <param name="amount2">Barycentric coordinate b3, which expresses the weighting factor toward vertex 3 (specified in <paramref name="value3"/>).</param>
        /// <param name="result">When the method completes, contains the 2D Cartesian coordinates of the specified point.</param>
        public static void Barycentric(ref Vector2 value1, ref Vector2 value2, ref Vector2 value3, float amount1, float amount2, out Vector2 result)
        {
            result = new Vector2((value1.X + (amount1 * (value2.X - value1.X))) + (amount2 * (value3.X - value1.X)),
                (value1.Y + (amount1 * (value2.Y - value1.Y))) + (amount2 * (value3.Y - value1.Y)));
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
        public static Vector2 CatmullRom(Vector2 value1, Vector2 value2, Vector2 value3, Vector2 value4, float amount)
        {
            Vector2 vector;
            float squared = amount * amount;
            float cubed = amount * squared;

            vector.X = 0.5f * ((((2.0f * value2.X) + ((-value1.X + value3.X) * amount)) +
                (((((2.0f * value1.X) - (5.0f * value2.X)) + (4.0f * value3.X)) - value4.X) * squared)) +
                ((((-value1.X + (3.0f * value2.X)) - (3.0f * value3.X)) + value4.X) * cubed));

            vector.Y = 0.5f * ((((2.0f * value2.Y) + ((-value1.Y + value3.Y) * amount)) +
                (((((2.0f * value1.Y) - (5.0f * value2.Y)) + (4.0f * value3.Y)) - value4.Y) * squared)) +
                ((((-value1.Y + (3.0f * value2.Y)) - (3.0f * value3.Y)) + value4.Y) * cubed));

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
        public static void CatmullRom(ref Vector2 value1, ref Vector2 value2, ref Vector2 value3, ref Vector2 value4, float amount, out Vector2 result)
        {
            float squared = amount * amount;
            float cubed = amount * squared;
            Vector2 r;

            r.X = 0.5f * ((((2.0f * value2.X) + ((-value1.X + value3.X) * amount)) +
                (((((2.0f * value1.X) - (5.0f * value2.X)) + (4.0f * value3.X)) - value4.X) * squared)) +
                ((((-value1.X + (3.0f * value2.X)) - (3.0f * value3.X)) + value4.X) * cubed));

            r.Y = 0.5f * ((((2.0f * value2.Y) + ((-value1.Y + value3.Y) * amount)) +
                (((((2.0f * value1.Y) - (5.0f * value2.Y)) + (4.0f * value3.Y)) - value4.Y) * squared)) +
                ((((-value1.Y + (3.0f * value2.Y)) - (3.0f * value3.Y)) + value4.Y) * cubed));

            result = r;
        }

        /// <summary>
        /// Restricts a value to be within a specified range.
        /// </summary>
        /// <param name="value">The value to clamp.</param>
        /// <param name="min">The minimum value.</param>
        /// <param name="max">The maximum value.</param>
        /// <returns>The clamped value.</returns>
        public static Vector2 Clamp(Vector2 value, Vector2 min, Vector2 max)
        {
            float x = value.X;
            x = (x > max.X) ? max.X : x;
            x = (x < min.X) ? min.X : x;

            float y = value.Y;
            y = (y > max.Y) ? max.Y : y;
            y = (y < min.Y) ? min.Y : y;

            return new Vector2(x, y);
        }

        /// <summary>
        /// Restricts a value to be within a specified range.
        /// </summary>
        /// <param name="value">The value to clamp.</param>
        /// <param name="min">The minimum value.</param>
        /// <param name="max">The maximum value.</param>
        /// <param name="result">When the method completes, contains the clamped value.</param>
        public static void Clamp(ref Vector2 value, ref Vector2 min, ref Vector2 max, out Vector2 result)
        {
            float x = value.X;
            x = (x > max.X) ? max.X : x;
            x = (x < min.X) ? min.X : x;

            float y = value.Y;
            y = (y > max.Y) ? max.Y : y;
            y = (y < min.Y) ? min.Y : y;

            result = new Vector2(x, y);
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
        public static Vector2 Hermite(Vector2 value1, Vector2 tangent1, Vector2 value2, Vector2 tangent2, float amount)
        {
            Vector2 vector;
            float squared = amount * amount;
            float cubed = amount * squared;
            float part1 = ((2.0f * cubed) - (3.0f * squared)) + 1.0f;
            float part2 = (-2.0f * cubed) + (3.0f * squared);
            float part3 = (cubed - (2.0f * squared)) + amount;
            float part4 = cubed - squared;

            vector.X = (((value1.X * part1) + (value2.X * part2)) + (tangent1.X * part3)) + (tangent2.X * part4);
            vector.Y = (((value1.Y * part1) + (value2.Y * part2)) + (tangent1.Y * part3)) + (tangent2.Y * part4);

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
        public static void Hermite(ref Vector2 value1, ref Vector2 tangent1, ref Vector2 value2, ref Vector2 tangent2, float amount, out Vector2 result)
        {
            float squared = amount * amount;
            float cubed = amount * squared;
            float part1 = ((2.0f * cubed) - (3.0f * squared)) + 1.0f;
            float part2 = (-2.0f * cubed) + (3.0f * squared);
            float part3 = (cubed - (2.0f * squared)) + amount;
            float part4 = cubed - squared;

            Vector2 r;
            r.X = (((value1.X * part1) + (value2.X * part2)) + (tangent1.X * part3)) + (tangent2.X * part4);
            r.Y = (((value1.Y * part1) + (value2.Y * part2)) + (tangent1.Y * part3)) + (tangent2.Y * part4);

            result = r;
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
        public static Vector2 Lerp(Vector2 start, Vector2 end, float amount)
        {
            Vector2 vector;

            vector.X = start.X + ((end.X - start.X) * amount);
            vector.Y = start.Y + ((end.Y - start.Y) * amount);

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
        public static void Lerp(ref Vector2 start, ref Vector3 end, float amount, out Vector2 result)
        {
            Vector2 r;
            r.X = start.X + ((end.X - start.X) * amount);
            r.Y = start.Y + ((end.Y - start.Y) * amount);

            result = r;
        }

        /// <summary>
        /// Performs a cubic interpolation between two vectors.
        /// </summary>
        /// <param name="start">Start vector.</param>
        /// <param name="end">End vector.</param>
        /// <param name="amount">Value between 0 and 1 indicating the weight of <paramref name="end"/>.</param>
        /// <returns>The cubic interpolation of the two vectors.</returns>
        public static Vector2 SmoothStep(Vector2 start, Vector2 end, float amount)
        {
            Vector2 vector;

            amount = (amount > 1.0f) ? 1.0f : ((amount < 0.0f) ? 0.0f : amount);
            amount = (amount * amount) * (3.0f - (2.0f * amount));

            vector.X = start.X + ((end.X - start.X) * amount);
            vector.Y = start.Y + ((end.Y - start.Y) * amount);

            return vector;
        }

        /// <summary>
        /// Performs a cubic interpolation between two vectors.
        /// </summary>
        /// <param name="start">Start vector.</param>
        /// <param name="end">End vector.</param>
        /// <param name="amount">Value between 0 and 1 indicating the weight of <paramref name="end"/>.</param>
        /// <param name="result">When the method completes, contains the cubic interpolation of the two vectors.</param>
        public static void SmoothStep(ref Vector2 start, ref Vector2 end, float amount, out Vector2 result)
        {
            amount = (amount > 1.0f) ? 1.0f : ((amount < 0.0f) ? 0.0f : amount);
            amount = (amount * amount) * (3.0f - (2.0f * amount));

            result.X = start.X + ((end.X - start.X) * amount);
            result.Y = start.Y + ((end.Y - start.Y) * amount);
        }

        /// <summary>
        /// Calculates the distance between two vectors.
        /// </summary>
        /// <param name="value1">The first vector.</param>
        /// <param name="value2">The second vector.</param>
        /// <returns>The distance between the two vectors.</returns>
        public static float Distance(Vector2 value1, Vector2 value2)
        {
            float x = value1.X - value2.X;
            float y = value1.Y - value2.Y;

            return (float)Math.Sqrt(x * x + y * y);
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
        public static float DistanceSquared(Vector2 value1, Vector2 value2)
        {
            float x = value1.X - value2.X;
            float y = value1.Y - value2.Y;

            return x * x + y * y;
        }

        /// <summary>
        /// Calculates the dot product of two vectors.
        /// </summary>
        /// <param name="left">First source vector.</param>
        /// <param name="right">Second source vector.</param>
        /// <returns>The dot product of the two vectors.</returns>
        public static float Dot(Vector2 left, Vector2 right)
        {
            return left.X * right.X + left.Y * right.Y;
        }

        /// <summary>
        /// Converts the vector into a unit vector.
        /// </summary>
        /// <param name="vector">The vector to normalize.</param>
        /// <returns>The normalized vector.</returns>
        public static Vector2 Normalize(Vector2 vector)
        {
            vector.Normalize();
            return vector;
        }

        /// <summary>
        /// Converts the vector into a unit vector.
        /// </summary>
        /// <param name="vector">The vector to normalize.</param>
        /// <param name="result">When the method completes, contains the normalized vector.</param>
        public static void Normalize(ref Vector2 vector, out Vector2 result)
        {
            result = vector;
            result.Normalize();
        }


        public static Vector2 Reflect(Vector2 vector, Vector2 normal)
        {
            Vector2 vector2;
            float num = (vector.X * normal.X) + (vector.Y * normal.Y);
            vector2.X = vector.X - ((2f * num) * normal.X);
            vector2.Y = vector.Y - ((2f * num) * normal.Y);
            return vector2;
        }

        public static void Reflect(ref Vector2 vector, ref Vector2 normal, out Vector2 result)
        {
            float num = (vector.X * normal.X) + (vector.Y * normal.Y);
            result.X = vector.X - ((2f * num) * normal.X);
            result.Y = vector.Y - ((2f * num) * normal.Y);
        }

        /// <summary>
        /// Transforms a 2D vector by the given <see cref="Matrix"/>.
        /// </summary>
        /// <param name="vector">The source vector.</param>
        /// <param name="transformation">The transformation <see cref="Matrix"/>.</param>
        /// <returns>The transformed <see cref="Vector4"/>.</returns>
        public static Vector4 Transform(Vector2 vector, Matrix transformation)
        {
            Vector4 result;

            result.X = (vector.X * transformation.M11) + (vector.Y * transformation.M21) + transformation.M41;
            result.Y = (vector.X * transformation.M12) + (vector.Y * transformation.M22) + transformation.M42;
            result.Z = (vector.X * transformation.M13) + (vector.Y * transformation.M23) + transformation.M43;
            result.W = (vector.X * transformation.M14) + (vector.Y * transformation.M24) + transformation.M44;

            return result;
        }

        /// <summary>
        /// Transforms a 2D vector by the given <see cref="Matrix"/>.
        /// </summary>
        /// <param name="vector">The source vector.</param>
        /// <param name="transformation">The transformation <see cref="Matrix"/>.</param>
        /// <param name="result">When the method completes, contains the transformed <see cref="Vector4"/>.</param>
        public static void Transform(ref Vector2 vector, ref  Matrix transformation, out Vector4 result)
        {
            Vector4 r;
            r.X = (vector.X * transformation.M11) + (vector.Y * transformation.M21) + transformation.M41;
            r.Y = (vector.X * transformation.M12) + (vector.Y * transformation.M22) + transformation.M42;
            r.Z = (vector.X * transformation.M13) + (vector.Y * transformation.M23) + transformation.M43;
            r.W = (vector.X * transformation.M14) + (vector.Y * transformation.M24) + transformation.M44;

            result = r;
        }

        /// <summary>
        /// Transforms an array of 2D vectors by the given <see cref="Matrix"/>.
        /// </summary>
        /// <param name="vectors">The source vectors.</param>
        /// <param name="transformation">The transformation <see cref="Matrix"/>.</param>
        /// <returns>The transformed <see cref="Vector4"/>s.</returns>
        public static Vector4[] Transform(Vector2[] vectors, ref Matrix transformation)
        {
            if (vectors == null)
                throw new ArgumentNullException("vectors");

            int count = vectors.Length;
            Vector4[] results = new Vector4[count];

            for (int i = 0; i < count; i++)
            {
                Vector4 r;
                r.X = (vectors[i].X * transformation.M11) + (vectors[i].Y * transformation.M21) + transformation.M41;
                r.Y = (vectors[i].X * transformation.M12) + (vectors[i].Y * transformation.M22) + transformation.M42;
                r.Z = (vectors[i].X * transformation.M13) + (vectors[i].Y * transformation.M23) + transformation.M43;
                r.W = (vectors[i].X * transformation.M14) + (vectors[i].Y * transformation.M24) + transformation.M44;

                results[i] = r;
            }

            return results;
        }

        /// <summary>
        /// Transforms a 2D vector by the given <see cref="Quaternion"/> rotation.
        /// </summary>
        /// <param name="vector">The vector to rotate.</param>
        /// <param name="rotation">The <see cref="Quaternion"/> rotation to apply.</param>
        /// <returns>The transformed <see cref="Vector4"/>.</returns>
        public static Vector4 Transform(Vector2 vector, Quaternion rotation)
        {
            Vector4 temp;
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

            temp.X = ((vector.X * ((1.0f - yy) - zz)) + (vector.Y * (xy - wz)));
            temp.Y = ((vector.X * (xy + wz)) + (vector.Y * ((1.0f - xx) - zz)));
            temp.Z = ((vector.X * (xz - wy)) + (vector.Y * (yz + wx)));
            temp.W = 1.0f;

            return temp;
        }

        /// <summary>
        /// Transforms a 2D vector by the given <see cref="Quaternion"/> rotation.
        /// </summary>
        /// <param name="vector">The vector to rotate.</param>
        /// <param name="rotation">The <see cref="Quaternion"/> rotation to apply.</param>
        /// <param name="result">When the method completes, contains the transformed <see cref="Vector4"/>.</param>
        public static void Transform(ref Vector2 vector, ref Quaternion rotation, out Vector4 result)
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
            r.X = ((vector.X * ((1.0f - yy) - zz)) + (vector.Y * (xy - wz)));
            r.Y = ((vector.X * (xy + wz)) + (vector.Y * ((1.0f - xx) - zz)));
            r.Z = ((vector.X * (xz - wy)) + (vector.Y * (yz + wx)));
            r.W = 1.0f;

            result = r;
        }

        /// <summary>
        /// Transforms an array of 2D vectors by the given <see cref="Quaternion"/> rotation.
        /// </summary>
        /// <param name="vectors">The vectors to rotate.</param>
        /// <param name="rotation">The <see cref="Quaternion"/> rotation to apply.</param>
        /// <returns>The transformed <see cref="Vector4"/>.</returns>
        public static Vector4[] Transform(Vector2[] vectors, ref Quaternion rotation)
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
                r.X = ((vectors[i].X * ((1.0f - yy) - zz)) + (vectors[i].Y * (xy - wz)));
                r.Y = ((vectors[i].X * (xy + wz)) + (vectors[i].Y * ((1.0f - xx) - zz)));
                r.Z = ((vectors[i].X * (xz - wy)) + (vectors[i].Y * (yz + wx)));
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
        public static Vector2 TransformCoordinate(Vector2 coordinate, Matrix transformation)
        {
            Vector4 vector;

            vector.X = (coordinate.X * transformation.M11) + (coordinate.Y * transformation.M21) + transformation.M41;
            vector.Y = (coordinate.X * transformation.M12) + (coordinate.Y * transformation.M22) + transformation.M42;
            vector.Z = (coordinate.X * transformation.M13) + (coordinate.Y * transformation.M23) + transformation.M43;
            vector.W = 1 / ((coordinate.X * transformation.M14) + (coordinate.Y * transformation.M24) + transformation.M44);

            return new Vector2(vector.X * vector.W, vector.Y * vector.W);
        }

        /// <summary>
        /// Performs a coordinate transformation using the given <see cref="Matrix"/>.
        /// </summary>
        /// <param name="coordinate">The coordinate vector to transform.</param>
        /// <param name="transformation">The transformation <see cref="Matrix"/>.</param>
        /// <param name="result">When the method completes, contains the transformed coordinates.</param>
        public static void TransformCoordinate(ref Vector2 coordinate, ref  Matrix transformation, out Vector2 result)
        {
            Vector4 vector;

            vector.X = (coordinate.X * transformation.M11) + (coordinate.Y * transformation.M21) + transformation.M41;
            vector.Y = (coordinate.X * transformation.M12) + (coordinate.Y * transformation.M22) + transformation.M42;
            vector.Z = (coordinate.X * transformation.M13) + (coordinate.Y * transformation.M23) + transformation.M43;
            vector.W = 1 / ((coordinate.X * transformation.M14) + (coordinate.Y * transformation.M24) + transformation.M44);

            result = new Vector2(vector.X * vector.W, vector.Y * vector.W);
        }

        /// <summary>
        /// Performs a coordinate transformation using the given <see cref="Matrix"/>.
        /// </summary>
        /// <param name="coordinates">The coordinate vectors to transform.</param>
        /// <param name="transformation">The transformation <see cref="Matrix"/>.</param>
        /// <returns>The transformed coordinates.</returns>
        public static Vector2[] TransformCoordinate(Vector2[] coordinates, ref Matrix transformation)
        {
            if (coordinates == null)
                throw new ArgumentNullException("coordinates");

            Vector4 vector;
            int count = coordinates.Length;
            Vector2[] results = new Vector2[count];

            for (int i = 0; i < count; i++)
            {
                vector.X = (coordinates[i].X * transformation.M11) + (coordinates[i].Y * transformation.M21) + transformation.M41;
                vector.Y = (coordinates[i].X * transformation.M12) + (coordinates[i].Y * transformation.M22) + transformation.M42;
                vector.Z = (coordinates[i].X * transformation.M13) + (coordinates[i].Y * transformation.M23) + transformation.M43;
                vector.W = 1 / ((coordinates[i].X * transformation.M14) + (coordinates[i].Y * transformation.M24) + transformation.M44);
                results[i] = new Vector2(vector.X * vector.W, vector.Y * vector.W);
            }

            return results;
        }

        /// <summary>
        /// Performs a normal transformation using the given <see cref="Matrix"/>.
        /// </summary>
        /// <param name="normal">The normal vector to transform.</param>
        /// <param name="transformation">The transformation <see cref="Matrix"/>.</param>
        /// <returns>The transformed normal.</returns>
        public static Vector2 TransformNormal(Vector2 normal, Matrix transformation)
        {
            Vector2 vector;

            vector.X = (normal.X * transformation.M11) + (normal.Y * transformation.M21);
            vector.Y = (normal.X * transformation.M12) + (normal.Y * transformation.M22);

            return vector;
        }

        /// <summary>
        /// Performs a normal transformation using the given <see cref="Matrix"/>.
        /// </summary>
        /// <param name="normal">The normal vector to transform.</param>
        /// <param name="transformation">The transformation <see cref="Matrix"/>.</param>
        /// <param name="result">When the method completes, contains the transformed normal.</param>
        public static void TransformNormal(ref Vector2 normal, ref Matrix transformation, out Vector2 result)
        {
            Vector2 r;
            r.X = (normal.X * transformation.M11) + (normal.Y * transformation.M21);
            r.Y = (normal.X * transformation.M12) + (normal.Y * transformation.M22);

            result = r;
        }

        /// <summary>
        /// Performs a normal transformation using the given <see cref="Matrix"/>.
        /// </summary>
        /// <param name="normals">The normal vectors to transform.</param>
        /// <param name="transformation">The transformation <see cref="Matrix"/>.</param>
        /// <returns>The transformed normals.</returns>
        public static Vector2[] TransformNormal(Vector2[] normals, ref Matrix transformation)
        {
            if (normals == null)
                throw new ArgumentNullException("normals");

            int count = normals.Length;
            Vector2[] results = new Vector2[count];

            for (int i = 0; i < count; i++)
            {
                Vector2 r;
                r.X = (normals[i].X * transformation.M11) + (normals[i].Y * transformation.M21);
                r.Y = (normals[i].X * transformation.M12) + (normals[i].Y * transformation.M22);

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
        public static Vector2 Minimize(Vector2 left, Vector2 right)
        {
            Vector2 vector;
            vector.X = (left.X < right.X) ? left.X : right.X;
            vector.Y = (left.Y < right.Y) ? left.Y : right.Y;
            return vector;
        }

        /// <summary>
        /// Returns a vector containing the smallest components of the specified vectors.
        /// </summary>
        /// <param name="left">The first source vector.</param>
        /// <param name="right">The second source vector.</param>
        /// <param name="result">When the method completes, contains an new vector composed of the smallest components of the source vectors.</param>
        public static void Minimize(ref Vector2 left, ref Vector2 right, out Vector2 result)
        {
            Vector2 r;
            r.X = (left.X < right.X) ? left.X : right.X;
            r.Y = (left.Y < right.Y) ? left.Y : right.Y;

            result = r;
        }

        /// <summary>
        /// Returns a vector containing the largest components of the specified vectors.
        /// </summary>
        /// <param name="left">The first source vector.</param>
        /// <param name="right">The second source vector.</param>
        /// <returns>A vector containing the largest components of the source vectors.</returns>
        public static Vector2 Maximize(Vector2 left, Vector2 right)
        {
            Vector2 vector;
            vector.X = (left.X > right.X) ? left.X : right.X;
            vector.Y = (left.Y > right.Y) ? left.Y : right.Y;
            return vector;
        }

        /// <summary>
        /// Returns a vector containing the smallest components of the specified vectors.
        /// </summary>
        /// <param name="left">The first source vector.</param>
        /// <param name="right">The second source vector.</param>
        /// <param name="result">When the method completes, contains an new vector composed of the largest components of the source vectors.</param>
        public static void Maximize(ref Vector2 left, ref Vector2 right, out Vector2 result)
        {
            Vector2 r;
            r.X = (left.X > right.X) ? left.X : right.X;
            r.Y = (left.Y > right.Y) ? left.Y : right.Y;

            result = r;
        }

        /// <summary>
        /// Adds two vectors.
        /// </summary>
        /// <param name="left">The first vector to add.</param>
        /// <param name="right">The second vector to add.</param>
        /// <returns>The sum of the two vectors.</returns>
        public static Vector2 operator +(Vector2 left, Vector2 right)
        {
            return new Vector2(left.X + right.X, left.Y + right.Y);
        }

        /// <summary>
        /// Subtracts two vectors.
        /// </summary>
        /// <param name="left">The first vector to subtract.</param>
        /// <param name="right">The second vector to subtract.</param>
        /// <returns>The difference of the two vectors.</returns>
        public static Vector2 operator -(Vector2 left, Vector2 right)
        {
            return new Vector2(left.X - right.X, left.Y - right.Y);
        }

        /// <summary>
        /// Reverses the direction of a given vector.
        /// </summary>
        /// <param name="value">The vector to negate.</param>
        /// <returns>A vector facing in the opposite direction.</returns>
        public static Vector2 operator -(Vector2 value)
        {
            return new Vector2(-value.X, -value.Y);
        }

        /// <summary>
        /// Scales a vector by the given value.
        /// </summary>
        /// <param name="vector">The vector to scale.</param>
        /// <param name="scale">The amount by which to scale the vector.</param>
        /// <returns>The scaled vector.</returns>
        public static Vector2 operator *(Vector2 vector, float scale)
        {
            return new Vector2(vector.X * scale, vector.Y * scale);
        }

        /// <summary>
        /// Scales a vector by the given value.
        /// </summary>
        /// <param name="vector">The vector to scale.</param>
        /// <param name="scale">The amount by which to scale the vector.</param>
        /// <returns>The scaled vector.</returns>
        public static Vector2 operator *(float scale, Vector2 vector)
        {
            return vector * scale;
        }

        /// <summary>
        /// Scales a vector by the given value.
        /// </summary>
        /// <param name="vector">The vector to scale.</param>
        /// <param name="scale">The amount by which to scale the vector.</param>
        /// <returns>The scaled vector.</returns>
        public static Vector2 operator /(Vector2 vector, float scale)
        {
            return new Vector2(vector.X / scale, vector.Y / scale);
        }

        /// <summary>
        /// Tests for equality between two objects.
        /// </summary>
        /// <param name="left">The first value to compare.</param>
        /// <param name="right">The second value to compare.</param>
        /// <returns><c>true</c> if <paramref name="left"/> has the same value as <paramref name="right"/>; otherwise, <c>false</c>.</returns>
        public static bool operator ==(Vector2 left, Vector2 right)
        {
            return Vector2.Equals(ref left, ref right);
        }

        /// <summary>
        /// Tests for inequality between two objects.
        /// </summary>
        /// <param name="left">The first value to compare.</param>
        /// <param name="right">The second value to compare.</param>
        /// <returns><c>true</c> if <paramref name="left"/> has a different value than <paramref name="right"/>; otherwise, <c>false</c>.</returns>
        public static bool operator !=(Vector2 left, Vector2 right)
        {
            return !Vector2.Equals(ref left, ref right);
        }

        /// <summary>
        /// Converts the value of the object to its equivalent string representation.
        /// </summary>
        /// <returns>The string representation of the value of this instance.</returns>
        public override string ToString()
        {
            return string.Format(CultureInfo.CurrentCulture, 
                "X:{0} Y:{1}", X.ToString(CultureInfo.CurrentCulture), Y.ToString(CultureInfo.CurrentCulture));
        }

        /// <summary>
        /// Returns the hash code for this instance.
        /// </summary>
        /// <returns>A 32-bit signed integer hash code.</returns>
        public override int GetHashCode()
        {
            return X.GetHashCode() + Y.GetHashCode();
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

            return Equals((Vector2)obj);
        }

        /// <summary>
        /// Returns a value that indicates whether the current instance is equal to the specified object. 
        /// </summary>
        /// <param name="other">Object to make the comparison with.</param>
        /// <returns><c>true</c> if the current instance is equal to the specified object; <c>false</c> otherwise.</returns>
        public bool Equals(Vector2 other)
        {
            return (X == other.X && Y == other.Y);
        }

        /// <summary>
        /// Determines whether the specified object instances are considered equal. 
        /// </summary>
        /// <param name="value1">The first value to compare.</param>
        /// <param name="value2">The second value to compare.</param>
        /// <returns><c>true</c> if <paramref name="value1"/> is the same instance as <paramref name="value2"/> or 
        /// if both are <c>null</c> references or if <c>value1.Equals(value2)</c> returns <c>true</c>; otherwise, <c>false</c>.</returns>
        public static bool Equals(ref Vector2 value1, ref Vector2 value2)
        {
            return (value1.X == value2.X && value1.Y == value2.Y);
        }
    }
}
