using System;
using System.Collections.Generic;
using System.Text;
using SlimDX;

namespace Apoc.Graphics.Effects
{
    public static class EffectParams
    {
        public static float TerrainHeightScale
        {
            get;
            set;
        }

        public static ShadowMap ShadowMap
        {
            get;
            set;
        }

        public static Atmosphere Atmosphere
        {
            get;
            set;
        }

        //public static Matrix WorldTransform
        //{
        //    get;
        //    set;
        //}

        //public static Matrix ViewTransform
        //{
        //    get;
        //    set;
        //}

        public static ICamera CurrentCamera
        {
            get;
            set;
        }
    }
}
