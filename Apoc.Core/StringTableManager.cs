using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using Apoc.Vfs;

namespace Apoc
{
    public class StringTableManager
    {
        static StringTable strTbl;
        static StringTableManager singleton;

        public static StringTableManager Instance
        {
            get
            {
                return singleton;
            }
        }
        public static void Initialize()
        {
            if (singleton == null)
            {
                singleton = new StringTableManager();
            }
            EngineConsole.Instance.Write("字符串对照表管理器初始化完毕。", ConsoleMessageType.Information);
        }

        public static StringTable StringTable
        {
            get { return strTbl; }
        }

        Dictionary<string, StringTableFormat> formats = new Dictionary<string, StringTableFormat>(CaseInsensitiveStringComparer.Instance);

        public void Register(StringTableFormat fmt)
        {
            string[] exts = fmt.Filers;
            for (int i = 0; i < exts.Length; i++)
            {
                formats.Add(exts[i], fmt);
            }
        }

        public void LoadStringTable(FileLocation fl)
        {
            if (strTbl == null)
            {
                string ext = Path.GetExtension(fl.Path);

                StringTableFormat fmt;
                if (formats.TryGetValue(ext, out fmt))
                {
                    strTbl = fmt.Load(fl);
                }
                else
                    throw new NotSupportedException(ext);
            }
        }



    }
}
