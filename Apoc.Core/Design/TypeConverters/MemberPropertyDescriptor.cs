using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Reflection;
using System.Text;

namespace Apoc.Design
{
    internal abstract class MemberPropertyDescriptor : PropertyDescriptor
    {
        // Fields
        private MemberInfo _member;

        // Methods
        public MemberPropertyDescriptor(MemberInfo member)
            : base(member.Name, (Attribute[])member.GetCustomAttributes(typeof(Attribute), true))
        {
            this._member = member;
        }

        public override bool CanResetValue(object component)
        {
            return false;
        }

        public override bool Equals(object obj)
        {
            MemberPropertyDescriptor descriptor = obj as MemberPropertyDescriptor;
            return ((descriptor != null) && descriptor._member.Equals(this._member));
        }

        public override int GetHashCode()
        {
            return this._member.GetHashCode();
        }

        public override void ResetValue(object component)
        {
        }

        public override bool ShouldSerializeValue(object component)
        {
            return true;
        }

        // Properties
        public override Type ComponentType
        {
            get
            {
                return this._member.DeclaringType;
            }
        }

        public override bool IsReadOnly
        {
            get
            {
                return false;
            }
        }
    }
    internal class PropertyPropertyDescriptor : MemberPropertyDescriptor
    {
        // Fields
        private PropertyInfo _property;

        // Methods
        public PropertyPropertyDescriptor(PropertyInfo property)
            : base(property)
        {
            this._property = property;
        }

        public override object GetValue(object component)
        {
            return this._property.GetValue(component, null);
        }

        public override void SetValue(object component, object value)
        {
            this._property.SetValue(component, value, null);
            this.OnValueChanged(component, EventArgs.Empty);
        }

        // Properties
        public override Type PropertyType
        {
            get
            {
                return this._property.PropertyType;
            }
        }
    }


}
