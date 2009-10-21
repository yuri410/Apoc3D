using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Globalization;
using System.Runtime.InteropServices;
using System.Text;
using Apoc.Design;

namespace Apoc.MathLib
{
    public enum PlaneIntersectionType
    {
        Front,
        Back,
        Intersecting
    }

    /// <summary>
    /// Defines a plane in three dimensions.
    /// </summary>
    [Serializable, StructLayout(LayoutKind.Sequential), TypeConverter(typeof(PlaneConverter))]
    public struct Plane : IEquatable<Plane>
    {
        /// <summary>
        /// The normal vector of the plane.
        /// </summary>
        public Vector3 Normal;

        /// <summary>
        /// The distance of the plane along its normal from the origin.
        /// </summary>
        public float D;

        /// <summary>
        /// Initializes a new instance of the <see cref="Plane"/> class.
        /// </summary>
        /// <param name="a">X component of the normal defining the plane.</param>
        /// <param name="b">Y component of the normal defining the plane.</param>
        /// <param name="c">Z component of the normal defining the plane.</param>
        /// <param name="d">Distance of the plane along its normal from the origin.</param>
        public Plane(float a, float b, float c, float d)
        {
            Normal = new Vector3(a, b, c);
            D = d;
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="Plane"/> class.
        /// </summary>
        /// <param name="normal">The normal vector to the plane.</param>
        /// <param name="d">Distance of the plane along its normal from the origin.</param>
        public Plane(Vector3 normal, float d)
        {
            Normal = normal;
            D = d;
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="Plane"/> class.
        /// </summary>
        /// <param name="point">Any point that lies along the plane.</param>
        /// <param name="normal">The normal vector to the plane.</param>
        public Plane(Vector3 point, Vector3 normal)
        {
            Normal = normal;
            D = -Vector3.Dot(normal, point);
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="Plane"/> class.
        /// </summary>
        /// <param name="point1">First point of a triangle defining the plane.</param>
        /// <param name="point2">Second point of a triangle defining the plane.</param>
        /// <param name="point3">Third point of a triangle defining the plane.</param>
        public Plane(Vector3 point1, Vector3 point2, Vector3 point3)
        {
            float x1 = point2.X - point1.X;
            float y1 = point2.Y - point1.Y;
            float z1 = point2.Z - point1.Z;
            float x2 = point3.X - point1.X;
            float y2 = point3.Y - point1.Y;
            float z2 = point3.Z - point1.Z;
            float yz = (y1 * z2) - (z1 * y2);
            float xz = (z1 * x2) - (x1 * z2);
            float xy = (x1 * y2) - (y1 * x2);
            float invPyth = 1.0f / (float)Math.Sqrt((yz * yz) + (xz * xz) + (xy * xy));

            Normal.X = yz * invPyth;
            Normal.Y = xz * invPyth;
            Normal.Z = xy * invPyth;
            D = -((Normal.X * point1.X) + (Normal.Y * point1.Y) + (Normal.Z * point1.Z));
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="Plane"/> class.
        /// </summary>
        /// <param name="value">
        /// A vector with the X, Y, and Z components defining the normal to the plane.
        /// The W component defines the distance of the plane along its normal from the origin.
        /// </param>
        public Plane(Vector4 value)
        {
            Normal = new Vector3(value.X, value.Y, value.Z);
            D = value.W;
        }

        /// <summary>
        /// Calculates the dot product of the specified vector and plane.
        /// </summary>
        /// <param name="plane">The source plane.</param>
        /// <param name="point">The source vector.</param>
        /// <returns>The dot product of the specified vector and plane.</returns>
        public static float Dot(Plane plane, Vector4 point)
        {
            return (plane.Normal.X * point.X) + (plane.Normal.Y * point.Y) + (plane.Normal.Z * point.Z) + (plane.D * point.W);
        }

        /// <summary>
        /// Calculates the dot product of a specified vector and the normal of the plane plus the distance value of the plane.
        /// </summary>
        /// <param name="plane">The source plane.</param>
        /// <param name="point">The source vector.</param>
        /// <returns>The dot product of a specified vector and the normal of the Plane plus the distance value of the plane.</returns>
        public static float DotCoordinate(Plane plane, Vector3 point)
        {
            return (plane.Normal.X * point.X) + (plane.Normal.Y * point.Y) + (plane.Normal.Z * point.Z) + plane.D;
        }

        /// <summary>
        /// Calculates the dot product of the specified vector and the normal of the plane.
        /// </summary>
        /// <param name="plane">The source plane.</param>
        /// <param name="point">The source vector.</param>
        /// <returns>The dot product of the specified vector and the normal of the plane.</returns>
        public static float DotNormal(Plane plane, Vector3 point)
        {
            return (plane.Normal.X * point.X) + (plane.Normal.Y * point.Y) + (plane.Normal.Z * point.Z);
        }

        /// <summary>
        /// Changes the coefficients of the normal vector of the plane to make it of unit length.
        /// </summary>
        public void Normalize()
        {
            float magnitude = 1.0f / (float)Math.Sqrt(Normal.X * Normal.X + Normal.Y * Normal.Y + Normal.Z * Normal.Z);

            Normal.X *= magnitude;
            Normal.Y *= magnitude;
            Normal.Z *= magnitude;
            D *= magnitude;
        }

        /// <summary>
        /// Changes the coefficients of the normal vector of the plane to make it of unit length.
        /// </summary>
        /// <param name="plane">The source plane.</param>
        /// <returns>The normalized plane.</returns>
        public static Plane Normalize(Plane plane)
        {
            float magnitude = 1.0f /
                (float)Math.Sqrt(plane.Normal.X * plane.Normal.X + plane.Normal.Y * plane.Normal.Y + plane.Normal.Z * plane.Normal.Z);

            return new Plane(plane.Normal.X * magnitude, plane.Normal.Y * magnitude, plane.Normal.Z * magnitude, plane.D * magnitude);
        }

        /// <summary>
        /// Changes the coefficients of the normal vector of the plane to make it of unit length.
        /// </summary>
        /// <param name="plane">The source plane.</param>
        /// <param name="result">When the method completes, contains the normalized plane.</param>
        public static void Normalize(ref Plane plane, out Plane result)
        {
            float magnitude = 1.0f / (float)Math.Sqrt(plane.Normal.X * plane.Normal.X + plane.Normal.Y * plane.Normal.Y + plane.Normal.Z * plane.Normal.Z);

            result = new Plane(plane.Normal.X * magnitude, plane.Normal.Y * magnitude, plane.Normal.Z * magnitude, plane.D * magnitude);
        }

        /// <summary>
        /// Transforms a normalized plane by a matrix.
        /// </summary>
        /// <param name="plane">The normalized source plane.</param>
        /// <param name="transformation">The transformation matrix.</param>
        /// <returns>The transformed plane.</returns>
        public static Plane Transform(Plane plane, Matrix transformation)
        {
            Plane result;
            float x = plane.Normal.X;
            float y = plane.Normal.Y;
            float z = plane.Normal.Z;
            float d = plane.D;

            transformation.Invert();
            result.Normal.X = (((x * transformation.M11) + (y * transformation.M12)) + (z * transformation.M13)) + (d * transformation.M14);
            result.Normal.Y = (((x * transformation.M21) + (y * transformation.M22)) + (z * transformation.M23)) + (d * transformation.M24);
            result.Normal.Z = (((x * transformation.M31) + (y * transformation.M32)) + (z * transformation.M33)) + (d * transformation.M34);
            result.D = (((x * transformation.M41) + (y * transformation.M42)) + (z * transformation.M43)) + (d * transformation.M44);

            return result;
        }

        /// <summary>
        /// Transforms a normalized plane by a matrix.
        /// </summary>
        /// <param name="plane">The normalized source plane.</param>
        /// <param name="transformation">The transformation matrix.</param>
        /// <param name="result">When the method completes, contains the transformed plane.</param>
        public static void Transform(ref Plane plane, ref Matrix transformation, out Plane result)
        {
            float x = plane.Normal.X;
            float y = plane.Normal.Y;
            float z = plane.Normal.Z;
            float d = plane.D;

            Matrix temp = Matrix.Invert(transformation);

            Plane r;
            r.Normal.X = (((x * temp.M11) + (y * temp.M12)) + (z * temp.M13)) + (d * temp.M14);
            r.Normal.Y = (((x * temp.M21) + (y * temp.M22)) + (z * temp.M23)) + (d * temp.M24);
            r.Normal.Z = (((x * temp.M31) + (y * temp.M32)) + (z * temp.M33)) + (d * temp.M34);
            r.D = (((x * temp.M41) + (y * temp.M42)) + (z * temp.M43)) + (d * temp.M44);

            result = r;
        }

        /// <summary>
        /// Transforms an array of normalized planes by a matrix.
        /// </summary>
        /// <param name="planes">The normalized source planes.</param>
        /// <param name="transformation">The transformation matrix.</param>
        /// <returns>The transformed planes.</returns>
        public static Plane[] Transform(Plane[] planes, ref Matrix transformation)
        {
            if (planes == null)
                throw new ArgumentNullException("planes");

            int count = planes.Length;
            Plane[] results = new Plane[count];
            Matrix temp = Matrix.Invert(transformation);

            for (int i = 0; i < count; i++)
            {
                float x = planes[i].Normal.X;
                float y = planes[i].Normal.Y;
                float z = planes[i].Normal.Z;
                float d = planes[i].D;

                Plane r;
                r.Normal.X = (((x * temp.M11) + (y * temp.M12)) + (z * temp.M13)) + (d * temp.M14);
                r.Normal.Y = (((x * temp.M21) + (y * temp.M22)) + (z * temp.M23)) + (d * temp.M24);
                r.Normal.Z = (((x * temp.M31) + (y * temp.M32)) + (z * temp.M33)) + (d * temp.M34);
                r.D = (((x * temp.M41) + (y * temp.M42)) + (z * temp.M43)) + (d * temp.M44);

                results[i] = r;
            }

            return results;
        }

        /// <summary>
        /// Transforms a normalized plane by a quaternion rotation.
        /// </summary>
        /// <param name="plane">The normalized source plane.</param>
        /// <param name="rotation">The quaternion rotation.</param>
        /// <returns>The transformed plane.</returns>
        public static Plane Transform(Plane plane, Quaternion rotation)
        {
            Plane result;
            float x2 = rotation.X + rotation.X;
            float y2 = rotation.Y + rotation.Y;
            float z2 = rotation.Z + rotation.Z;
            float wx = rotation.W * x2;
            float wy = rotation.W * y2;
            float wz = rotation.W * z2;
            float xx = rotation.X * x2;
            float xy = rotation.X * y2;
            float xz = rotation.X * z2;
            float yy = rotation.Y * y2;
            float yz = rotation.Y * z2;
            float zz = rotation.Z * z2;

            float x = plane.Normal.X;
            float y = plane.Normal.Y;
            float z = plane.Normal.Z;

            result.Normal.X = ((x * ((1.0f - yy) - zz)) + (y * (xy - wz))) + (z * (xz + wy));
            result.Normal.Y = ((x * (xy + wz)) + (y * ((1.0f - xx) - zz))) + (z * (yz - wx));
            result.Normal.Z = ((x * (xz - wy)) + (y * (yz + wx))) + (z * ((1.0f - xx) - yy));
            result.D = plane.D;
            return result;
        }

        /// <summary>
        /// Transforms a normalized plane by a quaternion rotation.
        /// </summary>
        /// <param name="plane">The normalized source plane.</param>
        /// <param name="rotation">The quaternion rotation.</param>
        /// <param name="result">When the method completes, contains the transformed plane.</param>
        public static void Transform(ref  Plane plane, ref Quaternion rotation, out Plane result)
        {
            float x2 = rotation.X + rotation.X;
            float y2 = rotation.Y + rotation.Y;
            float z2 = rotation.Z + rotation.Z;
            float wx = rotation.W * x2;
            float wy = rotation.W * y2;
            float wz = rotation.W * z2;
            float xx = rotation.X * x2;
            float xy = rotation.X * y2;
            float xz = rotation.X * z2;
            float yy = rotation.Y * y2;
            float yz = rotation.Y * z2;
            float zz = rotation.Z * z2;

            float x = plane.Normal.X;
            float y = plane.Normal.Y;
            float z = plane.Normal.Z;

            Plane r;
            r.Normal.X = ((x * ((1.0f - yy) - zz)) + (y * (xy - wz))) + (z * (xz + wy));
            r.Normal.Y = ((x * (xy + wz)) + (y * ((1.0f - xx) - zz))) + (z * (yz - wx));
            r.Normal.Z = ((x * (xz - wy)) + (y * (yz + wx))) + (z * ((1.0f - xx) - yy));
            r.D = plane.D;

            result = r;
        }

        /// <summary>
        /// Transforms an array of normalized planes by a quaternion rotation.
        /// </summary>
        /// <param name="planes">The normalized source planes.</param>
        /// <param name="rotation">The quaternion rotation.</param>
        /// <returns>The transformed planes.</returns>
        public static Plane[] Transform(Plane[] planes, ref Quaternion rotation)
        {
            if (planes == null)
                throw new ArgumentNullException("planes");

            int count = planes.Length;
            Plane[] results = new Plane[count];

            float x2 = rotation.X + rotation.X;
            float y2 = rotation.Y + rotation.Y;
            float z2 = rotation.Z + rotation.Z;
            float wx = rotation.W * x2;
            float wy = rotation.W * y2;
            float wz = rotation.W * z2;
            float xx = rotation.X * x2;
            float xy = rotation.X * y2;
            float xz = rotation.X * z2;
            float yy = rotation.Y * y2;
            float yz = rotation.Y * z2;
            float zz = rotation.Z * z2;

            for (int i = 0; i < count; i++)
            {
                float x = planes[i].Normal.X;
                float y = planes[i].Normal.Y;
                float z = planes[i].Normal.Z;

                Plane r;
                r.Normal.X = ((x * ((1.0f - yy) - zz)) + (y * (xy - wz))) + (z * (xz + wy));
                r.Normal.Y = ((x * (xy + wz)) + (y * ((1.0f - xx) - zz))) + (z * (yz - wx));
                r.Normal.Z = ((x * (xz - wy)) + (y * (yz + wx))) + (z * ((1.0f - xx) - yy));
                r.D = planes[i].D;

                results[i] = r;
            }

            return results;
        }

#warning test
        /// <summary>
        /// Finds the intersection between a plane and a line.
        /// </summary>
        /// <param name="plane">The source plane.</param>
        /// <param name="start">The start point of the line.</param>
        /// <param name="end">The end point of the line.</param>
        /// <param name="intersectPoint">If an intersection is found, contains the intersection point between the line and the plane.</param>
        /// <returns><c>true</c> if an intersection is found; <c>false</c> otherwise.</returns>
        public static bool Intersects(Plane plane, Vector3 start, Vector3 end, out Vector3 intersectPoint)
        {
            Vector3 dir = end - start;

            float cos = Vector3.Dot(dir, plane.Normal);

            if (cos < float.Epsilon) 
            {
                intersectPoint = Vector3.Zero;
                return false;
            }

            float d1 = Vector3.Dot(start, plane.Normal);
            float d2 = Vector3.Dot(end, plane.Normal);

            if (d1 * d2 < 0)
            {
                intersectPoint = Vector3.Zero;
                return false;
            }

            cos /= dir.Length();
            float sin = (float)Math.Sqrt(1 - cos * cos);

            float dist = d1 / sin;

            intersectPoint = start + dir * dist;
            return true;
        }

        /// <summary>
        /// Finds the intersection between a plane and a box.
        /// </summary>
        /// <param name="plane">The source plane.</param>
        /// <param name="box">The box to check for intersection.</param>
        /// <returns>A value from the <see cref="PlaneIntersectionType"/> enumeration describing the result of the intersection test.</returns>
        public static PlaneIntersectionType Intersects(Plane plane, BoundingBox box)
        {
            Vector3 min;
            Vector3 max;
            max.X = (plane.Normal.X >= 0.0f) ? box.Minimum.X : box.Maximum.X;
            max.Y = (plane.Normal.Y >= 0.0f) ? box.Minimum.Y : box.Maximum.Y;
            max.Z = (plane.Normal.Z >= 0.0f) ? box.Minimum.Z : box.Maximum.Z;
            min.X = (plane.Normal.X >= 0.0f) ? box.Maximum.X : box.Minimum.X;
            min.Y = (plane.Normal.Y >= 0.0f) ? box.Maximum.Y : box.Minimum.Y;
            min.Z = (plane.Normal.Z >= 0.0f) ? box.Maximum.Z : box.Minimum.Z;

            float dot = (plane.Normal.X * max.X) + (plane.Normal.Y * max.Y) + (plane.Normal.Z * max.Z);

            if (dot + plane.D > 0.0f)
                return PlaneIntersectionType.Front;

            dot = (plane.Normal.X * min.X) + (plane.Normal.Y * min.Y) + (plane.Normal.Z * min.Z);

            if (dot + plane.D < 0.0f)
                return PlaneIntersectionType.Back;

            return PlaneIntersectionType.Intersecting;
        }

        /// <summary>
        /// Finds the intersection between a plane and a sphere.
        /// </summary>
        /// <param name="plane">The source plane.</param>
        /// <param name="sphere">The sphere to check for intersection.</param>
        /// <returns>A value from the <see cref="PlaneIntersectionType"/> enumeration describing the result of the intersection test.</returns>
        public static PlaneIntersectionType Intersects(Plane plane, BoundingSphere sphere)
        {
            float dot = (sphere.Center.X * plane.Normal.X) + (sphere.Center.Y * plane.Normal.Y) + (sphere.Center.Z * plane.Normal.Z) + plane.D;

            if (dot > sphere.Radius)
                return PlaneIntersectionType.Front;

            if (dot < -sphere.Radius)
                return PlaneIntersectionType.Back;

            return PlaneIntersectionType.Intersecting;
        }

        /// <summary>
        /// Scales the plane by the given scaling factor.
        /// </summary>
        /// <param name="plane">The source plane.</param>
        /// <param name="scale">The scaling factor.</param>
        /// <returns>The scaled plane.</returns>
        public static Plane Multiply(Plane plane, float scale) 
        {
            plane.D *= scale;
            plane.Normal.X *= scale;
            plane.Normal.Y *= scale;
            plane.Normal.Z *= scale;
            return plane;
        }

        /// <summary>
        /// Scales the plane by the given scaling factor.
        /// </summary>
        /// <param name="plane">The source plane.</param>
        /// <param name="scale">The scaling factor.</param>
        /// <param name="result">When the method completes, contains the scaled plane.</param>
        public static void Multiply(ref Plane plane, float scale, out Plane result) 
        {
            result.D = plane.D * scale;
            result.Normal.X = plane.Normal.X * scale;
            result.Normal.Y = plane.Normal.Y * scale;
            result.Normal.Z = plane.Normal.Z * scale;
        }

        /// <summary>
        /// Scales the plane by the given scaling factor.
        /// </summary>
        /// <param name="plane">The source plane.</param>
        /// <param name="scale">The scaling factor.</param>
        /// <returns>The scaled plane.</returns>
        public static Plane operator *(Plane plane, float scale) 
        {
            plane.D *= scale;
            plane.Normal.X *= scale;
            plane.Normal.Y *= scale;
            plane.Normal.Z *= scale;
            return plane;
        }

        /// <summary>
        /// Scales the plane by the given scaling factor.
        /// </summary>
        /// <param name="plane">The source plane.</param>
        /// <param name="scale">The scaling factor.</param>
        /// <returns>The scaled plane.</returns>
        public static Plane operator *(float scale, Plane plane)
        {
            return plane * scale;
        }

        /// <summary>
        /// Tests for equality between two objects.
        /// </summary>
        /// <param name="left">The first value to compare.</param>
        /// <param name="right">The second value to compare.</param>
        /// <returns><c>true</c> if <paramref name="left"/> has the same value as <paramref name="right"/>; otherwise, <c>false</c>.</returns>
        public static bool operator ==(Plane left, Plane right)
        {
            return Plane.Equals(left, right);
        }

        /// <summary>
        /// Tests for inequality between two objects.
        /// </summary>
        /// <param name="left">The first value to compare.</param>
        /// <param name="right">The second value to compare.</param>
        /// <returns><c>true</c> if <paramref name="left"/> has a different value than <paramref name="right"/>; otherwise, <c>false</c>.</returns>
        public static bool operator !=(Plane left, Plane right)
        {
            return !Plane.Equals(left, right);
        }

        /// <summary>
        /// Converts the value of the object to its equivalent string representation.
        /// </summary>
        /// <returns>The string representation of the value of this instance.</returns>
        public override string ToString()
        {
            return String.Format(CultureInfo.CurrentCulture, "Normal:{0} D:{1}", Normal.ToString(), D.ToString(CultureInfo.CurrentCulture));
        }

        /// <summary>
        /// Returns the hash code for this instance.
        /// </summary>
        /// <returns>A 32-bit signed integer hash code.</returns>
        public override int GetHashCode()
        {
            return Normal.GetHashCode() + D.GetHashCode();
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

            return Equals((Plane)obj);
        }

        /// <summary>
        /// Returns a value that indicates whether the current instance is equal to the specified object. 
        /// </summary>
        /// <param name="other">Object to make the comparison with.</param>
        /// <returns><c>true</c> if the current instance is equal to the specified object; <c>false</c> otherwise.</returns>
        public bool Equals(Plane other)
        {
            return Normal == other.Normal && D == other.D;
        }

        /// <summary>
        /// Determines whether the specified object instances are considered equal. 
        /// </summary>
        /// <param name="value1"></param>
        /// <param name="value2"></param>
        /// <returns><c>true</c> if <paramref name="value1"/> is the same instance as <paramref name="value2"/> or 
        /// if both are <c>null</c> references or if <c>value1.Equals(value2)</c> returns <c>true</c>; otherwise, <c>false</c>.</returns>
        public static bool Equals(ref Plane value1, ref Plane value2)
        {
            return (value1.Normal == value2.Normal && value1.D == value2.D);
        }

    }
}
