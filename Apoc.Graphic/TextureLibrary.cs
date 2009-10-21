using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using SlimDX.Direct3D9;
using Apoc.Config;
using Apoc.Graphics;
using Apoc.IO;

namespace Apoc.Graphics
{
    public class TextureLibrary : Singleton
    {
        static TextureLibrary singleton;


        public static TextureLibrary Instance
        {
            get { return singleton; }
        }

        public static void Initialize(Device device)
        {
            singleton = new TextureLibrary(device);
        }

        protected struct Entry
        {
            public GameTexture colorMap;
            public GameTexture normalMap;

            public string name;
        }

        Device device;
        Dictionary<string, Entry> detailedMaps;

        static string defaultMap = "default";

        public static string DefaultMap
        {
            get { return defaultMap; }
            private set { defaultMap = value; }
        }

        public void LoadTextureSet(FileLocation configLoc)
        {
            Configuration config = ConfigurationManager.Instance.CreateInstance(configLoc);

            ConfigurationSection sect = config["DetailedMapsList"];

            ConfigurationSection.ValueCollection entries = sect.Values;

            foreach (string s in entries)
            {
                Entry entry;
                ConfigurationSection texSect = config[s];

                string fileName = texSect["ColorMap"];
                FileLocation fl = FileSystem.Instance.Locate(Path.Combine(Paths.Textures, fileName), FileLocateRules.Default);

                entry.colorMap = TextureManager.Instance.CreateInstance(device, fl);

                fileName = texSect["NormalMap"];
                fl = FileSystem.Instance.Locate(Path.Combine(Paths.Textures, fileName), FileLocateRules.Default);

                entry.normalMap = TextureManager.Instance.CreateInstance(device, fl);

                entry.name = s;

                detailedMaps.Add(s, entry);
            }

            string msg = "细节纹理库初始化完毕。加载了{0}种纹理。";

            EngineConsole.Instance.Write(string.Format(msg, detailedMaps.Count.ToString()), ConsoleMessageType.Information);
        }

        private TextureLibrary(Device device)
        {
            this.device = device;
            this.detailedMaps = new Dictionary<string, Entry>(CaseInsensitiveStringComparer.Instance);
        }

        public GameTexture GetColorMap(string name)
        {
            return detailedMaps[name].colorMap;
        }

        public GameTexture GetNormalMap(string name)
        {
            return detailedMaps[name].normalMap;
        }

        public string[] GetNames()
        {
            string[] result = new string[detailedMaps.Count];

            int index = 0;
            foreach (KeyValuePair<string, Entry> e in detailedMaps)
            {
                result[index++] = e.Key;
            }
            return result;
        }

        protected override void dispose()
        {
            foreach (KeyValuePair<string, Entry> e in detailedMaps)
            {
                GameTexture tex = e.Value.colorMap;
                TextureManager.Instance.DestroyInstance(tex);
                tex = e.Value.normalMap;
                TextureManager.Instance.DestroyInstance(tex);
            }
            detailedMaps.Clear();

            singleton = null;
        }
    }
}
