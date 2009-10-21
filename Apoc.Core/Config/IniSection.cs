using System;
using System.Collections.Generic;
using System.Text;
using Apoc.Vfs;
using System.Drawing;

namespace Apoc.Config
{
    /// <summary>
    ///  表示Ini文件中的段落
    /// </summary>
    public class IniSection : ConfigurationSection
    {
        const char ValueSeprater = ',';
        const char PathSeprater = '|';
        const char Percentage = '%';

        static readonly char[] ValueSepArray = new char[] { ValueSeprater };


        public IniSection(IniConfiguration parent, string name, int capacity)
            : base(parent, name, capacity)
        { }

        public IniSection(IniConfiguration parent, string name, IDictionary<string, string> dictionary)
            : base(parent, name, dictionary)
        { }

        public IniSection(IniConfiguration parent, string name)
            : base(parent, name)
        { }


        public IniSection(string name, int capacity)
            : base(name, capacity)
        { }

        public IniSection(string name, IDictionary<string, string> dictionary)
            : base(name, dictionary)
        { }

        public IniSection(string name)
            : base(name)
        { }

        public override string ToString()
        {
            return base.ToString();
        }

        #region Parser

        //public override CameoInfo GetCameo(FileLocateRule rule)
        //{
        //    CameoInfo res = new CameoInfo();
        //    string cameoName = GetString("Cameo", "xxicon.shp");
        //    string altCameoName = GetString("AltCameo", cameoName);

        //    string ext = Path.GetExtension(cameoName);
        //    if (ext.Length == 0)
        //    {
        //        ext = ".shp";
        //        cameoName += ext;
        //    }
        //    string altExt = Path.GetExtension(altCameoName);
        //    if (altExt.Length == 0)
        //    {
        //        altExt = ".shp";
        //        altCameoName += ext;
        //    }


        //    res.FileNames = new string[] { FileSystem.Cameo_Mix + cameoName, FileSystem.CameoOld_Mix + cameoName };
        //    res.AltFileNames = new string[] { FileSystem.Cameo_Mix + altCameoName, FileSystem.CameoOld_Mix + altCameoName };
        //    res.LocateRule = rule;

        //    res.HasTransparentColor = GetBool("HasTransparentColor", true);

        //    if (CaseInsensitiveStringComparer.Compare(ext, ".shp"))
        //    {
        //        res.UsePalette = true;
        //        res.FilePalettes = new string[] { FileSystem.CacheOldMix + "cameo.pal" };

        //        if (res.HasTransparentColor)
        //        {
        //            res.PaletteTransparentIndex = GetInt("TransparentIndex", 0);
        //        }
        //    }
        //    else
        //    {
        //        if (res.HasTransparentColor)
        //        {
        //            Color tranClr;
        //            if (TryGetColorRGBA("CameoTransparent", out tranClr))
        //            {
        //                res.TransparentColor = tranClr;
        //            }
        //        }
        //    }
        //    return res;
        //}

        //public override UIImageInformation GetImage(string key, FileLocateRule rule)
        //{
        //    UIImageInformation res = new UIImageInformation();
        //    res.FileNames = GetPaths(key + "Image");
        //    res.LocateRule = rule;

        //    string[] pals;
        //    if (TryGetPaths(key + "Palette", out pals))
        //    {
        //        res.UsePalette = true;
        //        res.FilePalettes = pals;

        //        res.Frame = GetInt(key + "Frame", 1) - 1;
        //    }

        //    Color tranClr;
        //    if (TryGetColorRGBA(key + "Transparent", out tranClr))
        //    {
        //        res.HasTransparentColor = true;
        //        res.TransparentColor = tranClr;
        //    }
        //    return res;
        //}

        //public override UIImageInformation GetImage(FileLocateRule rule)
        //{
        //    UIImageInformation res = new UIImageInformation();
        //    res.FileNames = GetPaths("Image");
        //    res.LocateRule = rule;

        //    string[] pals;
        //    if (TryGetPaths("ImagePalette", out pals))
        //    {
        //        res.UsePalette = true;
        //        res.FilePalettes = pals;

        //        res.Frame = GetInt("Frame", 1) - 1;
        //    }

        //    Color tranClr;
        //    if (TryGetColorRGBA("Transparent", out tranClr))
        //    {
        //        res.HasTransparentColor = true;
        //        res.TransparentColor = tranClr;
        //    }
        //    return res;
        //}
        ///// <summary>
        ///// 
        ///// </summary>
        ///// <remarks>SHP是唯一允许的Anim</remarks>
        ///// <param name="rule"></param>
        ///// <returns></returns>
        //public override ImageBase GetImage(FileLocateRule rule)
        //{
        //    FileLocation fl;
        //    ImageBase res;
        //    Color tranClr;

        //    string[] paths = GetPaths("Image");

        //    string[] pals;
        //    if (TryGetPaths("ImagePalette", out pals))
        //    {
        //        fl = FileSystem.Instance.Locate(paths, rule);
        //        ShpAnim shp = AnimManager.Instance.CreateInstance(fl) as ShpAnim; // ShpAnim.FromFile(fl);

        //        if (shp == null)
        //            throw new NotSupportedException();

        //        ResourceLocation pall = FileSystem.Instance.Locate(pals, rule);
        //        shp.Palette = Palette.FromFile(pall);

        //        int frame = GetInt("Frame", 1) - 1;

        //        res = shp.GetImage(frame);
        //        if (TryGetColorRGBA("Transparent", out tranClr))
        //            res.MakeTransparent(tranClr);
        //        shp.Dispose();
        //        return res;
        //    }
        //    fl = FileSystem.Instance.Locate(paths, rule);
        //    res = ImageManager.Instance.CreateInstance(fl);


        //    if (TryGetColorRGBA("Transparent", out tranClr))
        //        res.MakeTransparent(tranClr);
        //    return res;
        //}

        public override bool TryGetSubSection(string key, out ConfigurationSection sect)
        {
            if (ParentConfig == null)
            {
                throw new InvalidOperationException();
            }

            string res;
            if (TryGetValue(key, out res))
            {
                sect = ParentConfig[res];
                return true;
            }
            sect = null;
            return false;
        }
        public override ConfigurationSection GetSubSection(string key)
        {
            if (ParentConfig == null)
            {
                throw new InvalidOperationException();
            }
            return ParentConfig[this[key]];
        }
        public override bool TryGetPaths(string key, out string[] res)
        {
            string v;
            if (TryGetValue(key, out v))
            {
                string[] pams = v.Split(PathSeprater);

                for (int i = 0; i < pams.Length; i++)
                {
                    pams[i] = FileNameTokens.Instance.Filter(pams[i]);
                    //pams[i] = pams[i].Trim();

                    //pams[i] = pams[i].Replace("<s>", Game.Suffix);
                    //pams[i] = pams[i].Replace("<d>", new string(new char[] { Path.DirectorySeparatorChar }));
                    //pams[i] = pams[i].Replace("<mix>", FileSystem.dotMix);
                    //pams[i] = pams[i].Replace("<ra2.mix>", FileSystem.Ra2_Mix);
                    //pams[i] = pams[i].Replace("<theme.mix>", FileSystem.Theme_Mix);
                    //pams[i] = pams[i].Replace("<multi.mix>", FileSystem.Multi_Mix);
                    //pams[i] = pams[i].Replace("<lang.mix>", FileSystem.LangMix);
                    //pams[i] = pams[i].Replace("<local.mix>", FileSystem.LocalMix);
                    //pams[i] = pams[i].Replace("<neutral.mix>", FileSystem.NeutralMix);
                    //pams[i] = pams[i].Replace("<cache.mix>", FileSystem.CacheMix);
                    //pams[i] = pams[i].Replace("<load.mix>", FileSystem.LoadMix);
                    //pams[i] = pams[i].Replace("<conq.mix>", FileSystem.ConquerMix);
                }
                res = pams;
                return true;
            }
            res = null;
            return false;
        }

        public override string[] GetPaths(string key)
        {
            string[] pams = this[key].Split(PathSeprater);
            for (int i = 0; i < pams.Length; i++)
            {
                pams[i] = FileNameTokens.Instance.Filter(pams[i]);
            }
            return pams;
        }
        public override void GetRectangle(string key, out Rectangle rect)
        {
            string v = this[key];
            string[] pams = v.Split(ValueSeprater);
            int len = pams.Length;

            rect = new Rectangle(int.Parse(pams[0]),
                                 int.Parse(pams[1]),
                                 int.Parse(pams[2]),
                                 int.Parse(pams[3]));

        }

        public override bool TryGetColorRGBA(string key, out Color clr)
        {
            string v;
            if (this.TryGetValue(key, out v))
            {
                string[] val = v.Split(ValueSeprater);
                clr = Color.FromArgb(byte.Parse(val[3]), byte.Parse(val[0]), byte.Parse(val[1]), byte.Parse(val[2]));
                return true;
            }

            clr = default(Color);
            return false;
        }
        public override Color GetColorRGBA(string key, Color def)
        {
            string v;
            if (this.TryGetValue(key, out v))
            {
                string[] val = v.Split(ValueSeprater);

                return Color.FromArgb(byte.Parse(val[3]), 
                    byte.Parse(val[0]),
                    byte.Parse(val[1]),
                    byte.Parse(val[2]));
            }

            return def;
        }
        public override Color GetColorRGBA(string key)
        {
            string v = this[key];
            string[] val = v.Split(ValueSeprater);

            return Color.FromArgb(byte.Parse(val[3]), 
                byte.Parse(val[0]), 
                byte.Parse(val[1]), 
                byte.Parse(val[2]));
        }

        public override int GetColorRGBInt(string key)
        {
            string v = this[key];
            string[] val = v.Split(ValueSeprater);

            unchecked
            {
                return ((int)0xff000000 | ((int.Parse(val[0]) & 0xff) << 16) | ((int.Parse(val[1]) & 0xff) << 8) | (int.Parse(val[2]) & 0xff));
            }
        }
        public override int GetColorRGBInt(string key, int def)
        {
            string v;

            if (TryGetValue(key, out v))
            {
                string[] val = v.Split(ValueSeprater);

                unchecked
                {
                    return ((int)0xff000000 | ((int.Parse(val[0]) & 0xff) << 16) | ((int.Parse(val[1]) & 0xff) << 8) | (int.Parse(val[2]) & 0xff));
                }
            }
            return def;
        }

        public override bool TryGetBool(string key, out bool res)
        {
            string v;
            if (this.TryGetValue(key, out v))
            {
                v = v.ToUpperInvariant();
                if (v == "YES")
                    res = true;
                else if (v == "NO")
                    res = false;
                else if (v == "TRUE")
                    res = true;
                else if (v == "FALSE")
                    res = false;
                else
                    res = int.Parse(v) != 0;
                return true;
            }

            res = default(bool);
            return false;
        }
        public override bool GetBool(string key)
        {
            string v = this[key];
            v = v.ToUpperInvariant();
            if (v == "YES")
                return true;
            else if (v == "NO")
                return false;
            else if (v == "TRUE")
                return true;
            else if (v == "FALSE")
                return false;
            else
                return int.Parse(v) != 0;
        }
        public override bool GetBool(string key, bool def)
        {
            string v;
            if (this.TryGetValue(key, out v))
            {
                v = v.ToUpperInvariant();
                if (v == "YES")
                    return true;
                else if (v == "NO")
                    return false;
                else if (v == "TRUE")
                    return true;
                else if (v == "FALSE")
                    return false;
                else
                    return int.Parse(v) != 0;
            }

            return def;
        }

        public override float GetSingle(string key)
        {
            return float.Parse(this[key]);
        }
        public override float GetSingle(string key, float def)
        {
            string v;
            if (this.TryGetValue(key, out v))
                return float.Parse(v);
            else
                return def;
        }

        public override float[] GetSingleArray(string key)
        {
            string v = this[key];
            string[] pams = v.Split(ValueSeprater);
            int len = pams.Length;

            float[] res = new float[len];

            for (int i = 0; i < len; i++)
                res[i] = float.Parse(pams[i]);
            return res;
        }
        public override float[] GetSingleArray(string key, float[] def)
        {
            string v;
            if (this.TryGetValue(key, out v))
            {
                string[] pams = v.Split(ValueSeprater);
                int len = pams.Length;

                float[] res = new float[len];

                for (int i = 0; i < len; i++)
                    res[i] = float.Parse(pams[i]);
                return res;
            }
            else
                return def;
        }

        public override string[] GetStringArray(string key, string[] def)
        {
            string res;
            if (TryGetValue(key, out res))
            {
                string[] arr = res.Split(ValueSepArray, StringSplitOptions.RemoveEmptyEntries);
                for (int i = 0; i < arr.Length; i++)
                {
                    arr[i] = arr[i].Trim();
                }
                return arr;
            }
            return def;
        }
        public override string GetString(string key, string def)
        {
            string res;
            if (TryGetValue(key, out res))
            {
                return res.Trim();
            }
            return def;
        }
        public override string[] GetStringArray(string key)
        {
            string res = this[key];
            string[] arr = res.Split(ValueSeprater);
            for (int i = 0; i < arr.Length; i++)
            {
                arr[i] = arr[i].Trim();
            }
            return arr;
        }

        public override int GetInt(string key)
        {
            return int.Parse(this[key]);
        }
        public override int GetInt(string key, int def)
        {
            string v;
            if (this.TryGetValue(key, out v))
                return int.Parse(v);
            else
                return def;
        }

        public override int[] GetIntArray(string key)
        {
            string v = this[key];

            string[] pams = v.Split(ValueSeprater);
            int len = pams.Length;

            int[] res = new int[len];

            for (int i = 0; i < len; i++)
                res[i] = int.Parse(pams[i]);
            return res;
        }
        public override int[] GetIntArray(string key, int[] def)
        {
            string v;
            if (this.TryGetValue(key, out v))
            {
                string[] pams = v.Split(ValueSeprater);
                int len = pams.Length;

                int[] res = new int[len];

                for (int i = 0; i < len; i++)
                    res[i] = int.Parse(pams[i]);
                return res;
            }
            else
                return def;

        }

        public override Size GetSize(string key)
        {
            string v = this[key];

            string[] pams = v.Split(ValueSeprater);
            int len = pams.Length;

            if (len == 2)
            {
                int[] res = new int[len];

                for (int i = 0; i < len; i++)
                    res[i] = int.Parse(pams[i]);

                return new Size(res[0], res[1]);
            }
            throw new FormatException();
        }
        public override Size GetSize(string key, Size def)
        {
            string v;
            if (this.TryGetValue(key, out v))
            {
                string[] pams = v.Split(ValueSeprater);
                int len = pams.Length;

                if (len == 2)
                {
                    int[] res = new int[len];

                    for (int i = 0; i < len; i++)
                        res[i] = int.Parse(pams[i]);

                    return new Size(res[0], res[1]);
                }
                throw new FormatException();
            }
            else
                return def;
        }

        public override Point GetPoint(string key)
        {
            string v = this[key];

            string[] pams = v.Split(ValueSeprater);
            int len = pams.Length;

            if (len == 2)
            {
                int[] res = new int[len];

                for (int i = 0; i < len; i++)
                    res[i] = int.Parse(pams[i]);

                return new Point(res[0], res[1]);
            }
            throw new FormatException();
        }
        public override Point GetPoint(string key, Point def)
        {
            string v;
            if (this.TryGetValue(key, out v))
            {
                string[] pams = v.Split(ValueSeprater);
                int len = pams.Length;

                if (len == 2)
                {
                    int[] res = new int[len];

                    for (int i = 0; i < len; i++)
                        res[i] = int.Parse(pams[i]);

                    return new Point(res[0], res[1]);
                }
                throw new FormatException();
            }
            else
                return def;
        }


        public override float GetPercentage(string key)
        {
            string val = this[key];
            int pos = val.IndexOf(Percentage);
            return float.Parse(val.Substring(0, val.Length - pos - 1));
        }
        public override float GetPercentage(string key, float def)
        {
            string val;
            if (TryGetValue(key, out val))
            {
                int pos = val.IndexOf(Percentage);
                return float.Parse(val.Substring(0, val.Length - pos)) * 0.01f;
            }
            else
            {
                return def;
            }
        }

        public override float[] GetPercetageArray(string key)
        {
            string[] v = this[key].Split(ValueSeprater);
            float[] res = new float[v.Length];

            for (int i = 0; i < v.Length; i++)
            {
                int pos = v[i].IndexOf(Percentage);
                res[i] = float.Parse(v[i].Substring(0, v[i].Length - pos)) * 0.01f;

            }
            return res;
        }


        public override string GetUIString(string key)
        {
            string v = this[key];
            string[] pams = v.Split(ValueSeprater);
            int len = pams.Length;

            StringBuilder sb = new StringBuilder(len);
            for (int i = 0; i < len; i++)
                sb.Append(StringTableManager.StringTable[pams[i].Trim()]);
            return sb.ToString();
        }
        public override string GetUIString(string key, string def)
        {
            string v;
            if (!this.TryGetValue(key, out v))
                v = def;

            string[] pams = v.Split(ValueSeprater);
            int len = pams.Length;

            StringBuilder sb = new StringBuilder(len);
            for (int i = 0; i < len; i++)
                sb.Append(StringTableManager.StringTable[pams[i].Trim()]);
            return sb.ToString();
        }


        #endregion
    }
}
