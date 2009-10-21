using System;
using System.Collections.Generic;
using System.Text;
using Apoc.Vfs;

namespace Apoc.RenderSystem
{
    public class TextureManager : ResourceManager
    {
        static TextureManager singleton;

        public static TextureManager Instance
        {
            get
            {
                if (singleton == null)
                {
                    singleton = new TextureManager();
                }
                return singleton;
            }
        }

        private TextureManager()
        {
            CreationUsage = TextureUsage.Static;
        }

        public ObjectFactory Factory
        {
            get;
            set;
        }
        public TextureUsage CreationUsage
        {
            get;
            set;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="image"></param>
        /// <returns></returns>
        /// <remarks>以这种方式创建的资源不会被管理</remarks>
        public Texture CreateInstance(Image image)
        {
            if (Factory == null)
            {
                throw new InvalidOperationException();
            }

            return Factory.CreateTexture(image, CreationUsage);
        }
        /// <remarks>以这种方式创建的资源不会被管理</remarks>
        public Texture CreateInstance(int width, int height, int surfaceCount, PixelFormat format)
        {
            if (Factory == null)
            {
                throw new InvalidOperationException();
            }

            return Factory.CreateTexture(width, height, surfaceCount, CreationUsage, format);
        }
        /// <remarks>以这种方式创建的资源不会被管理</remarks>
        public Texture CreateInstance(int width, int height, int depth, int surfaceCount, PixelFormat format)
        {
            if (Factory == null)
            {
                throw new InvalidOperationException();
            }

            return Factory.CreateTexture(width, height, depth, surfaceCount, CreationUsage, format);
        }
        /// <remarks>以这种方式创建的资源不会被管理</remarks>
        public Texture CreateInstance(int length, int surfaceCount, PixelFormat format)
        {
            if (Factory == null)
            {
                throw new InvalidOperationException();
            }
            return Factory.CreateTexture(length, surfaceCount, CreationUsage, format);
        }

        public Texture CreateInstance(FileLocation fl)
        {
            ImageLoader il = ImageManager.Instance.CreateInstance(fl);

            return CreateInstance(il);
        }
        public Texture CreateInstance(ImageLoader image)
        {
            if (Factory == null)
            {
                throw new InvalidOperationException();
            }

            Resource retrived = base.Exists(image.GetHashCode());
            if (retrived == null)
            {
                Texture tex = Factory.CreateTexture(image, CreationUsage);
                retrived = tex;
                base.NewResource(tex);
            }
            else
            {
                retrived.Use();
            }

            return Factory.CreateReferenceTexture((Texture)retrived);
        }
        public void DestroyInstance(Texture texture)
        {
            base.DestoryResource(texture);
        }
    }

    //public class TextureManager : ObjectTracker<Texture>
    //{
    //    static TextureManager singleton;

    //    static void InvalidOpErr()
    //    {
    //        throw new InvalidOperationException();
    //    }

    //    public static void Initialize(Device dev)
    //    {
    //        singleton = new TextureManager(dev);
    //    }

    //    public static TextureManager Instance
    //    {
    //        get
    //        {
    //            if (singleton == null)
    //                InvalidOpErr();
    //            return singleton;
    //        }
    //    }

    //    RenderSystem renderSys;
    //    TextureUsage usage;
    //    Pool pool;


    //    private TextureManager(RenderSystem rs)
    //    {
    //        renderSys = rs;
    //        usage = TextureUsage.Static;
    //        pool = Pool.Managed;
    //    }

    //    public Usage CreationUsage
    //    {
    //        get { return usage; }
    //        set { usage = value; }
    //    }
    //    public Pool CreationPool
    //    {
    //        get { return pool; }
    //        set { pool = value; }
    //    }

    //    protected override Texture create(ResourceLocation rl)
    //    {
    //        throw new NotImplementedException();
    //        //return Texture.FromStream(device, rl.GetStream, usage, pool);//Texture.FromStream(device, rl.GetStream, 0, 0, 1, usage, Format.Unknown, pool, Filter.None, Filter.None, 0);
    //    }

    //    protected override void destroy(Texture obj)
    //    {
    //        if (obj != null && !obj.Disposed)
    //            obj.Dispose();
    //    }


    //}
}
