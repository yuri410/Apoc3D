using System;
using System.Collections.Generic;
using System.Text;
using Apoc.Core;

namespace Apoc
{
    public class CaseInsensitiveStringComparer : IEqualityComparer<string>
    {
        static CaseInsensitiveStringComparer singleton;

        public static CaseInsensitiveStringComparer Instance
        {
            get
            {
                if (singleton == null)
                {
                    singleton = new CaseInsensitiveStringComparer();
                }
                return singleton;
            }
        }

        public static bool Compare(string a, string b)
        {
            return string.Compare(a, b, StringComparison.OrdinalIgnoreCase) == 0;
        }

        private CaseInsensitiveStringComparer() { }

        #region IEqualityComparer<string> 成员

        public bool Equals(string x, string y)
        {
            return string.Compare(x, y, StringComparison.OrdinalIgnoreCase) == 0;
        }

        public int GetHashCode(string obj)
        {
            return Resource.GetHashCode(obj);
        }

        #endregion
    }
}
