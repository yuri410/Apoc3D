using System;
using System.Collections.Generic;
using System.IO;
using System.IO.Compression;
using System.Text;
using SlimDX.Direct3D9;
using Apoc.Core;
using Apoc.Graphics;
using Apoc.IO;
using VBC = Apoc.Core;

namespace Apoc.Scene
{
    public class TerrainTexture : GameTexture
    {
        bool isDisplacement;

        #region 构造函数

        public TerrainTexture(VBC.ResourceManager mgr, Texture texture, bool isDisp)
            : base(mgr, texture)
        {
            isDisplacement = isDisp;

        }

        /// <summary>
        ///  创建资源引用 
        /// </summary>
        /// <param name="texture"></param>
        public TerrainTexture(VBC.ResourceManager mgr, TerrainTexture texture)
            : base(mgr, texture)
        {
            isDisplacement = texture.isDisplacement;
        }

        public TerrainTexture(VBC.ResourceManager mgr, Device device, ResourceLocation rl, Usage usage, Pool pool, bool isDisp)
            : base(mgr, device, rl, usage, pool)
        {
            isDisplacement = isDisp;
        }

        #endregion

        #region 方法

        public override void ReadCacheData(Stream stream)
        {
            StreamedLocation sl = new StreamedLocation(stream);

            if (isDisplacement)
            {
                texture = TextureLoader.LoadDisplacementMap(Device, sl);
            }
            else
            {
                texture = TextureLoader.LoadUITexture(Device, sl);
            }
        }
        public override void WriteCacheData(Stream stream)
        {
            Use();
            if (isDisplacement)
            {
                byte[] data = TextureLoader.SaveDisplacementMap(Device, texture);
                stream.Write(data, 0, data.Length);
            }
            else
            {
                TextureLoader.SaveUITexture(Device, texture, new VirtualStream(stream));
            }
        }
        public void Save(Stream stream)
        {
            if (IsResourceEntity)
            {
                Use();
                if (isDisplacement)
                {
                    byte[] data = TextureLoader.SaveDisplacementMap(Device, texture);
                    stream.Write(data, 0, data.Length);
                }
                else
                {
                    TextureLoader.SaveUITexture(Device, texture, new VirtualStream(stream));
                }
            }
            else
            {
                ((TerrainTexture)ResourceEntity).Save(stream);
            }
        }

        public void NotifyChanged()
        {
            if (HasCache && cacheMem.Type == CacheType.Dynamic)
            {
                Stream stm = cacheMem.ResourceLocation.GetStream;
                WriteCacheData(new VirtualStream(stm));
            }
        }

        protected unsafe override void load()
        {
            if (ResourceLocation != null)
            {
                if (isDisplacement)
                {
                    texture = TextureLoader.LoadDisplacementMap(Device, ResourceLocation);
                }
                else
                {
                    texture = TextureLoader.LoadUITexture(Device, ResourceLocation);
                }
            }
        }

        protected override void unload()
        {
            texture.Dispose();
            texture = null;
        }

        #endregion
    }
}
