using System;
using System.Collections.Generic;
using System.Text;
using VirtualBicycle.Ide.Designers;
using VirtualBicycle;
using VirtualBicycle.IO;

namespace VirtualBicycle.Ide
{
    public class DesignerManager
    {
        static DesignerManager singleton;

        public static DesignerManager Instance
        {
            get
            {
                if (singleton == null)
                {
                    singleton = new DesignerManager();
                }
                return singleton;
            }
        }

        Dictionary<string, DesignerAbstractFactory> factories;

        private DesignerManager()
        {
            factories = new Dictionary<string, DesignerAbstractFactory>(CaseInsensitiveStringComparer.Instance);
        }

        public bool RegisterDesigner(DesignerAbstractFactory fac)
        {
            bool passed = true;
            string[] exts = fac.Filters;
            for (int i = 0; i < exts.Length; i++)
            {
                if (!factories.ContainsKey(exts[i]))
                {
                    factories.Add(exts[i], fac);
                }
                else
                {
                    passed = false;
                }
            }
            return passed;
        }

        public void UnregisterDesigner(DesignerAbstractFactory dfac)
        {
            string[] exts = dfac.Filters;
            for (int i = 0; i < exts.Length; i++)
            {
                DesignerAbstractFactory fac;
                if (factories.TryGetValue(exts[i], out fac))
                {
                    if (fac == dfac)
                    {
                        factories.Remove(exts[i]);
                    }
                }
            }
        }


        /// <summary>
        /// 
        /// </summary>
        /// <param name="rl"></param>
        /// <param name="extension">用于识别内容类型的扩展名</param>
        public DocumentBase CreateDocument(ResourceLocation rl, string extension)
        {
            DesignerAbstractFactory fac;
            if (factories.TryGetValue(extension, out fac))
            {
                return fac.CreateInstance(rl);
            }
            else
                throw new NotSupportedException();
        }

        /// <param name="extension">用于识别类型的扩展名</param>
        public DesignerAbstractFactory FindFactory(string ext)
        {
            DesignerAbstractFactory res;
            factories.TryGetValue(ext, out res);
            return res;
        }

        public DesignerAbstractFactory FindFactory(Type type)
        {
            Dictionary<string, DesignerAbstractFactory>.ValueCollection vals = factories.Values;
            foreach (DesignerAbstractFactory fac in vals)
            {
                if (fac.GetType() == type)
                    return fac;
            }
            return null;
        }

        /// <summary>
        /// 获得所有格式的过滤器
        /// </summary>
        /// <returns></returns>
        public Pair<string, string>[] GetAllFormats()
        {
            Dictionary<string, DesignerAbstractFactory>.ValueCollection val = factories.Values;

            List<Pair<string, string>> fmts = new List<Pair<string, string>>();

            DesignerAbstractFactory lastFac = null;
            foreach (DesignerAbstractFactory fac in val)
            {
                if (fac != lastFac)
                {
                    string[] flts = fac.Filters;

                    StringBuilder sb = new StringBuilder();
                    for (int j = 0; j < flts.Length; j++)
                    {
                        sb.Append('*');
                        sb.Append(flts[j]);
                        if (j < flts.Length - 1)
                            sb.Append(';');
                    }

                    fmts.Add(new Pair<string, string>(fac.Description, sb.ToString()));
                    lastFac = fac;
                }
            }

            return fmts.ToArray();
        }

        public string GetFilter()
        {
            Dictionary<string, DesignerAbstractFactory>.ValueCollection val = factories.Values;

            //List<Pair<string, string>> fmts = new List<Pair<string, string>>();
            StringBuilder flt = new StringBuilder(val.Count * 4 + 4);

            DesignerAbstractFactory lastFac = null;
            foreach (DesignerAbstractFactory fac in val)
            {
                if (fac != lastFac)
                {
                    //tring[] flts = fac.Filters;
                    flt.Append(DevUtils.GetFilter(fac.Description, fac.Filters));
                    //StringBuilder sb = new StringBuilder();
                    //for (int j = 0; j < flts.Length; j++)
                    //{
                    //    sb.Append('*');
                    //    sb.Append(flts[j]);
                    //    if (j < flts.Length - 1)
                    //        sb.Append(';');
                    //}
                    //flt.Append(fac.Description);
                    //flt.Append("(" + sb.ToString() + ")|");
                    //flt.Append(sb.ToString());
                    flt.Append('|');
                    //fmts.Add(new Pair<string, string>(fac.ValType, sb.ToString()));

                    lastFac = fac;
                }
            }
            flt.Remove(flt.Length - 1, 1);
            return flt.ToString();

        }
    }
}
