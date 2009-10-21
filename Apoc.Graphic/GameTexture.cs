using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using SlimDX;
using SlimDX.Direct3D9;
using Apoc.Core;
using Apoc.IO;

using VBC = Apoc.Core;

namespace Apoc.Graphics
{
    /// <summary>
    ///  定义游戏中的纹理
    /// </summary>
    /// <remarks>和D3D纹理相比，该纹理受到资源管理</remarks>
    public class GameTexture : VBC.Resource
    {
        protected Texture texture;

        ResourceLocation resLoc;

        Device device;

        Usage usage;
        Pool pool;

        GameTexture refResource;

        ImageFileFormat textureFormat = ImageFileFormat.Dds;

        public GameTexture(Texture texture)
        {
            this.device = texture.Device;
            this.texture = texture;
        }

        public GameTexture(VBC.ResourceManager mgr, Texture texture)
            : base(mgr, "RawTexture: " + texture.ComPointer.ToString() + texture.GetHashCode().ToString())
        {
            this.device = texture.Device;
            this.texture = texture;
            this.pool = Pool.Managed;
        }

        /// <summary>
        ///  创建资源引用 
        /// </summary>
        /// <param name="texture"></param>
        public GameTexture(VBC.ResourceManager mgr, GameTexture texture)
            : base(mgr, texture)
        {
            this.device = texture.device;
            this.refResource = texture;
        }

        public GameTexture(VBC.ResourceManager mgr, Device device, ResourceLocation rl, Usage usage, Pool pool)
            : base(mgr, rl.Name)
        {
            this.device = device;
            this.resLoc = rl;
            this.usage = usage;
            this.pool = pool;
        }

        protected ResourceLocation ResourceLocation
        {
            get { return resLoc; }
            set { resLoc = value; }
        }

        protected Device Device
        {
            get { return device; }
            set { device = value; }
        }

        /// <summary>
        ///  获取D3D纹理
        /// </summary>
        public Texture GetTexture
        {
            get
            {                
                if (!IsResourceEntity)
                {
                    return refResource.GetTexture;
                }
                Use();
                return texture;
            }
        }


        public DataStream Save(ImageFileFormat format)
        {
            if (IsResourceEntity)
            {
                Use();
                DataStream stm = Texture.ToStream(texture, format);

                return stm;
            }
            else
            {
                return refResource.Save(format);
            }
        }

        //public override void SetCache(CacheMemory rl)
        //{           
        //    DataStream ds = Texture.ToStream(texture, ImageFileFormat.Dds);
                        
        //}
        public override void ReadCacheData(Stream stream)
        {
            texture = Texture.FromStream(device, resLoc.GetStream, usage, pool);
        }
        public unsafe override void WriteCacheData(Stream stream)
        {
            if (resLoc != null)
            {
                Stream stm = resLoc.GetStream;

                int len = (int)stm.Length;

                byte[] buffer = new byte[len];
                stm.Read(buffer, 0, len);
                stream.Write(buffer, 0, len);
            }
            else
            {
                Use();
                DataStream stm = Texture.ToStream(texture, textureFormat);

                byte[] buffer = new byte[stm.Length];

                fixed (void* dst = &buffer[0])
                {
                    void* src = stm.DataPointer.ToPointer();
                    Memory.Copy(src, dst, (int)stm.Length);
                }
                stream.Write(buffer, 0, buffer.Length);
            }
        }

        protected override void load()
        {
            if (resLoc != null)
            {
                ImageInformation info;
                Stream stream = resLoc.GetStream;
                texture = Texture.FromStream(device, stream,
                    (int)stream.Length, D3DX.Default, D3DX.Default, D3DX.Default, usage, Format.Unknown, pool, 
                    Filter.Default, Filter.Default, 0, out info);
                textureFormat = info.ImageFileFormat;
            }
        }
        protected override void unload()
        {
            texture.Dispose();
        }

        public override int GetSize()
        {
            if (resLoc != null) 
            {
                return resLoc.Size;
            }
            return 0;
        }

        protected override void Dispose(bool disposing)
        {
            base.Dispose(disposing);

            resLoc = null;
        } 

    }
}
