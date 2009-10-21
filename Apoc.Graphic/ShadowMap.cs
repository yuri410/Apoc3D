using System;
using System.Collections.Generic;
using System.Text;
using SlimDX;
using SlimDX.Direct3D9;
using Apoc.Graphics.Effects;
using Apoc.IO;

namespace Apoc.Graphics
{
    /// <summary>
    ///  定义 Shadow Mapping 所需要的阴影贴图
    /// </summary>
    public class ShadowMap : UnmanagedResource, IDisposable
    {
        struct TestVertex
        {
            public Vector3 pos;
            public float dummy;
            public Vector2 tex1;

            public static VertexFormat Format
            {
                get { return VertexFormat.PositionRhw | VertexFormat.Texture1; }
            }
        }

        Texture shadowDepthMap;
        Texture shadowRt;

        Surface shadowRtSurface;
        Surface shadowDepSurface;

        public const int ShadowMapLength = 1024;

        VertexBuffer pip;

        Viewport smVp;
        Device device;

        public DefaultSMGenEffect DefaultSMGen
        {
            get;
            private set;
        }

        public unsafe ShadowMap(Device dev)
        {
            device = dev;

            //FileLocation fl = FileSystem.Instance.Locate(FileSystem.CombinePath(Paths.Effects, "StandardShadow.fx"), FileLocateRules.Default);
            //ContentStreamReader sr = new ContentStreamReader(fl);
            //string code = sr.ReadToEnd();

            //string err;
            //effect = Effect.FromString(device, code, null, null, null, ShaderFlags.None, null, out err);
            //sr.Close();

            //mvpParam = new EffectHandle("mvp");

            //genSMTec = new EffectHandle("StandardShadow");
            //effect.Technique = genSMTec;
            //drawSceneTec = new EffectHandle("RenderScene");


            LoadUnmanagedResources();

            smVp.MinZ = 0;
            smVp.MaxZ = 1;
            smVp.Height = ShadowMapLength;
            smVp.Width = ShadowMapLength;
            smVp.X = 0;
            smVp.Y = 0;


            pip = new VertexBuffer(dev, sizeof(TestVertex) * 4, Usage.None, TestVertex.Format, Pool.Managed);

            TestVertex* ptr = (TestVertex*)pip.Lock(0, 0, LockFlags.None).DataPointer.ToPointer();
            ptr[0] = new TestVertex { pos = new Vector3(0, 0, 0), tex1 = new Vector2(0, 0), dummy = 0 };
            ptr[1] = new TestVertex { pos = new Vector3(0, 512, 0), tex1 = new Vector2(0, 1), dummy = 0 };
            ptr[2] = new TestVertex { pos = new Vector3(512, 0, 0), tex1 = new Vector2(1, 0), dummy = 0 };
            ptr[3] = new TestVertex { pos = new Vector3(512, 512, 0), tex1 = new Vector2(1, 1), dummy = 0 };

            pip.Unlock();


            DefaultSMGen = new DefaultSMGenEffect(device);
        }


        Surface stdDepth;
        Surface stdRenderTarget;
        Viewport stdVp;

        public Matrix LightProjection;
        public Matrix ViewTransform;
        public Matrix ViewProj;

        public void End()
        {
            //effect.EndPass();
            //effect.End();

            device.DepthStencilSurface = stdDepth;
            stdDepth = null;
            device.SetRenderTarget(0, stdRenderTarget);
            stdRenderTarget = null;
            device.Viewport = stdVp;
        }
        public void Begin(Vector3 lightDir, ICamera cam)
        {
            stdVp = device.Viewport;

            device.Viewport = smVp;

            stdDepth = device.DepthStencilSurface;
            stdRenderTarget = device.GetRenderTarget(0);


            device.SetRenderTarget(0, shadowRtSurface);
            device.DepthStencilSurface = shadowDepSurface;

            device.Clear(ClearFlags.Target | ClearFlags.ZBuffer, -1, 1, 0);


            //effect.Begin(FX.DoNotSaveSamplerState | FX.DoNotSaveShaderState | FX.DoNotSaveState);
            //effect.BeginPass(0);
            float zFar = cam.FarPlane;

            Matrix.OrthoRH((float)ShadowMapLength / 10f, (float)ShadowMapLength / 10f, cam.NearPlane, zFar, out LightProjection);

            Vector3 camPos = cam.Position;
            Vector3 up = cam.Front;


            Vector3 lightTarget = camPos;
            Vector3 offset = up;
            Vector3.Multiply(ref offset, 0.5f * zFar, out offset);

            Vector3.Add(ref lightTarget, ref offset, out lightTarget);


            Vector3 lightPos = lightTarget;
            offset = lightDir;
            Vector3.Multiply(ref offset, 0.5f * zFar, out offset);
            Vector3.Subtract(ref lightPos, ref offset, out lightPos);


            //Vector3 v1;
            //Vector3.Subtract(ref camPos, ref lPos, out v1);


            //Vector3.Cross(ref lightDir, ref v1, out up);
            //Vector3.Cross(ref lightDir, ref up, out up);
            up.Y = 0;
            if (up.LengthSquared() == 0)
            {
                up = new Vector3(0.707f, 0, 0.707f);
            }

            up = Vector3.UnitY;


            lightTarget = camPos + (cam.Front - lightDir) * 40;
            lightTarget.Y = camPos.Y;

            lightPos = lightTarget - lightDir * 50;

            Matrix.LookAtRH(ref lightPos, ref lightTarget, ref up, out ViewTransform);




            ViewProj = ViewTransform * LightProjection;

            //Matrix proj = cam.Frustum.proj;
            //Matrix view = cam.Frustum.view;

            //device.SetTransform(TransformState.Projection, proj);
            //device.SetTransform(TransformState.View, view);


            //ShadowTransform = view * proj * TexTransform;
        }

        public Texture ShadowColorMap
        {
            get { return shadowRt; }
        }

        protected override void loadUnmanagedResources()
        {
            shadowDepthMap = new Texture(device, ShadowMapLength, ShadowMapLength, 1, Usage.DepthStencil, Format.D24X8, Pool.Default);
            shadowRt = new Texture(device, ShadowMapLength, ShadowMapLength, 1, Usage.RenderTarget, Format.R32F, Pool.Default);

            shadowRtSurface = shadowRt.GetSurfaceLevel(0);
            shadowDepSurface = shadowDepthMap.GetSurfaceLevel(0);
        }

        protected override void unloadUnmanagedResources()
        {
            shadowDepthMap.Dispose();
            shadowRt.Dispose();

            shadowRtSurface.Dispose();
            shadowDepSurface.Dispose();
        }

        protected override void Dispose(bool disposing)
        {
            base.Dispose(disposing);

            if (disposing)
            {
                pip.Dispose();
                DefaultSMGen.Dispose();
            }
        }

    }
}
