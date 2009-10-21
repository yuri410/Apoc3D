using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Globalization;
using System.Runtime.InteropServices;
using System.Text;
using Apoc.Design;

namespace Apoc.MathLib
{
    public enum ContainmentType
    {
        Disjoint,
        Contains,
        Intersects
    }

    /// <summary>
    /// A bounding sphere, specified by a center vector and a radius.
    /// </summary>
    [Serializable, StructLayout(LayoutKind.Sequential), TypeConverter(typeof(BoundingSphereConverter))]
    public struct BoundingSphere : IEquatable<BoundingSphere>
    {
        /// <summary>
        /// Specifies the center point of the sphere.
        /// </summary>
        public Vector3 Center;

        /// <summary>
        /// The radius of the sphere.
        /// </summary>
        public float Radius;



        /// <summary>
        /// Initializes a new instance of the <see cref="BoundingSphere"/> structure.
        /// </summary>
        /// <param name="center">The center of the bounding sphere.</param>
        /// <param name="radius">The radius of the sphere.</param>
        public BoundingSphere(Vector3 center, float radius)
        {
            Center = center;
            Radius = radius;
        }

        /// <summary>
        /// Determines whether the sphere contains the specified box.
        /// </summary>
        /// <param name="sphere">The sphere that will be checked for containment.</param>
        /// <param name="box">The box that will be checked for containment.</param>
        /// <returns>A member of the <see cref="ContainmentType"/> enumeration indicating whether the two objects intersect, are contained, or don't meet at all.</returns>
        public static ContainmentType Contains(BoundingSphere sphere, BoundingBox box)
        {
            Vector3 vector;

            if (!BoundingBox.Intersects(box, sphere))
                return ContainmentType.Disjoint;

            float radius = sphere.Radius * sphere.Radius;
            vector.X = sphere.Center.X - box.Minimum.X;
            vector.Y = sphere.Center.Y - box.Maximum.Y;
            vector.Z = sphere.Center.Z - box.Maximum.Z;

            if (vector.LengthSquared() > radius)
                return ContainmentType.Intersects;

            vector.X = sphere.Center.X - box.Maximum.X;
            vector.Y = sphere.Center.Y - box.Maximum.Y;
            vector.Z = sphere.Center.Z - box.Maximum.Z;

            if (vector.LengthSquared() > radius)
                return ContainmentType.Intersects;

            vector.X = sphere.Center.X - box.Maximum.X;
            vector.Y = sphere.Center.Y - box.Minimum.Y;
            vector.Z = sphere.Center.Z - box.Maximum.Z;

            if (vector.LengthSquared() > radius)
                return ContainmentType.Intersects;

            vector.X = sphere.Center.X - box.Minimum.X;
            vector.Y = sphere.Center.Y - box.Minimum.Y;
            vector.Z = sphere.Center.Z - box.Maximum.Z;

            if (vector.LengthSquared() > radius)
                return ContainmentType.Intersects;

            vector.X = sphere.Center.X - box.Minimum.X;
            vector.Y = sphere.Center.Y - box.Maximum.Y;
            vector.Z = sphere.Center.Z - box.Minimum.Z;

            if (vector.LengthSquared() > radius)
                return ContainmentType.Intersects;

            vector.X = sphere.Center.X - box.Maximum.X;
            vector.Y = sphere.Center.Y - box.Maximum.Y;
            vector.Z = sphere.Center.Z - box.Minimum.Z;

            if (vector.LengthSquared() > radius)
                return ContainmentType.Intersects;

            vector.X = sphere.Center.X - box.Maximum.X;
            vector.Y = sphere.Center.Y - box.Minimum.Y;
            vector.Z = sphere.Center.Z - box.Minimum.Z;

            if (vector.LengthSquared() > radius)
                return ContainmentType.Intersects;

            vector.X = sphere.Center.X - box.Minimum.X;
            vector.Y = sphere.Center.Y - box.Minimum.Y;
            vector.Z = sphere.Center.Z - box.Minimum.Z;

            if (vector.LengthSquared() > radius)
                return ContainmentType.Intersects;

            return ContainmentType.Contains;
        }

        /// <summary>
        /// Determines whether the sphere contains the specified sphere.
        /// </summary>
        /// <param name="sphere1">The first sphere that will be checked for containment.</param>
        /// <param name="sphere2">The second sphere that will be checked for containment.</param>
        /// <returns>A member of the <see cref="ContainmentType"/> enumeration indicating whether the two objects intersect, are contained, or don't meet at all.</returns>
        public static ContainmentType Contains(BoundingSphere sphere1, BoundingSphere sphere2)
        {
            float distance;
            float x = sphere1.Center.X - sphere2.Center.X;
            float y = sphere1.Center.Y - sphere2.Center.Y;
            float z = sphere1.Center.Z - sphere2.Center.Z;

            distance = (float)(Math.Sqrt((x * x) + (y * y) + (z * z)));
            float radius = sphere1.Radius;
            float radius2 = sphere2.Radius;

            if (radius + radius < distance)
                return ContainmentType.Disjoint;

            if (radius - radius2 < distance)
                return ContainmentType.Intersects;

            return ContainmentType.Contains;
        }

        /// <summary>
        /// Determines whether the sphere contains the specified point.
        /// </summary>
        /// <param name="sphere">The sphere that will be checked for containment.</param>
        /// <param name="vector">The point that will be checked for containment.</param>
        /// <returns>A member of the <see cref="ContainmentType"/> enumeration indicating whether the two objects intersect, are contained, or don't meet at all.</returns>
        public static ContainmentType Contains(BoundingSphere sphere, Vector3 vector)
        {
            float x = vector.X - sphere.Center.X;
            float y = vector.Y - sphere.Center.Y;
            float z = vector.Z - sphere.Center.Z;

            float distance = (x * x) + (y * y) + (z * z);

            if (distance >= (sphere.Radius * sphere.Radius))
                return ContainmentType.Disjoint;

            return ContainmentType.Contains;
        }

        /// <summary>
        /// Constructs a <see cref="BoundingSphere"/> from a given box.
        /// </summary>
        /// <param name="box">The box that will designate the extents of the sphere.</param>
        /// <returns>The newly constructed bounding sphere.</returns>
        public static BoundingSphere FromBox(BoundingBox box)
        {
            BoundingSphere sphere;
            Vector3.Lerp(ref box.Minimum, ref box.Maximum, 0.5f, out sphere.Center);

            float x = box.Minimum.X - box.Maximum.X;
            float y = box.Minimum.Y - box.Maximum.Y;
            float z = box.Minimum.Z - box.Maximum.Z;

            float distance = (float)(Math.Sqrt((x * x) + (y * y) + (z * z)));

            sphere.Radius = distance * 0.5f;

            return sphere;
        }

        /// <summary>
        /// Constructs a <see cref="BoundingSphere"/> that fully contains the given points.
        /// </summary>
        /// <param name="points">The points that will be contained by the sphere.</param>
        /// <returns>The newly constructed bounding sphere.</returns>
        public static BoundingSphere FromPoints(Vector3[] points)
        {
            float num;
            float num2;
            Vector3 vector2;
            float num4;
            float num5;
            BoundingSphere sphere;
            Vector3 vector5;
            Vector3 vector6;
            Vector3 vector7;
            Vector3 vector8;
            Vector3 vector9;
            if (points == null)
            {
                throw new ArgumentNullException("points");
            }
            //IEnumerator<Vector3> enumerator = points.GetEnumerator();
            //if (!enumerator.MoveNext())
            //{
            //    throw new ArgumentException("BaseTexts.BoundingSphereZeroPoints");
            //}
            Vector3 vector4 = vector5 = vector6 = vector7 = vector8 = vector9 = points[0];
            for (int i = 0; i < points.Length; i++)
            {
                Vector3 vector = points[i];
                if (vector.X < vector4.X)
                {
                    vector4 = vector;
                }
                if (vector.X > vector5.X)
                {
                    vector5 = vector;
                }
                if (vector.Y < vector6.Y)
                {
                    vector6 = vector;
                }
                if (vector.Y > vector7.Y)
                {
                    vector7 = vector;
                }
                if (vector.Z < vector8.Z)
                {
                    vector8 = vector;
                }
                if (vector.Z > vector9.Z)
                {
                    vector9 = vector;
                }
            }
            num5 = Vector3.Distance(ref vector5, ref vector4);
            num4 = Vector3.Distance(ref vector7, ref vector6);
            num2 = Vector3.Distance(ref vector9, ref vector8);
            if (num5 > num4)
            {
                if (num5 > num2)
                {
                    Vector3.Lerp(ref vector5, ref vector4, 0.5f, out vector2);
                    num = num5 * 0.5f;
                }
                else
                {
                    Vector3.Lerp(ref vector9, ref vector8, 0.5f, out vector2);
                    num = num2 * 0.5f;
                }
            }
            else if (num4 > num2)
            {
                Vector3.Lerp(ref vector7, ref vector6, 0.5f, out vector2);
                num = num4 * 0.5f;
            }
            else
            {
                Vector3.Lerp(ref vector9, ref vector8, 0.5f, out vector2);
                num = num2 * 0.5f;
            }
            foreach (Vector3 vector10 in points)
            {
                Vector3 vector3;
                vector3.X = vector10.X - vector2.X;
                vector3.Y = vector10.Y - vector2.Y;
                vector3.Z = vector10.Z - vector2.Z;
                float num3 = vector3.Length();
                if (num3 > num)
                {
                    num = (num + num3) * 0.5f;
                    vector2 += (Vector3)((1f - (num / num3)) * vector3);
                }
            }
            sphere.Center = vector2;
            sphere.Radius = num;
            return sphere;
        }

        public static BoundingSphere FromFrustum(BoundingFrustum frustum)
        {
            if (frustum == null)
            {
                throw new ArgumentNullException("frustum");
            }
            return FromPoints(frustum.cornerArray);
        }

        /// <summary>
        /// Constructs a <see cref="BoundingSphere"/> that is the as large as the total combined area of the two specified spheres.
        /// </summary>
        /// <param name="sphere1">The first sphere to merge.</param>
        /// <param name="sphere2">The second sphere to merge.</param>
        /// <returns>The newly constructed bounding sphere.</returns>
        public static BoundingSphere Merge(BoundingSphere sphere1, BoundingSphere sphere2)
        {
            BoundingSphere sphere;
            Vector3 difference = sphere2.Center - sphere1.Center;

            float length = difference.Length();
            float radius = sphere1.Radius;
            float radius2 = sphere2.Radius;

            if (radius + radius2 >= length)
            {
                if (radius - radius2 >= length)
                    return sphere1;

                if (radius2 - radius >= length)
                    return sphere2;
            }

            Vector3 vector = difference * (1.0f / length);
            float min = Math.Min(-radius, length - radius2);
            float max = (Math.Max(radius, length + radius2) - min) * 0.5f;

            sphere.Center = sphere1.Center + vector * (max + min);
            sphere.Radius = max;

            return sphere;
        }

        /// <summary>
        /// Determines whether a sphere intersects the specified object.
        /// </summary>
        /// <param name="sphere">The sphere which will be tested for intersection.</param>
        /// <param name="box">The box that will be tested for intersection.</param>
        /// <returns><c>true</c> if the two objects are intersecting; otherwise, <c>false</c>.</returns>
        public static bool Intersects(BoundingSphere sphere, BoundingBox box)
        {
            return BoundingBox.Intersects(box, sphere);
        }

        /// <summary>
        /// Determines whether a sphere intersects the specified object.
        /// </summary>
        /// <param name="sphere1">The first sphere which will be tested for intersection.</param>
        /// <param name="sphere2">The second sphere that will be tested for intersection.</param>
        /// <returns><c>true</c> if the two objects are intersecting; otherwise, <c>false</c>.</returns>
        public static bool Intersects(BoundingSphere sphere1, BoundingSphere sphere2)
        {
            float distance = Vector3.DistanceSquared(ref sphere1.Center, ref sphere2.Center);
            float radius = sphere1.Radius;
            float radius2 = sphere2.Radius;

            if ((radius * radius) + (2.0f * radius * radius2) + (radius2 * radius2) <= distance)
                return false;

            return true;
        }

        /// <summary>
        /// Determines whether a sphere intersects the specified object.
        /// </summary>
        /// <param name="sphere">The sphere which will be tested for intersection.</param>
        /// <param name="ray">The ray that will be tested for intersection.</param>
        /// <param name="distance">When the method completes, contains the distance from the ray's origin in which the intersection with the sphere occured.</param>
        /// <returns><c>true</c> if the two objects are intersecting; otherwise, <c>false</c>.</returns>
        public static bool Intersects(BoundingSphere sphere, Ray ray, out float distance)
        {
            return Ray.Intersects(ray, sphere, out distance);
        }

        /// <summary>
        /// Finds the intersection between a plane and a sphere.
        /// </summary>
        /// <param name="sphere">The sphere to check for intersection.</param>
        /// <param name="plane">The source plane.</param>
        /// <returns>A value from the <see cref="PlaneIntersectionType"/> enumeration describing the result of the intersection test.</returns>
        public static PlaneIntersectionType Intersects(BoundingSphere sphere, Plane plane)
        {
            return Plane.Intersects(plane, sphere);
        }

        /// <summary>
        /// Tests for equality between two objects.
        /// </summary>
        /// <param name="left">The first value to compare.</param>
        /// <param name="right">The second value to compare.</param>
        /// <returns><c>true</c> if <paramref name="left"/> has the same value as <paramref name="right"/>; otherwise, <c>false</c>.</returns>
        public static bool operator ==(BoundingSphere left, BoundingSphere right)
        {
            return BoundingSphere.Equals(left, right);
        }

        /// <summary>
        /// Tests for inequality between two objects.
        /// </summary>
        /// <param name="left">The first value to compare.</param>
        /// <param name="right">The second value to compare.</param>
        /// <returns><c>true</c> if <paramref name="left"/> has a different value than <paramref name="right"/>; otherwise, <c>false</c>.</returns>
        public static bool operator !=(BoundingSphere left, BoundingSphere right)
        {
            return !BoundingSphere.Equals(left, right);
        }

        /// <summary>
        /// Converts the value of the object to its equivalent string representation.
        /// </summary>
        /// <returns>The string representation of the value of this instance.</returns>
        public override string ToString()
        {
            return String.Format(CultureInfo.CurrentCulture, "Center:{0} Radius:{1}", Center.ToString(), Radius.ToString(CultureInfo.CurrentCulture));
        }

        /// <summary>
        /// Returns the hash code for this instance.
        /// </summary>
        /// <returns>A 32-bit signed integer hash code.</returns>
        public override int GetHashCode()
        {
            return Center.GetHashCode() + Radius.GetHashCode();
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

            return Equals((BoundingSphere)obj);
        }

        /// <summary>
        /// Returns a value that indicates whether the current instance is equal to the specified object. 
        /// </summary>
        /// <param name="other">Object to make the comparison with.</param>
        /// <returns><c>true</c> if the current instance is equal to the specified object; <c>false</c> otherwise.</returns>
        public bool Equals(BoundingSphere other)
        {
            return Center == other.Center && Radius == other.Radius;
        }

        /// <summary>
        /// Determines whether the specified object instances are considered equal. 
        /// </summary>
        /// <param name="value1"></param>
        /// <param name="value2"></param>
        /// <returns><c>true</c> if <paramref name="value1"/> is the same instance as <paramref name="value2"/> or 
        /// if both are <c>null</c> references or if <c>value1.Equals(value2)</c> returns <c>true</c>; otherwise, <c>false</c>.</returns>
        public static bool Equals(ref BoundingSphere value1, ref BoundingSphere value2)
        {
            return (value1.Center == value2.Center && value1.Radius == value2.Radius);
        }


        public bool Intersects(BoundingFrustum frustum)
        {
            bool flag;
            if (null == frustum)
            {
                throw new ArgumentNullException("frustum", "BaseTexts.NullNotAllowed");
            }
            frustum.Intersects(ref this, out flag);
            return flag;
        }

        public ContainmentType Contains(BoundingFrustum frustum)
        {
            if (null == frustum)
            {
                throw new ArgumentNullException("frustum", "BaseTexts.NullNotAllowed");
            }
            if (!frustum.Intersects(this))
            {
                return ContainmentType.Disjoint;
            }
            float num2 = this.Radius * this.Radius;
            foreach (Vector3 vector2 in frustum.cornerArray)
            {
                Vector3 vector;
                vector.X = vector2.X - this.Center.X;
                vector.Y = vector2.Y - this.Center.Y;
                vector.Z = vector2.Z - this.Center.Z;
                if (vector.LengthSquared() > num2)
                {
                    return ContainmentType.Intersects;
                }
            }
            return ContainmentType.Contains;
        }

        internal void SupportMapping(ref Vector3 v, out Vector3 result)
        {
            float num2 = v.Length();
            float num = this.Radius / num2;
            result.X = this.Center.X + (v.X * num);
            result.Y = this.Center.Y + (v.Y * num);
            result.Z = this.Center.Z + (v.Z * num);
        }

        //[Obsolete()]
        //public BoundingSphere Transform(Matrix matrix)
        //{
        //    BoundingSphere sphere;
        //    sphere.Center = Vector3.Transform(this.Center, matrix);
        //    float num = ((matrix.M11 + matrix.M22) + matrix.M33) / 3f;
        //    sphere.Radius = this.Radius * num;
        //    return sphere;
        //}

        //[Obsolete()]
        //public void Transform(ref Matrix matrix, out BoundingSphere result)
        //{
        //    result.Center = Vector3.Transform(this.Center, matrix);
        //    float num = ((matrix.M11 + matrix.M22) + matrix.M33) / 3f;
        //    result.Radius = this.Radius * num;
        //}

    }
}
