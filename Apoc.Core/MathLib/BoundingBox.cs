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
    ///  An axis aligned bounding box, specified by minimum and maximum vectors.
    /// </summary>
    [Serializable, StructLayout(LayoutKind.Sequential), TypeConverter(typeof(BoundingBoxConverter))]
    public struct BoundingBox : IEquatable<BoundingBox>
    {
        /// <summary>
        /// The highest corner of the box.
        /// </summary>
        public Vector3 Maximum;

        /// <summary>
        /// The lowest corner of the box.
        /// </summary>
        public Vector3 Minimum;

        /// <summary>
        /// Initializes a new instance of the <see cref="BoundingBox"/> structure.
        /// </summary>
        /// <param name="minimum">The lowest corner of the box.</param>
        /// <param name="maximum">The highest corner of the box.</param>
        public BoundingBox(Vector3 minimum, Vector3 maximum)
        {
            this.Minimum = minimum;
            this.Maximum = maximum;
        }

        /// <summary>
        /// Retrieves the eight corners of the bounding box.
        /// </summary>
        /// <returns>An array of points representing the eight corners of the bounding box.</returns>
        public Vector3[] GetCorners()
        {
            Vector3[] vectorArray = new Vector3[8];

            vectorArray[0] = new Vector3(this.Minimum.X, this.Maximum.Y, this.Maximum.Z);
            vectorArray[1] = new Vector3(this.Maximum.X, this.Maximum.Y, this.Maximum.Z);
            vectorArray[2] = new Vector3(this.Maximum.X, this.Minimum.Y, this.Maximum.Z);
            vectorArray[3] = new Vector3(this.Minimum.X, this.Minimum.Y, this.Maximum.Z);
            vectorArray[4] = new Vector3(this.Minimum.X, this.Maximum.Y, this.Minimum.Z);
            vectorArray[5] = new Vector3(this.Maximum.X, this.Maximum.Y, this.Minimum.Z);
            vectorArray[6] = new Vector3(this.Maximum.X, this.Minimum.Y, this.Minimum.Z);
            vectorArray[7] = new Vector3(this.Minimum.X, this.Minimum.Y, this.Minimum.Z);

            return vectorArray;
        }

        /// <summary>
        /// Determines whether the box contains the specified point.
        /// </summary>
        /// <param name="box">The box that will be checked for containment.</param>
        /// <param name="vector">The point that will be checked for containment.</param>
        /// <returns>A member of the <see cref="ContainmentType"/> enumeration indicating whether the two objects intersect, are contained, or don't meet at all.</returns>
        public static ContainmentType Contains(BoundingBox box, Vector3 vector)
        {
            if (box.Minimum.X <= vector.X && vector.X <= box.Maximum.X &&
                box.Minimum.Y <= vector.Y && vector.Y <= box.Maximum.Y &&
                box.Minimum.Z <= vector.Z && vector.Z <= box.Maximum.Z)
            {
                return ContainmentType.Contains;
            }
            return ContainmentType.Disjoint;
        }

        /// <summary>
        /// Determines whether the box contains the specified sphere.
        /// </summary>
        /// <param name="box">The box that will be checked for containment.</param>
        /// <param name="sphere">The sphere that will be checked for containment.</param>
        /// <returns>A member of the <see cref="ContainmentType"/> enumeration indicating whether the two objects intersect, are contained, or don't meet at all.</returns>
        public static ContainmentType Contains(BoundingBox box, BoundingSphere sphere)
        {
            float dist;
            Vector3 clamped;

            Vector3.Clamp(ref sphere.Center, ref  box.Minimum, ref box.Maximum, out clamped);

            float x = sphere.Center.X - clamped.X;
            float y = sphere.Center.Y - clamped.Y;
            float z = sphere.Center.Z - clamped.Z;

            dist = (x * x) + (y * y) + (z * z);
            float radius = sphere.Radius;

            if (dist > (radius * radius))
                return ContainmentType.Disjoint;

            if (box.Minimum.X + radius <= sphere.Center.X && sphere.Center.X <= box.Maximum.X - radius &&
                box.Maximum.X - box.Minimum.X > radius && box.Minimum.Y + radius <= sphere.Center.Y &&
                sphere.Center.Y <= box.Maximum.Y - radius && box.Maximum.Y - box.Minimum.Y > radius &&
                box.Minimum.Z + radius <= sphere.Center.Z && sphere.Center.Z <= box.Maximum.Z - radius &&
                box.Maximum.X - box.Minimum.X > radius)
                return ContainmentType.Contains;

            return ContainmentType.Intersects;
        }

        /// <summary>
        /// Determines whether the box contains the specified box.
        /// </summary>
        /// <param name="box1">The first box that will be checked for containment.</param>
        /// <param name="box2">The second box that will be checked for containment.</param>
        /// <returns>A member of the <see cref="ContainmentType"/> enumeration indicating whether the two objects intersect, are contained, or don't meet at all.</returns>
        public static ContainmentType Contains(BoundingBox box1, BoundingBox box2)
        {
            if ((box1.Maximum.X < box2.Minimum.X) || (box1.Minimum.X > box2.Maximum.X))
            {
                return ContainmentType.Disjoint;
            }
            if ((box1.Maximum.Y < box2.Minimum.Y) || (box1.Minimum.Y > box2.Maximum.Y))
            {
                return ContainmentType.Disjoint;
            }
            if ((box1.Maximum.Z < box2.Minimum.Z) || (box1.Minimum.Z > box2.Maximum.Z))
            {
                return ContainmentType.Disjoint;
            }
            if (box1.Minimum.X <= box2.Minimum.X && box2.Maximum.X <= box1.Maximum.X &&
                box1.Minimum.Y <= box2.Minimum.Y && box2.Maximum.Y <= box1.Maximum.Y &&
                box1.Minimum.Z <= box2.Minimum.Z && box2.Maximum.Z <= box1.Maximum.Z)
            {
                return ContainmentType.Contains;
            }
            return ContainmentType.Intersects;
        }

        /// <summary>
        /// Constructs a <see cref="BoundingBox"/> that fully contains the given points.
        /// </summary>
        /// <param name="points">The points that will be contained by the box.</param>
        /// <returns>The newly constructed bounding box.</returns>
        public static BoundingBox FromPoints(Vector3[] points)
        {
            if (points == null || points.Length <= 0)
                throw new ArgumentNullException("points");

            Vector3 min = new Vector3(float.MaxValue);
            Vector3 max = new Vector3(float.MinValue);

            for (int i = 0; i < points.Length; i++)
            {
                Vector3.Minimize(ref min, ref points[i], out min);
                Vector3.Maximize(ref max, ref points[i], out max);
            }

            return new BoundingBox(min, max);
        }

        internal void SupportMapping(ref Vector3 v, out Vector3 result)
        {
            result.X = (v.X >= 0f) ? this.Maximum.X : this.Minimum.X;
            result.Y = (v.Y >= 0f) ? this.Maximum.Y : this.Minimum.Y;
            result.Z = (v.Z >= 0f) ? this.Maximum.Z : this.Minimum.Z;
        }

        /// <summary>
        /// Constructs a <see cref="BoundingBox"/> from a given sphere.
        /// </summary>
        /// <param name="sphere">The sphere that will designate the extents of the box.</param>
        /// <returns>The newly constructed bounding box.</returns>
        public static BoundingBox FromSphere(BoundingSphere sphere)
        {
            BoundingBox box;
            box.Minimum = new Vector3(
                sphere.Center.X - sphere.Radius,
                sphere.Center.Y - sphere.Radius,
                sphere.Center.Z - sphere.Radius);
            box.Maximum = new Vector3(
                sphere.Center.X + sphere.Radius,
                sphere.Center.Y + sphere.Radius,
                sphere.Center.Z + sphere.Radius);
            return box;
        }

        /// <summary>
        /// Constructs a <see cref="BoundingBox"/> that is the as large as the total combined area of the two specified boxes.
        /// </summary>
        /// <param name="box1">The first box to merge.</param>
        /// <param name="box2">The second box to merge.</param>
        /// <returns>The newly constructed bounding box.</returns>
        public static BoundingBox Merge(BoundingBox box1, BoundingBox box2)
        {
            BoundingBox box3;
            Vector3.Minimize(ref box1.Minimum, ref box2.Minimum, out box3.Minimum);
            Vector3.Maximize(ref box1.Maximum, ref box2.Maximum, out box3.Maximum);
            return box3;
        }

        /// <summary>
        /// Finds the intersection between a plane and a box.
        /// </summary>
        /// <param name="box">The box to check for intersection.</param>
        /// <param name="plane">The source plane.</param>
        /// <returns>A value from the <see cref="PlaneIntersectionType"/> enumeration describing the result of the intersection test.</returns>
        public static PlaneIntersectionType Intersects(BoundingBox box, Plane plane)
        {
            return Plane.Intersects(plane, box);
        }

        /// <summary>
        /// Determines whether a box intersects the specified object.
        /// </summary>
        /// <param name="box">The box which will be tested for intersection.</param>
        /// <param name="ray">The ray that will be tested for intersection.</param>
        /// <param name="distance">When the method completes, contains the distance from the ray's origin in which the intersection with the box occured.</param>
        /// <returns><c>true</c> if the two objects are intersecting; otherwise, <c>false</c>.</returns>
        public static bool Intersects(BoundingBox box, Ray ray, out float distance)
        {
            return Ray.Intersects(ray, box, out distance);
        }

        /// <summary>
        /// Determines whether a box intersects the specified object.
        /// </summary>
        /// <param name="box">The box which will be tested for intersection.</param>
        /// <param name="sphere">The sphere that will be tested for intersection.</param>
        /// <returns><c>true</c> if the two objects are intersecting; otherwise, <c>false</c>.</returns>
        public static bool Intersects(BoundingBox box, BoundingSphere sphere)
        {
            float dist;
            Vector3 clamped;

            Vector3.Clamp(ref sphere.Center, ref box.Minimum, ref box.Maximum, out clamped);

            float x = sphere.Center.X - clamped.X;
            float y = sphere.Center.Y - clamped.Y;
            float z = sphere.Center.Z - clamped.Z;

            dist = (x * x) + (y * y) + (z * z);

            return (dist <= (sphere.Radius * sphere.Radius));
        }

        /// <summary>
        /// Determines whether a box intersects the specified object.
        /// </summary>
        /// <param name="box1">The first box which will be tested for intersection.</param>
        /// <param name="box2">The second box that will be tested for intersection.</param>
        /// <returns><c>true</c> if the two objects are intersecting; otherwise, <c>false</c>.</returns>
        public static bool Intersects(BoundingBox box1, BoundingBox box2)
        {
            if (box1.Maximum.X < box2.Minimum.X || box1.Minimum.X > box2.Maximum.X)
                return false;

            if (box1.Maximum.Y < box2.Minimum.Y || box1.Minimum.Y > box2.Maximum.Y)
                return false;

            return (box1.Maximum.Z >= box2.Minimum.Z && box1.Minimum.Z <= box2.Maximum.Z);
        }

        /// <summary>
        /// Tests for equality between two objects.
        /// </summary>
        /// <param name="left">The first value to compare.</param>
        /// <param name="right">The second value to compare.</param>
        /// <returns><c>true</c> if <paramref name="left"/> has the same value as <paramref name="right"/>; otherwise, <c>false</c>.</returns>
        public static bool operator ==(BoundingBox left, BoundingBox right)
        {
            return Equals(ref left, ref right);
        }

        /// <summary>
        /// Tests for inequality between two objects.
        /// </summary>
        /// <param name="left">The first value to compare.</param>
        /// <param name="right">The second value to compare.</param>
        /// <returns><c>true</c> if <paramref name="left"/> has a different value than <paramref name="right"/>; otherwise, <c>false</c>.</returns>
        public static bool operator !=(BoundingBox left, BoundingBox right)
        {
            return !Equals(ref left, ref right);
        }

        /// <summary>
        /// Converts the value of the object to its equivalent string representation.
        /// </summary>
        /// <returns>The string representation of the value of this instance.</returns>
        public override string ToString()
        {
            return string.Format(CultureInfo.CurrentCulture, "Minimum:{0} Maximum:{1}", Minimum.ToString(), Maximum.ToString());
        }

        /// <summary>
        /// Returns the hash code for this instance.
        /// </summary>
        /// <returns>A 32-bit signed integer hash code.</returns>
        public override int GetHashCode()
        {
            return Minimum.GetHashCode() + Maximum.GetHashCode();
        }

        /// <summary>
        /// Determines whether the specified object instances are considered equal. 
        /// </summary>
        /// <param name="value1"></param>
        /// <param name="value2"></param>
        /// <returns><c>true</c> if <paramref name="value1"/> is the same instance as <paramref name="value2"/> or 
        /// if both are <c>null</c> references or if <c>value1.Equals(value2)</c> returns <c>true</c>; otherwise, <c>false</c>.</returns>
        public static bool Equals(ref BoundingBox value1, ref BoundingBox value2)
        {
            return (value1.Minimum == value2.Minimum && value1.Maximum == value2.Maximum);
        }

        /// <summary>
        /// Returns a value that indicates whether the current instance is equal to the specified object. 
        /// </summary>
        /// <param name="other">Object to make the comparison with.</param>
        /// <returns><c>true</c> if the current instance is equal to the specified object; <c>false</c> otherwise.</returns>
        public bool Equals(BoundingBox value)
        {
            return (Minimum == value.Minimum && Maximum == value.Maximum);
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

            return Equals((BoundingBox)obj);
        }
    }
}
