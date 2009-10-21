using System;
using System.IO;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using Apoc.Config;
using Apoc.Vfs;

namespace Apoc.ConfigModel
{
    public abstract class BasicConfigFactory
    {
        public abstract BasicConfigBase CreateInstance(ConfigurationSection sect);

        public abstract string Name { get; }
    }

    public sealed class AudioConfigFacotry : BasicConfigFactory
    {
        public static string AudioConfigName
        {
            get;
            private set;
        }

        static AudioConfigFacotry()
        {
            AudioConfigName = "Audio";
        }

        public override BasicConfigBase CreateInstance(ConfigurationSection sect)
        {
            return new AudioConfigs(sect);
        }

        public override string Name
        {
            get { return AudioConfigName; }
        }
    }
    public sealed class VideoConfigFactory : BasicConfigFactory
    {
        public static string VideoConfigName
        {
            get;
            private set;
        }

        static VideoConfigFactory()
        {
            VideoConfigName = "Video";
        }

        public override BasicConfigBase CreateInstance(ConfigurationSection sect)
        {
            return new VideoConfigs(sect);
        }

        public override string Name
        {
            get { return VideoConfigName; }
        }
    }

    public abstract class BasicConfigBase
    {
        public abstract void Write(ConfigurationSection sect);

        public abstract string Name
        {
            get;
        }
    }

    public sealed class AudioConfigs : BasicConfigBase
    {
        static readonly string SoundVolumeKey = "SoundVolume";
        static readonly string VoiceVolumeKey = "VoiceVolume";
        static readonly string ScoreVolumeKey = "ScoreVolume";
        static readonly string InGameMusicKey = "InGameMusic";
        static readonly string IsScoreRepeatKey = "IsScoreRepeat";
        static readonly string IsScoreShuffleKey = "IsScoreShuffle";


        public AudioConfigs(ConfigurationSection sect)
        {
            SoundVolume = sect.GetSingle(SoundVolumeKey, 0.7f);
            VoiceVolume = sect.GetSingle(VoiceVolumeKey, 0.7f);
            ScoreVolume = sect.GetSingle(ScoreVolumeKey, 0.3f);

            InGameMusic = sect.GetBool(InGameMusicKey, true);
            IsScoreRepeat = sect.GetBool(IsScoreRepeatKey, false);
            IsScoreShuffle = sect.GetBool(IsScoreShuffleKey, true);

        }

        public override string Name
        {
            get { return AudioConfigFacotry.AudioConfigName; }
        }

        public float SoundVolume
        {
            get;
            set;
        }
        public float VoiceVolume
        {
            get;
            set;
        }
        public float ScoreVolume
        {
            get;
            set;
        }
        public bool IsScoreRepeat
        {
            get;
            set;
        }
        public bool IsScoreShuffle
        {
            get;
            set;
        }
        public bool InGameMusic
        {
            get;
            set;
        }

        public override void Write(ConfigurationSection sect)
        {
            sect.Add(SoundVolumeKey, SoundVolume.ToString("#.######"));
            sect.Add(VoiceVolumeKey, VoiceVolume.ToString("#.######"));
            sect.Add(ScoreVolumeKey, ScoreVolume.ToString("#.######"));
            sect.Add(IsScoreRepeatKey, IsScoreRepeat.ToString());
            sect.Add(IsScoreShuffleKey, IsScoreShuffle.ToString());
            sect.Add(InGameMusicKey, InGameMusic.ToString());

        }

    }
    public sealed class VideoConfigs : BasicConfigBase
    {
        static readonly string ScreenHeightKey = "ScreenHeight";
        static readonly string ScreenWidthKey = "ScreenWidth";
        static readonly string StretchMoviestKey = "StretchMovies";
        static readonly string FullScreenKey = "FullScreen";

        public VideoConfigs(ConfigurationSection sect)
        {
            try
            {
                ScreenHeight = int.Parse(sect[ScreenHeightKey]);
                ScreenWidth = int.Parse(sect[ScreenWidthKey]);
            }
            catch (KeyNotFoundException)
            {
                EngineConsole.Instance.Write("屏幕分辨率设置无效", ConsoleMessageType.Warning);
                ScreenWidth = 800;
                ScreenHeight = 600;
            }

            StretchMovies = sect.GetBool(StretchMoviestKey, true);
            FullScreen = sect.GetBool(FullScreenKey, true);
        }

        public override string Name
        {
            get { return VideoConfigFactory.VideoConfigName; }
        }


        public int ScreenWidth
        {
            get;
            set;
        }
        public int ScreenHeight
        {
            get;
            set;
        }
        public bool StretchMovies
        {
            get;
            set;
        }
        public bool FullScreen
        {
            get;
            set;
        }


        public override void Write(ConfigurationSection sect)
        {
            sect.Add(ScreenHeightKey, ScreenHeight.ToString());
            sect.Add(ScreenWidthKey, ScreenWidth.ToString());
            sect.Add(StretchMoviestKey, StretchMovies.ToString());
            sect.Add(FullScreenKey, FullScreen.ToString());

        }
    }


    public class BasicConfigs
    {
        static BasicConfigs singleton;

        public static BasicConfigs Instance
        {
            get { return singleton; }
        }

        public static void Initialize()
        {
            singleton = new BasicConfigs();
        }


        Dictionary<string, BasicConfigBase> configs;

        Dictionary<string, BasicConfigFactory> configTypes;

        string ra2IniPath;

        private BasicConfigs()
        {
            configTypes = new Dictionary<string, BasicConfigFactory>(20);
        }


        public void RegisterConfigType(string name, BasicConfigFactory fac)
        {
            configTypes.Add(name, fac);
        }
        public void UnregisterConfigType(string name)
        {
            configTypes.Remove(name);
        }

        public void Load()
        {
            FileLocation fileLoc = FileSystem.Instance.Locate("configs.ini", FileLocateRules.Default);

            Dictionary<string, ConfigurationSection> content;
            try
            {
                content = ConfigurationManager.Instance.CreateInstance(fileLoc);
            }
            catch (FileNotFoundException)
            {
                EngineConsole.Instance.Write("配置文件丢失，使用默认设置", ConsoleMessageType.Warning);
                content = new Dictionary<string, ConfigurationSection>();
            }

            ra2IniPath = fileLoc.Path;
            configs = new Dictionary<string, BasicConfigBase>();

            foreach (KeyValuePair<string, BasicConfigFactory> e in configTypes)
            {
                ConfigurationSection sect;
                if (!content.TryGetValue(e.Key, out sect))
                {
                    sect = new IniSection(e.Key);
                }

                configs.Add(e.Key, e.Value.CreateInstance(sect));
            }

        }
        public void Save()
        {
            IniConfiguration ra2ini = new IniConfiguration(string.Empty, configs.Count);

            foreach (KeyValuePair<string, BasicConfigBase> e in configs)
            {
                IniSection sect = new IniSection(e.Key);

                e.Value.Write(sect);

                ra2ini.Add(e.Key, sect);
            }

            ra2ini.Save(ra2IniPath);
        }

        public BasicConfigBase this[string name]
        {
            get { return configs[name]; }
        }

    }
}
