using System;
using System.Collections.Generic;
using System.Drawing;
using System.Text;
using SlimDX.Direct3D9;
using Apoc.Core;
using Apoc.Graphics;
using Apoc.IO;
using VBC = Apoc.Core;

namespace Apoc.Graphics
{
    /// <summary>
    ///  管理纹理资源，并实现动态加载/释放
    /// </summary>
    public class TextureManager : VBC.ResourceManager
    {
        static TextureManager singleton;

        /// <summary>
        ///  获取TextureManager的实例
        /// </summary>
        public static TextureManager Instance
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
                singleton = new TextureManager(cacheSize);
            }
            EngineConsole.Instance.Write("纹理管理器初始化完毕。内存使用上限" + Math.Round(cacheSize / 1048576.0, 2).ToString() + "MB。", ConsoleMessageType.Information);
        }

        private TextureManager()
        {
            CreationUsage = Usage.None;
        }
        private TextureManager(int cacheSize)
            : base(cacheSize)
        {
            CreationUsage = Usage.None;
        }

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

        /// <summary>
        ///  创建一个新的纹理对象，并对其进行管理
        /// </summary>
        /// <param name="device"></param>
        /// <param name="rl"></param>
        /// <returns></returns>
        public GameTexture CreateInstance(Device device, ResourceLocation rl)
        {
            VBC.Resource retrived = base.Exists(rl.Name);
            if (retrived == null)
            {
                GameTexture tex = new GameTexture(this, device, rl, CreationUsage, CreationPool);
                retrived = tex;
                base.NewResource(tex, CacheType.Static);

                return new GameTexture(this, tex);
            }
            else
            {
                retrived.Use();

                if (retrived.IsResourceEntity)
                {
                    return new GameTexture(this, (GameTexture)retrived);
                }
                return (GameTexture)retrived;
            }
        }

        public GameTexture CreateInstance(Device deivce, Texture t) 
        {
            GameTexture tex = new GameTexture(this, t);
            base.NewResource(tex, CacheType.Static);

            return new GameTexture(this, tex);
        }

        /// <summary>
        ///  销毁一个纹理对象
        ///  即使该纹理不受管理也照常销毁
        /// </summary>
        /// <param name="texture"></param>
        public void DestroyInstance(GameTexture texture)
        {
            base.DestoryResource(texture);
        }
    }

}
