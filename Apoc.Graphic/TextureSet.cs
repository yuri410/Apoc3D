using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using SlimDX.Direct3D9;
using VirtualBike.Config;
using VirtualBike.IO;

namespace VirtualBike.Graphics
{
    public class TextureSet
    {
        protected struct Entry
        {
            public GameTexture colorMap;
            public GameTexture normalMap;

            public string name;
        }

        Dictionary<string, Entry> detailedMaps;

        static string defaultMap = "default";

        public static string DefaultMap
        {
            get { return defaultMap; }
            private set { defaultMap = value; }
        }



        public TextureSet(Device device, FileLocation configLoc)
        {
            detailedMaps = new Dictionary<string, Entry>(CaseInsensitiveStringComparer.Instance);

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
    }
}
