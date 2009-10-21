using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;

namespace Apoc.Config
{
    /// <summary>
    ///  表示配置中的配置段落
    /// </summary>
    public abstract class ConfigurationSection : Dictionary<string, string>
    {
        static readonly string none = "none";

        public static bool IsNone(string str)
        {
            return CaseInsensitiveStringComparer.Compare(str, none);
        }
        public static string CheckNone(string str)
        {
            if (str == null)
                return null;
            if (CaseInsensitiveStringComparer.Compare(str, none))
            {
                return string.Empty;
            }
            return str;
        }
        public static string CheckNoneNull(string str)
        {
            if (str == null)
                return null;
            if (CaseInsensitiveStringComparer.Compare(str, none))
            {
                return null;
            }
            return str;
        }
        public static string[] CheckNone(string[] arr)
        {
            if (arr.Length == 1)
            {
                if (CaseInsensitiveStringComparer.Compare(arr[0], none))
                {
                    return Utils.EmptyStringArray;
                }
                return arr;
            }
            return arr;
        }

        protected Configuration ParentConfig
        {
            get;
            private set;
        }

        public ConfigurationSection(Configuration parent, string name)
        {
            this.ParentConfig = parent;
            Name = name;
        }
        public ConfigurationSection(Configuration parent, string name, IDictionary<string, string> dictionary)
            : base(dictionary)
        {
            this.ParentConfig = parent;
            Name = name;
        }

        public ConfigurationSection(Configuration parent, string name, IEqualityComparer<string> comparer)
            : base(comparer)
        {
            this.ParentConfig = parent;
            Name = name;
        }

        public ConfigurationSection(Configuration parent, string name, int capacity)
            : base(capacity)
        {
            this.ParentConfig = parent;
            Name = name;
        }

        public ConfigurationSection(string name)
            : this(null, name)
        {
        }
        public ConfigurationSection(string name, IDictionary<string, string> dictionary)
            : this((Configuration)null, name, dictionary)
        {
        }

        public ConfigurationSection(string name, IEqualityComparer<string> comparer)
            : this((Configuration)null, name, comparer)
        {
        }

        public ConfigurationSection(string name, int capacity)
            : this((Configuration)null, name, capacity)
        {
        }

        public string Name
        {
            get;
            private set;
        }

        ///// <summary>
        ///// 
        ///// </summary>
        ///// <remarks>SHP是唯一允许的Anim</remarks>
        ///// <param name="rule"></param>
        ///// <returns></returns>
        //public abstract CameoInfo GetCameo(FileLocateRule rule);
        //public abstract UIImageInformation GetImage(FileLocateRule rule);

        //public abstract UIImageInformation GetImage(string key, FileLocateRule rule);

        public abstract ConfigurationSection GetSubSection(string key);
        public abstract bool TryGetSubSection(string key, out ConfigurationSection sect);


        public abstract bool TryGetPaths(string key, out string[] res);

        public abstract string[] GetPaths(string key);
        public abstract void GetRectangle(string key, out Rectangle rect);

        public abstract bool TryGetColorRGBA(string key, out Color clr);
        public abstract Color GetColorRGBA(string key, Color def);
        public abstract Color GetColorRGBA(string key);
        public abstract int GetColorRGBInt(string key);
        public abstract int GetColorRGBInt(string key, int def);

        //public abstract int GetColorRGB

        public abstract bool TryGetBool(string key, out bool res);
        public abstract bool GetBool(string key);
        public abstract bool GetBool(string key, bool def);

        public abstract float GetSingle(string key);
        public abstract float GetSingle(string key, float def);

        public abstract float[] GetSingleArray(string key);
        public abstract float[] GetSingleArray(string key, float[] def);

        //public abstract string GetUIString(string key);
        //public abstract string GetUIString(string key, string def);

        public abstract string GetString(string key, string def);

        public abstract string[] GetStringArray(string key);
        public abstract string[] GetStringArray(string key, string[] def);

        public abstract int GetInt(string key);
        public abstract int GetInt(string key, int def);

        public abstract int[] GetIntArray(string key);
        public abstract int[] GetIntArray(string key, int[] def);

        public abstract Size GetSize(string key);
        public abstract Size GetSize(string key, Size def);

        public abstract Point GetPoint(string key);
        public abstract Point GetPoint(string key, Point def);


        public abstract float GetPercentage(string key);
        public abstract float GetPercentage(string key, float def);

        public abstract float[] GetPercetageArray(string key);

        public abstract string GetUIString(string key);
        public abstract string GetUIString(string key, string def);
        
        
        
    }
}
