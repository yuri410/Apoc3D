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
    ///  A packed 32 bit (RGBA) color value
    /// </summary>
    [Serializable, StructLayout(LayoutKind.Sequential), TypeConverter(typeof(ColorConverter))]
    public struct ColorValue : IPackedVector<uint>, IPackedVector, IEquatable<ColorValue>
    {
        private uint packedValue;

        public ColorValue(uint packedValue)
        {
            this.packedValue = packedValue;
        }

        public ColorValue(int argb)
        {
            packedValue = (uint)argb;
        }
        public ColorValue(byte r, byte g, byte b)
        {
            this.packedValue = PackHelper(r, g, b, (byte)255);
        }


        public ColorValue(int r, int g, int b, int a)
        {
            this.packedValue = PackHelper(r, g, b, a);
        }


        public ColorValue(byte r, byte g, byte b, byte a)
        {
            this.packedValue = PackHelper(r, g, b, a);
        }

        public ColorValue(float r, float g, float b)
        {
            this.packedValue = PackHelper(r, g, b, 1f);
        }

        public ColorValue(float r, float g, float b, float a)
        {
            this.packedValue = PackHelper(r, g, b, a);
        }

        public ColorValue(Vector3 vector)
        {
            this.packedValue = PackHelper(vector.X, vector.Y, vector.Z, 1f);
        }

        public ColorValue(Vector4 vector)
        {
            this.packedValue = PackHelper(vector.X, vector.Y, vector.Z, vector.W);
        }

        public ColorValue(ColorValue rgb, byte a)
        {
            uint num2 = rgb.packedValue & 16777215;
            uint num = (uint)(a << 24);
            this.packedValue = num2 | num;
        }

        public ColorValue(ColorValue rgb, float a)
        {
            uint num2 = rgb.packedValue & 16777215;
            uint num = PackUtils.PackUNorm(255f, a) << 24;
            this.packedValue = num2 | num;
        }

        void IPackedVector.PackFromVector4(Vector4 vector)
        {
            this.packedValue = PackHelper(vector.X, vector.Y, vector.Z, vector.W);
        }

        private static uint PackHelper(float vectorX, float vectorY, float vectorZ, float vectorW)
        {
            uint num4 = PackUtils.PackUNorm(255f, vectorX) << 16;
            uint num3 = PackUtils.PackUNorm(255f, vectorY) << 8;
            uint num2 = PackUtils.PackUNorm(255f, vectorZ);
            uint num = PackUtils.PackUNorm(255f, vectorW) << 24;
            return (((num4 | num3) | num2) | num);
        }

        private static uint PackHelper(byte r, byte g, byte b, byte a)
        {
            uint num4 = (uint)(r << 16);
            uint num3 = (uint)(g << 8);
            uint num2 = b;
            uint num = (uint)(a << 24);
            return (((num4 | num3) | num2) | num);            
        }
        private static uint PackHelper(int r, int g, int b, int a)
        {
            uint num4 = (uint)((r & 0xff) << 16);
            uint num3 = (uint)((g & 0xff) << 8);
            uint num2 = (uint)(b & 0xff);
            uint num = (uint)((a & 0xff) << 24);
            return (num4 | num3 | num2 | num);
        }
        public Vector3 ToVector3()
        {
            Vector3 vector;
            vector.X = PackUtils.UnpackUNorm(255, this.packedValue >> 16);
            vector.Y = PackUtils.UnpackUNorm(255, this.packedValue >> 8);
            vector.Z = PackUtils.UnpackUNorm(255, this.packedValue);
            return vector;
        }

        public Vector4 ToVector4()
        {
            Vector4 vector;
            vector.X = PackUtils.UnpackUNorm(255, this.packedValue >> 16);
            vector.Y = PackUtils.UnpackUNorm(255, this.packedValue >> 8);
            vector.Z = PackUtils.UnpackUNorm(255, this.packedValue);
            vector.W = PackUtils.UnpackUNorm(255, this.packedValue >> 24);
            return vector;
        }

        public byte R
        {
            get
            {
                return (byte)(this.packedValue >> 16);
            }
            set
            {
                this.packedValue = (this.packedValue & uint.MaxValue) | ((uint)(value << 16));
            }
        }
        public byte G
        {
            get
            {
                return (byte)(this.packedValue >> 8);
            }
            set
            {
                this.packedValue = (this.packedValue & uint.MaxValue) | ((uint)(value << 8));
            }
        }
        public byte B
        {
            get
            {
                return (byte)this.packedValue;
            }
            set
            {
                this.packedValue = (this.packedValue & uint.MaxValue) | value;
            }
        }
        public byte A
        {
            get
            {
                return (byte)(this.packedValue >> 24);
            }
            set
            {
                this.packedValue = (this.packedValue & 16777215) | ((uint)(value << 24));
            }
        }
        //[CLSCompliant(false)]
        public uint PackedValue
        {
            get
            {
                return this.packedValue;
            }
            set
            {
                this.packedValue = value;
            }
        }
        public static ColorValue Lerp(ColorValue value1, ColorValue value2, float amount)
        {
            ColorValue color;
            uint packedValue = value1.packedValue;
            uint num2 = value2.packedValue;
            int num7 = (byte)(packedValue >> 16);
            int num6 = (byte)(packedValue >> 8);
            int num5 = (byte)packedValue;
            int num4 = (byte)(packedValue >> 24);
            int num15 = (byte)(num2 >> 16);
            int num14 = (byte)(num2 >> 8);
            int num13 = (byte)num2;
            int num12 = (byte)(num2 >> 24);
            int num = (int)PackUtils.PackUNorm(65536f, amount);
            int num11 = num7 + (((num15 - num7) * num) >> 16);
            int num10 = num6 + (((num14 - num6) * num) >> 16);
            int num9 = num5 + (((num13 - num5) * num) >> 16);
            int num8 = num4 + (((num12 - num4) * num) >> 16);
            color.packedValue = (uint)((((num11 << 16) | (num10 << 8)) | num9) | (num8 << 24));
            return color;
        }

        public override string ToString()
        {
            return string.Format(CultureInfo.CurrentCulture, "{{R:{0} G:{1} B:{2} A:{3}}}", new object[] { this.R, this.G, this.B, this.A });
        }

        public override int GetHashCode()
        {
            return this.packedValue.GetHashCode();
        }

        public override bool Equals(object obj)
        {
            return ((obj is ColorValue) && this.Equals((ColorValue)obj));
        }

        public bool Equals(ColorValue other)
        {
            return this.packedValue.Equals(other.packedValue);
        }

        public static bool operator ==(ColorValue a, ColorValue b)
        {
            return a.Equals(b);
        }

        public static bool operator !=(ColorValue a, ColorValue b)
        {
            return !a.Equals(b);
        }

        public static ColorValue TransparentBlack
        {
            get
            {
                return new ColorValue(0);
            }
        }
        public static ColorValue TransparentWhite
        {
            get
            {
                return new ColorValue(16777215);
            }
        }
        public static ColorValue AliceBlue
        {
            get
            {
                return new ColorValue(4293982463);
            }
        }
        public static ColorValue AntiqueWhite
        {
            get
            {
                return new ColorValue(4294634455);
            }
        }
        public static ColorValue Aqua
        {
            get
            {
                return new ColorValue(4278255615);
            }
        }
        public static ColorValue Aquamarine
        {
            get
            {
                return new ColorValue(4286578644);
            }
        }
        public static ColorValue Azure
        {
            get
            {
                return new ColorValue(4293984255);
            }
        }
        public static ColorValue Beige
        {
            get
            {
                return new ColorValue(4294309340);
            }
        }
        public static ColorValue Bisque
        {
            get
            {
                return new ColorValue(4294960324);
            }
        }
        public static ColorValue Black
        {
            get
            {
                return new ColorValue(4278190080);
            }
        }
        public static ColorValue BlanchedAlmond
        {
            get
            {
                return new ColorValue(4294962125);
            }
        }
        public static ColorValue Blue
        {
            get
            {
                return new ColorValue(4278190335);
            }
        }
        public static ColorValue BlueViolet
        {
            get
            {
                return new ColorValue(4287245282);
            }
        }
        public static ColorValue Brown
        {
            get
            {
                return new ColorValue(4289014314);
            }
        }
        public static ColorValue BurlyWood
        {
            get
            {
                return new ColorValue(4292786311);
            }
        }
        public static ColorValue CadetBlue
        {
            get
            {
                return new ColorValue(4284456608);
            }
        }
        public static ColorValue Chartreuse
        {
            get
            {
                return new ColorValue(4286578432);
            }
        }
        public static ColorValue Chocolate
        {
            get
            {
                return new ColorValue(4291979550);
            }
        }
        public static ColorValue Coral
        {
            get
            {
                return new ColorValue(4294934352);
            }
        }
        public static ColorValue CornflowerBlue
        {
            get
            {
                return new ColorValue(4284782061);
            }
        }
        public static ColorValue Cornsilk
        {
            get
            {
                return new ColorValue(4294965468);
            }
        }
        public static ColorValue Crimson
        {
            get
            {
                return new ColorValue(4292613180);
            }
        }
        public static ColorValue Cyan
        {
            get
            {
                return new ColorValue(4278255615);
            }
        }
        public static ColorValue DarkBlue
        {
            get
            {
                return new ColorValue(4278190219);
            }
        }
        public static ColorValue DarkCyan
        {
            get
            {
                return new ColorValue(4278225803);
            }
        }
        public static ColorValue DarkGoldenrod
        {
            get
            {
                return new ColorValue(4290283019);
            }
        }
        public static ColorValue DarkGray
        {
            get
            {
                return new ColorValue(4289309097);
            }
        }
        public static ColorValue DarkGreen
        {
            get
            {
                return new ColorValue(4278215680);
            }
        }
        public static ColorValue DarkKhaki
        {
            get
            {
                return new ColorValue(4290623339);
            }
        }
        public static ColorValue DarkMagenta
        {
            get
            {
                return new ColorValue(4287299723);
            }
        }
        public static ColorValue DarkOliveGreen
        {
            get
            {
                return new ColorValue(4283788079);
            }
        }
        public static ColorValue DarkOrange
        {
            get
            {
                return new ColorValue(4294937600);
            }
        }
        public static ColorValue DarkOrchid
        {
            get
            {
                return new ColorValue(4288230092);
            }
        }
        public static ColorValue DarkRed
        {
            get
            {
                return new ColorValue(4287299584);
            }
        }
        public static ColorValue DarkSalmon
        {
            get
            {
                return new ColorValue(4293498490);
            }
        }
        public static ColorValue DarkSeaGreen
        {
            get
            {
                return new ColorValue(4287609995);
            }
        }
        public static ColorValue DarkSlateBlue
        {
            get
            {
                return new ColorValue(4282924427);
            }
        }
        public static ColorValue DarkSlateGray
        {
            get
            {
                return new ColorValue(4281290575);
            }
        }
        public static ColorValue DarkTurquoise
        {
            get
            {
                return new ColorValue(4278243025);
            }
        }
        public static ColorValue DarkViolet
        {
            get
            {
                return new ColorValue(4287889619);
            }
        }
        public static ColorValue DeepPink
        {
            get
            {
                return new ColorValue(4294907027);
            }
        }
        public static ColorValue DeepSkyBlue
        {
            get
            {
                return new ColorValue(4278239231);
            }
        }
        public static ColorValue DimGray
        {
            get
            {
                return new ColorValue(4285098345);
            }
        }
        public static ColorValue DodgerBlue
        {
            get
            {
                return new ColorValue(4280193279);
            }
        }
        public static ColorValue Firebrick
        {
            get
            {
                return new ColorValue(4289864226);
            }
        }
        public static ColorValue FloralWhite
        {
            get
            {
                return new ColorValue(4294966000);
            }
        }
        public static ColorValue ForestGreen
        {
            get
            {
                return new ColorValue(4280453922);
            }
        }
        public static ColorValue Fuchsia
        {
            get
            {
                return new ColorValue(4294902015);
            }
        }
        public static ColorValue Gainsboro
        {
            get
            {
                return new ColorValue(4292664540);
            }
        }
        public static ColorValue GhostWhite
        {
            get
            {
                return new ColorValue(4294506751);
            }
        }
        public static ColorValue Gold
        {
            get
            {
                return new ColorValue(4294956800);
            }
        }
        public static ColorValue Goldenrod
        {
            get
            {
                return new ColorValue(4292519200);
            }
        }
        public static ColorValue Gray
        {
            get
            {
                return new ColorValue(4286611584);
            }
        }
        public static ColorValue Green
        {
            get
            {
                return new ColorValue(4278222848);
            }
        }
        public static ColorValue GreenYellow
        {
            get
            {
                return new ColorValue(4289593135);
            }
        }
        public static ColorValue Honeydew
        {
            get
            {
                return new ColorValue(4293984240);
            }
        }
        public static ColorValue HotPink
        {
            get
            {
                return new ColorValue(4294928820);
            }
        }
        public static ColorValue IndianRed
        {
            get
            {
                return new ColorValue(4291648604);
            }
        }
        public static ColorValue Indigo
        {
            get
            {
                return new ColorValue(4283105410);
            }
        }
        public static ColorValue Ivory
        {
            get
            {
                return new ColorValue(4294967280);
            }
        }
        public static ColorValue Khaki
        {
            get
            {
                return new ColorValue(4293977740);
            }
        }
        public static ColorValue Lavender
        {
            get
            {
                return new ColorValue(4293322490);
            }
        }
        public static ColorValue LavenderBlush
        {
            get
            {
                return new ColorValue(4294963445);
            }
        }
        public static ColorValue LawnGreen
        {
            get
            {
                return new ColorValue(4286381056);
            }
        }
        public static ColorValue LemonChiffon
        {
            get
            {
                return new ColorValue(4294965965);
            }
        }
        public static ColorValue LightBlue
        {
            get
            {
                return new ColorValue(4289583334);
            }
        }
        public static ColorValue LightCoral
        {
            get
            {
                return new ColorValue(4293951616);
            }
        }
        public static ColorValue LightCyan
        {
            get
            {
                return new ColorValue(4292935679);
            }
        }
        public static ColorValue LightGoldenrodYellow
        {
            get
            {
                return new ColorValue(4294638290);
            }
        }
        public static ColorValue LightGreen
        {
            get
            {
                return new ColorValue(4287688336);
            }
        }
        public static ColorValue LightGray
        {
            get
            {
                return new ColorValue(4292072403);
            }
        }
        public static ColorValue LightPink
        {
            get
            {
                return new ColorValue(4294948545);
            }
        }
        public static ColorValue LightSalmon
        {
            get
            {
                return new ColorValue(4294942842);
            }
        }
        public static ColorValue LightSeaGreen
        {
            get
            {
                return new ColorValue(4280332970);
            }
        }
        public static ColorValue LightSkyBlue
        {
            get
            {
                return new ColorValue(4287090426);
            }
        }
        public static ColorValue LightSlateGray
        {
            get
            {
                return new ColorValue(4286023833);
            }
        }
        public static ColorValue LightSteelBlue
        {
            get
            {
                return new ColorValue(4289774814);
            }
        }
        public static ColorValue LightYellow
        {
            get
            {
                return new ColorValue(4294967264);
            }
        }
        public static ColorValue Lime
        {
            get
            {
                return new ColorValue(4278255360);
            }
        }
        public static ColorValue LimeGreen
        {
            get
            {
                return new ColorValue(4281519410);
            }
        }
        public static ColorValue Linen
        {
            get
            {
                return new ColorValue(4294635750);
            }
        }
        public static ColorValue Magenta
        {
            get
            {
                return new ColorValue(4294902015);
            }
        }
        public static ColorValue Maroon
        {
            get
            {
                return new ColorValue(4286578688);
            }
        }
        public static ColorValue MediumAquamarine
        {
            get
            {
                return new ColorValue(4284927402);
            }
        }
        public static ColorValue MediumBlue
        {
            get
            {
                return new ColorValue(4278190285);
            }
        }
        public static ColorValue MediumOrchid
        {
            get
            {
                return new ColorValue(4290401747);
            }
        }
        public static ColorValue MediumPurple
        {
            get
            {
                return new ColorValue(4287852763);
            }
        }
        public static ColorValue MediumSeaGreen
        {
            get
            {
                return new ColorValue(4282168177);
            }
        }
        public static ColorValue MediumSlateBlue
        {
            get
            {
                return new ColorValue(4286277870);
            }
        }
        public static ColorValue MediumSpringGreen
        {
            get
            {
                return new ColorValue(4278254234);
            }
        }
        public static ColorValue MediumTurquoise
        {
            get
            {
                return new ColorValue(4282962380);
            }
        }
        public static ColorValue MediumVioletRed
        {
            get
            {
                return new ColorValue(4291237253);
            }
        }
        public static ColorValue MidnightBlue
        {
            get
            {
                return new ColorValue(4279834992);
            }
        }
        public static ColorValue MintCream
        {
            get
            {
                return new ColorValue(4294311930);
            }
        }
        public static ColorValue MistyRose
        {
            get
            {
                return new ColorValue(4294960353);
            }
        }
        public static ColorValue Moccasin
        {
            get
            {
                return new ColorValue(4294960309);
            }
        }
        public static ColorValue NavajoWhite
        {
            get
            {
                return new ColorValue(4294958765);
            }
        }
        public static ColorValue Navy
        {
            get
            {
                return new ColorValue(4278190208);
            }
        }
        public static ColorValue OldLace
        {
            get
            {
                return new ColorValue(4294833638);
            }
        }
        public static ColorValue Olive
        {
            get
            {
                return new ColorValue(4286611456);
            }
        }
        public static ColorValue OliveDrab
        {
            get
            {
                return new ColorValue(4285238819);
            }
        }
        public static ColorValue Orange
        {
            get
            {
                return new ColorValue(4294944000);
            }
        }
        public static ColorValue OrangeRed
        {
            get
            {
                return new ColorValue(4294919424);
            }
        }
        public static ColorValue Orchid
        {
            get
            {
                return new ColorValue(4292505814);
            }
        }
        public static ColorValue PaleGoldenrod
        {
            get
            {
                return new ColorValue(4293847210);
            }
        }
        public static ColorValue PaleGreen
        {
            get
            {
                return new ColorValue(4288215960);
            }
        }
        public static ColorValue PaleTurquoise
        {
            get
            {
                return new ColorValue(4289720046);
            }
        }
        public static ColorValue PaleVioletRed
        {
            get
            {
                return new ColorValue(4292571283);
            }
        }
        public static ColorValue PapayaWhip
        {
            get
            {
                return new ColorValue(4294963157);
            }
        }
        public static ColorValue PeachPuff
        {
            get
            {
                return new ColorValue(4294957753);
            }
        }
        public static ColorValue Peru
        {
            get
            {
                return new ColorValue(4291659071);
            }
        }
        public static ColorValue Pink
        {
            get
            {
                return new ColorValue(4294951115);
            }
        }
        public static ColorValue Plum
        {
            get
            {
                return new ColorValue(4292714717);
            }
        }
        public static ColorValue PowderBlue
        {
            get
            {
                return new ColorValue(4289781990);
            }
        }
        public static ColorValue Purple
        {
            get
            {
                return new ColorValue(4286578816);
            }
        }
        public static ColorValue Red
        {
            get
            {
                return new ColorValue(4294901760);
            }
        }
        public static ColorValue RosyBrown
        {
            get
            {
                return new ColorValue(4290547599);
            }
        }
        public static ColorValue RoyalBlue
        {
            get
            {
                return new ColorValue(4282477025);
            }
        }
        public static ColorValue SaddleBrown
        {
            get
            {
                return new ColorValue(4287317267);
            }
        }
        public static ColorValue Salmon
        {
            get
            {
                return new ColorValue(4294606962);
            }
        }
        public static ColorValue SandyBrown
        {
            get
            {
                return new ColorValue(4294222944);
            }
        }
        public static ColorValue SeaGreen
        {
            get
            {
                return new ColorValue(4281240407);
            }
        }
        public static ColorValue SeaShell
        {
            get
            {
                return new ColorValue(4294964718);
            }
        }
        public static ColorValue Sienna
        {
            get
            {
                return new ColorValue(4288696877);
            }
        }
        public static ColorValue Silver
        {
            get
            {
                return new ColorValue(4290822336);
            }
        }
        public static ColorValue SkyBlue
        {
            get
            {
                return new ColorValue(4287090411);
            }
        }
        public static ColorValue SlateBlue
        {
            get
            {
                return new ColorValue(4285160141);
            }
        }
        public static ColorValue SlateGray
        {
            get
            {
                return new ColorValue(4285563024);
            }
        }
        public static ColorValue Snow
        {
            get
            {
                return new ColorValue(4294966010);
            }
        }
        public static ColorValue SpringGreen
        {
            get
            {
                return new ColorValue(4278255487);
            }
        }
        public static ColorValue SteelBlue
        {
            get
            {
                return new ColorValue(4282811060);
            }
        }
        public static ColorValue Tan
        {
            get
            {
                return new ColorValue(4291998860);
            }
        }
        public static ColorValue Teal
        {
            get
            {
                return new ColorValue(4278222976);
            }
        }
        public static ColorValue Thistle
        {
            get
            {
                return new ColorValue(4292394968);
            }
        }
        public static ColorValue Tomato
        {
            get
            {
                return new ColorValue(4294927175);
            }
        }
        public static ColorValue Turquoise
        {
            get
            {
                return new ColorValue(4282441936);
            }
        }
        public static ColorValue Violet
        {
            get
            {
                return new ColorValue(4293821166);
            }
        }
        public static ColorValue Wheat
        {
            get
            {
                return new ColorValue(4294303411);
            }
        }
        public static ColorValue White
        {
            get
            {
                return new ColorValue(uint.MaxValue);
            }
        }
        public static ColorValue WhiteSmoke
        {
            get
            {
                return new ColorValue(4294309365);
            }
        }
        public static ColorValue Yellow
        {
            get
            {
                return new ColorValue(4294967040);
            }
        }
        public static ColorValue YellowGreen
        {
            get
            {
                return new ColorValue(4288335154);
            }
        }
        public static ColorValue Transparent
        {
            get
            {
                return new ColorValue(0);
            }
        }
        public static explicit operator System.Drawing.Color(ColorValue clr)
        {
            return System.Drawing.Color.FromArgb((int)clr.packedValue);
        }
        public static explicit operator ColorValue(System.Drawing.Color clr) 
        {
            return new ColorValue((uint)clr.ToArgb());
        }
    }


}
