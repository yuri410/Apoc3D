using System;
using System.Collections.Generic;
using System.Text;
using Apoc.Media;

namespace Apoc.RenderSystem
{
    public abstract class SamplerState
    {
        protected SamplerState(int index)
        {
            Index = index;
        }

        public int Index
        {
            get;
            private set;
        }

        public abstract TextureAddressMode AddressU
        {
            get;
            set;
        }
        public abstract TextureAddressMode AddressV
        {
            get;
            set;
        }
        public abstract TextureAddressMode AddressW
        {
            get;
            set;
        }
        public abstract ColorValue BorderColor
        {
            get;
            set;
        }
        public abstract TextureFilter MagFilter
        {
            get;
            set;
        }
        public abstract int MaxAnisotropy
        {
            get;
            set;
        }
        public abstract int MaxMipLevel
        {
            get;
            set;
        }
        public abstract TextureFilter MinFilter
        {
            get;
            set;
        }
        public abstract TextureFilter MipFilter
        {
            get;
            set;
        }
        public abstract float MipMapLevelOfDetailBias
        {
            get;
            set;
        }
    }
}
