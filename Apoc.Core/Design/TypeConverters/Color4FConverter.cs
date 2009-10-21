using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.ComponentModel.Design.Serialization;
using System.Drawing;
using System.Globalization;
using System.Reflection;
using System.Text;
using Apoc.MathLib;

namespace Apoc.Design
{
    public class Color4FConverter : ExpandableObjectConverter
    {
        // Methods
        public override bool CanConvertFrom(ITypeDescriptorContext context, Type sourceType)
        {
            return ((sourceType == typeof(string)) || base.CanConvertFrom(context, sourceType));
        }

        public override bool CanConvertTo(ITypeDescriptorContext context, Type destinationType)
        {
            return (((destinationType == typeof(string)) || (destinationType == typeof(InstanceDescriptor))) || base.CanConvertTo(context, destinationType));
        }

        public override object ConvertFrom(ITypeDescriptorContext context, CultureInfo culture, object value)
        {
            float red;
            float alpha;
            float green;
            float blue;
            int redi;
            int greeni;
            int bluei;
            int alphai;

            string[] strArray;
            TypeConverter converter = null;

            if (culture == null)
            {
                culture = CultureInfo.CurrentCulture;
            }
            string s = value as string;
            if (s != null)
            {
           
                s = s.Trim();
                converter = TypeDescriptor.GetConverter(typeof(float));
                char[] separator = new char[] { culture.TextInfo.ListSeparator[0] };
                strArray = s.Split(separator);
                if (strArray.Length == 1)
                {
                    int result = 0;
                    if (int.TryParse(s, out result))
                    {
                        return new Color4F(result);
                    }
                    TypeConverter converter2 = TypeDescriptor.GetConverter(typeof(Color));
                    return new Color4F((Color)converter2.ConvertFromString(context, culture, s));
                }
                if (strArray.Length == 3)
                {

                    if ((int.TryParse(strArray[0], out redi) && int.TryParse(strArray[1], out greeni)) && int.TryParse(strArray[2], out bluei))
                    {
                        Color4F type4 = new Color4F(redi, greeni, bluei);
                        return type4;
                    }
                    red = (float)converter.ConvertFromString(context, culture, strArray[1]);
                    green = (float)converter.ConvertFromString(context, culture, strArray[2]);
                    blue = (float)converter.ConvertFromString(context, culture, strArray[3]);
                    return new Color4F(red, green, blue);
                }
                if (strArray.Length != 4)
                {
                    throw new ArgumentException("Invalid color format.");
                }
                if ((int.TryParse(strArray[0], out alphai) && int.TryParse(strArray[1], out redi)) && (int.TryParse(strArray[2], out greeni) && int.TryParse(strArray[3], out bluei)))
                {
                    return new Color4F(alphai, redi, greeni, bluei);
                }
                alpha = (float)converter.ConvertFromString(context, culture, strArray[0]);
                red = (float)converter.ConvertFromString(context, culture, strArray[1]);
                green = (float)converter.ConvertFromString(context, culture, strArray[2]);
                blue = (float)converter.ConvertFromString(context, culture, strArray[3]);
                return new Color4F(alpha, red, green, blue);
            }
            return base.ConvertFrom(context, culture, value);
        }

        public override object ConvertTo(ITypeDescriptorContext context, CultureInfo culture, object value, Type destinationType)
        {
            //ValueType modopt(Color4F) modopt(IsBoxed) type = null;
            Type[] types = null;
            string[] strArray = null;
            TypeConverter converter = null;
            ConstructorInfo member = null;
            string separator = null;
            if (destinationType == null)
            {
                throw new ArgumentNullException("destinationType");
            }
            if (culture == null)
            {
                culture = CultureInfo.CurrentCulture;
            }

            bool isColor4F = value is Color4F;
            if ((destinationType == typeof(string)) && isColor4F)
            {
                Color4F clr = (Color4F)value;
                separator = culture.TextInfo.ListSeparator + " ";
                converter = TypeDescriptor.GetConverter(typeof(float));

                strArray = new string[]
            {
                converter.ConvertToString(context, culture, clr.Alpha), 
                converter.ConvertToString(context, culture, clr.Red),
                converter.ConvertToString(context, culture, clr.Green),
                converter.ConvertToString(context, culture, clr.Blue)
            };

                return string.Join(separator, strArray);
            }
            if ((destinationType == typeof(InstanceDescriptor)) && isColor4F)
            {
                Color4F clr = (Color4F)value;
                types = new Type[] { typeof(float), typeof(float), typeof(float), typeof(float) };
                member = typeof(Color4F).GetConstructor(types);
                if (member != null)
                {
                    return new InstanceDescriptor(member, new object[] { clr.Alpha, clr.Red, clr.Green, clr.Blue });
                }
            }
            return base.ConvertTo(context, culture, value, destinationType);
        }

        public override object CreateInstance(ITypeDescriptorContext context, IDictionary propertyValues)
        {
            if (propertyValues == null)
            {
                throw new ArgumentNullException("propertyValues");
            }

            return new Color4F((float)propertyValues["Alpha"], (float)propertyValues["Red"], (float)propertyValues["Green"], (float)propertyValues["Blue"]);
        }

        public override bool GetCreateInstanceSupported(ITypeDescriptorContext context)
        {
            return true;
        }
    }



}
