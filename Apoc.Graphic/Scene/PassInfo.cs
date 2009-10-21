using System;
using System.Collections.Generic;
using System.Text;
using Apoc.Collections;
using Apoc.Graphics;
using Apoc.Graphics.Effects;

namespace Apoc.Scene
{
    /// <summary>
    ///  渲染场景时，PassInfo对象用来从Cluster的场景管理器收集信息，并存储
    /// </summary>
    public class PassInfo
    {
        /// <summary>
        ///  按效果批次分组，一个效果批次有一个RenderOperation列表
        /// </summary>
        public Dictionary<string, FastList<RenderOperation>> batchTable;

        /// <summary>
        ///  按效果批次名称查询效果的哈希表
        /// </summary>
        public Dictionary<string, ModelEffect> effects;

        public Dictionary<string, Dictionary<MeshMaterial, Dictionary<GeomentryData, FastList<RenderOperation>>>> instanceTable;

        public FastList<SceneObject> visibleObjects;

        public int RenderedObjectCount;
    }

}
