using System;
using System.Collections.Generic;
using System.Drawing;
using System.Text;
using SlimDX;
using SlimDX.Direct3D9;
using Apoc.IO;
using Apoc.Scene;

namespace Apoc.Graphics.Effects
{
    public class StandardEffectFactory : ModelEffectFactory
    {
        static readonly string typeName = "Standard";


        public static string Name
        {
            get { return typeName; }
        }



        Device device;

        public StandardEffectFactory(Device dev)
        {
            device = dev;
        }

        public override ModelEffect CreateInstance()
        {
            return new StandardEffect(device);
        }

        public override void DestroyInstance(ModelEffect fx)
        {
            fx.Dispose();
        }
    }

    class StandardEffect : ModelEffect
    {
        bool stateSetted;

        Device device;

        Effect effect;
        Effect effectInst;

        EffectHandle tlParamLa;
        EffectHandle tlParamLd;
        EffectHandle tlParamLs;
        EffectHandle tlParamKa;
        EffectHandle tlParamKd;
        EffectHandle tlParamKs;
        EffectHandle tlParamKe;
        EffectHandle tlParamPwr;
        EffectHandle tlParamLdir;
        EffectHandle tlParamVpos;
        EffectHandle tlParamClrMap;
        EffectHandle tlParamMVP;
        EffectHandle tlParamVP;

        EffectHandle tlParamWorldT;
        EffectHandle shadowMapParam;
        EffectHandle shadowMapTransform;
        EffectHandle fogDensityParam;
        EffectHandle fogColorParam;


        Effect shadowMapGen;



        Texture noTexture;

        public unsafe StandardEffect(Device dev)
            : base(false, StandardEffectFactory.Name)
        {
            device = dev;

            noTexture = new Texture(dev, 1, 1, 1, Usage.None, Format.A8R8G8B8, Pool.Managed);
            *((int*)noTexture.LockRectangle(0, LockFlags.None).Data.DataPointer.ToPointer()) = Color.Gray.ToArgb();
            noTexture.UnlockRectangle(0);

          
            effect = LoadEffect(dev, "Standard.fx");
            effectInst = LoadEffect(dev, "StandardInst.fx");


            effect.Technique = new EffectHandle("Standard");
            effectInst.Technique = new EffectHandle("Standard");

            tlParamLa = new EffectHandle("I_a");
            tlParamLd = new EffectHandle("I_d");
            tlParamLs = new EffectHandle("I_s");
            tlParamKa = new EffectHandle("k_a");
            tlParamKd = new EffectHandle("k_d");
            tlParamKs = new EffectHandle("k_s");
            tlParamKe = new EffectHandle("k_e");
            tlParamPwr = new EffectHandle("power");
            tlParamLdir = new EffectHandle("lightDir");

            tlParamClrMap = new EffectHandle("clrMap");

            tlParamVpos = new EffectHandle("cameraPos");
            tlParamMVP = new EffectHandle("mvp");
            tlParamVP = new EffectHandle("vp");

            tlParamWorldT = new EffectHandle("worldTrans");

            fogDensityParam = new EffectHandle("fogDensity");
            fogColorParam = new EffectHandle("fogColor");

            shadowMapParam = new EffectHandle("shadowMap");
            shadowMapTransform = new EffectHandle("smTrans");

            // ======================================================================
            //fl = FileSystem.Instance.Locate(FileSystem.CombinePath(Paths.Effects, "StandardShadowInst.fx"), FileLocateRules.Default);
            //sr = new ContentStreamReader(fl);
            //code = sr.ReadToEnd();
            //shadowMapGen = Effect.FromString(dev, code, null, IncludeHandler.Instance, null, ShaderFlags.OptimizationLevel3, null, out err);
            //sr.Close();
            shadowMapGen = LoadEffect(dev, "StandardShadow.fx");
            shadowMapGen.Technique = new EffectHandle("StandardShadow");
        }

        protected override int beginInst()
        {
            stateSetted = false;
            return effectInst.Begin(FX.DoNotSaveState | FX.DoNotSaveShaderState | FX.DoNotSaveSamplerState);
        }
        protected override void endInst()
        {
            effectInst.End();
        }
        public override void BeginPassInst(int passId)
        {
            effectInst.BeginPass(passId);
        }
        public override void EndPassInst()
        {
            effectInst.EndPass();
        }


        protected override int begin()
        {
            stateSetted = false;
            return effect.Begin(FX.DoNotSaveState | FX.DoNotSaveShaderState | FX.DoNotSaveSamplerState);
        }
        protected override void end()
        {
            effect.End();
        }
        public override void BeginPass(int passId)
        {
            effect.BeginPass(passId);
        }
        public override void EndPass()
        {
            effect.EndPass();
        }



        public override void Setup(MeshMaterial mat, ref RenderOperation op)
        {
            if (!stateSetted)
            {
                Light light = EffectParams.Atmosphere.Light;
                Vector3 lightDir = light.Direction;
                effect.SetValue(tlParamLa, light.Ambient);
                effect.SetValue(tlParamLd, light.Diffuse);
                effect.SetValue(tlParamLs, light.Specular);
                effect.SetValue(tlParamLdir, new float[3] { lightDir.X, lightDir.Y, lightDir.Z });

                Vector3 pos = EffectParams.CurrentCamera.Position;
                effect.SetValue(tlParamVpos, new float[3] { pos.X, pos.Y, pos.Z });

                effect.SetTexture(shadowMapParam, EffectParams.ShadowMap.ShadowColorMap);

                stateSetted = true;
            }
            effect.SetValue(tlParamKa, mat.mat.Ambient);
            effect.SetValue(tlParamKd, mat.mat.Diffuse);
            effect.SetValue(tlParamKs, mat.mat.Specular);
            effect.SetValue(tlParamKe, mat.mat.Emissive);

            effect.SetValue(tlParamPwr, mat.mat.Power);

            GameTexture clrTex = mat.GetTexture(0);
            if (clrTex == null)
            {
                effect.SetTexture(tlParamClrMap, noTexture);
            }
            else
            {
                effect.SetTexture(tlParamClrMap, clrTex.GetTexture);
            }

            effect.SetValue(tlParamMVP, op.Transformation * EffectParams.CurrentCamera.ViewMatrix * EffectParams.CurrentCamera.ProjectionMatrix);

            effect.SetValue(fogColorParam, new Color4(EffectParams.Atmosphere.FogColor));
            effect.SetValue(fogDensityParam, EffectParams.Atmosphere.FogDensity);

            Matrix lightPrjTrans;
            Matrix.Multiply(ref op.Transformation, ref EffectParams.ShadowMap.ViewProj, out lightPrjTrans);

            effect.SetValue(shadowMapTransform, lightPrjTrans);
            effect.SetValue(tlParamWorldT, op.Transformation);

            effect.CommitChanges();

        }

        public override void SetupInstancing(MeshMaterial mat)
        {
            if (!stateSetted)
            {
                Light light = EffectParams.Atmosphere.Light;
                Vector3 lightDir = light.Direction;
                effectInst.SetValue(tlParamLa, light.Ambient);
                effectInst.SetValue(tlParamLd, light.Diffuse);
                effectInst.SetValue(tlParamLs, light.Specular);
                effectInst.SetValue(tlParamLdir, new float[3] { lightDir.X, lightDir.Y, lightDir.Z });

                Vector3 pos = EffectParams.CurrentCamera.Position;
                effectInst.SetValue(tlParamVpos, new float[3] { pos.X, pos.Y, pos.Z });

                effectInst.SetTexture(shadowMapParam, EffectParams.ShadowMap.ShadowColorMap);

                stateSetted = true;
            }
            effectInst.SetValue(tlParamKa, mat.mat.Ambient);
            effectInst.SetValue(tlParamKd, mat.mat.Diffuse);
            effectInst.SetValue(tlParamKs, mat.mat.Specular);
            effectInst.SetValue(tlParamKe, mat.mat.Emissive);

            effectInst.SetValue(tlParamPwr, mat.mat.Power);

            GameTexture clrTex = mat.GetTexture(0);
            if (clrTex == null)
            {
                effectInst.SetTexture(tlParamClrMap, noTexture);
            }
            else
            {
                effectInst.SetTexture(tlParamClrMap, clrTex.GetTexture);
            }

            effectInst.SetValue<Matrix>(tlParamVP, EffectParams.CurrentCamera.ViewMatrix * EffectParams.CurrentCamera.ProjectionMatrix);

            effectInst.SetValue(fogColorParam, new Color4(EffectParams.Atmosphere.FogColor));
            effectInst.SetValue(fogDensityParam, EffectParams.Atmosphere.FogDensity);

            effectInst.SetValue(shadowMapTransform, EffectParams.ShadowMap.ViewProj);

            effectInst.CommitChanges();
        }

        public override void SetupShadowPass(MeshMaterial mat, ref RenderOperation op)
        {
            GameTexture clrTex = mat.GetTexture(0);
            if (clrTex == null)
            {
                shadowMapGen.SetTexture(tlParamClrMap, noTexture);
            }
            else
            {
                shadowMapGen.SetTexture(tlParamClrMap, clrTex.GetTexture);
            }

            shadowMapGen.SetValue(tlParamMVP, op.Transformation * EffectParams.ShadowMap.ViewProj);

            shadowMapGen.CommitChanges();
        }

        public override void BeginShadowPass()
        {
            shadowMapGen.Begin(FX.DoNotSaveState | FX.DoNotSaveShaderState | FX.DoNotSaveSamplerState);
            shadowMapGen.BeginPass(0);
        }
        public override void EndShadowPass()
        {
            shadowMapGen.EndPass();
            shadowMapGen.End();
        }

        protected override void Dispose(bool disposing)
        {
            if (disposing)
            {
                effect.Dispose();

                tlParamLa.Dispose();
                tlParamLd.Dispose();
                tlParamLs.Dispose();
                tlParamKa.Dispose();
                tlParamKd.Dispose();
                tlParamKs.Dispose();
                tlParamPwr.Dispose();
                tlParamLdir.Dispose();

                //tlParamClrMap.Dispose();
                tlParamWorldT.Dispose();

                tlParamVpos.Dispose();
                tlParamVP.Dispose();
                tlParamMVP.Dispose();

                shadowMapParam.Dispose();
                shadowMapTransform.Dispose();

                shadowMapGen.Dispose();
            }
            effect = null;
            tlParamLa = null;
            tlParamLd = null;
            tlParamLs = null;
            tlParamKa = null;
            tlParamKd = null;
            tlParamKs = null;
            tlParamPwr = null;
            tlParamLdir = null;

            //tlParamClrMap = null;
            tlParamWorldT = null;

            tlParamVpos = null;
            tlParamVP = null;
            tlParamMVP = null;

            shadowMapParam = null;
            shadowMapTransform = null;

            shadowMapGen = null;
        }
    }
}
