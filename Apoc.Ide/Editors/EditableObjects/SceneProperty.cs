using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Design;
using System.Text;
using SlimDX;
using SlimDX.Direct3D9;
using VirtualBicycle.Graphics;
using VirtualBicycle.Logic;
using VirtualBicycle.Scene;

namespace VirtualBicycle.Ide.Editors.EditableObjects
{
    public class SceneProperty
    {
        EditableSceneData sceData;

        TerrainProperty terrProp;
        AtmosphereProperty atmoProp;

        EditableGameScene scene;

        public SceneProperty(EditableGameScene scene, EditableSceneData data)
        {
            this.sceData = data;
            this.scene = scene;
            this.terrProp = new TerrainProperty(scene, data.TerrainSettings);
            this.atmoProp = new AtmosphereProperty(scene, data.AtmosphereData);
        }

        [LocalizedDescription("PROP:CellUnit")]
        public float CellUnit
        {
            get { return sceData.CellUnit; }
            set { sceData.SetCellUnit(value); }
        }

        [LocalizedDescription("PROP:AtmosphereProperty")]
        public AtmosphereProperty Atmosphere
        {
            get { return atmoProp; }
        }

        [LocalizedDescription("PROP:TerrainProperty")]
        public TerrainProperty TerrainSettings
        {
            get { return terrProp; }
        }

        [LocalizedDescription("PROP:MapNameProperty")]
        public string Name
        {
            get { return sceData.Name; }
            set { sceData.SetName(value); }
        }

        [LocalizedDescription("PROP:MapDescriptionProperty")]
        public string Description
        {
            get { return sceData.Description; }
            set { sceData.SetDescription(value); }
        }

        public override string ToString()
        {
            return DevStringTable.Instance["PROP:SceneProperty"];
        }
    }

    [TypeConverter(typeof(ExpandableObjectConverter))]
    public class TerrainProperty
    {
        TerrainSettings settings;
        EditableGameScene scene;

        public TerrainProperty(EditableGameScene scene, TerrainSettings settings)
        {
            if (settings == null)
            {
                throw new ArgumentNullException();
            }
            this.scene = scene;
            this.settings = settings;
        }



        [LocalizedDescription("PROP:HeightScale")]
        public float HeightScale
        {
            get { return settings.HeightScale; }
            set { settings.SetHeightScale(value); }
        }


        [LocalizedDescription("PROP:MaterialAmbient")]
        [LocalizedCategory("PROP:Material")]
        [Editor(typeof(Color4Editor), typeof(UITypeEditor))]
        public Color4 MaterialAmbient
        {
            get { return settings.MaterialAmbient; }
            set { settings.SetMaterialAmbient(value); }
        }

        [LocalizedDescription("PROP:MaterialDiffuse")]
        [LocalizedCategory("PROP:Material")]
        [Editor(typeof(Color4Editor), typeof(UITypeEditor))]
        public Color4 MaterialDiffuse
        {
            get { return settings.MaterialDiffuse; }
            set { settings.SetMaterialDiffuse(value); }
        }

        [LocalizedDescription("PROP:MaterialSpecular")]
        [LocalizedCategory("PROP:Material")]
        [Editor(typeof(Color4Editor), typeof(UITypeEditor))]
        public Color4 MaterialSpecular
        {
            get { return settings.MaterialSpecular; }
            set { settings.SetMaterialSpecular(value); }
        }

        [LocalizedDescription("PROP:MaterialEmissive")]
        [LocalizedCategory("PROP:Material")]
        [Editor(typeof(Color4Editor), typeof(UITypeEditor))]
        public Color4 MaterialEmissive
        {
            get { return settings.MaterialEmissive; }
            set { settings.SetMaterialEmissive(value); }
        }

        [LocalizedDescription("PROP:MaterialPower")]
        [LocalizedCategory("PROP:Material")]
        public float MaterialPower
        {
            get { return settings.MaterialPower; }
            set { settings.SetMaterialPower(value); }
        }

        public override string ToString()
        {
            return DevStringTable.Instance["PROP:TerrainProperty"];
        }
    }


    [TypeConverter(typeof(ExpandableObjectConverter))]
    public class AtmosphereProperty
    {
        AtmosphereInfo data;
        EditableGameScene scene;

        public AtmosphereProperty(EditableGameScene scene, AtmosphereInfo data)
        {
            if (data == null)
            {
                throw new ArgumentNullException();
            }
            this.scene = scene;
            this.data = data;
        }

        [Editor(typeof(Color4Editor), typeof(UITypeEditor))]
        [LocalizedDescription("PROP:LightAmbient")]
        public Color4 AmbientColor
        {
            get { return data.AmbientColor; }
            set
            {
                if (value != data.AmbientColor)
                {
                    data.AmbientColor = value;
                    scene.Atmosphere.UpdateSettings();
                }
            }
        }

        [Editor(typeof(Color4Editor), typeof(UITypeEditor))]
        [LocalizedDescription("PROP:LightDiffuse")]
        public Color4 DiffuseColor
        {
            get { return data.DiffuseColor; }
            set 
            {
                if (value != data.DiffuseColor)
                {
                    data.DiffuseColor = value;
                    scene.Atmosphere.UpdateSettings();
                }
            }
        }

        [Editor(typeof(Color4Editor), typeof(UITypeEditor))]
        [LocalizedDescription("PROP:LightSpecular")]
        public Color4 SpecularColor
        {
            get { return data.SpecularColor; }
            set
            {
                if (value != data.SpecularColor)
                {
                    data.SpecularColor = value;
                    scene.Atmosphere.UpdateSettings();
                }
            }
        }

        [LocalizedDescription("PROP:SkyName")]
        public string SkyName
        {
            get { return data.SkyName; }
            set { data.SkyName = value; }
        }

        public WeatherType Weather
        {
            get { return data.Weather; }
            set { data.Weather = value; }
        }

        public float DayLength
        {
            get { return data.DayLength; }
            set { data.DayLength = value; }
        }
        public TimeSpan StartTime
        {
            get { return data.StartTime; }
            set { data.StartTime = value; }
        }

        public bool StartRealTime
        {
            get { return data.StartRealTime; }
            set { data.StartRealTime = value; }
        }
        public FogMode FogMode
        {
            get { return data.FogMode; }
            set
            {
                if (value != data.FogMode)
                {
                    data.FogMode = value;
                    scene.Atmosphere.UpdateSettings();
                }
            }
        }

        public float FogStart
        {
            get { return data.FogStart; }
            set
            {
                if (value != data.FogStart)
                {
                    data.FogStart = value;
                    scene.Atmosphere.UpdateSettings();
                }
            }
        }

        public float FogEnd
        {
            get { return data.FogEnd; }
            set
            {
                if (value != data.FogEnd)
                {
                    data.FogEnd = value;
                    scene.Atmosphere.UpdateSettings();
                }
            }
        }

        public float FogDensity
        {
            get { return data.FogDensity; }
            set
            {
                if (value != data.FogDensity)
                {
                    data.FogDensity = value;
                    scene.Atmosphere.UpdateSettings();
                }
            }
        }

        public Color FogColor
        {
            get { return data.FogColor; }
            set
            {
                if (value != data.FogColor)
                {
                    data.FogColor = value;
                    scene.Atmosphere.UpdateSettings();
                }
            }
        }

        public override string ToString()
        {
            return DevStringTable.Instance["PROP:AtmosphereProperty"];
        }

    }
}
