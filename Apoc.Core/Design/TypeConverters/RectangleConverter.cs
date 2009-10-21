using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.ComponentModel.Design.Serialization;
using System.Reflection;
using System.Text;
using Apoc.MathLib;
using System.Globalization;
using System.Collections;

namespace Apoc.Design
{
    public class RectangleConverter : MathTypeConverter
    {
        // Methods
        public RectangleConverter()
        {
            Type type = typeof(Rectangle);
            PropertyDescriptorCollection descriptors = new PropertyDescriptorCollection(new PropertyDescriptor[] { new FieldPropertyDescriptor(type.GetField("X")), new FieldPropertyDescriptor(type.GetField("Y")), new FieldPropertyDescriptor(type.GetField("Width")), new FieldPropertyDescriptor(type.GetField("Height")) });
            base.propertyDescriptions = descriptors;
            base.supportStringConvert = false;
        }

        public override object ConvertTo(ITypeDescriptorContext context, CultureInfo culture, object value, Type destinationType)
        {
            if (destinationType == null)
            {
                throw new ArgumentNullException("destinationType");
            }
            if ((destinationType == typeof(InstanceDescriptor)) && (value is Rectangle))
            {
                Rectangle rectangle = (Rectangle)value;
                ConstructorInfo constructor = typeof(Rectangle).GetConstructor(new Type[] { typeof(int), typeof(int), typeof(int), typeof(int) });
                if (constructor != null)
                {
                    return new InstanceDescriptor(constructor, new object[] { rectangle.X, rectangle.Y, rectangle.Width, rectangle.Height });
                }
            }
            return base.ConvertTo(context, culture, value, destinationType);
        }

        public override object CreateInstance(ITypeDescriptorContext context, IDictionary propertyValues)
        {
            if (propertyValues == null)
            {
                throw new ArgumentNullException("propertyValues", "BaseTexts.NullNotAllowed");
            }
            return new Rectangle((int)propertyValues["X"], (int)propertyValues["Y"], (int)propertyValues["Width"], (int)propertyValues["Height"]);
        }
    }

 

}
