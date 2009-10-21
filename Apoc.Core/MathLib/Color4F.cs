using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Globalization;
using System.Runtime.InteropServices;
using System.Text;
using Apoc.Design;

namespace Apoc.MathLib
{
    /// <summary>
    /// A four-component (RGBA) color value; each component is a float in the range [0,1].
    /// </summary>
    [Serializable, StructLayout(LayoutKind.Sequential), TypeConverter(typeof(Color4FConverter))]
    public struct Color4F
    {
        /// <summary>
        /// Gets or sets the color's red component.
        /// </summary>
        public float Red;

        /// <summary>
        /// Gets or sets the color's green component.
        /// </summary>
        public float Green;

        /// <summary>
        /// Gets or sets the color's blue component.
        /// </summary>
        public float Blue;

        /// <summary>
        /// Gets or sets the color's alpha component.
        /// </summary>
        public float Alpha;

        /// <summary>
        /// Initializes a new instance of the <see cref="Color4F"/> structure.
        /// </summary>
        /// <param name="color">The color whose components should be converted.</param>
        public Color4F(int argb)
        {
            this.Alpha = (float)((argb >> 24) & 0xff) / 255f;
            this.Red = (float)((argb >> 16) & 0xff) / 255f;
            this.Green = (float)((argb >> 8) & 0xff) / 255f;
            this.Blue = (float)(argb & 0xff) / 255f;
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="Color4F"/> structure.
        /// </summary>
        /// <param name="color">The color whose components should be converted.</param>
        public Color4F(Vector4 color)
        {
            this.Alpha = color.W;
            this.Red = color.X;
            this.Green = color.Y;
            this.Blue = color.Z;
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="Color4F"/> structure.
        /// </summary>
        /// <param name="color">The color whose components should be converted.</param>
        public Color4F(Vector3 color)
        {
            this.Alpha = 1f;
            this.Red = color.X;
            this.Green = color.Y;
            this.Blue = color.Z;
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="Color4F"/> structure.
        /// </summary>
        /// <param name="color">The color whose components should be converted.</param>
        public Color4F(ColorValue color)
        {
            this.Alpha = (float)((color.PackedValue >> 24) & 0xff) / 255f;
            this.Red = (float)((color.PackedValue >> 16) & 0xff) / 255f;
            this.Green = (float)((color.PackedValue >> 8) & 0xff) / 255f;
            this.Blue = (float)(color.PackedValue & 0xff) / 255f;
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="Color4F"/> structure.
        /// </summary>
        /// <param name="color">The color whose components should be converted.</param>
        public Color4F(Color color)
        {
            this.Alpha = (float)color.A / 255f;
            this.Red = (float)color.R / 255f;
            this.Green = (float)color.G / 255f;
            this.Blue = (float)color.B / 255f;
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="Color4F"/> structure.
        /// </summary>
        /// <param name="alpha">The alpha component of the color.</param>
        /// <param name="red">The red component of the color.</param>
        /// <param name="green">The green component of the color.</param>
        /// <param name="blue">The blue component of the color.</param>
        public Color4F(int alpha, int red, int green, int blue)
        {
            this.Alpha = (float)alpha / 255f;
            this.Red = (float)red / 255f;
            this.Green = (float)green / 255f;
            this.Blue = (float)blue / 255f;
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="Color4F"/> structure.
        /// </summary>
        /// <param name="red">The red component of the color.</param>
        /// <param name="green">The green component of the color.</param>
        /// <param name="blue">The blue component of the color.</param>
        public Color4F(int red, int green, int blue)
        {
            this.Alpha = 1;
            this.Red = (float)red / 255f;
            this.Green = (float)green / 255f;
            this.Blue = (float)blue / 255f;
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="Color4F"/> structure.
        /// </summary>
        /// <param name="red">The red component of the color.</param>
        /// <param name="green">The green component of the color.</param>
        /// <param name="blue">The blue component of the color.</param>
        public Color4F(float red, float green, float blue)
        {
            this.Alpha = 1f;
            this.Red = red;
            this.Green = green;
            this.Blue = blue;
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="Color4F"/> structure.
        /// </summary>
        /// <param name="alpha">The alpha component of the color.</param>
        /// <param name="red">The red component of the color.</param>
        /// <param name="green">The green component of the color.</param>
        /// <param name="blue">The blue component of the color.</param>
        public Color4F(float alpha, float red, float green, float blue)
        {
            this.Alpha = alpha;
            this.Red = red;
            this.Green = green;
            this.Blue = blue;
        }

        /// <summary>
        /// Converts the color to a <see cref="System::Drawing::Color"/>.
        /// </summary>
        /// <returns>The <see cref="System::Drawing::Color"/> that is equivalent to this instance.</returns>
        public Color ToColor()
        {
            return Color.FromArgb((int)(this.Alpha * 255f), (int)(this.Red * 255f), (int)(this.Green * 255f), (int)(this.Blue * 255f));
        }

        /// <summary>
        /// Converts the color to a <see cref="ColorValue"/>.
        /// </summary>
        /// <returns>The <see cref="ColorValue"/> that is equivalent to this instance.</returns>
        public ColorValue ToColorValue()
        {
            return new ColorValue(this.Alpha, this.Green, this.Blue, this.Alpha);
        }

        /// <summary>
        /// Converts the color into a packed integer.
        /// </summary>
        /// <returns>A packed integer containing all four color components.</returns>
        public int ToArgb()
        {
            uint num6 = (uint)(this.Alpha * 255f);
            uint num5 = (uint)(this.Red * 255f);
            uint num4 = (uint)(this.Green * 255f);
            uint num3 = (uint)(this.Blue * 255f);
            uint num = num3;
            num += num4 << 8;
            num += num5 << 16;
            num += num6 << 24;
            return (int)num;
        }

        /// <summary>
        /// Converts the color into a three component vector.
        /// </summary>
        /// <returns>A three component vector containing the red, green, and blue components of the color.</returns>
        public Vector3 ToVector3()
        {
            return new Vector3(this.Red, this.Green, this.Blue);
        }

        /// <summary>
        /// Converts the color into a four component vector.
        /// </summary>
        /// <returns>A four component vector containing all four color components.</returns>
        public Vector4 ToVector4()
        {
            return new Vector4(this.Red, this.Green, this.Blue, this.Alpha);
        }

        /// <summary>
        /// Adds two colors.
        /// </summary>
        /// <param name="color1">The first color to add.</param>
        /// <param name="color2">The second color to add.</param>
        /// <returns>The sum of the two colors.</returns>
        public static void Add(ref Color4F color1, ref Color4F color2, out Color4F result)
        {
            result = new Color4F(color1.Alpha + color2.Alpha, color1.Red + color2.Red, color1.Green + color2.Green, color1.Blue + color2.Blue);
        }

        /// <summary>
        /// Adds two colors.
        /// </summary>
        /// <param name="color1">The first color to add.</param>
        /// <param name="color2">The second color to add.</param>
        /// <param name="result">When the method completes, contains the sum of the two colors.</param>
        public static Color4F Add(Color4F color1, Color4F color2)
        {
            return new Color4F(color1.Alpha + color2.Alpha, color1.Red + color2.Red, color1.Green + color2.Green, color1.Blue + color2.Blue);
        }

        /// <summary>
        /// Subtracts two colors.
        /// </summary>
        /// <param name="color1">The first color to subtract.</param>
        /// <param name="color2">The second color to subtract.</param>
        /// <returns>The difference between the two colors.</returns>
        public static void Subtract(ref Color4F color1, ref Color4F color2, out Color4F result)
        {
            result = new Color4F(color1.Alpha - color2.Alpha, color1.Red - color2.Red, color1.Green - color2.Green, color1.Blue - color2.Blue);
        }

        /// <summary>
        /// Subtracts two colors.
        /// </summary>
        /// <param name="color1">The first color to subtract.</param>
        /// <param name="color2">The second color to subtract.</param>
        /// <param name="result">When the method completes, contains the difference between the two colors.</param>
        public static Color4F Subtract(Color4F color1, Color4F color2)
        {
            return new Color4F(color1.Alpha - color2.Alpha, color1.Red - color2.Red, color1.Green - color2.Green, color1.Blue - color2.Blue);
        }

        /// <summary>
        /// Modulates two colors.
        /// </summary>
        /// <param name="color1">The first color to modulate.</param>
        /// <param name="color2">The second color to modulate.</param>
        /// <returns>The modulation of the two colors.</returns>
        public static void Modulate(ref Color4F color1, ref Color4F color2, out Color4F result)
        {
            result = new Color4F(color1.Alpha * color2.Alpha, color1.Red * color2.Red, color1.Green * color2.Green, color1.Blue * color2.Blue);
        }

        /// <summary>
        /// Modulates two colors.
        /// </summary>
        /// <param name="color1">The first color to modulate.</param>
        /// <param name="color2">The second color to modulate.</param>
        /// <param name="result">When the method completes, contains the modulation of the two colors.</param>
        public static Color4F Modulate(Color4F color1, Color4F color2)
        {
            return new Color4F(color1.Alpha * color2.Alpha, color1.Red * color2.Red, color1.Green * color2.Green, color1.Blue * color2.Blue);
        }

        /// <summary>
        /// Performs a linear interpolation between two colors.
        /// </summary>
        /// <param name="color1">Start color.</param>
        /// <param name="color2">End color.</param>
        /// <param name="amount">Value between 0 and 1 indicating the weight of <paramref name="color2"/>.</param>
        /// <returns>The linear interpolation of the two colors.</returns>
        /// <remarks>
        /// This method performs the linear interpolation based on the following formula.
        /// <code>color1 + (color2 - color1) * amount</code>
        /// Passing <paramref name="amount"/> a value of 0 will cause <paramref name="color1"/> to be returned; a value of 1 will cause <paramref name="color2"/> to be returned. 
        /// </remarks>
        public static void Lerp(ref Color4F color1, ref Color4F color2, float amount, out Color4F result)
        {
            float alpha = color1.Alpha + (amount * (color2.Alpha - color1.Alpha));
            float red = color1.Red + (amount * (color2.Red - color1.Red));
            float green = color1.Green + (amount * (color2.Green - color1.Green));
            float blue = color1.Blue + (amount * (color2.Blue - color1.Blue));
            result = new Color4F(alpha, red, green, blue);
        }

        /// <summary>
        /// Performs a linear interpolation between two colors.
        /// </summary>
        /// <param name="color1">Start color.</param>
        /// <param name="color2">End color.</param>
        /// <param name="amount">Value between 0 and 1 indicating the weight of <paramref name="color2"/>.</param>
        /// <param name="result">When the method completes, contains the linear interpolation of the two colors.</param>
        /// <remarks>
        /// This method performs the linear interpolation based on the following formula.
        /// <code>color1 + (color2 - color1) * amount</code>
        /// Passing <paramref name="amount"/> a value of 0 will cause <paramref name="color1"/> to be returned; a value of 1 will cause <paramref name="color2"/> to be returned. 
        /// </remarks>
        public static Color4F Lerp(Color4F color1, Color4F color2, float amount)
        {
            float alpha = color1.Alpha + (amount * (color2.Alpha - color1.Alpha));
            float red = color1.Red + (amount * (color2.Red - color1.Red));
            float green = color1.Green + (amount * (color2.Green - color1.Green));
            return new Color4F(alpha, red, green, color1.Blue + (amount * (color2.Blue - color1.Blue)));
        }

        /// <summary>
        /// Negates a color.
        /// </summary>
        /// <param name="color">The color to negate.</param>
        /// <returns>The negated color.</returns>
        public static void Negate(ref Color4F color, out Color4F result)
        {
            result = new Color4F(1.0f - color.Alpha, 1.0f - color.Red, 1.0f - color.Green, 1.0f - color.Blue);
        }

        /// <summary>
        /// Negates a color.
        /// </summary>
        /// <param name="color">The color to negate.</param>
        /// <param name="result">When the method completes, contains the negated color.</param>
        public static Color4F Negate(Color4F color)
        {
            return new Color4F(1.0f - color.Alpha, 1.0f - color.Red, 1.0f - color.Green, 1.0f - color.Blue);
        }

        /// <summary>
        /// Scales a color by the specified amount.
        /// </summary>
        /// <param name="color">The color to scale.</param>
        /// <param name="scale">The amount by which to scale.</param>
        /// <returns>The scaled color.</returns>
        public static void Scale(ref Color4F color, float scale, out Color4F result)
        {
            result = new Color4F(color.Alpha, color.Red * scale, color.Green * scale, color.Blue * scale);
        }

        /// <summary>
        /// Scales a color by the specified amount.
        /// </summary>
        /// <param name="color">The color to scale.</param>
        /// <param name="scale">The amount by which to scale.</param>
        /// <param name="result">When the method completes, contains the scaled color.</param>
        public static Color4F Scale(Color4F color, float scale)
        {
            return new Color4F(color.Alpha, color.Red * scale, color.Green * scale, color.Blue * scale);
        }

        /// <summary>
        /// Adjusts the contrast of a color.
        /// </summary>
        /// <param name="color">The color whose contrast is to be adjusted.</param>
        /// <param name="contrast">The amount by which to adjust the contrast.</param>
        /// <returns>The adjusted color.</returns>
        public static void AdjustContrast(ref Color4F color, float contrast, out Color4F result)
        {
            float r = 0.5f + contrast * (color.Red - 0.5f);
            float g = 0.5f + contrast * (color.Green - 0.5f);
            float b = 0.5f + contrast * (color.Blue - 0.5f);

            result = new Color4F(color.Alpha, r, g, b);
        }

        /// <summary>
        /// Adjusts the contrast of a color.
        /// </summary>
        /// <param name="color">The color whose contrast is to be adjusted.</param>
        /// <param name="contrast">The amount by which to adjust the contrast.</param>
        /// <param name="result">When the method completes, contains the adjusted color.</param>
        public static Color4F AdjustContrast(Color4F color, float contrast)
        {
            float r = 0.5f + contrast * (color.Red - 0.5f);
            float g = 0.5f + contrast * (color.Green - 0.5f);
            float b = 0.5f + contrast * (color.Blue - 0.5f);

            return new Color4F(color.Alpha, r, g, b);
        }

        /// <summary>
        /// Adjusts the saturation of a color.
        /// </summary>
        /// <param name="color">The color whose saturation is to be adjusted.</param>
        /// <param name="saturation">The amount by which to adjust the saturation.</param>
        /// <returns>The adjusted color.</returns>
        public static void AdjustSaturation(ref Color4F color, float saturation, out Color4F result)
        {
            float grey = color.Red * 0.2125f + color.Green * 0.7154f + color.Blue * 0.0721f;
            float r = grey + saturation * (color.Red - grey);
            float g = grey + saturation * (color.Green - grey);
            float b = grey + saturation * (color.Blue - grey);

            result = new Color4F(color.Alpha, r, g, b);
        }

        /// <summary>
        /// Adjusts the saturation of a color.
        /// </summary>
        /// <param name="color">The color whose saturation is to be adjusted.</param>
        /// <param name="saturation">The amount by which to adjust the saturation.</param>
        /// <param name="result">When the method completes, contains the adjusted color.</param>
        public static Color4F AdjustSaturation(Color4F color, float saturation)
        {
            float grey = color.Red * 0.2125f + color.Green * 0.7154f + color.Blue * 0.0721f;
            float r = grey + saturation * (color.Red - grey);
            float g = grey + saturation * (color.Green - grey);
            float b = grey + saturation * (color.Blue - grey);

            return new Color4F(color.Alpha, r, g, b);
        }

        /// <summary>
        /// Adds two colors.
        /// </summary>
        /// <param name="left">The first color to add.</param>
        /// <param name="right">The second color to add.</param>
        /// <returns>The sum of the two colors.</returns>
        public static Color4F operator +(Color4F left, Color4F right)
        {
            return new Color4F(left.Alpha + right.Alpha, left.Red + right.Red, left.Green + right.Green, left.Blue + right.Blue);
        }

        /// <summary>
        /// Subtracts two colors.
        /// </summary>
        /// <param name="left">The first color to subtract.</param>
        /// <param name="right">The second color to subtract.</param>
        /// <returns>The difference between the two colors.</returns>
        public static Color4F operator -(Color4F value)
        {
            return new Color4F(1.0f - value.Alpha, 1.0f - value.Red, 1.0f - value.Green, 1.0f - value.Blue);
        }

        /// <summary>
        /// Negates a color.
        /// </summary>
        /// <param name="value">The color to negate.</param>
        /// <returns>The negated color.</returns>
        public static Color4F operator -(Color4F left, Color4F right)
        {
            return new Color4F(left.Alpha - right.Alpha, left.Red - right.Red, left.Green - right.Green, left.Blue - right.Blue);
        }

        /// <summary>
        /// Scales a color by the specified amount.
        /// </summary>
        /// <param name="value">The color to scale.</param>
        /// <param name="scale">The amount by which to scale.</param>
        /// <returns>The scaled color.</returns>
        public static Color4F operator *(Color4F color1, Color4F color2)
        {
            return new Color4F(color1.Alpha * color2.Alpha, color1.Red * color2.Red, color1.Green * color2.Green, color1.Blue * color2.Blue);
        }

        /// <summary>
        /// Scales a color by the specified amount.
        /// </summary>
        /// <param name="value">The color to scale.</param>
        /// <param name="scale">The amount by which to scale.</param>
        /// <returns>The scaled color.</returns>
        public static Color4F operator *(float scale, Color4F value)
        {
            return new Color4F(value.Alpha, value.Red * scale, value.Green * scale, value.Blue * scale);
        }

        /// <summary>
        /// Modulates two colors.
        /// </summary>
        /// <param name="color1">The first color to modulate.</param>
        /// <param name="color2">The second color to modulate.</param>
        /// <returns>The modulation of the two colors.</returns>
        public static Color4F operator *(Color4F value, float scale)
        {
            return new Color4F(value.Alpha, value.Red * scale, value.Green * scale, value.Blue * scale);
        }

        /// <summary>
        /// Tests for equality between two objects.
        /// </summary>
        /// <param name="left">The first value to compare.</param>
        /// <param name="right">The second value to compare.</param>
        /// <returns><c>true</c> if <paramref name="left"/> has the same value as <paramref name="right"/>; otherwise, <c>false</c>.</returns>
        public static bool operator ==(Color4F left, Color4F right)
        {
            return Equals(ref left, ref right);
        }

        /// <summary>
        /// Tests for inequality between two objects.
        /// </summary>
        /// <param name="left">The first value to compare.</param>
        /// <param name="right">The second value to compare.</param>
        /// <returns><c>true</c> if <paramref name="left"/> has a different value than <paramref name="right"/>; otherwise, <c>false</c>.</returns>
        public static bool operator !=(Color4F left, Color4F right)
        {
            return !Equals(ref left, ref right);
        }

        /// <summary>
        /// Performs an explicit conversion from <see cref="Color4F"/> to <see cref="System::Int32"/>.
        /// </summary>
        /// <param name="value">The value to be converted.</param>
        /// <returns>The converted value.</returns>
        public static explicit operator int(Color4F value)
        {
            return value.ToArgb();
        }

        /// <summary>
        /// Performs an explicit conversion from <see cref="Color4F"/> to <see cref="Vector3"/>.
        /// </summary>
        /// <param name="value">The value to be converted.</param>
        /// <returns>The converted value.</returns>
        public static explicit operator Vector3(Color4F value)
        {
            return value.ToVector3();
        }

        /// <summary>
        /// Performs an explicit conversion from <see cref="Color4F"/> to <see cref="Vector4"/>.
        /// </summary>
        /// <param name="value">The value to be converted.</param>
        /// <returns>The converted value.</returns>
        public static explicit operator Vector4(Color4F value)
        {
            return value.ToVector4();
        }

        /// <summary>
        /// Performs an explicit conversion from <see cref="Color"/> to <see cref="Color4F"/>.
        /// </summary>
        /// <param name="value">The value to be converted.</param>
        /// <returns>The converted value.</returns>
        public static explicit operator Color4F(Color value)
        {
            return new Color4F(value);
        }

        /// <summary>
        /// Performs an explicit conversion from <see cref="System::Int32"/> to <see cref="Color4F"/>.
        /// </summary>
        /// <param name="value">The value to be converted.</param>
        /// <returns>The converted value.</returns>
        public static explicit operator Color4F(Vector4 value)
        {
            return new Color4F(value);
        }

        /// <summary>
        /// Performs an explicit conversion from <see cref="Vector3"/> to <see cref="Color4F"/>.
        /// </summary>
        /// <param name="value">The value to be converted.</param>
        /// <returns>The converted value.</returns>
        public static explicit operator Color4F(Vector3 value)
        {
            return new Color4F(value);
        }

        /// <summary>
        /// Performs an explicit conversion from <see cref="System::Int32"/> to <see cref="Color4F"/>.
        /// </summary>
        /// <param name="value">The value to be converted.</param>
        /// <returns>The converted value.</returns>
        public static explicit operator Color4F(int value)
        {
            return new Color4F(value);
        }

        /// <summary>
        /// Performs an implicit conversion from <see cref="Color4F"/> to <see cref="System::Drawing::Color"/>.
        /// </summary>
        /// <param name="value">The value to be converted.</param>
        /// <returns>The converted value.</returns>
        public static explicit operator Color(Color4F value)
        {
            return value.ToColor();
        }

        /// <summary>
        /// Performs an implicit conversion from <see cref="Color4F"/> to <see cref="ColorValue"/>.
        /// </summary>
        /// <param name="value">The value to be converted.</param>
        /// <returns>The converted value.</returns>
        public static explicit operator ColorValue(Color4F value)
        {
            return new ColorValue(value.Red, value.Green, value.Blue, value.Alpha);
        }

        /// <summary>
        /// Performs an implicit conversion from <see cref="ColorValue"/> to <see cref="Color4F"/>.
        /// </summary>
        /// <param name="value">The value to be converted.</param>
        /// <returns>The converted value.</returns>
        public static explicit operator Color4F(ColorValue value)
        {
            return new Color4F(value);
        }

        /// <summary>
        /// Converts the value of the object to its equivalent string representation.
        /// </summary>
        /// <returns>The string representation of the value of this instance.</returns>
        public override string ToString()
        {
            return String.Format(CultureInfo.CurrentCulture, "A:{0} R:{1} G:{2} B:{3}",
                Alpha.ToString(CultureInfo.CurrentCulture), Red.ToString(CultureInfo.CurrentCulture),
                Green.ToString(CultureInfo.CurrentCulture), Blue.ToString(CultureInfo.CurrentCulture));
        }

        /// <summary>
        /// Returns the hash code for this instance.
        /// </summary>
        /// <returns>A 32-bit signed integer hash code.</returns>
        public override int GetHashCode()
        {
            return this.Alpha.GetHashCode() +
                this.Red.GetHashCode() +
                this.Green.GetHashCode() +
                this.Blue.GetHashCode();
        }

        /// <summary>
        /// Determines whether the specified object instances are considered equal. 
        /// </summary>
        /// <param name="value1"></param>
        /// <param name="value2"></param>
        /// <returns><c>true</c> if <paramref name="value1"/> is the same instance as <paramref name="value2"/> or 
        /// if both are <c>null</c> references or if <c>value1.Equals(value2)</c> returns <c>true</c>; otherwise, <c>false</c>.</returns>
        public static bool Equals(ref Color4F value1, ref Color4F value2)
        {
            return (value1.Alpha == value2.Alpha &&
                value1.Red == value2.Red &&
                value1.Green == value2.Green &&
                value1.Blue == value2.Blue);
        }

        /// <summary>
        /// Returns a value that indicates whether the current instance is equal to the specified object. 
        /// </summary>
        /// <param name="other">Object to make the comparison with.</param>
        /// <returns><c>true</c> if the current instance is equal to the specified object; <c>false</c> otherwise.</returns>
        public bool Equals(Color4F other)
        {
            return (this.Alpha == other.Alpha &&
                this.Red == other.Red &&
                this.Green == other.Green &&
                this.Blue == other.Blue);
        }

        /// <summary>
        /// Returns a value that indicates whether the current instance is equal to a specified object. 
        /// </summary>
        /// <param name="obj">Object to make the comparison with.</param>
        /// <returns><c>true</c> if the current instance is equal to the specified object; <c>false</c> otherwise.</returns>
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
            return this.Equals((Color4F)obj);
        }
    }
}
