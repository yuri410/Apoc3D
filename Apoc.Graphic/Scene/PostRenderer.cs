using System;
using System.Collections.Generic;
using System.Drawing;
using System.Text;
using SlimDX;
using SlimDX.Direct3D9;
using Apoc.Graphics.Effects;
using Apoc.Vfs;
using Apoc.Graphics;

namespace Apoc.Scene
{
    /// <summary>
    ///  定义可以渲染场景的接口，供后期效果渲染器使用。
    /// </summary>
    public interface ISceneRenderer
    {
        /// <summary>
        ///  正常渲染场景
        /// </summary>
        /// <param name="target">场景渲染目标</param>
        void RenderScenePost(Surface target);
    }

    /// <summary>
    ///  定义后期效果渲染器的接口
    /// </summary>
    public interface IPostSceneRenderer : IDisposable
    {
        /// <summary>
        ///  进行全部渲染，形成最总图像（带后期渲染）
        /// </summary>
        /// <param name="renderer">实现ISceneRenderer可以渲染场景的对象</param>
        /// <param name="screenTarget">渲染目标</param>
        void Render(ISceneRenderer renderer, Surface screenTarget);
    }

    /// <summary>
    ///  后期效果渲染器
    /// </summary>
    public class PostRenderer : UnmanagedResource, IPostSceneRenderer
    {
        struct RectVertex
        {
            public Vector4 Position;

            public Vector2 TexCoord;

            public static VertexFormat Format
            {
                get { return VertexFormat.PositionRhw | VertexFormat.Texture1; }
            }

            public static int Size
            {
                get { return Vector4.SizeInBytes + Vector2.SizeInBytes; }
            }
        }
        Device device;

        Texture colorTarget;
        Texture bloom;

        Surface clrRt;
        Surface blmRt;

        Effect bloomEffect;
        Effect compEffect;

        Effect gaussXBlur;
        Effect gaussYBlur;

        EffectHandle rtParam;
        EffectHandle clrRtParam;
        EffectHandle blmRtParam;

        VertexDeclaration vtxDecl;

        IndexBuffer indexBuffer;
        VertexBuffer quad;
        VertexBuffer smallQuad;

        Sprite spr;

        Effect LoadEffect(string fileName) 
        {
            FileLocation fl = FileSystem.Instance.Locate(FileSystem.CombinePath(Paths.Effects, fileName), FileLocateRules.Default);
            ContentStreamReader sr = new ContentStreamReader(fl);
            string code = sr.ReadToEnd();
            string err;
            Effect effect = Effect.FromString(device, code, null, IncludeHandler.Instance, null, ShaderFlags.OptimizationLevel3, null, out err);
            sr.Close();

            return effect;
        }

        public PostRenderer(Device device)
        {
            this.device = device;

            bloomEffect = LoadEffect("bloom.fx");
            compEffect = LoadEffect("composite.fx");
            gaussXBlur = LoadEffect("gaussXBlur.fx");
            gaussYBlur = LoadEffect("gaussYBlur.fx");

            bloomEffect.Technique = new EffectHandle("Bloom");
            compEffect.Technique = new EffectHandle("Composite");
            gaussXBlur.Technique = new EffectHandle("GaussX");
            gaussYBlur.Technique = new EffectHandle("GaussY");


            rtParam = new EffectHandle("rt");
            clrRtParam = new EffectHandle("clrRt");
            blmRtParam = new EffectHandle("blmRt");

            VertexElement[] elems = new VertexElement[3];
            elems[0] = new VertexElement(0, 0, DeclarationType.Float4, DeclarationMethod.Default, DeclarationUsage.PositionTransformed, 0);
            elems[1] = new VertexElement(0, (short)Vector4.SizeInBytes, DeclarationType.Float2, DeclarationMethod.Default, DeclarationUsage.TextureCoordinate, 0);
            elems[2] = VertexElement.VertexDeclarationEnd;
            vtxDecl = new VertexDeclaration(device, elems);

            LoadUnmanagedResources();
        }

        void DrawBigQuad()
        {
            device.SetStreamSource(0, quad, 0, RectVertex.Size);
            device.VertexFormat = RectVertex.Format;
            device.Indices = indexBuffer;
            device.VertexDeclaration = vtxDecl;

            device.DrawIndexedPrimitives(PrimitiveType.TriangleList, 0, 0, 4, 0, 2);
        }
        void DrawSmallQuad()
        {
            device.SetStreamSource(0, smallQuad, 0, RectVertex.Size);
            device.VertexFormat = RectVertex.Format;
            device.VertexDeclaration = vtxDecl;
            device.Indices = indexBuffer;
            device.DrawIndexedPrimitives(PrimitiveType.TriangleList, 0, 0, 4, 0, 2);
        }

        /// <summary>
        ///  见接口
        /// </summary>
        /// <param name="renderer"></param>
        /// <param name="screenTarget"></param>
        public void Render(ISceneRenderer renderer, Surface screenTarget)
        {
            renderer.RenderScenePost(clrRt);
            device.SetRenderState<Cull>(RenderState.CullMode, Cull.None);

            #region 分离高光
            device.SetRenderTarget(0, blmRt);

            bloomEffect.Begin(FX.DoNotSaveState | FX.DoNotSaveShaderState | FX.DoNotSaveSamplerState);
            bloomEffect.SetTexture(rtParam, colorTarget);
            bloomEffect.CommitChanges();
            bloomEffect.BeginPass(0);

            DrawSmallQuad();

            bloomEffect.EndPass();
            bloomEffect.End();
            #endregion

            #region 高斯X
            gaussXBlur.Begin(FX.DoNotSaveState | FX.DoNotSaveShaderState | FX.DoNotSaveSamplerState);
            gaussXBlur.SetTexture(rtParam, bloom);
            gaussXBlur.CommitChanges();
            gaussXBlur.BeginPass(0);

            DrawSmallQuad();

            gaussXBlur.EndPass();
            gaussXBlur.End();
            #endregion

            #region 高斯Y
            gaussYBlur.Begin(FX.DoNotSaveState | FX.DoNotSaveShaderState | FX.DoNotSaveSamplerState);
            gaussYBlur.SetTexture(rtParam, bloom);
            gaussYBlur.CommitChanges();
            gaussYBlur.BeginPass(0);

            DrawSmallQuad();

            gaussYBlur.EndPass();
            gaussYBlur.End();
            #endregion

            #region 合成

            device.SetRenderTarget(0, screenTarget);

            device.VertexShader = null;
            device.PixelShader = null;

            spr.Transform = Matrix.Identity;

            spr.Begin(SpriteFlags.DoNotSaveState);
            spr.Draw(colorTarget, -1);
            spr.End();

            device.SetRenderState(RenderState.AlphaBlendEnable, true);
            device.SetRenderState<BlendOperation>(RenderState.BlendOperation, BlendOperation.Add);
            device.SetRenderState<Blend>(RenderState.DestinationBlend, Blend.One);
            device.SetRenderState<Blend>(RenderState.DestinationBlendAlpha, Blend.One);
            device.SetRenderState<Blend>(RenderState.SourceBlend, Blend.One);
            device.SetRenderState<Blend>(RenderState.SourceBlendAlpha, Blend.One);


            compEffect.Begin(FX.DoNotSaveState | FX.DoNotSaveShaderState | FX.DoNotSaveSamplerState);
            //compEffect.SetTexture(clrRtParam, colorTarget);
            compEffect.SetTexture(blmRtParam, bloom);
            compEffect.CommitChanges();
            compEffect.BeginPass(0);

            DrawBigQuad();

            compEffect.EndPass();
            compEffect.End();
            device.SetRenderState(RenderState.AlphaBlendEnable, false);

            #endregion

        }

        protected unsafe override void loadUnmanagedResources()
        {
            Surface s = device.GetBackBuffer(0, 0);
            SurfaceDescription desc = s.Description;

            Size blmSize = new Size(512, 512);
            Size scrnSize = new Size(desc.Width, desc.Height);
            //s.Dispose();

            //colorTarget = TextureLoader.LoadUITexture(device, new FileLocation(@"E:\Desktop\123.png"));
            colorTarget = new Texture(device, scrnSize.Width, scrnSize.Height, 1, Usage.RenderTarget, Format.X8R8G8B8, Pool.Default);
            bloom = new Texture(device, blmSize.Width, blmSize.Height, 1, Usage.RenderTarget, Format.R32F, Pool.Default);

            spr = new Sprite(device);

            clrRt = colorTarget.GetSurfaceLevel(0);
            blmRt = bloom.GetSurfaceLevel(0);


            quad = new VertexBuffer(device, RectVertex.Size * 4, Usage.None, RectVertex.Format, Pool.Managed);

            RectVertex* vdst = (RectVertex*)quad.Lock(0, 0, LockFlags.None).DataPointer;
            vdst[0].Position = new Vector4(0, 0, 0, 1);
            vdst[0].TexCoord = new Vector2(0, 0);
            vdst[1].Position = new Vector4(scrnSize.Width, 0, 0, 1);
            vdst[1].TexCoord = new Vector2(1, 0);
            vdst[2].Position = new Vector4(0, scrnSize.Height, 0, 1);
            vdst[2].TexCoord = new Vector2(0, 1);
            vdst[3].Position = new Vector4(scrnSize.Width, scrnSize.Height, 0, 1);
            vdst[3].TexCoord = new Vector2(1, 1);
            quad.Unlock();


            smallQuad = new VertexBuffer(device, RectVertex.Size * 4, Usage.None, RectVertex.Format, Pool.Managed);
            vdst = (RectVertex*)smallQuad.Lock(0, 0, LockFlags.None).DataPointer;
            vdst[0].Position = new Vector4(0, 0, 0, 1);
            vdst[0].TexCoord = new Vector2(0, 0);
            vdst[1].Position = new Vector4(blmSize.Width, 0, 0, 1);
            vdst[1].TexCoord = new Vector2(1, 0);
            vdst[2].Position = new Vector4(0, blmSize.Height, 0, 1);
            vdst[2].TexCoord = new Vector2(0, 1);
            vdst[3].Position = new Vector4(blmSize.Width, blmSize.Height, 0, 1);
            vdst[3].TexCoord = new Vector2(1, 1);
            smallQuad.Unlock();


            indexBuffer = new IndexBuffer(device, sizeof(int) * 6, Usage.None, Pool.Managed, false);
            int* idst = (int*)indexBuffer.Lock(0, 0, LockFlags.None).DataPointer;

            idst[0] = 0;
            idst[1] = 1;
            idst[2] = 3;
            idst[3] = 0;
            idst[4] = 2;
            idst[5] = 3;
            indexBuffer.Unlock();

        }


        protected override void unloadUnmanagedResources()
        {
            spr.Dispose();

            colorTarget.Dispose();
            bloom.Dispose();

            clrRt.Dispose();
            blmRt.Dispose();

            indexBuffer.Dispose();
            quad.Dispose();
            smallQuad.Dispose();

            quad = null;            
            smallQuad = null;
            indexBuffer = null;

            clrRt = null;
            blmRt = null;
            colorTarget = null;
            bloom = null;
        }
    }
}
