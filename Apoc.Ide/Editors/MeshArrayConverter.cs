using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing.Design;
using System.Text;
using VirtualBicycle.Ide.Editors.EditableObjects;

namespace VirtualBicycle.Ide.Editors
{
    [Obsolete()]
    public class MeshArrayConverter : TypeConverter
    {
        private class ArrayPropertyDescriptor : TypeConverter.SimplePropertyDescriptor
        {
            // Fields
            private int index;

            // Methods
            public ArrayPropertyDescriptor(Type arrayType, Type elementType, int index)
                : base(arrayType, "[" + index + "]", elementType, new Attribute[] { new EditorAttribute(typeof(EmbededMeshEditor), typeof(UITypeEditor)) })
            {
                this.index = index;
            }

            public override object GetValue(object instance)
            {
                EditableMesh[] list = instance as EditableMesh[];
                if (instance != null)
                {
                    if (list.Length > this.index)
                    {
                        return list[this.index];
                    }
                }
                return null;
            }

            public override void SetValue(object instance, object value)
            {
                EditableMesh[] list = instance as EditableMesh[];

                if (instance != null)
                {
                    if (list.Length > this.index)
                    {
                        list[this.index] = (EditableMesh)value;
                    }
                    this.OnValueChanged(instance, EventArgs.Empty);
                }
            }
        }

        public override PropertyDescriptorCollection GetProperties(ITypeDescriptorContext context, object value, Attribute[] attributes)
        {
            PropertyDescriptor[] properties = null;
            EditableMesh[] list = value as EditableMesh[];
            if (list != null)
            {
                int length = list.Length;
                properties = new PropertyDescriptor[length];
                for (int i = 0; i < length; i++)
                {
                    properties[i] = new ArrayPropertyDescriptor(typeof(EditableMesh[]), typeof(EditableMesh), i);
                }
            }
            return new PropertyDescriptorCollection(properties);
        }
        public override bool GetPropertiesSupported(ITypeDescriptorContext context)
        {
            return true;
        }
    }
}
