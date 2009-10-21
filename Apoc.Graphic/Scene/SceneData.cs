using System;
using System.Collections.Generic;
using System.IO;
using System.IO.Compression;
using System.Text;
using SlimDX;
using SlimDX.Direct3D9;
using Apoc.Graphics;
using Apoc.IO;
using Apoc.Logic;
using Apoc.Logic.Traffic;
using Apoc.MathLib;

namespace Apoc.Scene
{
    /// <summary>
    ///  记录场景地形（全局）的参数
    /// </summary>
    public class TerrainSettings
    {
        #region 常量

        protected static readonly string HeightScaleTag = "HeightScale";
        protected static readonly string MaterialTag = "Material";

        #endregion

        #region 属性

        public float HeightScale
        {
            get;
            protected set;
        }
        public Color4 MaterialAmbient
        {
            get;
            protected set;
        }
        public Color4 MaterialDiffuse
        {
            get;
            protected set;
        }
        public Color4 MaterialSpecular
        {
            get;
            protected set;
        }
        public Color4 MaterialEmissive
        {
            get;
            protected set;
        }
        public float MaterialPower
        {
            get;
            protected set;
        }
        #endregion

        #region 方法

        public void SetHeightScale(float value)
        {
            HeightScale = value;
        }

        public void SetMaterialAmbient(Color4 color)
        {
            MaterialAmbient = color;
        }

        public void SetMaterialDiffuse(Color4 color)
        {
            MaterialDiffuse = color;
        }

        public void SetMaterialSpecular(Color4 color)
        {
            MaterialSpecular = color;
        }

        public void SetMaterialEmissive(Color4 color)
        {
            MaterialEmissive = color;
        }

        public void SetMaterialPower(float value)
        {
            MaterialPower = value;
        }

        public virtual void ReadData(BinaryDataReader data)
        {
            HeightScale = data.GetDataSingle("HeightScale");

            ContentBinaryReader br = data.GetData("Material");

            Material mat;
            br.ReadMaterial(out mat);

            br.Close();

            this.MaterialAmbient = mat.Ambient;
            this.MaterialDiffuse = mat.Diffuse;
            this.MaterialEmissive = mat.Emissive;
            this.MaterialSpecular = mat.Specular;
            this.MaterialPower = mat.Power;
        }
        public virtual BinaryDataWriter WriteData()
        {
            BinaryDataWriter data = new BinaryDataWriter();

            data.AddEntry(HeightScaleTag, HeightScale);

            ContentBinaryWriter bw = data.AddEntry(MaterialTag);

            Material mat = new Material();
            mat.Ambient = MaterialAmbient;
            mat.Diffuse = MaterialDiffuse;
            mat.Emissive = MaterialEmissive;
            mat.Specular = MaterialSpecular;
            mat.Power = MaterialPower;

            bw.Write(ref mat);

            bw.Close();

            return data;
        }

        #endregion

    }

    /// <summary>
    ///  场景数据（地图数据）
    /// </summary>
    public class SceneData : SceneDataBase
    {
        #region 构造函数

        public SceneData(Device device, InGameObjectManager mgr)
            : base(device, mgr)
        {

        }

        #endregion

        #region 静态方法

        public static SceneData FromFile(Device device, InGameObjectManager mgr, string file, ProgressCallBack cbk)
        {
            return FromFile(device, mgr, new FileLocation(file), cbk);
        }

        public static SceneData FromFile(Device device, InGameObjectManager mgr, FileLocation fl, ProgressCallBack cbk)
        {
            ContentBinaryReader br = new ContentBinaryReader(fl);

            if (br.ReadInt32() == FileId)
            {
                SceneData result = new SceneData(device, mgr);

                BinaryDataReader data = br.ReadBinaryData();

                result.ReadData(data, cbk);

                data.Close();

                return result;
            }

            br.Close();
            throw new DataFormatException();
        }

        #endregion
    }
}
