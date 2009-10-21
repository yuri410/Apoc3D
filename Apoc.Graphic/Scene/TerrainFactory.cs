using System;
using System.Collections.Generic;
using System.Text;
using SlimDX.Direct3D9;
using Apoc.IO;
using Apoc.Logic;

namespace Apoc.Scene
{
    public class TerrainFactory : InGameObjectFactory
    {
        #region 常量

        static readonly string typeTag = "Terrain";
        
        #endregion
        
        #region 字段

        Device device;

        #endregion

        #region 属性

        public Device Device
        {
            get { return device; }
        }

        public static string TypeId
        {
            get { return typeTag; }
        }

        public override string TypeTag
        {
            get { return typeTag; }
        }

        #endregion

        public TerrainFactory(Device device, InGameObjectManager mgr)
            : base(mgr)
        {
            this.device = device;
        }

        public override SceneObject Deserialize(BinaryDataReader data, SceneDataBase sceneData)
        {
            Terrain terrain = new Terrain(device, sceneData.CellUnit, sceneData.TerrainSettings);
            terrain.LoadData(data);
            return terrain;
        }
    }
}
