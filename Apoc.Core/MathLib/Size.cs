using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Globalization;
using System.Runtime.InteropServices;
using System.Text;
using Apoc.Design;

namespace Apoc.MathLib
{
    [Serializable, StructLayout(LayoutKind.Sequential), TypeConverter(typeof(SizeConverter)), ComVisible(true)]
    public struct Size
    {
        public static readonly Size Empty;
        public int Width;
        public int Height;
        public Size(Point pt)
        {
            this.Width = pt.X;
            this.Height = pt.Y;
        }

        public Size(int width, int height)
        {
            this.Width = width;
            this.Height = height;
        }

        public static Size operator +(Size sz1, Size sz2)
        {
            return Add(sz1, sz2);
        }

        public static Size operator -(Size sz1, Size sz2)
        {
            return Subtract(sz1, sz2);
        }

        public static bool operator ==(Size sz1, Size sz2)
        {
            return ((sz1.Width == sz2.Width) && (sz1.Height == sz2.Height));
        }

        public static bool operator !=(Size sz1, Size sz2)
        {
            return !(sz1 == sz2);
        }

        public static explicit operator Point(Size size)
        {
            return new Point(size.Width, size.Height);
        }
        public static explicit operator System.Drawing.Point(Size size)
        {
            return new System.Drawing.Point(size.Width, size.Height);
        }

        [Browsable(false)]
        public bool IsEmpty
        {
            get
            {
                return ((this.Width == 0) && (this.Height == 0));
            }
        }
        public static Size Add(Size sz1, Size sz2)
        {
            return new Size(sz1.Width + sz2.Width, sz1.Height + sz2.Height);
        }

        //public static Size Ceiling(SizeF value)
        //{
        //    return new Size((int)Math.Ceiling((double)value.Width), (int)Math.Ceiling((double)value.Height));
        //}

        public static Size Subtract(Size sz1, Size sz2)
        {
            return new Size(sz1.Width - sz2.Width, sz1.Height - sz2.Height);
        }

        //public static Size Truncate(SizeF value)
        //{
        //    return new Size((int)value.Width, (int)value.Height);
        //}

        //public static Size Round(SizeF value)
        //{
        //    return new Size((int)Math.Round((double)value.Width), (int)Math.Round((double)value.Height));
        //}

        public override bool Equals(object obj)
        {
            if (!(obj is Size))
            {
                return false;
            }
            Size size = (Size)obj;
            return ((size.Width == this.Width) && (size.Height == this.Height));
        }

        public override int GetHashCode()
        {
            return (this.Width ^ this.Height);
        }

        public override string ToString()
        {
            return ("{Width=" + this.Width.ToString(CultureInfo.CurrentCulture) + ", Height=" + this.Height.ToString(CultureInfo.CurrentCulture) + "}");
        }

        static Size()
        {
            Empty = new Size();
        }

        public static explicit operator System.Drawing.Size(Size s)
        {
            return new System.Drawing.Size(s.Width, s.Height);
        }

        public static explicit operator Size(System.Drawing.Size s)
        {
            return new Size(s.Width, s.Height);
        }
    }



}
