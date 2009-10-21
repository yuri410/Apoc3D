using System;
using System.Collections.Generic;
using System.Text;
using Apoc.Collections;
using Apoc.MathLib;

namespace Apoc.RenderSystem
{
    public abstract class RenderSystem
    {
        //unsafe struct TransformInfo
        //{
        //    public Matrix view;
        //    public Matrix projection;
        //    public fixed Matrix world[4];
        //    public fixed Matrix texture[8];

        //    public bool isViewDirty;
        //    public bool isProjDirty;
        //    public fixed bool isTexDirty[8];
        //    public fixed bool isWorldDirty[4];
        //}

        //TransformInfo transformInfo;
        Capabilities caps;

        int batchCount;
        int primitiveCount;
        int vertexCount;

        public int BatchCount
        {
            get { return batchCount; }
        }
        public int PrimitiveCount
        {
            get { return primitiveCount; }
        }
        public int VertexCount
        {
            get { return vertexCount; }
        }

        protected RenderSystem(string renderSystemName)
        {
            RenderSystemName = renderSystemName;
        }

        public abstract void Init();


        public Capabilities RenderSystemCaps
        {
            get { return caps; }
            protected set { caps = value; }
        }

        public string RenderSystemName
        {
            get;
            private set;
        }

        public ObjectFactory ObjectFactory
        {
            get;
            protected set;
        }

        public RenderStateManager RenderStates
        {
            get;
            protected set;
        }

        public virtual void BeginFrame()
        {
            batchCount = 0;
            primitiveCount = 0;
            vertexCount = 0;
        }
        public virtual void EndFrame() { }

        public abstract void Clear(ClearFlags flags, int color, float depth, int stencil);

        public abstract void SetFog(FogMode mode, ColorValue color, float start, float end, float density);

        public abstract void SetLight(int index, Light light);
        public abstract Light GetLight(int index);

        public abstract void SetRenderTarget(int index, RenderTarget rt);
        public abstract RenderTarget GetRenderTarget(int index);

        public abstract void SetTexture(int index, Texture texture);

        public virtual void SetStencilFunction(CompareFunction func,
            int refValue, int mask,
            StencilOperation stencilFailOp,
            StencilOperation depthFailOp,
            StencilOperation passOp,
            bool twoSidedOperation)
        {

        }
        public virtual void SetPointParameters(float size, bool attenuationEnabled,
          float constant, float linear, float quadratic,
          float minSize, float maxSize)
        {
            if (attenuationEnabled)
            {
                RenderStates.PointSpriteEnable = true;
                RenderStates.PointScaleA = constant;
                RenderStates.PointScaleB = linear;
                RenderStates.PointScaleC = quadratic;
                //RenderStates.
                //dDevice.SetRenderState(RenderState.PointScaleEnable, true);
                //dDevice.SetRenderState(RenderState.PointScaleA, constant);
                //dDevice.SetRenderState(RenderState.PointScaleB, linear);
                //dDevice.SetRenderState(RenderState.PointScaleC, quadratic);
            }
            else
            {
                RenderStates.PointSpriteEnable = false;
                //dDevice.SetRenderState(RenderState.PointScaleEnable, false);
            }
            //dDevice.SetRenderState(RenderState.PointSize, size);
            //dDevice.SetRenderState(RenderState.PointSizeMin, minSize);
            RenderStates.PointSize = size;
            RenderStates.PointSizeMin = minSize;

            if (maxSize == 0f)
            {
                maxSize = caps.MaxPointSize;
            }
            RenderStates.PointSizeMax = maxSize;
            //if (maxSize == 0.0f) maxSize = (float)rCapabilities.MaxPointSize;
            //dDevice.SetRenderState(RenderState.PointSizeMax, maxSize);
        }

        //public abstract void SetTextureStageState(int sampler, TextureStage type, TextureArgument arg);
        //public abstract void SetTextureStageState(int sampler, TextureStage type, TextureOperation arg);
        //public abstract void SetTextureStageState(int sampler, TextureStage type, TextureTransform arg);

        //public abstract void SetTextureStageState(int sampler, TextureStage type, int value);
        //public abstract void SetTextureStageState(int sampler, TextureStage type, float value);

        public abstract SamplerStateCollection GetSamplerStates();
        //public abstract void SetSamplerState(int sampler, SamplerState type, TextureAddressMode textureAddress);
        //public abstract void SetSamplerState(int sampler, SamplerState type, TextureFilter textureFilter);
        //public abstract void SetSamplerState(int sampler, SamplerState type, int value);
        //public abstract void SetSamplerState(int sampler, SamplerState type, float value);


        public abstract void SetMaterialColor(Material mat);

        //public abstract void SetEffect(Effect eff);

        public abstract void BindShader(VertexShader shader);
        public abstract void BindShader(PixelShader shader);

        public virtual void Render(RenderOperation[] op)
        {
            if (op == null)
            {
                return;
            }
            for (int i = 0; i < op.Length; i++)
            {
                batchCount++;
                primitiveCount += op[i].Geomentry.PrimCount;
                vertexCount += op[i].Geomentry.VertexCount;
            }
        }


        public abstract Matrix GetTransform(TransformState state);

        public abstract void SetTransform(TransformState state, Matrix matrix);



        //public unsafe void SetTransform(TransformState state, Matrix matrix)
        //{
        //    if (state >= TransformState.World)
        //    {
        //        int idx = (int)(state - TransformState.World);
        //        transformInfo.world[idx] = matrix;
        //        transformInfo.isWorldDirty[idx] = true;
        //        return;
        //    }
        //    if (state >= TransformState.Texture0)
        //    {
        //        int idx = (int)(state - TransformState.Texture0);

        //        transformInfo.texture[idx] = matrix;
        //        transformInfo.isTexDirty[idx] = true;
        //        return;
        //    }
        //    if (state == TransformState.View)
        //    {
        //        transformInfo.view = matrix;
        //        transformInfo.isViewDirty = true;
        //        return;
        //    }
        //    if (state == TransformState.Projection)
        //    {
        //        transformInfo.projection = matrix;
        //        transformInfo.isProjDirty = true;
        //        return;
        //    }
        //}


    }
}
