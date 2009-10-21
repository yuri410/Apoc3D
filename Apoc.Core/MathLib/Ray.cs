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
    /// Defines a ray in three dimensions, specified by a starting position and a direction.
    /// </summary>
    [Serializable, StructLayout(LayoutKind.Sequential), TypeConverter(typeof(RayConverter))]
    public struct Ray : IEquatable<Ray>
    {
        /// <summary>
        /// Specifies the location of the ray's origin.
        /// </summary>
        public Vector3 Position;

        /// <summary>
        /// A unit vector specifying the direction in which the ray is pointing.
        /// </summary>
        public Vector3 Direction;

        /// <summary>
        /// Initializes a new instance of the <see cref="Ray"/> structure.
        /// </summary>
        /// <param name="position">The location of the ray's origin.</param>
        /// <param name="direction">A unit vector specifying the direction in which the ray is pointing.</param>
        public Ray(Vector3 position, Vector3 direction)
        {
            Position = position;
            Direction = direction;
        }

        /// <summary>
        /// Determines whether a ray intersects the specified object.
        /// </summary>
        /// <param name="ray">The ray which will be tested for intersection.</param>
        /// <param name="plane">A plane that will be tested for intersection.</param>
        /// <param name="distance">When the method completes, contains the distance at which the ray intersected the plane.</param>
        /// <returns><c>true</c> if the ray intersects the plane; otherwise, <c>false</c>.</returns>
        public static bool Intersects(Ray ray, Plane plane, out float distance)
        {
            float dotDirection = (plane.Normal.X * ray.Direction.X) + (plane.Normal.Y * ray.Direction.Y) + (plane.Normal.Z * ray.Direction.Z);

            if (Math.Abs(dotDirection) < 0.000001f)
            {
                distance = 0;
                return false;
            }

            float dotPosition = (plane.Normal.X * ray.Position.X) + (plane.Normal.Y * ray.Position.Y) + (plane.Normal.Z * ray.Position.Z);
            float num = (-plane.D - dotPosition) / dotDirection;

            if (num < 0.0f)
            {
                if (num < -0.000001f)
                {
                    distance = 0;
                    return false;
                }
                num = 0.0f;
            }

            distance = num;
            return true;
        }

        /// <summary>
        /// Determines whether a ray intersects the specified object.
        /// </summary>
        /// <param name="ray">The ray which will be tested for intersection.</param>
        /// <param name="vertex1">The first vertex of a triangle that will be tested for intersection.</param>
        /// <param name="vertex2">The second vertex of a triangle that will be tested for intersection.</param>
        /// <param name="vertex3">The third vertex of a triangle that will be tested for intersection.</param>
        /// <param name="distance">When the method completes, contains the distance at which the ray intersected the plane.</param>
        /// <returns><c>true</c> if the ray intersects the plane; otherwise, <c>false</c>.</returns>
        public static bool Intersects(Ray ray, Vector3 vertex1, Vector3 vertex2, Vector3 vertex3, out float distance)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Determines whether a ray intersects the specified object.
        /// </summary>
        /// <param name="ray">The ray which will be tested for intersection.</param>
        /// <param name="box">A box that will be tested for intersection.</param>
        /// <param name="distance">When the method completes, contains the distance at which the ray intersected the plane.</param>
        /// <returns><c>true</c> if the ray intersects the plane; otherwise, <c>false</c>.</returns>
        public static bool Intersects(Ray ray, BoundingBox box, out float distance)
        {
            float d = 0.0f;
            float maxValue = float.MaxValue;

            if (Math.Abs(ray.Direction.X) < 0.0000001)
            {
                if (ray.Position.X < box.Minimum.X || ray.Position.X > box.Maximum.X)
                {
                    distance = 0.0f;
                    return false;
                }
            }
            else
            {
                float inv = 1.0f / ray.Direction.X;
                float min = (box.Minimum.X - ray.Position.X) * inv;
                float max = (box.Maximum.X - ray.Position.X) * inv;

                if (min > max)
                {
                    float temp = min;
                    min = max;
                    max = temp;
                }

                d = Math.Max(min, d);
                maxValue = Math.Min(max, maxValue);

                if (d > maxValue)
                {
                    distance = 0.0f;
                    return false;
                }
            }

            if (Math.Abs(ray.Direction.Y) < 0.0000001)
            {
                if (ray.Position.Y < box.Minimum.Y || ray.Position.Y > box.Maximum.Y)
                {
                    distance = 0.0f;
                    return false;
                }
            }
            else
            {
                float inv = 1.0f / ray.Direction.Y;
                float min = (box.Minimum.Y - ray.Position.Y) * inv;
                float max = (box.Maximum.Y - ray.Position.Y) * inv;

                if (min > max)
                {
                    float temp = min;
                    min = max;
                    max = temp;
                }

                d = Math.Max(min, d);
                maxValue = Math.Min(max, maxValue);

                if (d > maxValue)
                {
                    distance = 0.0f;
                    return false;
                }
            }

            if (Math.Abs(ray.Direction.Z) < 0.0000001)
            {
                if (ray.Position.Z < box.Minimum.Z || ray.Position.Z > box.Maximum.Z)
                {
                    distance = 0.0f;
                    return false;
                }
            }
            else
            {
                float inv = 1.0f / ray.Direction.Z;
                float min = (box.Minimum.Z - ray.Position.Z) * inv;
                float max = (box.Maximum.Z - ray.Position.Z) * inv;

                if (min > max)
                {
                    float temp = min;
                    min = max;
                    max = temp;
                }

                d = Math.Max(min, d);
                maxValue = Math.Min(max, maxValue);

                if (d > maxValue)
                {
                    distance = 0.0f;
                    return false;
                }
            }

            distance = d;
            return true;
        }

        /// <summary>
        /// Determines whether a ray intersects the specified object.
        /// </summary>
        /// <param name="ray">The ray which will be tested for intersection.</param>
        /// <param name="sphere">A sphere that will be tested for intersection.</param>
        /// <param name="distance">When the method completes, contains the distance at which the ray intersected the plane.</param>
        /// <returns><c>true</c> if the ray intersects the plane; otherwise, <c>false</c>.</returns>
        public static bool Intersects(Ray ray, BoundingSphere sphere, out float distance)
        {
            float x = sphere.Center.X - ray.Position.X;
            float y = sphere.Center.Y - ray.Position.Y;
            float z = sphere.Center.Z - ray.Position.Z;
            float pyth = (x * x) + (y * y) + (z * z);
            float rr = sphere.Radius * sphere.Radius;

            if (pyth <= rr)
            {
                distance = 0.0f;
                return true;
            }

            float dot = (x * ray.Direction.X) + (y * ray.Direction.Y) + (z * ray.Direction.Z);
            if (dot < 0.0f)
            {
                distance = 0.0f;
                return false;
            }

            float temp = pyth - (dot * dot);
            if (temp > rr)
            {
                distance = 0.0f;
                return false;
            }

            distance = dot - (float)Math.Sqrt(rr - temp);
            return true;
        }

        /// <summary>
        /// Tests for equality between two objects.
        /// </summary>
        /// <param name="left">The first value to compare.</param>
        /// <param name="right">The second value to compare.</param>
        /// <returns><c>true</c> if <paramref name="left"/> has the same value as <paramref name="right"/>; otherwise, <c>false</c>.</returns>
        public static bool operator ==(Ray left, Ray right)
        {
            return Ray.Equals(ref left, ref right);
        }

        /// <summary>
        /// Tests for inequality between two objects.
        /// </summary>
        /// <param name="left">The first value to compare.</param>
        /// <param name="right">The second value to compare.</param>
        /// <returns><c>true</c> if <paramref name="left"/> has a different value than <paramref name="right"/>; otherwise, <c>false</c>.</returns>
        public static bool operator !=(Ray left, Ray right)
        {
            return !Ray.Equals(ref left, ref right);
        }

        /// <summary>
        /// Converts the value of the object to its equivalent string representation.
        /// </summary>
        /// <returns>The string representation of the value of this instance.</returns>
        public override string ToString()
        {
            return string.Format(CultureInfo.CurrentCulture, "Position:{0} Direction:{1}", Position.ToString(), Direction.ToString());
        }

        /// <summary>
        /// Returns the hash code for this instance.
        /// </summary>
        /// <returns>A 32-bit signed integer hash code.</returns>
        public override int GetHashCode()
        {
            return Position.GetHashCode() + Direction.GetHashCode();
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

            return Equals((Ray)obj);
        }

        /// <summary>
        /// Returns a value that indicates whether the current instance is equal to the specified object. 
        /// </summary>
        /// <param name="other">Object to make the comparison with.</param>
        /// <returns><c>true</c> if the current instance is equal to the specified object; <c>false</c> otherwise.</returns>
        public bool Equals(Ray other)
        {
            return Position == other.Position && Direction == other.Direction;
        }

        /// <summary>
        /// Determines whether the specified object instances are considered equal. 
        /// </summary>
        /// <param name="value1"></param>
        /// <param name="value2"></param>
        /// <returns><c>true</c> if <paramref name="value1"/> is the same instance as <paramref name="value2"/> or 
        /// if both are <c>null</c> references or if <c>value1.Equals(value2)</c> returns <c>true</c>; otherwise, <c>false</c>.</returns>
        public static bool Equals(ref Ray value1, ref Ray value2)
        {
            return value1.Position == value2.Position && value1.Direction == value2.Direction;
        }
    }
}
