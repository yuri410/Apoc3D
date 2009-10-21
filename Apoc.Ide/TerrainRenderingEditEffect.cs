using System;
using System.Collections.Generic;
using System.Text;
using SlimDX;
using SlimDX.Direct3D9;
using VirtualBicycle.Ide.Designers;
using VirtualBicycle.Ide.Designers.WorldBuilder;
using VirtualBicycle.Graphics;
using VirtualBicycle.Graphics.Effects;
using VirtualBicycle.IO;
using VirtualBicycle.Scene;

namespace VirtualBicycle.Ide
{
    public class TerrainRenderingEditEffectFactory : ModelEffectFactory
    {
        static readonly string typeName = "TerrainRenderingEdit";

        Device device;

        public TerrainRenderingEditEffectFactory(Device device)
        {
            this.device = device;
        }

        public override ModelEffect CreateInstance()
        {
            return new TerrainRenderingEditEffect(device);
        }

        public override void DestroyInstance(ModelEffect fx)
        {
            fx.Dispose();
        }

        public static string Name
        {
            get { return typeName; }
        }
    }

    public class TerrainRenderingEditEffect : ModelEffect
    {
        Device device;
        Effect effect;

        EffectHandle ehLightDir;

        EffectHandle ehWorldT;
        EffectHandle ehMVP;
        EffectHandle ehCameraPos;

        EffectHandle ehHeightScale;
        EffectHandle ehBrushSize;
        EffectHandle ehBrushPos;

        EffectHandle tlParamLa;
        EffectHandle tlParamLd;
        EffectHandle tlParamLs;
        EffectHandle tlParamKa;
        EffectHandle tlParamKd;
        EffectHandle tlParamKs;
        EffectHandle tlParamPwr;

        EffectHandle fogDensityParam;
        EffectHandle fogColorParam;

        EffectHandle ehDispMap;
        EffectHandle ehNormalMap;
        EffectHandle ehColorMap;
        EffectHandle ehIndexMap;

        EffectHandle ehBrushMap;

        EffectHandle[] ehDetailMap;
        EffectHandle[] ehDetailNrmMap;
        EffectHandle shadowMapParam;
        EffectHandle shadowMapTransform;

        Effect shadowMapGen;

        bool stateSetted;

        public TerrainRenderingEditEffect(Device device)
            : base(false, TerrainRenderingEffectFactory.Name)
        {
            this.device = device;

            FileLocation fl = FileSystem.Instance.Locate(FileSystem.CombinePath(VirtualBicycle.IO.Paths.Effects, "TerrainEdit.fx"), FileLocateRules.Default);
            ContentStreamReader sr = new ContentStreamReader(fl);

            string err;
            string code = sr.ReadToEnd();
            effect = Effect.FromString(device, code, null, IncludeHandler.Instance, null, ShaderFlags.OptimizationLevel3, null, out err);

            sr.Close();
            effect.Technique = new EffectHandle("Terrain");

            ehLightDir = new EffectHandle("lightDir");
            ehCameraPos = new EffectHandle("cameraPos");

            ehMVP = new EffectHandle("mvp");
            ehWorldT = new EffectHandle("worldT");


            ehHeightScale = new EffectHandle("heightScale");

            tlParamLa = new EffectHandle("I_a");
            tlParamLd = new EffectHandle("I_d");
            tlParamLs = new EffectHandle("I_s");
            tlParamKa = new EffectHandle("k_a");
            tlParamKd = new EffectHandle("k_d");
            tlParamKs = new EffectHandle("k_s");
            tlParamPwr = new EffectHandle("power");

            fogDensityParam = new EffectHandle("fogDensity");
            fogColorParam = new EffectHandle("fogColor");

            ehNormalMap = new EffectHandle("normalMap");
            ehColorMap = new EffectHandle("colorMap");
            ehIndexMap = new EffectHandle("indexMap");
            ehDispMap = new EffectHandle("disMap");

            ehBrushMap = new EffectHandle("brushTex");
            ehBrushSize = new EffectHandle("brushSize");
            ehBrushPos = new EffectHandle("brushPos");

            ehDetailMap = new EffectHandle[4];
            ehDetailNrmMap = new EffectHandle[4];

            ehDetailMap[0] = new EffectHandle("detailMap1");
            ehDetailMap[1] = new EffectHandle("detailMap2");
            ehDetailMap[2] = new EffectHandle("detailMap3");
            ehDetailMap[3] = new EffectHandle("detailMap4");

            ehDetailNrmMap[0] = new EffectHandle("detailNrmMap1");
            ehDetailNrmMap[1] = new EffectHandle("detailNrmMap2");
            ehDetailNrmMap[2] = new EffectHandle("detailNrmMap3");
            ehDetailNrmMap[3] = new EffectHandle("detailNrmMap4");


            shadowMapParam = new EffectHandle("shadowMap");
            shadowMapTransform = new EffectHandle("smTrans");


            // ======================================================================
            fl = FileSystem.Instance.Locate(FileSystem.CombinePath(VirtualBicycle.IO.Paths.Effects, "terrainShadow.fx"), FileLocateRules.Default);
            sr = new ContentStreamReader(fl);
            code = sr.ReadToEnd();
            shadowMapGen = Effect.FromString(device, code, null, null, null, ShaderFlags.None, null, out err);
            sr.Close();
            shadowMapGen.Technique = new EffectHandle("TerrainShadow");
        }

        protected override int begin()
        {
            stateSetted = false;
            return effect.Begin(FX.DoNotSaveSamplerState | FX.DoNotSaveShaderState | FX.DoNotSaveState);
        }

        protected override void end()
        {
            effect.End();
        }

        public override void BeginShadowPass()
        {
            shadowMapGen.Begin(FX.DoNotSaveSamplerState | FX.DoNotSaveShaderState | FX.DoNotSaveState);
            shadowMapGen.BeginPass(0);
        }
        public override void EndShadowPass()
        {
            shadowMapGen.EndPass();
            shadowMapGen.End();
        }
        public override void SetupShadowPass(MeshMaterial mat, ref RenderOperation op)
        {
            shadowMapGen.SetTexture(ehDispMap, mat.GetTexture(0).GetTexture);
            shadowMapGen.SetValue(ehHeightScale, EffectParams.TerrainHeightScale);


            shadowMapGen.SetValue(ehMVP, op.Transformation * EffectParams.ShadowMap.ViewProj);

            shadowMapGen.CommitChanges();
        }
        


        public override void BeginPass(int passId)
        {
            effect.BeginPass(passId);
        }

        public override void EndPass()
        {
            effect.EndPass();
        }


        public bool DrawBrush
        {
            get;
            set;
        }

        public Vector2 BrushPosition
        {
            get;
            set;
        }

        public float BrushSize
        {
            get;
            set;
        }

        public override void Setup(MeshMaterial mat, ref RenderOperation op)
        {
            if (!stateSetted)
            {
                Light light = EffectParams.Atmosphere.Light;
                effect.SetValue(tlParamLa, light.Ambient);
                effect.SetValue(tlParamLd, light.Diffuse);
                effect.SetValue(tlParamLs, light.Specular);
                effect.SetValue(ehLightDir, new float[3] { light.Direction.X, light.Direction.Y, light.Direction.Z });

                Vector3 pos = EffectParams.CurrentCamera.Position;
                effect.SetValue(ehCameraPos, new float[3] { pos.X, pos.Y, pos.Z });

                effect.SetValue(ehHeightScale, EffectParams.TerrainHeightScale);
                effect.SetTexture(shadowMapParam, EffectParams.ShadowMap.ShadowColorMap);

                stateSetted = true;
            }

            Matrix mv = Matrix.Multiply(op.Transformation, EffectParams.CurrentCamera.ViewMatrix);

            effect.SetValue(ehMVP, mv * EffectParams.CurrentCamera.Frustum.Projection);
            effect.SetValue(ehWorldT, op.Transformation);



            effect.SetValue(tlParamKa, mat.Ambient);
            effect.SetValue(tlParamKd, mat.Diffuse);
            effect.SetValue(tlParamKs, mat.Specular);
            effect.SetValue(tlParamPwr, mat.Power);

            effect.SetValue(fogColorParam, new Color4(EffectParams.Atmosphere.FogColor));
            effect.SetValue(fogDensityParam, EffectParams.Atmosphere.FogDensity);


            effect.SetTexture(ehIndexMap, mat.GetTexture(3).GetTexture);
            effect.SetTexture(ehNormalMap, mat.GetTexture(2).GetTexture);
            effect.SetTexture(ehColorMap, mat.GetTexture(1).GetTexture);
            effect.SetTexture(ehDispMap, mat.GetTexture(0).GetTexture);

            if (DrawBrush)
            {
                effect.SetTexture(ehBrushMap, WorldDesigner.BrushMap);
            }
            else
            {
                effect.SetTexture(ehBrushMap, null);
            }

            effect.SetValue(ehBrushPos, BrushPosition);

            if (BrushSize < 1) BrushSize = 1;

            effect.SetValue(ehBrushSize, BrushSize);


            effect.SetTexture(ehDetailMap[0], mat.GetTexture(4).GetTexture);
            effect.SetTexture(ehDetailNrmMap[0], mat.GetTexture(5).GetTexture);

            effect.SetTexture(ehDetailMap[1], mat.GetTexture(6).GetTexture);
            effect.SetTexture(ehDetailNrmMap[1], mat.GetTexture(7).GetTexture);

            effect.SetTexture(ehDetailMap[2], mat.GetTexture(8).GetTexture);
            effect.SetTexture(ehDetailNrmMap[2], mat.GetTexture(9).GetTexture);

            effect.SetTexture(ehDetailMap[3], mat.GetTexture(10).GetTexture);
            effect.SetTexture(ehDetailNrmMap[3], mat.GetTexture(11).GetTexture);

            Matrix lightPrjTrans;
            Matrix.Multiply(ref op.Transformation, ref EffectParams.ShadowMap.ViewProj, out lightPrjTrans);

            effect.SetValue(shadowMapTransform, lightPrjTrans);


            effect.CommitChanges();
        }

        protected override void Dispose(bool disposing)
        {
            if (disposing)
            {
                effect.Dispose();

                ehLightDir.Dispose();
                ehCameraPos.Dispose();

                ehMVP.Dispose();
                ehWorldT.Dispose();

                ehHeightScale.Dispose();

                tlParamLa.Dispose();
                tlParamLd.Dispose();
                tlParamLs.Dispose();
                tlParamKa.Dispose();
                tlParamKd.Dispose();
                tlParamKs.Dispose();
                tlParamPwr.Dispose();

                ehNormalMap.Dispose();
                ehDispMap.Dispose();

                shadowMapGen.Dispose();
            }
            effect = null;

            ehLightDir = null;
            ehCameraPos = null;

            ehMVP = null;
            ehWorldT = null;

            ehHeightScale = null;

            tlParamLa = null;
            tlParamLd = null;
            tlParamLs = null;
            tlParamKa = null;
            tlParamKd = null;
            tlParamKs = null;
            tlParamPwr = null;

            ehNormalMap = null;
            ehDispMap = null;
            shadowMapGen = null;
        }
    }
}
