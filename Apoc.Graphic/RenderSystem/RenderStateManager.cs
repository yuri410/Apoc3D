using System;
using System.Collections.Generic;
using System.Text;
using Apoc.Media;

namespace Apoc.RenderSystem
{
    public abstract class RenderStateManager
    {

        public RenderStateManager(RenderSystem rdrSys)
        {
            RenderSystem = rdrSys;
        }

        public RenderSystem RenderSystem
        {
            get;
            private set;
        }

        public abstract bool AlphaBlendEnable
        {
            get;
            set;
        }
        public abstract BlendFunction AlphaBlendOperation
        {
            get;
            set;
        }
        public abstract Blend AlphaDestinationBlend
        {
            get;
            set;
        }
        public abstract CompareFunction AlphaFunction
        {
            get;
            set;
        }
        public abstract Blend AlphaSourceBlend
        {
            get;
            set;
        }
        public abstract bool AlphaTestEnable
        {
            get;
            set;
        }
        public abstract ColorValue BlendFactor
        {
            get;
            set;
        }
        public abstract BlendFunction BlendFunction
        {
            get;
            set;
        }
        public abstract ColorWriteChannels ColorWriteChannels
        {
            get;
            set;
        }
        public abstract ColorWriteChannels ColorWriteChannels1
        {
            get;
            set;
        }
        public abstract ColorWriteChannels ColorWriteChannels2
        {
            get;
            set;
        }
        public abstract ColorWriteChannels ColorWriteChannels3
        {
            get;
            set;
        }
        public abstract StencilOperation CounterClockwiseStencilDepthBufferFail
        {
            get;
            set;
        }
        public abstract StencilOperation CounterClockwiseStencilFail
        {
            get;
            set;
        }
        public abstract CompareFunction CounterClockwiseStencilFunction
        {
            get;
            set;
        }
        public abstract StencilOperation CounterClockwiseStencilPass
        {
            get;
            set;
        }
        public abstract CullMode CullMode
        {
            get;
            set;
        }
        public abstract float DepthBias
        {
            get;
            set;
        }
        public abstract bool DepthBufferEnable
        {
            get;
            set;
        }
        public abstract CompareFunction DepthBufferFunction
        {
            get;
            set;
        }
        public abstract bool DepthBufferWriteEnable
        {
            get;
            set;
        }
        public abstract Blend DestinationBlend
        {
            get;
            set;
        }
        public abstract FillMode FillMode
        {
            get;
            set;
        }
        public abstract ColorValue FogColor
        {
            get;
            set;
        }
        public abstract float FogDensity
        {
            get;
            set;
        }
        public abstract bool FogEnable
        {
            get;
            set;
        }
        public abstract float FogEnd
        {
            get;
            set;
        }
        public abstract float FogStart
        {
            get;
            set;
        }
        public abstract FogMode FogTableMode
        {
            get;
            set;
        }
        public abstract FogMode FogVertexMode
        {
            get;
            set;
        }

        public abstract bool Lighting
        {
            get;
            set;
        }
        public abstract bool MultiSampleAntiAlias
        {
            get;
            set;
        }
        public abstract int MultiSampleMask
        {
            get;
            set;
        }
        public abstract float PointSize
        {
            get;
            set;
        }
        public abstract float PointScaleA
        {
            get;
            set;
        }
        public abstract float PointScaleB
        {
            get;
            set;
        }
        public abstract float PointScaleC
        {
            get;
            set;
        }

        public abstract float PointSizeMax
        {
            get;
            set;
        }
        public abstract float PointSizeMin
        {
            get;
            set;
        }
        public abstract bool PointSpriteEnable
        {
            get;
            set;
        }
        public abstract bool RangeFogEnable
        {
            get;
            set;
        }
        public abstract int ReferenceAlpha
        {
            get;
            set;
        }
        public abstract int ReferenceStencil
        {
            get;
            set;
        }
        public abstract bool ScissorTestEnable
        {
            get;
            set;
        }
        public abstract bool SeparateAlphaBlendEnabled
        {
            get;
            set;
        }
        public abstract ShadeMode ShadingMode
        {
            get;
            set;
        }
        public abstract float SlopeScaleDepthBias
        {
            get;
            set;
        }
        public abstract Blend SourceBlend
        {
            get;
            set;
        }
        public abstract StencilOperation StencilDepthBufferFail
        {
            get;
            set;
        }
        public abstract bool StencilEnable
        {
            get;
            set;
        }
        public abstract StencilOperation StencilFail
        {
            get;
            set;
        }
        public abstract CompareFunction StencilFunction
        {
            get;
            set;
        }
        public abstract int StencilMask
        {
            get;
            set;
        }
        public abstract StencilOperation StencilPass
        {
            get;
            set;
        }
        public abstract int StencilWriteMask
        {
            get;
            set;
        }
        public abstract bool TwoSidedStencilMode
        {
            get;
            set;
        }
        public abstract TextureWrapCoordinates Wrap0
        {
            get;
            set;
        }
        public abstract TextureWrapCoordinates Wrap1
        {
            get;
            set;
        }
        public abstract TextureWrapCoordinates Wrap10
        {
            get;
            set;
        }
        public abstract TextureWrapCoordinates Wrap11
        {
            get;
            set;
        }
        public abstract TextureWrapCoordinates Wrap12
        {
            get;
            set;
        }
        public abstract TextureWrapCoordinates Wrap13
        {
            get;
            set;
        }
        public abstract TextureWrapCoordinates Wrap14
        {
            get;
            set;
        }
        public abstract TextureWrapCoordinates Wrap15
        {
            get;
            set;
        }
        public abstract TextureWrapCoordinates Wrap2
        {
            get;
            set;
        }
        public abstract TextureWrapCoordinates Wrap3
        {
            get;
            set;
        }
        public abstract TextureWrapCoordinates Wrap4
        {
            get;
            set;
        }
        public abstract TextureWrapCoordinates Wrap5
        {
            get;
            set;
        }
        public abstract TextureWrapCoordinates Wrap6
        {
            get;
            set;
        }
        public abstract TextureWrapCoordinates Wrap7
        {
            get;
            set;
        }
        public abstract TextureWrapCoordinates Wrap8
        {
            get;
            set;
        }
        public abstract TextureWrapCoordinates Wrap9
        {
            get;
            set;
        }

    }
}
