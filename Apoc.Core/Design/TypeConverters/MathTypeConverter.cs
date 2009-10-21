using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;
using System.ComponentModel.Design.Serialization;
using System.Globalization;

namespace Apoc.Design
{
    public class MathTypeConverter : ExpandableObjectConverter
    {
        // Fields
        protected PropertyDescriptorCollection propertyDescriptions;
        protected bool supportStringConvert = true;

        // Methods
        public override bool CanConvertFrom(ITypeDescriptorContext context, Type sourceType)
        {
            return ((this.supportStringConvert && (sourceType == typeof(string))) || base.CanConvertFrom(context, sourceType));
        }

        public override bool CanConvertTo(ITypeDescriptorContext context, Type destinationType)
        {
            return ((destinationType == typeof(InstanceDescriptor)) || base.CanConvertTo(context, destinationType));
        }

        internal static string ConvertFromValues<T>(ITypeDescriptorContext context, CultureInfo culture, T[] values)
        {
            if (culture == null)
            {
                culture = CultureInfo.CurrentCulture;
            }
            string separator = culture.TextInfo.ListSeparator + " ";
            TypeConverter converter = TypeDescriptor.GetConverter(typeof(T));
            string[] strArray = new string[values.Length];
            for (int i = 0; i < values.Length; i++)
            {
                strArray[i] = converter.ConvertToString(context, culture, values[i]);
            }
            return string.Join(separator, strArray);
        }

        internal static T[] ConvertToValues<T>(ITypeDescriptorContext context, CultureInfo culture, object value, int arrayCount, string messageParam)
        {
            string str = value as string;
            if (str == null)
            {
                return null;
            }
            str = str.Trim();
            if (culture == null)
            {
                culture = CultureInfo.CurrentCulture;
            }
            char ch = culture.TextInfo.ListSeparator[0];
            string[] strArray = str.Split(new char[] { ch });
            T[] localArray = new T[strArray.Length];
            TypeConverter converter = TypeDescriptor.GetConverter(typeof(T));
            for (int i = 0; i < localArray.Length; i++)
            {
                try
                {
                    localArray[i] = (T)converter.ConvertFromString(context, culture, strArray[i]);
                }
                catch (Exception exception)
                {
                    throw new ArgumentException(string.Format(CultureInfo.CurrentCulture, "BaseTexts.InvalidStringFormat", new object[] { messageParam }), exception);
                }
            }
            if (localArray.Length != arrayCount)
            {
                throw new ArgumentException(string.Format(CultureInfo.CurrentCulture, "BaseTexts.InvalidStringFormat", new object[] { messageParam }));
            }
            return localArray;
        }

        public override bool GetCreateInstanceSupported(ITypeDescriptorContext context)
        {
            return true;
        }

        public override PropertyDescriptorCollection GetProperties(ITypeDescriptorContext context, object value, Attribute[] attributes)
        {
            return this.propertyDescriptions;
        }

        public override bool GetPropertiesSupported(ITypeDescriptorContext context)
        {
            return true;
        }
    }


}
