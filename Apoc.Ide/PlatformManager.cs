using System;
using System.Collections.Generic;
using System.Text;

namespace VirtualBicycle.Ide
{
    public static class PresetedPlatform
    {
        public const int VirtualBike = 1;
        public const int YurisRevenge = 1 << 1;
        public const int Ra2Reload = 1 << 2;


        public static string VirtualBikeName
        {
            get { return DevStringTable.Instance["GUI:VB"]; }
        }
        public static string YurisRevengeName
        {
            get { return DevStringTable.Instance["GUI:RA2YR"]; }
        }

        public static string Ra2ReloadName
        {
            get { return DevStringTable.Instance["GUI:RA2RL"]; }
        }
    }

    public class PlatformManager
    {
        static PlatformManager singleton;

        public static PlatformManager Instance
        {
            get
            {
                if (singleton == null)
                {
                    singleton = new PlatformManager();
                }
                return singleton; 
            }
        }

        Dictionary<int, string> platforms;

        private PlatformManager()
        {
            platforms = new Dictionary<int, string>(10);
        }

        public void RegisterPlatform(int fieldCode, string platformName)
        {
            platforms.Add(fieldCode, platformName);
        }

        public void UnregisterPlatform(int fieldCode)
        {
            platforms.Remove(fieldCode);
        }


        public bool Exists(string platformName)
        {
            return platforms.ContainsValue(platformName);
        }
        public bool Exists(int fieldCode)
        {
            return platforms.ContainsKey(fieldCode);
        }

        public string this[int fieldCode]
        {
            get
            {
                return platforms[fieldCode];
            }
        }
        public int Count
        {
            get { return platforms.Count; }
        }
        public KeyValuePair<int, string>[] GetPlatforms()
        {
            KeyValuePair<int, string>[] res = new KeyValuePair<int, string>[platforms.Count];
            int index = 0;
            foreach (KeyValuePair<int, string> e in platforms)
            {
                res[index++] = e;
            }
            return res;
        }
        
    }
}
