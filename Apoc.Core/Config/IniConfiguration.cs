using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using Apoc.Vfs;

namespace Apoc.Config
{
    /// <summary>
    ///  表示一个Ini配置文件
    /// </summary>
    public class IniConfiguration : Configuration
    {
        static readonly string SegmentL = "[";
        static readonly string SegmentR = "]";
        const char CommetChar = ';';
        //const string ScriptL = "{";
        //const string ScriptR = "}";
        const char Equal = '=';
        //const string ScriptType = "ScriptName";
        //const string Space = " ";

        static readonly string InheritKeyword = "InheritsFrom";

        static readonly char[] EqualArray = new char[] { Equal };

        public void Save(ResourceLocation dest)
        {
            ContentStreamWriter sw = new ContentStreamWriter(dest);

            string keyValue = " = ";
            string sl = SegmentL;
            string sr = SegmentR;

            foreach (KeyValuePair<string, ConfigurationSection> sect in this)
            {
                sw.WriteLine(sl + sect.Key + sr);
                foreach (KeyValuePair<string, string> kw in sect.Value)
                {
                    sw.WriteLine(kw.Key + keyValue + kw.Value);
                }
                sw.WriteLine();
            }

            sw.Close();
        }
        /// <summary>
        /// 仅支持ra2.ini
        /// </summary>
        /// <param name="fileName"></param>
        public void Save(string fileName)
        {
            FileStream fstm = new FileStream(fileName, FileMode.OpenOrCreate, FileAccess.Write, FileShare.Write);
            fstm.SetLength(0);
            StreamWriter sw = new StreamWriter(fstm, Encoding.Default);

            string keyValue = " = ";
            string sl = SegmentL;
            string sr = SegmentR;

            foreach (KeyValuePair<string, ConfigurationSection> sect in this)
            {
                sw.WriteLine(sl + sect.Key + sr);
                foreach (KeyValuePair<string, string> kw in sect.Value)
                {
                    sw.WriteLine(kw.Key + keyValue + kw.Value);
                }
                sw.WriteLine();
            }

            sw.Close();
        }


        public IniConfiguration(string file)
            : this(new FileLocation(file))
        { }

        public IniConfiguration(ResourceLocation file) :
            base(file.Name, CaseInsensitiveStringComparer.Instance)
        {
            ContentStreamReader sr = new ContentStreamReader(file.GetStream, Encoding.Default);
            ConfigurationSection curSect = null;
            string curSectName = string.Empty;

            while (!sr.EndOfStream)
            {
                string line = sr.ReadLine();

                // 去除注释
                int cpos = line.IndexOf(CommetChar);
                if (cpos != -1)
                    line = line.Substring(0, cpos);

                line = line.Trim();

                if (!string.IsNullOrEmpty(line))
                {
                    if (line.StartsWith(SegmentL) & line.EndsWith(SegmentR))
                    {
                        curSectName = line.Substring(1, line.Length - 2).Trim();
                        curSect = new IniSection(this, curSectName);

                        try
                        {
                            Add(curSectName, curSect);
                        }
                        catch (ArgumentException)
                        {
                            EngineConsole.Instance.Write(ConsoleMessageType.Error, "ConfigTexts.CM_IniSegmentRep", Name, curSectName);
                            curSect = this[curSectName];
                        }
                    }
                    else if (curSect != null)
                    {
                        string[] arg = line.Split(EqualArray);

                        if (arg.Length > 1)
                        {
                            string keyword = arg[0].TrimEnd();
                            string value = arg[1].TrimStart();
                            try
                            {
                                curSect.Add(keyword, value);
                            }
                            catch (ArgumentException)
                            {
                                EngineConsole.Instance.Write(ConsoleMessageType.Error, "ConfigTexts.CM_IniKeywordRep", Name, curSectName, keyword);
                                if (!string.IsNullOrEmpty(value))
                                {
                                    curSect.Remove(keyword);
                                    curSect.Add(keyword, value);
                                }
                            }
                        }
                    }
                }
            }

            sr.Close();
            ProcessSectionHierarchy();

        }

        public IniConfiguration(string name, int cap)
            : base(name, cap, CaseInsensitiveStringComparer.Instance)
        { }


        /// <summary>
        ///   Unfold the inherited section
        /// </summary>
        void ProcessSectionHierarchy()
        {
            foreach (KeyValuePair<string, ConfigurationSection> e1 in this)
            {
                ConfigurationSection sect = e1.Value;

                ConfigurationSection curSect = sect;

                string parent;
                while (curSect.TryGetValue(InheritKeyword, out parent))
                {
                    ConfigurationSection parentSect;
                    if (this.TryGetValue(parent, out parentSect))
                    {
                        foreach (KeyValuePair<string, string> e2 in parentSect)
                        {
                            if (!sect.ContainsKey(e2.Key))
                            {
                                sect.Add(e2.Key, e2.Value);
                            }
                        }

                        curSect = parentSect;
                    }
                    else
                    {
                        EngineConsole.Instance.Write(ConsoleMessageType.Warning, "ConfigTexts.CM_IniParentMissing", sect.Name, parent);
                        break;
                    }
                }
            }
        }

        /// <summary>
        ///  见基类
        /// </summary>
        public override void Merge(Configuration config)
        {
            Configuration copy = config.Clone();

            foreach (KeyValuePair<string, ConfigurationSection> e1 in copy)
            {
                ConfigurationSection sect;
                if (!TryGetValue(e1.Key, out sect))
                {
                    Add(e1.Key, e1.Value);
                }
                else
                {
                    foreach (KeyValuePair<string, string> e2 in e1.Value)
                    {
                        if (sect.ContainsKey(e2.Key))
                        {
                            sect.Remove(e2.Key);
                        }

                        sect.Add(e2.Key, e2.Value);

                    }
                }
            }
        }

        /// <summary>
        ///  见基类
        /// </summary>
        public override Configuration Clone()
        {
            IniConfiguration ini = new IniConfiguration(base.Name, this.Count);

            foreach (KeyValuePair<string, ConfigurationSection> e1 in this)
            {
                //Dictionary<string, string> newSectData = new Dictionary<string, string>(e1.Value.Count);
                IniSection newSect = new IniSection(ini, e1.Key, e1.Value.Count);

                foreach (KeyValuePair<string, string> e2 in e1.Value)
                {
                    newSect.Add(e2.Key, e2.Value);
                }

                ini.Add(e1.Key, newSect);
            }

            return ini;
        }
    }

    /// <summary>
    ///  表示Ini配置文件的格式
    /// </summary>
    public class ConfigurationIniFormat : ConfigurationFormat
    {
        public override string[] Filters
        {
            get { return new string[] { ".ini" }; }
        }

        /// <summary>
        ///  从资源中读取Ini配置
        /// </summary>
        /// <param name="rl">表示资源的位置的<see cref="ResourceLocation"/></param>
        /// <returns>一个<see cref="Configuration"/>，表示创建好的配置</returns>
        public override Configuration Load(ResourceLocation rl)
        {
            return new IniConfiguration(rl);
        }
    }
}
