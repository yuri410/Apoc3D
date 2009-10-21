using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using SlimDX;
using VirtualBicycle.MathLib;

namespace VirtualBicycle.Ide.Designers
{
    [Serializable()]
    public class ModelDesignerConfigs : DocumentConfigBase
    {
        static readonly string ConfigFile = Path.Combine("Configs", "meshdc.xml");


        static ModelDesignerConfigs singleton;

        public static ModelDesignerConfigs Instance
        {
            get
            {
                if (singleton == null)
                {
                    Initialize();
                }
                return singleton;
            }
        }

        static void Initialize()
        {
            if (File.Exists(ConfigFile))
            {
                singleton = Serialization.XmlDeserialize<ModelDesignerConfigs>(ConfigFile);
            }
            else
            {
                singleton = new ModelDesignerConfigs();
            }
        }

        float eyeAngleX;
        float eyeAngleY;

        float eyeDist;
        float fovy;

        Vector3 lightPos;

        Color4 lgtAmbient;
        Color4 lgtDiffuse;
        Color4 lgtSpecular;


        /// <summary>
        /// xoz平面上的视角
        /// </summary>
        public float EyeAngleX
        {
            get { return eyeAngleX; }
            set { eyeAngleX = value; }
        }
        public float EyeAngleY
        {
            get { return eyeAngleY; }
            set
            {
                if (value < -MathEx.PIf / 2f)
                    value = -MathEx.PIf / 2f;
                if (value > MathEx.PIf / 2f)
                    value = MathEx.PIf / 2f;
                eyeAngleY = value;
            }
        }
        public float Fovy
        {
            get { return fovy; }
            set { fovy = value; }
        }
        public float EyeDistance
        {
            get { return eyeDist; }
            set { eyeDist = value; }
        }
        public Vector3 LightPosition
        {
            get { return lightPos; }
            set { lightPos = value; }
        }
        public Color4 LightAmbient
        {
            get { return lgtAmbient; }
            set { lgtAmbient = value; }
        }
        public Color4 LightDiffuse
        {
            get { return lgtDiffuse; }
            set { lgtDiffuse = value; }
        }
        public Color4 LightSpecular
        {
            get { return lgtSpecular; }
            set { lgtSpecular = value; }
        }

        private ModelDesignerConfigs()
        {
            //if (File.Exists(ConfigFile))
            //{
            //    FileStream fs = new FileStream(ConfigFile, FileMode.Open, FileAccess.Read, FileShare.Read);
            //    BinaryReader br = new BinaryReader(fs, Encoding.Default);

            //    eyeAngleX = br.ReadSingle();
            //    eyeAngleY = br.ReadSingle();
            //    eyeDist = br.ReadSingle();

            //    fovy = br.ReadSingle();

            //    lightPos.X = br.ReadSingle();
            //    lightPos.Y = br.ReadSingle();
            //    lightPos.Z = br.ReadSingle();

            //    lgtAmbient.Alpha = br.ReadSingle();
            //    lgtAmbient.Red = br.ReadSingle();
            //    lgtAmbient.Green = br.ReadSingle();
            //    lgtAmbient.Blue = br.ReadSingle();

            //    lgtDiffuse.Alpha = br.ReadSingle();
            //    lgtDiffuse.Red = br.ReadSingle();
            //    lgtDiffuse.Green = br.ReadSingle();
            //    lgtDiffuse.Blue = br.ReadSingle();


            //    lgtSpecular.Alpha = br.ReadSingle();
            //    lgtSpecular.Red = br.ReadSingle();
            //    lgtSpecular.Green = br.ReadSingle();
            //    lgtSpecular.Blue = br.ReadSingle();


            //    br.Close();
            //}
            //else
            //{
                eyeAngleX = 0;
                eyeAngleY = MathEx.PIf / 6;
                eyeDist = 25;
                fovy = MathEx.PIf / 3;

                lightPos = new Vector3(0, 50, 50);
                lgtAmbient = new Color4(1, 0.25f, 0.25f, 0.25f);
                lgtDiffuse = new Color4(1, 0.6f, 0.6f, 0.6f);
                lgtSpecular = new Color4(1, 0.6f, 0.6f, 0.6f);

            //}
        }

        protected override void Save()
        {
            Serialization.XmlSerialize<ModelDesignerConfigs>(this, ConfigFile);
            //FileStream fs = new FileStream(ConfigFile, FileMode.OpenOrCreate, FileAccess.Write, FileShare.Write);
            //BinaryWriter bw = new BinaryWriter(fs, Encoding.Default);

            //bw.Write(eyeAngleX);
            //bw.Write(eyeAngleY);
            //bw.Write(eyeDist);
            //bw.Write(fovy);

            //bw.Write(lightPos.X);
            //bw.Write(lightPos.Y);
            //bw.Write(lightPos.Z);

            //bw.Write(lgtAmbient.Alpha);
            //bw.Write(lgtAmbient.Red);
            //bw.Write(lgtAmbient.Green);
            //bw.Write(lgtAmbient.Blue);

            //bw.Write(lgtDiffuse.Alpha);
            //bw.Write(lgtDiffuse.Red);
            //bw.Write(lgtDiffuse.Green);
            //bw.Write(lgtDiffuse.Blue);

            //bw.Write(lgtSpecular.Alpha);
            //bw.Write(lgtSpecular.Red);
            //bw.Write(lgtSpecular.Green);
            //bw.Write(lgtSpecular.Blue);



            //bw.Close();
        }
    }
}
