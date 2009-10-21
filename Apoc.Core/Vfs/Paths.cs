using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace Apoc.Vfs
{
    public static class Paths
    {
        //static readonly string 
        static readonly string maps = "maps";
        static readonly string models = "models";
        static readonly string effects = "effects";
        static readonly string music = "music";
        static readonly string sounds = "sounds";
        static readonly string dataUI = Path.Combine("data", "ui");
        static readonly string dataSkybox = Path.Combine("data", "skybox");
        static readonly string dataParticle = Path.Combine("data", "particles");
        static readonly string textures = "textures";
        static readonly string configs = Path.Combine("data", "configs");

        static readonly string mapFilter = maps + Path.DirectorySeparatorChar.ToString() + "*.vmp";
        
        public static string Maps
        {
            get { return maps; }
        }
        public static string MapFilter
        {
            get { return mapFilter; }
        }



        public static string Models
        {
            get { return models; }
        }

        public static string Effects
        {
            get { return effects; }
        }
        public static string Sounds
        {
            get { return sounds; }
        }
        public static string Music
        {
            get { return music; }
        }
        public static string DataUI
        {
            get { return dataUI; }
        }
        public static string DataSkybox
        {
            get { return dataSkybox; }
        }
        public static string DataParticles
        {
            get { return dataParticle; }
        }
        public static string Textures 
        {
            get { return textures; }
        }
        public static string Configs
        {
            get { return configs; }
        }


    }
}
