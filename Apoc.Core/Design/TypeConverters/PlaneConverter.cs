using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.ComponentModel.Design.Serialization;
using System.Globalization;
using System.Reflection;
using System.Text;
using Apoc.MathLib;

namespace Apoc.Design
{
    public class PlaneConverter : MathTypeConverter
    {
        // Methods
        public PlaneConverter()
        {
            Type type = typeof(Plane);
            base.propertyDescriptions = new PropertyDescriptorCollection(new PropertyDescriptor[] { new FieldPropertyDescriptor(type.GetField("Normal")), new FieldPropertyDescriptor(type.GetField("D")) }).Sort(new string[] { "Normal", "D" });
            base.supportStringConvert = false;
        }

        public override object ConvertTo(ITypeDescriptorContext context, CultureInfo culture, object value, Type destinationType)
        {
            if (destinationType == null)
            {
                throw new ArgumentNullException("destinationType");
            }
            if ((destinationType == typeof(InstanceDescriptor)) && (value is Plane))
            {
                Plane plane = (Plane)value;
                ConstructorInfo constructor = typeof(Plane).GetConstructor(new Type[] { typeof(Vector3), typeof(float) });
                if (constructor != null)
                {
                    return new InstanceDescriptor(constructor, new object[] { plane.Normal, plane.D });
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
            return new Plane((Vector3)propertyValues["Normal"], (float)propertyValues["D"]);
        }
    }


}
