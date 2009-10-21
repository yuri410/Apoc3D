using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;

namespace Apoc.MathLib
{
    /// <summary>
    /// Defines a rectangular region of a render-target surface onto which 
    /// a 3D rendering is projected.
    /// </summary>
    public struct Viewport
    {
        int x;
        int y;
        int width;
        int height;
        float minZ;
        float maxZ;

        /// <summary>
        /// Gets or sets the viewport's X position.
        /// </summary>
        public int X
        {
            get { return x; }
            set { x = value; }
        }

        /// <summary>
        /// Gets or sets the viewport's Y position.
        /// </summary>
        public int Y
        {
            get { return y; }
            set { y = value; }
        }

        /// <summary>
        /// Gets or sets the viewport's width.
        /// </summary>
        public int Width
        {
            get { return width; }
            set { width = value; }
        }

        /// <summary>
        /// Gets or sets the viewport's height.
        /// </summary>
        public int Height
        {
            get { return height; }
            set { height = value; }
        }

        /// <summary>
        /// Gets or sets the viewport's minimum Z depth.
        /// </summary>
        public float MinZ
        {
            get { return minZ; }
            set { minZ = value; }
        }

        /// <summary>
        /// Gets or sets the viewport's maximum Z depth.
        /// </summary>
        public float MaxZ
        {
            get { return maxZ; }
            set { maxZ = value; }
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="Viewport"/> structure.
        /// </summary>
        /// <param name="x">The X coordinate of the viewport.</param>
        /// <param name="y">The Y coordinate of the viewport.</param>
        /// <param name="width">The width of the viewport.</param>
        /// <param name="height">The height of the viewport.</param>
        public Viewport(int x, int y, int width, int height)
        {
            this.x = x;
            this.y = y;
            this.width = width;
            this.height = height;

            this.minZ = 0;
            this.maxZ = 1;
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="Viewport"/> structure.
        /// </summary>
        /// <param name="x">The X coordinate of the viewport.</param>
        /// <param name="y">The Y coordinate of the viewport.</param>
        /// <param name="width">The width of the viewport.</param>
        /// <param name="height">The height of the viewport.</param>
        /// <param name="minZ">The minimum Z distance of the viewport.</param>
        /// <param name="maxZ">The maximum Z distance of the viewport.</param>
        public Viewport(int x, int y, int width, int height, float minZ, float maxZ)
        {
            this.x = x;
            this.y = y;
            this.width = width;
            this.height = height;

            this.minZ = minZ;
            this.maxZ = maxZ;
        }

        /// <summary>
        /// Tests for equality between two viewports.
        /// </summary>
        /// <param name="left">The first value to compare.</param>
        /// <param name="right">The second value to compare.</param>
        /// <returns><c>true</c> if <paramref name="left"/> has the same value as <paramref name="right"/>; otherwise, <c>false</c>.</returns>
        public static bool operator ==(Viewport left, Viewport right) 
        {
            return Equals(ref left, ref right);
        }

        /// <summary>
        /// Tests for inequality between two viewports.
        /// </summary>
        /// <param name="left">The first value to compare.</param>
        /// <param name="right">The second value to compare.</param>
        /// <returns><c>true</c> if <paramref name="left"/> has a different value than <paramref name="right"/>; otherwise, <c>false</c>.</returns>
        public static bool operator !=(Viewport left, Viewport right)
        {
            return !Equals(ref left, ref right);
        }

        /// <summary>
        /// Converts the value of the viewport to its equivalent string representation.
        /// </summary>
        /// <returns>The string representation of the value of this instance.</returns>
        public override string ToString()
        {
            return !string.Format(CultureInfo.CurrentCulture, "X:{0} Y:{1} Width:{2} Height:{3} MinZ:{4} MaxZ:{5}",
                X, Y, Width, Height, MinZ, MaxZ);
        }

        /// <summary>
        /// Returns the hash code for this instance.
        /// </summary>
        /// <returns>A 32-bit signed integer hash code.</returns>
        public override int GetHashCode()
        {
            return x.GetHashCode() + y.GetHashCode() + width.GetHashCode()
                 + height.GetHashCode() + minZ.GetHashCode() + maxZ.GetHashCode();
        }

        /// <summary>
        /// Returns a value indicating whether this instance is equal to the specified object.
        /// </summary>
        /// <param name="obj">An object to compare with this instance.</param>
        /// <returns><c>true</c> if <paramref name="obj"/> has the same value as this instance; otherwise, <c>false</c>.</returns>
        public override bool Equals(object obj)
        {
            if (obj == null)
                return false;

            if (obj.GetType() != GetType())
                return false;

            return Equals((Viewport)obj);
        }

        /// <summary>
        /// Returns a value indicating whether this instance is equal to the specified object.
        /// </summary>
        /// <param name="other">A <see cref="Viewport"/> to compare with this instance.</param>
        /// <returns><c>true</c> if <paramref name="other"/> has the same value as this instance; otherwise, <c>false</c>.</returns>
        public bool Equals(Viewport other)
        {
            return (x == other.x && y == other.y && 
                width == other.width && height == other.height &&
                minZ == other.minZ && maxZ == other.maxZ);
        }

        /// <summary>
        /// Returns a value indicating whether the two viewports are equivalent.
        /// </summary>
        /// <param name="value1">The first value to compare.</param>
        /// <param name="value2">The second value to compare.</param>
        /// <returns><c>true</c> if <paramref name="value1"/> has the same value as <paramref name="value2"/>; otherwise, <c>false</c>.</returns>
        public static bool Equals(ref Viewport value1, ref Viewport value2)
        {
            return (value1.x == value2.x && value1.y == value2.y && value1.width == value2.width
                 && value1.height == value2.height && value1.minZ == value2.minZ && value1.maxZ == value2.maxZ);
        }
    }
}
