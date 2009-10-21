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
    /// Defines a four dimensional mathematical quaternion.
    /// </summary>
    [Serializable, StructLayout(LayoutKind.Sequential), TypeConverter(typeof(QuaternionConverter))]
    public struct Quaternion : IEquatable<Quaternion>
    {
        /// <summary>
        /// Gets or sets the X component of the quaternion.
        /// </summary>
        /// <value>The X component of the quaternion.</value>
        public float X;

        /// <summary>
        /// Gets or sets the Y component of the quaternion.
        /// </summary>
        /// <value>The Y component of the quaternion.</value>
        public float Y;

        /// <summary>
        /// Gets or sets the Z component of the quaternion.
        /// </summary>
        /// <value>The Z component of the quaternion.</value>
        public float Z;

        /// <summary>
        /// Gets or sets the W component of the quaternion.
        /// </summary>
        /// <value>The W component of the quaternion.</value>
        public float W;

        /// <summary>
        /// Initializes a new instance of the <see cref="Quaternion"/> structure.
        /// </summary>
        /// <param name="x">The X component of the quaternion.</param>
        /// <param name="y">The Y component of the quaternion.</param>
        /// <param name="z">The Z component of the quaternion.</param>
        /// <param name="w">The W component of the quaternion.</param>
        public Quaternion(float x, float y, float z, float w)
        {
            X = x;
            Y = y;
            Z = z;
            W = w;
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="Quaternion"/> structure.
        /// </summary>
        /// <param name="value">A <see cref="Vector3"/> containing the first three values of the quaternion.</param>
        /// <param name="w">The W component of the quaternion.</param>
        public Quaternion(Vector3 value, float w)
        {
            X = value.X;
            Y = value.Y;
            Z = value.Z;
            W = w;
        }

        /// <summary>
        /// Gets the identity <see cref="Quaternion"/> (0, 0, 0, 1).
        /// </summary>
        public static Quaternion Identity
        {
            get
            {
                Quaternion result;
                result.X = 0.0f;
                result.Y = 0.0f;
                result.Z = 0.0f;
                result.W = 1.0f;
                return result;
            }
        }

        /// <summary>
        /// Gets a value indicating whether this instance is an identity <see cref="Quaternion"/>.
        /// </summary>
        [Browsable(false)]
        public bool IsIdentity
        {
            get
            {
                if (X != 0.0f || Y != 0.0f || Z != 0.0f)
                    return false;

                return (W == 1.0f);
            }
        }

        /// <summary>
        /// Gets the axis components of the quaternion.
        /// </summary>
        public Vector3 Axis
        {
            get 
            {
                float len = LengthSquared();
                if (len > float.Epsilon)
                {
                    return new Vector3(X, Y, Z);
                }
                return Vector3.Zero;
            }
        }

        /// <summary>
        /// Gets the angle of the quaternion.
        /// </summary>
        public float Angle
        {
            get
            {
                float len = LengthSquared();
                if (len > float.Epsilon)
                {
                    return (float)(2 * Math.Acos(W));
                }
                return 0;
            }
        }


        public void GetAxisAngle(out Vector3 axis, out float angle)
        {
            float len = LengthSquared();

            angle = 0;
            if (len > float.Epsilon)
            {
                axis.X = X;
                axis.Y = Y;
                axis.Z = Z;

                if (Math.Abs(W) < 1)
                {
                    angle = (float)(2 * Math.Acos(W));
                }
            }
            else
            {
                axis.X = 0;
                axis.Y = 0;
                axis.Z = 0;
            }
        }

        /// <summary>
        /// Calculates the length of the quaternion.
        /// </summary>
        /// <returns>The length of the quaternion.</returns>
        public float Length() 
        {
            return (float)Math.Sqrt(X * X + Y * Y + Z * Z + W * W); 
        }

        /// <summary>
        /// Calculates the squared length of the quaternion.
        /// </summary>
        /// <returns>The squared length of the quaternion.</returns>
        public float LengthSquared()
        {
            return X * X + Y * Y + Z * Z + W * W;
        }

        /// <summary>
        /// Converts the quaternion into a unit quaternion.
        /// </summary>
        public void Normalize()
        {
            {
                float length = 1.0f / Length();
                X *= length;
                Y *= length;
                Z *= length;
                W *= length;
            }
        }

        /// <summary>
        /// Conjugates the quaternion.
        /// </summary>
        public void Conjugate()
        {
            X = -X;
            Y = -Y;
            Z = -Z;
        }

        /// <summary>
        /// Conjugates and renormalizes the quaternion.
        /// </summary>
        public void Invert()
        {
            float lengthSq = 1.0f / (X * X + Y * Y + Z * Z + W * W);
            X = -X * lengthSq;
            Y = -Y * lengthSq;
            Z = -Z * lengthSq;
            W = W * lengthSq;
        }

        /// <summary>
        /// Adds two quaternions.
        /// </summary>
        /// <param name="left">The first quaternion to add.</param>
        /// <param name="right">The second quaternion to add.</param>
        /// <returns>The sum of the two quaternions.</returns>
        public static Quaternion Add(Quaternion left, Quaternion right)
        {
            Quaternion result;
            result.X = left.X + right.X;
            result.Y = left.Y + right.Y;
            result.Z = left.Z + right.Z;
            result.W = left.W + right.W;
            return result;
        }


        /// <summary>
        /// Adds two quaternions.
        /// </summary>
        /// <param name="left">The first quaternion to add.</param>
        /// <param name="right">The second quaternion to add.</param>
        /// <param name="result">When the method completes, contains the sum of the two quaternions.</param>
        public static void Add(ref Quaternion left, ref Quaternion right, out Quaternion result)
        {
            Quaternion r;
            r.X = left.X + right.X;
            r.Y = left.Y + right.Y;
            r.Z = left.Z + right.Z;
            r.W = left.W + right.W;

            result = r;
        }

#warning optimize
        /// <summary>
        /// Returns a <see cref="Quaternion"/> containing the 4D Cartesian coordinates of a point specified in Barycentric coordinates relative to a 2D triangle.
        /// </summary>
        /// <param name="source1">A <see cref="Quaternion"/> containing the 4D Cartesian coordinates of vertex 1 of the triangle.</param>
        /// <param name="source2">A <see cref="Quaternion"/> containing the 4D Cartesian coordinates of vertex 2 of the triangle.</param>
        /// <param name="source3">A <see cref="Quaternion"/> containing the 4D Cartesian coordinates of vertex 3 of the triangle.</param>
        /// <param name="weight1">Barycentric coordinate b2, which expresses the weighting factor toward vertex 2 (specified in <paramref name="source2"/>).</param>
        /// <param name="weight2">Barycentric coordinate b3, which expresses the weighting factor toward vertex 3 (specified in <paramref name="source3"/>).</param>
        /// <returns>A new <see cref="Quaternion"/> containing the 4D Cartesian coordinates of the specified point.</returns>
        public static Quaternion Barycentric(Quaternion source1, Quaternion source2, Quaternion source3, float weight1, float weight2)
        {
            return Slerp(
                Slerp(source1, source2, weight1 + weight2),
                Slerp(source1, source3, weight1 + weight2), 
                weight2 / (weight1 + weight2));
        }

#warning optimize
        /// <summary>
        /// Returns a <see cref="Quaternion"/> containing the 4D Cartesian coordinates of a point specified in Barycentric coordinates relative to a 2D triangle.
        /// </summary>
        /// <param name="source1">A <see cref="Quaternion"/> containing the 4D Cartesian coordinates of vertex 1 of the triangle.</param>
        /// <param name="source2">A <see cref="Quaternion"/> containing the 4D Cartesian coordinates of vertex 2 of the triangle.</param>
        /// <param name="source3">A <see cref="Quaternion"/> containing the 4D Cartesian coordinates of vertex 3 of the triangle.</param>
        /// <param name="weight1">Barycentric coordinate b2, which expresses the weighting factor toward vertex 2 (specified in <paramref name="source2"/>).</param>
        /// <param name="weight2">Barycentric coordinate b3, which expresses the weighting factor toward vertex 3 (specified in <paramref name="source3"/>).</param>
        /// <param name="result">When the method completes, contains a new <see cref="Quaternion"/> containing the 4D Cartesian coordinates of the specified point.</param>
        public static void Barycentric(ref Quaternion source1, ref Quaternion source2, ref Quaternion source3, float weight1, float weight2, out Quaternion result) 
        {
            result = Slerp(
                Slerp(source1, source2, weight1 + weight2),
                Slerp(source1, source3, weight1 + weight2),
                weight2 / (weight1 + weight2));
        }

        /// <summary>
        /// Concatenates two quaternions.
        /// </summary>
        /// <param name="left">The first quaternion to concatenate.</param>
        /// <param name="right">The second quaternion to concatenate.</param>
        /// <returns>The concatentated quaternion.</returns>
        public static Quaternion Concatenate(Quaternion left, Quaternion right)
        {
            Quaternion quaternion;
            float rx = right.X;
            float ry = right.Y;
            float rz = right.Z;
            float rw = right.W;
            float lx = left.X;
            float ly = left.Y;
            float lz = left.Z;
            float lw = left.W;
            float yz = (ry * lz) - (rz * ly);
            float xz = (rz * lx) - (rx * lz);
            float xy = (rx * ly) - (ry * lx);
            float lengthSq = ((rx * lx) + (ry * ly)) + (rz * lz);

            quaternion.X = ((rx * lw) + (lx * rw)) + yz;
            quaternion.Y = ((ry * lw) + (ly * rw)) + xz;
            quaternion.Z = ((rz * lw) + (lz * rw)) + xy;
            quaternion.W = (rw * lw) - lengthSq;

            return quaternion;
        }

        /// <summary>
        /// Concatenates two quaternions.
        /// </summary>
        /// <param name="left">The first quaternion to concatenate.</param>
        /// <param name="right">The second quaternion to concatenate.</param>
        /// <param name="result">When the method completes, contains the concatentated quaternion.</param>
        public static void Concatenate(ref Quaternion left, ref Quaternion right, out Quaternion result)
        {
            float rx = right.X;
            float ry = right.Y;
            float rz = right.Z;
            float rw = right.W;
            float lx = left.X;
            float ly = left.Y;
            float lz = left.Z;
            float lw = left.W;
            float yz = (ry * lz) - (rz * ly);
            float xz = (rz * lx) - (rx * lz);
            float xy = (rx * ly) - (ry * lx);
            float lengthSq = ((rx * lx) + (ry * ly)) + (rz * lz);

            Quaternion r;
            r.X = ((rx * lw) + (lx * rw)) + yz;
            r.Y = ((ry * lw) + (ly * rw)) + xz;
            r.Z = ((rz * lw) + (lz * rw)) + xy;
            r.W = (rw * lw) - lengthSq;

            result = r;
        }

        /// <summary>
        /// Conjugates a quaternion.
        /// </summary>
        /// <param name="quaternion">The quaternion to conjugate.</param>
        /// <returns>The conjugated quaternion.</returns>
        public static Quaternion Conjugate(Quaternion quaternion)
        {
            Quaternion result;
            result.X = -quaternion.X;
            result.Y = -quaternion.Y;
            result.Z = -quaternion.Z;
            result.W = quaternion.W;
            return result;
        }

        /// <summary>
        /// Conjugates a quaternion.
        /// </summary>
        /// <param name="quaternion">The quaternion to conjugate.</param>
        /// <param name="result">When the method completes, contains the conjugated quaternion.</param>
        public static void Conjugate(ref Quaternion quaternion, out Quaternion result)
        {
            result.X = -quaternion.X;
            result.Y = -quaternion.Y;
            result.Z = -quaternion.Z;
            result.W = quaternion.W;
        }

        /// <summary>
        /// Divides a quaternion by another.
        /// </summary>
        /// <param name="left">The first quaternion to divide.</param>
        /// <param name="right">The second quaternion to divide.</param>
        /// <returns>The divided quaternion.</returns>
        public static Quaternion Divide(Quaternion left, Quaternion right)
        {
            Quaternion result;
            result.X = left.X / right.X;
            result.Y = left.Y / right.Y;
            result.Z = left.Z / right.Z;
            result.W = left.W / right.W;
            return result;
        }

        /// <summary>
        /// Divides a quaternion by another.
        /// </summary>
        /// <param name="left">The first quaternion to divide.</param>
        /// <param name="right">The second quaternion to divide.</param>
        /// <returns>The divided quaternion.</returns>
        public static void Divide(ref Quaternion left, ref Quaternion right, out Quaternion result)
        {
            result.X = left.X / right.X;
            result.Y = left.Y / right.Y;
            result.Z = left.Z / right.Z;
            result.W = left.W / right.W;
        }

        /// <summary>
        /// Calculates the dot product of two quaternions.
        /// </summary>
        /// <param name="left">First source quaternion.</param>
        /// <param name="right">Second source quaternion.</param>
        /// <returns>The dot product of the two quaternions.</returns>
        public static float Dot(Quaternion left, Quaternion right)
        {
            return (left.X * right.X) + (left.Y * right.Y) + (left.Z * right.Z) + (left.W * right.W);
        }

        /// <summary>
        /// Exponentiates a quaternion.
        /// </summary>
        /// <param name="quaternion">The quaternion to exponentiate.</param>
        /// <returns>The exponentiated quaternion.</returns>
        public static Quaternion Exponential(Quaternion quaternion) 
        {
            throw new NotImplementedException(); 
        }

        /// <summary>
        /// Exponentiates a quaternion.
        /// </summary>
        /// <param name="quaternion">The quaternion to exponentiate.</param>
        /// <param name="result">When the method completes, contains the exponentiated quaternion.</param>
        public static void Exponential(ref Quaternion quaternion, out Quaternion result) 
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Conjugates and renormalizes the quaternion.
        /// </summary>
        /// <param name="quaternion">The quaternion to conjugate and renormalize.</param>
        /// <returns>The conjugated and renormalized quaternion.</returns>
        public static Quaternion Invert(Quaternion quaternion)
        {
            Quaternion result;
            float lengthSq = 1.0f / ((quaternion.X * quaternion.X) + (quaternion.Y * quaternion.Y) + (quaternion.Z * quaternion.Z) + (quaternion.W * quaternion.W));

            result.X = -quaternion.X * lengthSq;
            result.Y = -quaternion.Y * lengthSq;
            result.Z = -quaternion.Z * lengthSq;
            result.W = quaternion.W * lengthSq;

            return result;
        }

        /// <summary>
        /// Conjugates and renormalizes the quaternion.
        /// </summary>
        /// <param name="quaternion">The quaternion to conjugate and renormalize.</param>
        /// <param name="result">When the method completes, contains the conjugated and renormalized quaternion.</param>
        public static void Invert(ref Quaternion quaternion, out Quaternion result)
        {
            float lengthSq = 1.0f / ((quaternion.X * quaternion.X) + (quaternion.Y * quaternion.Y) + (quaternion.Z * quaternion.Z) + (quaternion.W * quaternion.W));

            result.X = -quaternion.X * lengthSq;
            result.Y = -quaternion.Y * lengthSq;
            result.Z = -quaternion.Z * lengthSq;
            result.W = quaternion.W * lengthSq;
        }

        /// <summary>
        /// Performs a linear interpolation between two quaternion.
        /// </summary>
        /// <param name="start">Start quaternion.</param>
        /// <param name="end">End quaternion.</param>
        /// <param name="amount">Value between 0 and 1 indicating the weight of <paramref name="end"/>.</param>
        /// <returns>The linear interpolation of the two quaternions.</returns>
        /// <remarks>
        /// This method performs the linear interpolation based on the following formula.
        /// <code>start + (end - start) * amount</code>
        /// Passing <paramref name="amount"/> a value of 0 will cause <paramref name="start"/> to be returned; a value of 1 will cause <paramref name="end"/> to be returned. 
        /// </remarks>
        public static Quaternion Lerp(Quaternion start, Quaternion end, float amount)
        {
            Quaternion result;
            float inverse = 1.0f - amount;
            float dot = (start.X * end.X) + (start.Y * end.Y) + (start.Z * end.Z) + (start.W * end.W);

            if (dot >= 0.0f)
            {
                result.X = (inverse * start.X) + (amount * end.X);
                result.Y = (inverse * start.Y) + (amount * end.Y);
                result.Z = (inverse * start.Z) + (amount * end.Z);
                result.W = (inverse * start.W) + (amount * end.W);
            }
            else
            {
                result.X = (inverse * start.X) - (amount * end.X);
                result.Y = (inverse * start.Y) - (amount * end.Y);
                result.Z = (inverse * start.Z) - (amount * end.Z);
                result.W = (inverse * start.W) - (amount * end.W);
            }

            float invLength = 1.0f / result.Length();

            result.X *= invLength;
            result.Y *= invLength;
            result.Z *= invLength;
            result.W *= invLength;

            return result;
        }

        /// <summary>
        /// Performs a linear interpolation between two quaternions.
        /// </summary>
        /// <param name="start">Start quaternion.</param>
        /// <param name="end">End quaternion.</param>
        /// <param name="amount">Value between 0 and 1 indicating the weight of <paramref name="end"/>.</param>
        /// <param name="result">When the method completes, contains the linear interpolation of the two quaternions.</param>
        /// <remarks>
        /// This method performs the linear interpolation based on the following formula.
        /// <code>start + (end - start) * amount</code>
        /// Passing <paramref name="amount"/> a value of 0 will cause <paramref name="start"/> to be returned; a value of 1 will cause <paramref name="end"/> to be returned. 
        /// </remarks>
        public static void Lerp(ref Quaternion start, ref Quaternion end, float amount, out Quaternion result)
        {
            float inverse = 1.0f - amount;
            float dot = (start.X * end.X) + (start.Y * end.Y) + (start.Z * end.Z) + (start.W * end.W);

            if (dot >= 0.0f)
            {
                result.X = (inverse * start.X) + (amount * end.X);
                result.Y = (inverse * start.Y) + (amount * end.Y);
                result.Z = (inverse * start.Z) + (amount * end.Z);
                result.W = (inverse * start.W) + (amount * end.W);
            }
            else
            {
                result.X = (inverse * start.X) - (amount * end.X);
                result.Y = (inverse * start.Y) - (amount * end.Y);
                result.Z = (inverse * start.Z) - (amount * end.Z);
                result.W = (inverse * start.W) - (amount * end.W);
            }

            float invLength = 1.0f / result.Length();

            result.X *= invLength;
            result.Y *= invLength;
            result.Z *= invLength;
            result.W *= invLength;
        }

        /// <summary>
        /// Calculates the natural logarithm of the specified quaternion.
        /// </summary>
        /// <param name="quaternion">The quaternion whose logarithm will be calculated.</param>
        /// <returns>The natural logarithm of the quaternion.</returns>
        public static Quaternion Logarithm(Quaternion quaternion) 
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Calculates the natural logarithm of the specified quaternion.
        /// </summary>
        /// <param name="quaternion">The quaternion whose logarithm will be calculated.</param>
        /// <param name="result">When the method completes, contains the natural logarithm of the quaternion.</param>
        public static void Logarithm(ref Quaternion quaternion, out Quaternion result)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Modulates a quaternion by another.
        /// </summary>
        /// <param name="left">The first quaternion to modulate.</param>
        /// <param name="right">The second quaternion to modulate.</param>
        /// <returns>The modulated quaternion.</returns>
        public static Quaternion Multiply(Quaternion left, Quaternion right)
        {
            Quaternion quaternion;
            float rx = right.X;
            float ry = right.Y;
            float rz = right.Z;
            float rw = right.W;
            float lx = left.X;
            float ly = left.Y;
            float lz = left.Z;
            float lw = left.W;
            float yz = (ry * lz) - (rz * ly);
            float xz = (rz * lx) - (rx * lz);
            float xy = (rx * ly) - (ry * lx);
            float lengthSq = ((rx * lx) + (ry * ly)) + (rz * lz);

            quaternion.X = ((rx * lw) + (lx * rw)) + yz;
            quaternion.Y = ((ry * lw) + (ly * rw)) + xz;
            quaternion.Z = ((rz * lw) + (lz * rw)) + xy;
            quaternion.W = (rw * lw) - lengthSq;

            return quaternion;
        }

        /// <summary>
        /// Modulates a quaternion by another.
        /// </summary>
        /// <param name="left">The first quaternion to modulate.</param>
        /// <param name="right">The second quaternion to modulate.</param>
        /// <param name="result">When the moethod completes, contains the modulated quaternion.</param>
        public static void Multiply(ref Quaternion left, ref Quaternion right, out Quaternion result)
        {
            float rx = right.X;
            float ry = right.Y;
            float rz = right.Z;
            float rw = right.W;
            float lx = left.X;
            float ly = left.Y;
            float lz = left.Z;
            float lw = left.W;
            float yz = (ry * lz) - (rz * ly);
            float xz = (rz * lx) - (rx * lz);
            float xy = (rx * ly) - (ry * lx);
            float lengthSq = ((rx * lx) + (ry * ly)) + (rz * lz);

            result.X = ((rx * lw) + (lx * rw)) + yz;
            result.Y = ((ry * lw) + (ly * rw)) + xz;
            result.Z = ((rz * lw) + (lz * rw)) + xy;
            result.W = (rw * lw) - lengthSq;
        }

        /// <summary>
        /// Scales a quaternion by the given value.
        /// </summary>
        /// <param name="quaternion">The quaternion to scale.</param>
        /// <param name="scale">The amount by which to scale the quaternion.</param>
        /// <returns>The scaled quaternion.</returns>
        public static Quaternion Multiply(Quaternion quaternion, float scale)
        {
            Quaternion result;
            result.X = quaternion.X * scale;
            result.Y = quaternion.Y * scale;
            result.Z = quaternion.Z * scale;
            result.W = quaternion.W * scale;
            return result;
        }

        /// <summary>
        /// Scales a quaternion by the given value.
        /// </summary>
        /// <param name="quaternion">The quaternion to scale.</param>
        /// <param name="scale">The amount by which to scale the quaternion.</param>
        /// <param name="result">When the method completes, contains the scaled quaternion.</param>
        public static void Multiply(ref Quaternion quaternion, float scale, out Quaternion result)
        {
            result.X = quaternion.X * scale;
            result.Y = quaternion.Y * scale;
            result.Z = quaternion.Z * scale;
            result.W = quaternion.W * scale;
        }

        /// <summary>
        /// Reverses the direction of a given quaternion.
        /// </summary>
        /// <param name="quaternion">The quaternion to negate.</param>
        /// <returns>A quaternion facing in the opposite direction.</returns>
        public static Quaternion Negate(Quaternion quaternion)
        {
            Quaternion result;
            result.X = -quaternion.X;
            result.Y = -quaternion.Y;
            result.Z = -quaternion.Z;
            result.W = -quaternion.W;
            return result;
        }

        /// <summary>
        /// Reverses the direction of a given quaternion.
        /// </summary>
        /// <param name="quaternion">The quaternion to negate.</param>
        /// <param name="result">When the method completes, contains a quaternion facing in the opposite direction.</param>
        public static void Negate(ref Quaternion quaternion, out Quaternion result)
        {
            result.X = -quaternion.X;
            result.Y = -quaternion.Y;
            result.Z = -quaternion.Z;
            result.W = -quaternion.W;
        }

        /// <summary>
        /// Converts the quaternion into a unit quaternion.
        /// </summary>
        /// <param name="quaternion">The quaternion to normalize.</param>
        /// <returns>The normalized quaternion.</returns>
        public static Quaternion Normalize(Quaternion quaternion)
        {
            quaternion.Normalize();
            return quaternion;
        }

        /// <summary>
        /// Converts the quaternion into a unit quaternion.
        /// </summary>
        /// <param name="quaternion">The quaternion to normalize.</param>
        /// <param name="result">When the method completes, contains the normalized quaternion.</param>
        public static void Normalize(ref Quaternion quaternion, out Quaternion result)
        {
            float length = 1.0f / quaternion.Length();
            result.X = quaternion.X * length;
            result.Y = quaternion.Y * length;
            result.Z = quaternion.Z * length;
            result.W = quaternion.W * length;
        }

        /// <summary>
        /// Creates a quaternion given a rotation and an axis.
        /// </summary>
        /// <param name="axis">The axis of rotation.</param>
        /// <param name="angle">The angle of rotation.</param>
        /// <returns>The newly created quaternion.</returns>
        public static Quaternion RotationAxis(Vector3 axis, float angle)
        {
            Quaternion result;

            Vector3.Normalize(ref axis, out axis);

            float half = angle * 0.5f;
            float sin = (float)Math.Sin(half);
            float cos = (float)Math.Cos(half);

            result.X = axis.X * sin;
            result.Y = axis.Y * sin;
            result.Z = axis.Z * sin;
            result.W = cos;

            return result;
        }

        /// <summary>
        /// Creates a quaternion given a rotation and an axis.
        /// </summary>
        /// <param name="axis">The axis of rotation.</param>
        /// <param name="angle">The angle of rotation.</param>
        /// <param name="result">When the method completes, contains the newly created quaternion.</param>
        public static void RotationAxis(ref Vector3 axis, float angle, out Quaternion result)
        {
            Vector3.Normalize(ref axis, out axis);

            float half = angle * 0.5f;
            float sin = (float)Math.Sin(half);
            float cos = (float)Math.Cos(half);

            result.X = axis.X * sin;
            result.Y = axis.Y * sin;
            result.Z = axis.Z * sin;
            result.W = cos;
        }

        /// <summary>
        /// Creates a quaternion given a rotation matrix.
        /// </summary>
        /// <param name="matrix">The rotation matrix.</param>
        /// <returns>The newly created quaternion.</returns>
        public static Quaternion RotationMatrix(Matrix matrix)
        {
            Quaternion result;
            float scale = matrix.M11 + matrix.M22 + matrix.M33;
            float sqrt;
            float half;

            if (scale > 0.0f)
            {
                sqrt = (float)Math.Sqrt(scale + 1);

                result.W = sqrt * 0.5f;
                sqrt = 0.5f / sqrt;

                result.X = (matrix.M23 - matrix.M32) * sqrt;
                result.Y = (matrix.M31 - matrix.M13) * sqrt;
                result.Z = (matrix.M12 - matrix.M21) * sqrt;

                return result;
            }

            if ((matrix.M11 >= matrix.M22) && (matrix.M11 >= matrix.M33))
            {
                sqrt = (float)Math.Sqrt(1.0f + matrix.M11 - matrix.M22 - matrix.M33);
                half = 0.5f / sqrt;

                result.X = 0.5f * sqrt;
                result.Y = (matrix.M12 + matrix.M21) * half;
                result.Z = (matrix.M13 + matrix.M31) * half;
                result.W = (matrix.M23 - matrix.M32) * half;

                return result;
            }

            if (matrix.M22 > matrix.M33)
            {
                sqrt = (float)Math.Sqrt(1.0 + matrix.M22 - matrix.M11 - matrix.M33);
                half = 0.5f / sqrt;

                result.X = (matrix.M21 + matrix.M12) * half;
                result.Y = 0.5f * sqrt;
                result.Z = (matrix.M32 + matrix.M23) * half;
                result.W = (matrix.M31 - matrix.M13) * half;

                return result;
            }

            sqrt = (float)Math.Sqrt(1 + matrix.M33 - matrix.M11 - matrix.M22);
            half = 0.5f / sqrt;

            result.X = (matrix.M31 + matrix.M13) * half;
            result.Y = (matrix.M32 + matrix.M23) * half;
            result.Z = 0.5f * sqrt;
            result.W = (matrix.M12 - matrix.M21) * half;

            return result;
        }

        /// <summary>
        /// Creates a quaternion given a rotation matrix.
        /// </summary>
        /// <param name="matrix">The rotation matrix.</param>
        /// <param name="result">When the method completes, contains the newly created quaternion.</param>
        public static void RotationMatrix(ref Matrix matrix, out Quaternion result)
        {
            float scale = matrix.M11 + matrix.M22 + matrix.M33;
            float sqrt; 
            float half;

            if (scale > 0.0f)
            {
                sqrt = (float)Math.Sqrt(scale + 1);

                result.W = sqrt * 0.5f;
                sqrt = 0.5f / sqrt;

                result.X = (matrix.M23 - matrix.M32) * sqrt;
                result.Y = (matrix.M31 - matrix.M13) * sqrt;
                result.Z = (matrix.M12 - matrix.M21) * sqrt;
                return;
            }

            if ((matrix.M11 >= matrix.M22) && (matrix.M11 >= matrix.M33))
            {
                sqrt = (float)Math.Sqrt(1 + matrix.M11 - matrix.M22 - matrix.M33);
                half = 0.5f / sqrt;

                result.X = 0.5f * sqrt;
                result.Y = (matrix.M12 + matrix.M21) * half;
                result.Z = (matrix.M13 + matrix.M31) * half;
                result.W = (matrix.M23 - matrix.M32) * half;
                return;
            }

            if (matrix.M22 > matrix.M33)
            {
                sqrt = (float)Math.Sqrt(1 + matrix.M22 - matrix.M11 - matrix.M33);
                half = 0.5f / sqrt;

                result.X = (matrix.M21 + matrix.M12) * half;
                result.Y = 0.5f * sqrt;
                result.Z = (matrix.M32 + matrix.M23) * half;
                result.W = (matrix.M31 - matrix.M13) * half;
                return;
            }

            sqrt = (float)Math.Sqrt(1 + matrix.M33 - matrix.M11 - matrix.M22);
            half = 0.5f / sqrt;

            result.X = (matrix.M31 + matrix.M13) * half;
            result.Y = (matrix.M32 + matrix.M23) * half;
            result.Z = 0.5f * sqrt;
            result.W = (matrix.M12 - matrix.M21) * half;
        }

        /// <summary>
        /// Creates a quaternion given a yaw, pitch, and roll value.
        /// </summary>
        /// <param name="yaw">The yaw of rotation.</param>
        /// <param name="pitch">The pitch of rotation.</param>
        /// <param name="roll">The roll of rotation.</param>
        /// <returns>The newly created quaternion.</returns>
        public static Quaternion RotationYawPitchRoll(float yaw, float pitch, float roll)
        {
            Quaternion result;

            float halfRoll = roll * 0.5f;
            float sinRoll = (float)Math.Sin(halfRoll);
            float cosRoll = (float)Math.Cos(halfRoll);
            float halfPitch = pitch * 0.5f;
            float sinPitch = (float)Math.Sin(halfPitch);
            float cosPitch = (float)Math.Cos(halfPitch);
            float halfYaw = yaw * 0.5f;
            float sinYaw = (float)Math.Sin(halfYaw);
            float cosYaw = (float)Math.Cos(halfYaw);

            result.X = cosYaw * sinPitch * cosRoll + sinYaw * cosPitch * sinRoll;
            result.Y = sinYaw * cosPitch * cosRoll - cosYaw * sinPitch * sinRoll;
            result.Z = cosYaw * cosPitch * sinRoll - sinYaw * sinPitch * cosRoll;
            result.W = cosYaw * cosPitch * cosRoll + sinYaw * sinPitch * sinRoll;

            return result;
        }

        /// <summary>
        /// Creates a quaternion given a yaw, pitch, and roll value.
        /// </summary>
        /// <param name="yaw">The yaw of rotation.</param>
        /// <param name="pitch">The pitch of rotation.</param>
        /// <param name="roll">The roll of rotation.</param>
        /// <param name="result">When the method completes, contains the newly created quaternion.</param>
        public static void RotationYawPitchRoll(float yaw, float pitch, float roll, out Quaternion result)
        {
            float halfRoll = roll * 0.5f;
            float sinRoll = (float)Math.Sin(halfRoll);
            float cosRoll = (float)Math.Cos(halfRoll);
            float halfPitch = pitch * 0.5f;
            float sinPitch = (float)Math.Sin(halfPitch);
            float cosPitch = (float)Math.Cos(halfPitch);
            float halfYaw = yaw * 0.5f;
            float sinYaw = (float)Math.Sin(halfYaw);
            float cosYaw = (float)Math.Cos(halfYaw);

            result.X = cosYaw * sinPitch * cosRoll + sinYaw * cosPitch * sinRoll;
            result.Y = sinYaw * cosPitch * cosRoll - cosYaw * sinPitch * sinRoll;
            result.Z = cosYaw * cosPitch * sinRoll - sinYaw * sinPitch * cosRoll;
            result.W = cosYaw * cosPitch * cosRoll + sinYaw * sinPitch * sinRoll;

        }

        /// <summary>
        /// Interpolates between two quaternions, using spherical linear interpolation.
        /// </summary>
        /// <param name="start">Start quaternion.</param>
        /// <param name="end">End quaternion.</param>
        /// <param name="amount">Value between 0 and 1 indicating the weight of <paramref name="end"/>.</param>
        /// <returns>The spherical linear interpolation of the two quaternions.</returns>
        public static Quaternion Slerp(Quaternion start, Quaternion end, float amount)
        {
            Quaternion result;

            float opposite;
            float inverse;
            float dot = (start.X * end.X) + (start.Y * end.Y) + (start.Z * end.Z) + (start.W * end.W);
            bool flag = false;

            if (dot < 0.0f)
            {
                flag = true;
                dot = -dot;
            }

            if (dot > 0.999999f)
            {
                inverse = 1.0f - amount;
                opposite = flag ? -amount : amount;
            }
            else
            {
                float acos = (float)Math.Acos(dot);
                float invSin = (float)(1.0f / Math.Sin(acos));

                inverse = (float)Math.Sin((1.0f - amount) * acos) * invSin;
                opposite = flag ? -(float)Math.Sin(amount * acos) * invSin : (float)Math.Sin(amount * acos) * invSin;
            }

            result.X = (inverse * start.X) + (opposite * end.X);
            result.Y = (inverse * start.Y) + (opposite * end.Y);
            result.Z = (inverse * start.Z) + (opposite * end.Z);
            result.W = (inverse * start.W) + (opposite * end.W);

            return result;
        }

        /// <summary>
        /// Interpolates between two quaternions, using spherical linear interpolation.
        /// </summary>
        /// <param name="start">Start quaternion.</param>
        /// <param name="end">End quaternion.</param>
        /// <param name="amount">Value between 0 and 1 indicating the weight of <paramref name="end"/>.</param>
        /// <param name="result">When the method completes, contains the spherical linear interpolation of the two quaternions.</param>
        public static void Slerp(ref Quaternion start, ref Quaternion end, float amount, out Quaternion result)
        {
            float opposite;
            float inverse;
            float dot = (start.X * end.X) + (start.Y * end.Y) + (start.Z * end.Z) + (start.W * end.W);
            bool flag = false;

            if (dot < 0.0f)
            {
                flag = true;
                dot = -dot;
            }

            if (dot > 0.999999f)
            {
                inverse = 1.0f - amount;
                opposite = flag ? -amount : amount;
            }
            else
            {
                float acos = (float)Math.Acos(dot);
                float invSin = (float)(1.0f / Math.Sin(acos));

                inverse = (float)Math.Sin((1.0f - amount) * acos) * invSin;
                opposite = flag ? -(float)Math.Sin(amount * acos) * invSin : (float)Math.Sin(amount * acos) * invSin;
            }

            result.X = (inverse * start.X) + (opposite * end.X);
            result.Y = (inverse * start.Y) + (opposite * end.Y);
            result.Z = (inverse * start.Z) + (opposite * end.Z);
            result.W = (inverse * start.W) + (opposite * end.W);

        }

        /// <summary>
        /// Interpolates between quaternions, using spherical quadrangle interpolation.
        /// </summary>
        /// <param name="source1">First source quaternion.</param>
        /// <param name="source2">Second source quaternion.</param>
        /// <param name="source3">Thrid source quaternion.</param>
        /// <param name="source4">Fourth source quaternion.</param>
        /// <param name="amount">Value between 0 and 1 indicating the weight of interpolation.</param>
        /// <returns>The spherical quadrangle interpolation of the quaternions.</returns>
        public static Quaternion Squad(Quaternion source1, Quaternion source2, Quaternion source3, Quaternion source4, float amount) 
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Interpolates between quaternions, using spherical quadrangle interpolation.
        /// </summary>
        /// <param name="source1">First source quaternion.</param>
        /// <param name="source2">Second source quaternion.</param>
        /// <param name="source3">Thrid source quaternion.</param>
        /// <param name="source4">Fourth source quaternion.</param>
        /// <param name="amount">Value between 0 and 1 indicating the weight of interpolation.</param>
        /// <param name="result">When the method completes, contains the spherical quadrangle interpolation of the quaternions.</param>
        public static void Squad(ref Quaternion source1, ref Quaternion source2, ref Quaternion source3, ref Quaternion source4, float amount, out Quaternion result)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets up control points for spherical quadrangle interpolation.
        /// </summary>
        /// <param name="source1">First source quaternion.</param>
        /// <param name="source2">Second source quaternion.</param>
        /// <param name="source3">Third source quaternion.</param>
        /// <param name="source4">Fourth source quaternion.</param>
        /// <returns>An array of three quaternions that represent control points for spherical quadrangle interpolation.</returns>
        public static Quaternion[] SquadSetup(Quaternion source1, Quaternion source2, Quaternion source3, Quaternion source4)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Subtracts two quaternions.
        /// </summary>
        /// <param name="left">The first quaternion to subtract.</param>
        /// <param name="right">The second quaternion to subtract.</param>
        /// <returns>The difference of the two quaternions.</returns>
        public static Quaternion Subtract(Quaternion left, Quaternion right)
        {
            Quaternion result;
            result.X = left.X - right.X;
            result.Y = left.Y - right.Y;
            result.Z = left.Z - right.Z;
            result.W = left.W - right.W;
            return result;
        }

        /// <summary>
        /// Subtracts two quaternions.
        /// </summary>
        /// <param name="left">The first quaternion to subtract.</param>
        /// <param name="right">The second quaternion to subtract.</param>
        /// <param name="result">When the method completes, contains the difference of the two quaternions.</param>
        public static void Subtract(ref Quaternion left, ref Quaternion right, out Quaternion result)
        {
            result.X = left.X - right.X;
            result.Y = left.Y - right.Y;
            result.Z = left.Z - right.Z;
            result.W = left.W - right.W;
        }

        /// <summary>
        /// Multiplies a quaternion by another.
        /// </summary>
        /// <param name="left">The first quaternion to multiply.</param>
        /// <param name="right">The second quaternion to multiply.</param>
        /// <returns>The multiplied quaternion.</returns>
        public static Quaternion operator *(Quaternion left, Quaternion right)
        {
            Quaternion quaternion;
            float rx = right.X;
            float ry = right.Y;
            float rz = right.Z;
            float rw = right.W;
            float lx = left.X;
            float ly = left.Y;
            float lz = left.Z;
            float lw = left.W;
            float yz = (ry * lz) - (rz * ly);
            float xz = (rz * lx) - (rx * lz);
            float xy = (rx * ly) - (ry * lx);
            float lengthSq = ((rx * lx) + (ry * ly)) + (rz * lz);

            quaternion.X = ((rx * lw) + (lx * rw)) + yz;
            quaternion.Y = ((ry * lw) + (ly * rw)) + xz;
            quaternion.Z = ((rz * lw) + (lz * rw)) + xy;
            quaternion.W = (rw * lw) - lengthSq;

            return quaternion;
        }

        /// <summary>
        /// Scales a quaternion by the given value.
        /// </summary>
        /// <param name="quaternion">The quaternion to scale.</param>
        /// <param name="scale">The amount by which to scale the quaternion.</param>
        /// <returns>The scaled quaternion.</returns>
        public static Quaternion operator *(Quaternion quaternion, float scale)
        {
            Quaternion result;
            result.X = quaternion.X * scale;
            result.Y = quaternion.Y * scale;
            result.Z = quaternion.Z * scale;
            result.W = quaternion.W * scale;
            return result;
        }

        /// <summary>
        /// Scales a quaternion by the given value.
        /// </summary>
        /// <param name="quaternion">The quaternion to scale.</param>
        /// <param name="scale">The amount by which to scale the quaternion.</param>
        /// <returns>The scaled quaternion.</returns>
        public static Quaternion operator *(float scale, Quaternion quaternion)
        {
            Quaternion result;
            result.X = quaternion.X * scale;
            result.Y = quaternion.Y * scale;
            result.Z = quaternion.Z * scale;
            result.W = quaternion.W * scale;
            return result;
        }

        /// <summary>
        /// Divides a quaternion by another.
        /// </summary>
        /// <param name="left">The first quaternion to divide.</param>
        /// <param name="right">The second quaternion to divide.</param>
        /// <returns>The divided quaternion.</returns>
        public static Quaternion operator /(Quaternion left, float right)
        {
            Quaternion result;
            result.X = left.X / right;
            result.Y = left.Y / right;
            result.Z = left.Z / right;
            result.W = left.W / right;
            return result;
        }

        /// <summary>
        /// Adds two quaternions.
        /// </summary>
        /// <param name="left">The first quaternion to add.</param>
        /// <param name="right">The second quaternion to add.</param>
        /// <returns>The sum of the two quaternions.</returns>
        public static Quaternion operator +(Quaternion left, Quaternion right)
        {
            Quaternion result;
            result.X = left.X + right.X;
            result.Y = left.Y + right.Y;
            result.Z = left.Z + right.Z;
            result.W = left.W + right.W;
            return result;
        }

        /// <summary>
        /// Subtracts two quaternions.
        /// </summary>
        /// <param name="left">The first quaternion to subtract.</param>
        /// <param name="right">The second quaternion to subtract.</param>
        /// <returns>The difference of the two quaternions.</returns>
        public static Quaternion operator -(Quaternion left, Quaternion right)
        {
            Quaternion result;
            result.X = left.X * right.X;
            result.Y = left.Y * right.Y;
            result.Z = left.Z * right.Z;
            result.W = left.W * right.W;
            return result;
        }

        /// <summary>
        /// Reverses the direction of a given quaternion.
        /// </summary>
        /// <param name="quaternion">The quaternion to negate.</param>
        /// <returns>A quaternion facing in the opposite direction.</returns>
        public static Quaternion operator -(Quaternion quaternion)
        {
            Quaternion result;
            result.X = -quaternion.X;
            result.Y = -quaternion.Y;
            result.Z = -quaternion.Z;
            result.W = -quaternion.W;
            return result;
        }

        /// <summary>
        /// Tests for equality between two objects.
        /// </summary>
        /// <param name="left">The first value to compare.</param>
        /// <param name="right">The second value to compare.</param>
        /// <returns><c>true</c> if <paramref name="left"/> has the same value as <paramref name="right"/>; otherwise, <c>false</c>.</returns>
        public static bool operator ==(Quaternion left, Quaternion right)
        {
            return Quaternion.Equals(left, right);
        }

        /// <summary>
        /// Tests for inequality between two objects.
        /// </summary>
        /// <param name="left">The first value to compare.</param>
        /// <param name="right">The second value to compare.</param>
        /// <returns><c>true</c> if <paramref name="left"/> has a different value than <paramref name="right"/>; otherwise, <c>false</c>.</returns>
        public static bool operator !=(Quaternion left, Quaternion right)
        {
            return !Quaternion.Equals(left, right);
        }

        /// <summary>
        /// Converts the value of the object to its equivalent string representation.
        /// </summary>
        /// <returns>The string representation of the value of this instance.</returns>
        public override string ToString()
        {
            return string.Format(CultureInfo.CurrentCulture, "X:{0} Y:{1} Z:{2} W:{3}", X.ToString(CultureInfo.CurrentCulture),
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

            return Equals((Quaternion)obj);
        }

        /// <summary>
        /// Returns a value that indicates whether the current instance is equal to the specified object. 
        /// </summary>
        /// <param name="other">Object to make the comparison with.</param>
        /// <returns><c>true</c> if the current instance is equal to the specified object; <c>false</c> otherwise.</returns>
        public bool Equals(Quaternion other)
        {
            return (X == other.X && Y == other.Y && Z == other.Z && W == other.W);
        }

        /// <summary>
        /// Determines whether the specified object instances are considered equal. 
        /// </summary>
        /// <param name="value1"></param>
        /// <param name="value2"></param>
        /// <returns><c>true</c> if <paramref name="value1"/> is the same instance as <paramref name="value2"/> or 
        /// if both are <c>null</c> references or if <c>value1.Equals(value2)</c> returns <c>true</c>; otherwise, <c>false</c>.</returns>
        public static bool Equals(ref Quaternion value1, ref Quaternion value2)
        {
            return (value1.X == value2.X && value1.Y == value2.Y && value1.Z == value2.Z && value1.W == value2.W);
        }

    }
}
