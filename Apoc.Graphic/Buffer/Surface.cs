using System;
using System.Collections.Generic;
using System.Text;

namespace Apoc.RenderSystem
{


    public abstract class Surface : HardwareBuffer
    {
        //static Surface()
        //{
        //    const int arrLen = 11;
        //    bytesPP = new int[arrLen];
        //    for (int i = 0; i < arrLen; i++)
        //        bytesPP [i]= -1;

        //    bytesPP[(int)DepthFormat.Depth15Stencil1] = 2;
        //    bytesPP[(int)DepthFormat.Depth16] = 2;
        //    bytesPP[(int)DepthFormat.Depth16Lockable] = 2;
        //    bytesPP[(int)DepthFormat.Depth24] = 4;
        //    bytesPP[(int)DepthFormat.Depth24Stencil4] = 4;
        //    bytesPP[(int)DepthFormat.Depth24Stencil8] = 4;
        //    bytesPP[(int)DepthFormat.Depth24Stencil8Single] = 4;
        //    bytesPP[(int)DepthFormat.Depth32] = 4;
        //    bytesPP[(int)DepthFormat.Depth32Lockable] = 4;
        //    bytesPP[(int)DepthFormat.Depth32Single] = 4;
        //}

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

        public SurfaceType Type
        {
            get;
            private set;
        }

        public PixelFormat ColorFormat
        {
            get;
            private set;
        }

        public bool IsLockable
        {
            get;
            private set;
        }

        protected Surface(int width, int height, BufferUsage usage, PixelFormat format)
            : base(usage, PixelUtil.GetMemorySize(width, height, 1, format), false)
        {
            Width = width;
            Height = height;

            ColorFormat = format;
            DepthFormat = DepthFormat.Unknown;
            Type = SurfaceType.ColorBuffer;
            IsLockable = format == PixelFormat.Depth16Lockable || format == PixelFormat.Depth32Lockable;

        }

        //static int[] bytesPP;
        //static int GetMemorySize(int width, int height, DepthFormat format)
        //{
        //    return bytesPP[(int)format] * width * height;
        //}


        //protected override IntPtr @lock(int offset, int size, LockMode mode)
        //{
        //    throw new NotImplementedException();
        //}

        //protected override void unlock()
        //{
        //    throw new NotImplementedException();
        //}

        //public override void Dispose(bool disposing)
        //{
        //    throw new NotImplementedException();
        //}
    }
}
 