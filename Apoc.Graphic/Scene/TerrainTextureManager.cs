using System;
using System.Collections.Generic;
using System.Text;
using SlimDX.Direct3D9;
using Apoc.Core;
using Apoc.Graphics;
using Apoc.IO;
using VBC = Apoc.Core;

namespace Apoc.Scene
{
    /// <summary>
    ///  负责管理DisplacementMap，ColorMap，NormalMap以及IndexMap
    /// </summary>
    public class TerrainTextureManager : VBC.ResourceManager
    {
        static int defaultCacheSize = 128 * 1048576;

        public static int DefaultCacheSize
        {
            get { return defaultCacheSize; }
            private set { defaultCacheSize = value; }
        }

        #region 单例相关

        static TerrainTextureManager singleton;

        /// <summary>
        ///  获取TextureManager的实例
        /// </summary>
        public static TerrainTextureManager Instance
        {
            get
            {
                return singleton;
            }
        }

        public static void Initialize(int cacheSize)
        {
            if (singleton == null)
            {
                singleton = new TerrainTextureManager(cacheSize);
            }
            EngineConsole.Instance.Write("地形纹理管理器初始化完毕。内存使用上限" + Math.Round(cacheSize / 1048576.0, 2).ToString() + "MB。", ConsoleMessageType.Information);
        }

        #endregion

        private TerrainTextureManager(int cacheSize)
            : base(cacheSize)
        {
            CreationUsage = Usage.None;
        }

        #region 属性

        /// <summary>
        ///  获取或设置新创建纹理的用法(Usage)
        /// </summary>
        public Usage CreationUsage
        {
            get;
            set;
        }

        /// <summary>
        ///  获取或设置新创建纹理的资源管理方式(Pool)
        /// </summary>
        public Pool CreationPool
        {
            get;
            set;
        }

        #endregion

        #region 方法

        /// <summary>
        ///  创建一个新的纹理对象，并对其进行管理
        /// </summary>
        /// <param name="device"></param>
        /// <param name="rl"></param>
        /// <returns></returns>
        public TerrainTexture CreateInstance(Device device, ResourceLocation rl, bool isDisp)
        {
            VBC.Resource retrived = base.Exists(rl.Name);
            if (retrived == null)
            {
                TerrainTexture tex = new TerrainTexture(this, device, rl, CreationUsage, CreationPool, isDisp);
                retrived = tex;
                base.NewResource(tex, CacheType.Static);

                return new TerrainTexture(this, tex);
            }
            else
            {
                retrived.Use();

                if (retrived.IsResourceEntity)
                {
                    return new TerrainTexture(this, (TerrainTexture)retrived);
                }
                return (TerrainTexture)retrived;
            }
        }

        public TerrainTexture CreateInstance(Texture texture, bool isDisp)
        {
            TerrainTexture tex = new TerrainTexture(this, texture, isDisp);

            base.NewCached(tex, CacheType.Static);

            return new TerrainTexture(this, tex);
        }

        /// <summary>
        ///  销毁一个纹理对象
        ///  即使该纹理不受管理也照常销毁
        /// </summary>
        /// <param name="texture"></param>
        public void DestroyInstance(TerrainTexture texture)
        {
            base.DestoryResource(texture);
        }

        #endregion
    }
}
