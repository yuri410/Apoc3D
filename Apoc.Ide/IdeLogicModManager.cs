using System;
using System.Collections.Generic;
using System.IO;
using System.Reflection;
using System.Text;
using VirtualBicycle;
using VirtualBicycle.IO;
using System.Windows.Forms;

namespace VirtualBicycle.Ide
{
    class IdeLogicModManager : Singleton
    {
        protected override void dispose()
        {
            
        }

        public static void Initialize()
        {
            if (Instance == null)
                Instance = new IdeLogicModManager();
        }

        public static IdeLogicModManager Instance
        {
            get;
            private set;
        }

        public List<VirtualBicycle.Logic.Mod.IPlugin> Plugins
        {
            get;
            private set;
        }

        private IdeLogicModManager() 
        {
            LoadPlugins();
        }



        void LoadPlugins()
        {
            string[] dlls = Directory.GetFiles(Application.StartupPath, "*.lgc");
            Plugins = new List<VirtualBicycle.Logic.Mod.IPlugin>(dlls.Length);

            Type baseType = typeof(VirtualBicycle.Logic.Mod.IPlugin);

            for (int i = 0; i < dlls.Length; i++)
            {
                Assembly am = Assembly.LoadFile(dlls[i]);

                EngineConsole.Instance.Write(string.Format("发现逻辑模块 {0}", dlls[i]), ConsoleMessageType.Information);

                Type[] types = am.GetTypes();

                for (int j = 0; j < types.Length; j++)
                {
                    Type[] interfaces = types[j].GetInterfaces();
                    if (interfaces.Length == 1 && interfaces[0] == baseType)
                    {
                        VirtualBicycle.Logic.Mod.IPlugin plg = (VirtualBicycle.Logic.Mod.IPlugin)Activator.CreateInstance(types[j]);
                        Plugins.Add(plg);
                    }
                }
            }
        }
    }
}
