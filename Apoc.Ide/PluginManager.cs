using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Windows.Forms;
using System.Reflection;

namespace VirtualBicycle.Ide
{
    public delegate void PluginErrorCallback(IPlugin plugin, Exception e);

    public delegate void PluginProgressCallBack(IPlugin plugin, int index, int count);

    public class PluginManager
    {
        static PluginManager singleton;

        public static bool IsInitialized
        {
            get { return singleton != null; }
        }
        public static PluginManager Instance
        {
            get { return singleton; }
        }

        Dictionary<string, IPlugin> plugins;

        public static void Initiailze(PluginErrorCallback errcbk, PluginProgressCallBack prgcbk)
        {
            singleton = new PluginManager();

            string pluginPath = Path.Combine(Application.StartupPath, Paths.PluginPath);
            if (Directory.Exists(pluginPath))
            {
                string[] files = Directory.GetFiles(pluginPath, "*.dll", SearchOption.TopDirectoryOnly);

                Type iplugin = typeof(IPlugin);

                for (int i = 0; i < files.Length; i++)
                {
                    IPlugin obj = null;

                    try
                    {
                        Assembly assembly = Assembly.LoadFile(files[i]);
                        
                        Type[] types = assembly.GetTypes();

                        bool found = false;
                        for (int j = 0; j < types.Length && !found; j++)
                        {
                            Type[] interfaces = types[j].GetInterfaces();

                            for (int k = 0; k < interfaces.Length && !found; k++)
                            {
                                if (interfaces[k] == iplugin)
                                {
                                    obj = (IPlugin)Activator.CreateInstance(types[j]);

                                    try
                                    {
                                        obj.Load();
                                    }
                                    catch (Exception e)
                                    {
                                        if (errcbk != null)
                                        {
                                            errcbk(obj, e);
                                        }
                                    }

                                    singleton.plugins.Add(obj.Name, obj);

                                    found = true;
                                }
                            }
                        }
                    }
                    //catch (FileLoadException)
                    //{

                    //}
                    catch (BadImageFormatException)
                    {

                    }
                    catch (Exception e)
                    {
                        Console.Write(e.Message);
                    }
                    if (prgcbk != null)
                    {
                        prgcbk(obj, i, files.Length);
                    }
                }
            }
        }

        private PluginManager()
        {
            plugins = new Dictionary<string, IPlugin>();
        }

        public int PluginCount
        {
            get { return plugins.Count; }
        }

        public IPlugin GetPlugin(string name) 
        {
            IPlugin result;
            plugins.TryGetValue(name, out result);
            return result;
        }

       
    }
}
