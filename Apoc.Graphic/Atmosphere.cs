using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Text;
using Apoc.Config;
using Apoc.Vfs;
using Apoc.MathLib;

namespace Apoc.Graphics
{
    /// <summary>
    ///  提供大气效果的信息
    /// </summary>
    [TypeConverter(typeof(ExpandableObjectConverter))]
    public class AtmosphereInfo : IConfigurable
    {
        Color4F ambientColor;

        Color4F diffuseColor;

        Color4F specularColor;



        /// <summary>
        ///  获取或设置光照环境光成分
        /// </summary>
        public Color4F AmbientColor
        {
            get { return ambientColor; }
            set { ambientColor = value; }
        }


        /// <summary>
        ///  获取或设置光照漫反射成分
        /// </summary>
        public Color4F DiffuseColor
        {
            get { return diffuseColor; }
            set { diffuseColor = value; }
        }

        /// <summary>
        ///  获取或设置光照镜面光成分
        /// </summary>
        public Color4F SpecularColor
        {
            get { return specularColor; }
            set { specularColor = value; }
        }

        /// <summary>
        ///  获取或设置天空体的名称
        /// </summary>
        private string skyName;

        public string SkyName
        {
            get { return skyName; }
            set { skyName = value; }
        }

        private WeatherType weather;

        public WeatherType Weather
        {
            get { return weather; }
            set { weather = value; }
        }
        private float dayLength;

        public float DayLength
        {
            get { return dayLength; }
            set { dayLength = value; }
        }

        private TimeSpan startTime;

        public TimeSpan StartTime
        {
            get { return startTime; }
            set { startTime = value; }
        }

        private bool startWithRealTime;

        public bool StartRealTime
        {
            get { return startWithRealTime; }
            set { startWithRealTime = value; }
        }


        private FogMode fogMode;

        public FogMode FogMode
        {
            get { return fogMode; }
            set { fogMode = value; }
        }
        private float fogStart;

        public float FogStart
        {
            get { return fogStart; }
            set { fogStart = value; }
        }
        private float fogEnd;

        public float FogEnd
        {
            get { return fogEnd; }
            set { fogEnd = value; }
        }
        private float fogDensity;

        public float FogDensity
        {
            get { return fogDensity; }
            set { fogDensity = value; }
        }
        public int fogColor;

        public Color FogColor
        {
            get { return Color.FromArgb(fogColor); }
            set { fogColor = value.ToArgb(); }
        }

        [Browsable(false)]
        public bool FogEnabled
        {
            get { return fogMode != FogMode.None; }
        }

        [Browsable(false)]
        public bool HasSky
        {
            get { return !string.IsNullOrEmpty(skyName); }
        }

        [Browsable(false)]
        public bool HasDayNight
        {
            get { return dayLength > 10; }
        }

        #region IConfigurable 成员

        public void Parse(ConfigurationSection sect)
        {
            dayLength = sect.GetSingle("DayLength", 0);
            startWithRealTime = sect.GetBool("StartWithRealTime", false);
            weather = (WeatherType)Enum.Parse(typeof(WeatherType), sect.GetString("WeatherType", "None"), true);

            skyName = sect.GetString("Sky", "DefaultSkyBox");

            ambientColor = new Color4(
                    sect.GetSingle("AmbientRed", 0.3f),
                    sect.GetSingle("AmbientGreen", 0.3f),
                    sect.GetSingle("AmbientBlue", 0.3f));
            diffuseColor = new Color4(
                    sect.GetSingle("DiffuseRed", 0.6f),
                    sect.GetSingle("DiffuseGreen", 0.6f),
                    sect.GetSingle("DiffuseBlue", 0.6f));
            specularColor = new Color4(
                    sect.GetSingle("SpecularRed", 0.0f),
                    sect.GetSingle("SpecularGreen", 0.0f),
                    sect.GetSingle("SpecularBlue", 0.0f));


            fogMode = (FogMode)Enum.Parse(typeof(FogMode), sect.GetString("FogMode", "None"), true);
            fogDensity = sect.GetSingle("FogDensity", 0.002f);
            fogStart = sect.GetSingle("FogStart", 150f);
            fogEnd = sect.GetSingle("FogEnd", 200);
            fogColor = sect.GetColorRGBA("FogColor", Color.DarkGray).ToArgb();


        }

        #endregion



        static readonly string DayLengthTag = "DayLength";

        static readonly string StartRealtimeTag = "StartWithRealTime";

        static readonly string WeatherTypeTag = "WeatherType";

        static readonly string SkyTag = "Sky";

        static readonly string AmbientTag = "Ambient";

        static readonly string DiffuseTag = "Diffuse";

        static readonly string SpecularTag = "Specular";

        static readonly string FogModeTag = "FogMode";

        static readonly string FogDensityTag = "FogDensity";

        static readonly string FogStartTag = "FogStart";

        static readonly string FogEndTag = "FogEnd";

        static readonly string FogColorTag = "FogColor";


        public void ReadData(BinaryDataReader data)
        {
            dayLength = data.GetDataSingle(DayLengthTag);
            startWithRealTime = data.GetDataBool(StartRealtimeTag);
            weather = (WeatherType)data.GetDataInt32(WeatherTypeTag);

            ContentBinaryReader br = data.GetData(SkyTag);
            skyName = br.ReadStringUnicode();
            br.Close();

            br = data.GetData(AmbientTag);
            ambientColor.Red = br.ReadSingle();
            ambientColor.Green = br.ReadSingle();
            ambientColor.Blue = br.ReadSingle();
            ambientColor.Alpha = br.ReadSingle();
            br.Close();


            br = data.GetData(DiffuseTag);
            diffuseColor.Red = br.ReadSingle();
            diffuseColor.Green = br.ReadSingle();
            diffuseColor.Blue = br.ReadSingle();
            diffuseColor.Alpha = br.ReadSingle();
            br.Close();


            br = data.GetData(SpecularTag);
            specularColor.Red = br.ReadSingle();
            specularColor.Green = br.ReadSingle();
            specularColor.Blue = br.ReadSingle();
            specularColor.Alpha = br.ReadSingle();
            br.Close();

            fogMode = (FogMode)data.GetDataInt32(FogModeTag);
            fogDensity = data.GetDataSingle(FogDensityTag);
            fogStart = data.GetDataSingle(FogStartTag);
            fogEnd = data.GetDataSingle(FogEndTag);
            fogColor = data.GetDataInt32(FogColorTag);
        }

        public BinaryDataWriter WriteData()
        {
            BinaryDataWriter data = new BinaryDataWriter();

            data.AddEntry(DayLengthTag, dayLength);
            data.AddEntry(StartRealtimeTag, startWithRealTime);
            data.AddEntry(WeatherTypeTag, (int)weather);

            ContentBinaryWriter bw = data.AddEntry(SkyTag);
            bw.WriteStringUnicode(skyName);
            bw.Close();

            bw = data.AddEntry(AmbientTag);
            bw.Write(ambientColor.Red);
            bw.Write(ambientColor.Green);
            bw.Write(ambientColor.Blue);
            bw.Write(ambientColor.Alpha);
            bw.Close();


            bw = data.AddEntry(DiffuseTag);
            bw.Write(diffuseColor.Red);
            bw.Write(diffuseColor.Green);
            bw.Write(diffuseColor.Blue);
            bw.Write(diffuseColor.Alpha);
            bw.Close();


            bw = data.AddEntry(SpecularTag);
            bw.Write(specularColor.Red);
            bw.Write(specularColor.Green);
            bw.Write(specularColor.Blue);
            bw.Write(specularColor.Alpha);
            bw.Close();

            data.AddEntry(FogModeTag, (int)fogMode);
            data.AddEntry(FogDensityTag, fogDensity);
            data.AddEntry(FogStartTag, fogStart);
            data.AddEntry(FogEndTag, fogEnd);
            data.AddEntry(FogColorTag, fogColor);

            return data;
        }
    }

    /// <summary>
    ///  大气效果渲染器。负责渲染大气效果。
    /// </summary>
    public class Atmosphere
    {
        public delegate SkyBox SkyBoxLoadCallback(string name);

        SkyBox skyBox;
        AtmosphereInfo info;
        Device device;

        Light light;
        Light currentLight;

        FogMode fogMode;
        Color fogColor;
        int currentFogColor;

        float fogStart;
        float fogEnd;
        float fogDensity;

        float sunAngle;

        /// <summary>
        ///  获取或设置太阳角度
        /// </summary>
        public float SunAngle
        {
            get { return sunAngle; }
            set { sunAngle = value; }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="dev"></param>
        /// <param name="info"></param>
        /// <param name="sblcbk">用于创建天空盒的回调函数</param>
        public Atmosphere(Device dev, AtmosphereInfo info, SkyBoxLoadCallback sblcbk)
        {
            device = dev;
            this.info = info;

            //shadowMap = new ShadowMap(dev);


            light.Ambient = info.AmbientColor;
            light.Diffuse = info.DiffuseColor;
            light.Specular = info.SpecularColor;
            light.Type = LightType.Directional;
            currentLight.Ambient = light.Ambient;
            currentLight.Diffuse = light.Diffuse;
            currentLight.Specular = light.Specular;
            currentLight.Type = LightType.Directional;


            sunAngle = 3 * MathEx.PIf / 4;
            light.Direction = new Vector3(-(float)Math.Cos(sunAngle), -(float)Math.Sin(sunAngle), 0f);
            currentLight.Direction = light.Direction;

            fogMode = info.FogMode;
            fogStart = info.FogStart;
            fogEnd = info.FogEnd;
            fogDensity = info.FogDensity;
            fogColor = Color.FromArgb(info.fogColor);
            currentFogColor = info.fogColor;

            if (info.HasSky)
            {
                skyBox = sblcbk(info.SkyName);
            }
        }

        public void UpdateSettings()
        {
            light.Ambient = info.AmbientColor;
            light.Diffuse = info.DiffuseColor;
            light.Specular = info.SpecularColor;
            light.Type = LightType.Directional;
            currentLight.Ambient = light.Ambient;
            currentLight.Diffuse = light.Diffuse;
            currentLight.Specular = light.Specular;
            currentLight.Type = LightType.Directional;


            fogMode = info.FogMode;
            fogStart = info.FogStart;
            fogEnd = info.FogEnd;
            fogDensity = info.FogDensity;
            fogColor = Color.FromArgb(info.fogColor);
            currentFogColor = info.fogColor;
        }

        /// <summary>
        ///  获取或设置雾的浓度
        /// </summary>
        public float FogDensity
        {
            get { return fogDensity; }
            set { fogDensity = value; }
        }

        /// <summary>
        /// 获取或设置线性雾的渐变结束处
        /// </summary>
        public float FogEnd
        {
            get { return fogEnd; }
            set { fogEnd = value; }
        }
        /// <summary>
        ///  获取或设置线性雾的渐变起始处
        /// </summary>
        public float FogStart
        {
            get { return fogStart; }
            set { fogStart = value; }
        }

        /// <summary>
        ///  获取或设置雾的类型
        /// </summary>
        public FogMode FogMode
        {
            get { return fogMode; }
            set { fogMode = value; }
        }

        /// <summary>
        ///  获取一个 System.Boolean， 指示雾是否已经启用
        /// </summary>
        public bool FogEnabled
        {
            get { return fogMode != FogMode.None; }
        }

        /// <summary>
        ///  获取或设置雾的颜色
        /// </summary>
        public int FogColor
        {
            get { return currentFogColor; }
            set { currentFogColor = value; }
        }

        /// <summary>
        ///  渲染大气效果
        /// </summary>
        public void Render()
        {
            if (skyBox != null)
            {
                skyBox.Render();
            }
            if (FogEnabled)
            {
                device.SetRenderState(RenderState.FogEnable, true);
                device.SetRenderState<FogMode>(RenderState.FogTableMode, fogMode);
                device.SetRenderState<FogMode>(RenderState.FogVertexMode, fogMode);

                device.SetRenderState(RenderState.FogStart, fogStart);
                device.SetRenderState(RenderState.FogEnd, fogEnd);
                device.SetRenderState(RenderState.FogColor, currentFogColor);
                device.SetRenderState(RenderState.FogDensity, fogDensity);
            }
            else
            {
                device.SetRenderState(RenderState.FogEnable, false);
            }
        }

        int MultiplyColor(ref Color clr, float bgn)
        {
            return (0xff << 24) | ((int)(clr.R * bgn) << 16) | ((int)(clr.G * bgn) << 8) | (int)(clr.B * bgn);
        }

        /// <summary>
        ///  更新状态
        /// </summary>
        /// <param name="dt">间隔时间</param>
        public void Update(float dt)
        {
            float angle = MathEx.Radian2Degree(sunAngle) % 360f;
            float sin = (float)Math.Sin(sunAngle);


            const float fadeRange = 25f;
            const float fadeRange2 = 10f;

            const float totalRange = fadeRange + fadeRange2;

            const float lowestBrightness = 0f;
            const float invLowestBrightness = 1 - lowestBrightness;
            const float lowestBrightness2 = 0.2f;

            if (info.HasDayNight)
            {
                float step = (dt / info.DayLength) * (MathEx.PIf * 2);

                sunAngle += step;

                if (angle > 180 + fadeRange2 && angle < 360 - fadeRange2)
                {
                    currentLight.Direction = new Vector3((float)Math.Cos(sunAngle), sin, 0.5f);
                }
                else
                {
                    currentLight.Direction = new Vector3(-(float)Math.Cos(sunAngle), -sin, -0.5f);
                }
                currentLight.Direction.Normalize();
            }

            if (sunAngle > 450)
            {
                sunAngle = 90;
            }

            //Color currFogClr = fogColor;


            if (skyBox != null)
            {
                if (angle >= 360 - fadeRange2 || angle <= fadeRange)
                {
                    if (angle > 90)
                    {
                        skyBox.DayNightLerpParam = (360 + fadeRange - angle) / totalRange;

                        float brightness = invLowestBrightness * (1f - (360 + fadeRange - angle) / totalRange) + lowestBrightness;
                        currentLight.Diffuse = light.Diffuse * brightness;
                        currentFogColor = MultiplyColor(ref fogColor, brightness);
                    }
                    else
                    {
                        skyBox.DayNightLerpParam = (fadeRange - angle) / totalRange;

                        float brightness = invLowestBrightness * (1f - (fadeRange - angle) / totalRange) + lowestBrightness;
                        currentLight.Diffuse = light.Diffuse * brightness;
                        currentFogColor = MultiplyColor(ref fogColor, brightness);
                    }
                }
                else if (angle >= 180 - fadeRange && angle <= fadeRange2 + 180)
                {
                    skyBox.DayNightLerpParam = 1f - ((180f + fadeRange2 - angle) / totalRange);

                    float brightness = invLowestBrightness * ((180f + fadeRange2 - angle) / totalRange) + lowestBrightness;
                    currentLight.Diffuse = light.Diffuse * brightness;
                    currentFogColor = MultiplyColor(ref fogColor, brightness);
                }
                else if (angle > fadeRange && angle < 180 - fadeRange)
                {
                    currentLight.Diffuse = light.Diffuse;
                    currentFogColor = fogColor.ToArgb();

                    skyBox.DayNightLerpParam = 0;
                }
                else if (angle > 180 + fadeRange2 && angle < 360 - fadeRange2)
                {
                    float extraDiffuse = (2 * lowestBrightness2) * Math.Abs(Math.Max(0f, ((angle - (180 + fadeRange2)) / (180 - fadeRange2 * 2) * 0.5f - 1f)));
                    float brightness = lowestBrightness + extraDiffuse;

                    currentLight.Diffuse = light.Diffuse * brightness;
                    currentFogColor = MultiplyColor(ref fogColor, brightness);
                    skyBox.DayNightLerpParam = 1;
                }


            }

            //currentFogColor = currFogClr.ToArgb();
        }

        /// <summary>
        ///  获取光源信息
        /// </summary>
        public Light Light
        {
            get { return currentLight; }
        }

        /// <summary>
        ///  获取光线方向
        /// </summary>
        public Vector3 LightDirection
        {
            get { return currentLight.Direction; }
        }
        //public ShadowMap ShadowMap
        //{
        //    get { return shadowMap; }
        //}

    }
}
