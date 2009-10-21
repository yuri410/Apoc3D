using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;

namespace VirtualBicycle.Ide
{
    [AttributeUsage(AttributeTargets.All)]
    internal sealed class LocalizedDescriptionAttribute : DescriptionAttribute
    {
        private bool initialized = false;

        public LocalizedDescriptionAttribute(string key)
            : base(key)
        {
        }

        public override string Description
        {
            get
            {
                if (!initialized)
                {
                    string key = base.Description;
                    DescriptionValue = DevStringTable.Instance[key];
                    if (DescriptionValue == null)
                        DescriptionValue = String.Empty;

                    initialized = true;
                }

                return DescriptionValue;
            }
        }
    }

    [AttributeUsage(AttributeTargets.All)]
    internal sealed class LocalizedCategoryAttribute : CategoryAttribute
    {
        public LocalizedCategoryAttribute(string key)
            : base(key)
        {
        }

        protected override string GetLocalizedString(string key)
        {
            return DevStringTable.Instance[key];
        }
    }
}
