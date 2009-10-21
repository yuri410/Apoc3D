using System;
using System.Collections.Generic;
using System.Text;

namespace Apoc.RenderSystem
{
    public abstract class Texture : Resource, IDisposable
    {
        static TextureType[] convTable;
        

        static Texture()
        {
            convTable = new TextureType[4];
            convTable[(int)ImageType.CubeImage] = TextureType.CubeTexture;
            convTable[(int)ImageType.Image1D] = TextureType.Texture1D;
            convTable[(int)ImageType.Image2D] = TextureType.Texture2D;
            convTable[(int)ImageType.Image3D] = TextureType.Texture3D;
        }


        protected static TextureType ConvertEnum(ImageType type)
        {
            return convTable[(int)type];
        }


        public RenderSystem RenderSystem
        {
            get;
            private set;
        }

        ImageLoader imageLoader;

        protected Texture(RenderSystem rs)
        {
            this.RenderSystem = rs;
        }

        protected Texture(RenderSystem rs, Surface[] surface, TextureUsage usage)
        {
            this.RenderSystem = rs;
            this.SurfaceCount = surface.Length;
            this.Width = surface[0].Width;
            this.Height = surface[0].Height;
            this.Depth = 1;

            this.Type = TextureType.Texture2D;
            this.Format = surface[0].ColorFormat;
            this.BytesPerPixel = Image.GetBytesPerPixel(Format);
            this.ContentSize = PixelUtil.GetMemorySize(Width, Height, 1, Format);

            this.Usage = usage;


        }

        protected Texture(RenderSystem rs, System.Drawing.Bitmap bitmap, TextureUsage usage)
        {
            this.RenderSystem = rs;
            this.Width = bitmap.Width;
            this.Height = bitmap.Height;
            this.SurfaceCount = 1;
            this.Depth = 1;
            this.Type = TextureType.Texture2D;
            this.Format = PixelFormat.A8R8G8B8;
            this.BytesPerPixel = 4;
            this.ContentSize = Width * Height * 4;
            this.Usage = usage;
        }

        protected Texture(RenderSystem rs, int width, int height, int depth, int surfaceCount, PixelFormat format, TextureUsage usage)
        {
            this.RenderSystem = rs;
            this.SurfaceCount = surfaceCount;
            this.Width = width;
            this.Height = height;
            this.Depth = depth;
            this.Usage = usage;
            this.Format = format;

            this.BytesPerPixel = Image.GetBytesPerPixel(format);

            if (depth == 1)
            {
                if (width == 1 || height == 1)
                {
                    this.Type = TextureType.Texture1D;
                }
                else
                {
                    this.Type = TextureType.Texture2D;
                }
            }
            else
            {
                this.Type = TextureType.Texture3D;
            }
            this.ContentSize = PixelUtil.GetMemorySize(width, height, 1, format);
        }

        protected Texture(RenderSystem rs, int length, int levelCount, TextureUsage usage, PixelFormat format)
        {
            this.RenderSystem = rs;
            this.SurfaceCount = levelCount;
            this.Width = length;
            this.Height = length;
            this.Depth = 1;
            this.Usage = usage;
            this.Format = format;
            this.Type = TextureType.CubeTexture;

            this.BytesPerPixel = Image.GetBytesPerPixel(format);

            this.ContentSize = 6 * PixelUtil.GetMemorySize(length, length, 1, format);
        }

        protected Texture(RenderSystem rs, ImageLoader image, TextureUsage usage)
            : base(TextureManager.Instance, image.GetHashCode())
        {
            this.Usage = usage;
            this.imageLoader = image;
        }

        protected Texture(RenderSystem rs, Texture resourceEntity)
            : base(TextureManager.Instance, resourceEntity)
        {
            this.BytesPerPixel = resourceEntity.BytesPerPixel;
            this.ContentSize = resourceEntity.ContentSize;

            this.Type = resourceEntity.Type;
            this.Usage = resourceEntity.Usage;

            this.Width = resourceEntity.Width;
            this.Height = resourceEntity.Height;
            this.Format = resourceEntity.Format;
            this.SurfaceCount = resourceEntity.SurfaceCount;
        }

        protected Texture(RenderSystem rs, Image image, TextureUsage usage)
            : base()
        {
            this.SurfaceCount = image.MipmapCount;
            this.Width = image.Width;
            this.Height = image.Height;
            this.Depth = image.Depth;
            this.Usage = usage;
            this.Format = image.Format;
            this.Type = ConvertEnum(image.Type);

            this.BytesPerPixel = Image.GetBytesPerPixel(Format);

            this.ContentSize = image.SizeInBytes;
            LoadImage(image);
        }

        public TextureType Type
        {
            get;
            private set;
        }
        public int Width
        {
            get;
            private set;
        }

        public int Height
        {
            get;
            private set;
        }

        public int Depth
        {
            get;
            private set;
        }

        public int BytesPerPixel
        {
            get;
            private set;
        }
        public int ContentSize
        {
            get;
            private set;
        }

        public int SurfaceCount
        {
            get;
            private set;
        }

        public TextureUsage Usage
        {
            get;
            private set;
        }

        public PixelFormat Format
        {
            get;
            private set;
        }

        public bool IsLocked
        {
            get;
            private set;
        }


        public override int GetSize()
        {
            return ContentSize;
        }
       

        protected abstract void LoadImage(Image image);
       

        protected override void load()
        {
            Image image = imageLoader.Load();
            SurfaceCount = image.MipmapCount;
            Width = image.Width;
            Height = image.Height;
            Depth = image.Depth;
            Format = image.Format;
            Type = ConvertEnum(image.Type);
            BytesPerPixel = image.BytesPerPixel;
            ContentSize = image.SizeInBytes;

            LoadImage(image);
        }

        protected abstract DataRectangle @lock(int surface, LockMode mode, Rectangle rect);
        protected abstract DataBox @lock(int surface, LockMode mode, Box box);
        protected abstract DataRectangle @lock(int surface, CubeMapFace cubemapFace, LockMode mode, Rectangle rect);

        public DataRectangle Lock(int surface, LockMode mode, CubeMapFace cubemapFace, Rectangle rect) 
        {
            if (!IsLocked)
            {
                DataRectangle res = @lock(surface, cubemapFace, mode, rect);
                IsLocked = true;
                return res;
            }
            throw new InvalidOperationException();
        }
        public DataRectangle Lock(int surface, LockMode mode, CubeMapFace cubemapFace)
        {
            if (!IsLocked)
            {
                DataRectangle res = @lock(surface, cubemapFace, mode, new Rectangle(0, 0, Width, Width));
                IsLocked = true;
                return res;
            }
            throw new InvalidOperationException();
        }

        public DataRectangle Lock(int surface, LockMode mode, Rectangle rect)
        {
            if (!IsLocked)
            {
                DataRectangle res = @lock(surface, mode, rect);
                IsLocked = true;
                return res;
            }
            throw new InvalidOperationException();
        }
        public DataRectangle Lock(int surface, LockMode mode)
        {
            if (!IsLocked)
            {
                DataRectangle res = @lock(surface, mode, new Rectangle(0, 0, Width, Height));
                IsLocked = true;
                return res;
            }
            throw new InvalidOperationException();
        }


        public DataBox Lock(int surface, LockMode mode, Box box)
        {
            if (!IsLocked)
            {
                DataBox res = @lock(surface, mode, box);
                IsLocked = true;
                return res;
            }
            throw new InvalidOperationException();
        }
        //public unsafe DataStream LockStream(int surface, LockMode mode, CubeMapFace cubemapFace, Rectangle rect)
        //{
        //    if (!IsLocked)
        //    {
        //        IntPtr res = @lock(surface, cubemapFace, mode, rect);
        //        IsLocked = true;
        //        return new DataStream(res.ToPointer(), Width * Width * BytesPerPixel, mode == LockMode.ReadOnly);
        //    }
        //    throw new InvalidOperationException();
        //}
        //public IntPtr Lock(int surface, CubeMapFace cubemapFace, LockMode mode, Rectangle rect)
        //{
        //    if (!IsLocked)
        //    {
        //        IntPtr res = @lock(surface, cubemapFace, mode, rect);
        //        IsLocked = true;
        //        return res;
        //    }
        //    throw new InvalidOperationException();
        //}

        //public unsafe DataStream LockStream(int surface, LockMode mode, CubeMapFace cubemapFace)
        //{
        //    if (!IsLocked)
        //    {
        //        IntPtr res = @lock(surface, cubemapFace, mode, new Rectangle(0, 0, Width, Width));
        //        IsLocked = true;
        //        return new DataStream(res.ToPointer(), Width * Width * BytesPerPixel, mode == LockMode.ReadOnly);
        //    }
        //    throw new InvalidOperationException();

        //}
        //public IntPtr Lock(int surface, CubeMapFace cubemapFace, LockMode mode)
        //{
        //    if (!IsLocked)
        //    {
        //        IntPtr res = @lock(surface, cubemapFace, mode, new Rectangle(0, 0, Width, Width));
        //        IsLocked = true;
        //        return res;
        //    }
        //    throw new InvalidOperationException();
        //}



        //public unsafe DataStream LockStream(int surface, LockMode mode)
        //{
        //    if (!IsLocked)
        //    {
        //        switch (Type)
        //        {
        //            case TextureType.Texture1D:
        //            case TextureType.Texture2D:
        //                IntPtr res = @lock(surface, mode, new Rectangle(0, 0, Width, Height));
        //                IsLocked = true;
        //                return new DataStream(res.ToPointer(), Width * Height * BytesPerPixel, mode == LockMode.ReadOnly);
        //            case TextureType.Texture3D:
        //                res = @lock(surface, mode, new Box(0, 0, 0, Width, Height, Depth));
        //                IsLocked = true;
        //                return new DataStream(res.ToPointer(), Width * Height * Depth * BytesPerPixel, mode == LockMode.ReadOnly);
        //        }
        //        throw new InvalidOperationException();
        //    }
        //    throw new InvalidOperationException();
        //}
        //public IntPtr Lock(int surface, LockMode mode)
        //{
        //    if (!IsLocked)
        //    {
        //        IntPtr res;
        //        switch (Type)
        //        {
        //            case TextureType.Texture1D:
        //            case TextureType.Texture2D:
        //                res = @lock(surface, mode, new Rectangle(0, 0, Width, Height));
        //                IsLocked = true;
        //                return res;
        //            case TextureType.Texture3D:
        //                res = @lock(surface, mode, new Box(0, 0, 0, Width, Height, Depth));
        //                IsLocked = true;
        //                return res;
        //        }
        //        throw new InvalidOperationException();
        //    }
        //    throw new InvalidOperationException();
        //}

        //public unsafe DataStream LockStream(int surface, LockMode mode, Rectangle rect)
        //{
        //    if (!IsLocked)
        //    {
        //        IntPtr res = @lock(surface, mode, rect);
        //        IsLocked = true;
        //        return new DataStream(res.ToPointer(), rect.Width * rect.Height * BytesPerPixel, mode == LockMode.ReadOnly);
        //    }
        //    throw new InvalidOperationException();
        //}
        //public IntPtr Lock(int surface, LockMode mode, Rectangle rect)
        //{
        //    if (!IsLocked)
        //    {
        //        IntPtr res = @lock(surface, mode, rect);
        //        IsLocked = true;
        //        return res;
        //    }
        //    throw new InvalidOperationException();
        //}

        //public unsafe DataStream LockStream(int surface, LockMode mode, Box box)
        //{
        //    if (!IsLocked)
        //    {
        //        IntPtr res = @lock(surface, mode, box);
        //        IsLocked = true;
        //        return new DataStream(res.ToPointer(), box.Width * box.Height * box.Depth * BytesPerPixel, mode == LockMode.ReadOnly);
        //    }
        //    throw new InvalidOperationException();
        //}
        //public IntPtr Lock(int surface, LockMode mode, Box box)
        //{
        //    if (!IsLocked)
        //    {
        //        IntPtr res = @lock(surface, mode, box);
        //        IsLocked = true;
        //        return res;
        //    }
        //    throw new InvalidOperationException();
        //}

        protected abstract void unlock(int surface);

        protected abstract void unlock(CubeMapFace cubemapFace, int surface);

        public void Unlock(int surface)
        {
            if (IsLocked)
            {
                unlock(surface);
                IsLocked = false;
            }
            throw new InvalidOperationException();
        }
        public void Unlock(CubeMapFace cubemapFace, int surface) 
        {
            if (IsLocked )
            {
                unlock(cubemapFace, surface);
                IsLocked = false;
            }
            throw new InvalidOperationException();
        }
    }
}
