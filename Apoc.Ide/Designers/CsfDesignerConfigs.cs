using System;
using System.Collections.Generic;
using System.Text;

using System.IO;
using System.Xml.Serialization;

namespace VirtualBicycle.Ide.Designers
{
    /// <summary>
    /// singleton
    /// </summary> 
    [Serializable()]
    public class CsfDesignerConfigs : DocumentConfigBase
    {
        static readonly string ConfigFile = Path.Combine("Configs", "csfc.xml");
        static CsfDesignerConfigs singleton;


        int spliterDistVert;
        int spliterDistHoz;

        private CsfDesignerConfigs()
        {
            spliterDistVert = 200;
            spliterDistHoz = 240;
        }

        public int SpliterDistVert
        {
            get { return spliterDistVert; }
            set { spliterDistVert = value; }
        }
        public int SpliterDistHoz
        {
            get { return spliterDistHoz; }
            set { spliterDistHoz = value; }
        }

        public static CsfDesignerConfigs Instance
        {
            get
            {
                if (singleton == null)
                    Initialize();
                return singleton;
            }
        }

        

        static void Initialize()
        {
            if (File.Exists(ConfigFile))
            {
                singleton = Serialization.XmlDeserialize<CsfDesignerConfigs>(ConfigFile);
            }
            else
            {
                singleton = new CsfDesignerConfigs();
            }
        }

        protected override void Save()
        {
            Serialization.XmlSerialize<CsfDesignerConfigs>(this, ConfigFile);
        }
    }
}
