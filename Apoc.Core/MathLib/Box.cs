using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace Apoc.MathLib
{
    /// <summary>
    /// Defines a volume.
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct Box : IEquatable<Box>
    {
        public int Left;
        public int Top;
        public int Right;

        public int Bottom;
        public int Front;
        public int Back;

       
        public Box(int top, int left, int front, int width, int height, int depth)
        {
            this.Top = top;
            this.Left = left;
            this.Front = front;

            this.Bottom = top + height;
            this.Right = left + width;
            this.Back = front + depth;
        }

        public int Width
        {
            get { return Right - Left; }           
        }

        public int Height 
        {
            get { return Bottom - Top; }
        }

        public int Depth
        {
            get { return Back - Front; }
        }


        public static bool operator ==(Box left, Box right)
        {
            return Equals(ref left, ref right);
        }


        public static bool operator !=(Box left, Box right)
        {
            return !Equals(ref left, ref right);
        }

        public override int GetHashCode()
        {
            return Left.GetHashCode() + Top.GetHashCode() + Right.GetHashCode() + 
                Bottom.GetHashCode() + Front.GetHashCode() + Back.GetHashCode();
        }

        public static bool Equals(ref Box value1, ref Box value2)
        {
            if (value1.Left == value2.Left && value1.Top == value2.Top && 
                value1.Right == value2.Right && value1.Bottom == value2.Bottom && 
                value1.Front == value2.Front && value1.Back == value2.Back)
            {
                return true;
            }
            return false;
        }

        public bool Equals(Box other)
        {
            if (this.Left == other.Left && this.Top == other.Top && 
                this.Right == other.Right && this.Bottom == other.Bottom && 
                this.Front == other.Front && this.Back == other.Back)
            {
                return true;
            }
            return false;
        }

        public override bool Equals(object obj)
        {
            if (obj == null)
            {
                return false;
            }
            if (obj.GetType() != base.GetType())
            {
                return false;
            }
            return this.Equals((Box)obj);
        }
    }
}
