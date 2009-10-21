using System;
using System.Collections.Generic;
using System.Text;
using Apoc.Media;

namespace Apoc.RenderSystem
{
    public abstract class RenderTarget
    {
        protected Surface colorBuffer;
        protected Surface depthStencilBuffer;

        

        protected RenderTarget(RenderSystem renderSystem, int width, int height,
            PixelFormat clrBufFormat, PixelFormat depBufFmt)
        {
            Width = width;
            Height = height;
            ColorBufferFormat = clrBufFormat;
            DepthBufferFormat = depBufFmt;
        }

        public abstract Texture GetColorBufferTexture();
        public abstract Texture GetDepthBufferTexture();       

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

        public DepthFormat DepthBufferFormat
        {
            get;
            private set;
        }
        public PixelFormat ColorBufferFormat
        {
            get;
            private set;
        }

    }
}
