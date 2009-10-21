using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using VirtualBicycle.Ide.Converters;
using VirtualBicycle;

namespace VirtualBicycle.Ide
{
    public class ConverterManager
    {
        static ConverterManager singleton;
        public static ConverterManager Instance
        {
            get
            {
                if (singleton == null)
                    singleton = new ConverterManager();
                return singleton; 
            }
        }


        List<ConverterBase> converters;

        private ConverterManager()
        {
            converters = new List<ConverterBase>();
        }
        public void Register(ConverterBase fac)
        {
            converters.Add(fac);
        }
        public void Unregister(ConverterBase fac) 
        {
            converters.Remove(fac);
        }

        public ConverterBase[] GetAllConverters()
        {
            return converters.ToArray();
        }
        public ConverterBase[] GetConvertersDest(string dstExt)
        {
            List<ConverterBase> res = new List<ConverterBase>(converters.Count);

            for (int i = 0; i < converters.Count; i++)
            {
                string[] dest = converters[i].DestExt;
                for (int j = 0; j < dest.Length; j++)
                {
                    if (CaseInsensitiveStringComparer.Compare(dstExt, dest[j]))
                    {
                        res.Add(converters[i]);
                        break;
                    }
                }
            }

            return res.ToArray();
        }
        public ConverterBase[] GetConvertersSrc(string srcExt)
        {
            List<ConverterBase> res = new List<ConverterBase>(converters.Count);

            for (int i = 0; i < converters.Count; i++)
            {
                string[] source = converters[i].SourceExt;
                for (int j = 0; j < source.Length; j++)
                {
                    if (CaseInsensitiveStringComparer.Compare(srcExt, source[j]))
                    {
                        res.Add(converters[i]);
                        break;
                    }
                }
            }

            return res.ToArray();
        }
        public ConverterBase[] GetConverters(string srcExt, string dstExt)
        {
            List<ConverterBase> res = new List<ConverterBase>(converters.Count);

            for (int i = 0; i < converters.Count; i++)
            {
                string[] source = converters[i].SourceExt;
                string[] dest = converters[i].DestExt;
                bool p1 = false;
                bool p2 = false;
                for (int j = 0; j < source.Length; j++)
                {
                    if (CaseInsensitiveStringComparer.Compare(srcExt, source[j]))
                    {
                        p1 = true;
                        break;
                    }
                }
                
                for (int j = 0; j < dest.Length; j++)
                {
                    if (CaseInsensitiveStringComparer.Compare(dstExt, dest[j]))
                    {
                        p2 = true;
                        break;
                    }
                }

                if (p1 & p2)
                {
                    res.Add(converters[i]);
                }
            }

            return res.ToArray();
        }
    }
}
