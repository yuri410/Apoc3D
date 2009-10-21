using System;
using System.Collections.Generic;
using System.Text;
using SlimDX.Direct3D9;
using Apoc.IO;

namespace Apoc.Graphics.Effects
{
    public class DefaultSMGenEffect : ModelEffect
    {
        #region Not Supported

        protected override int begin()
        {
            throw new NotSupportedException();
        }

        protected override void end()
        {
            throw new NotSupportedException();
        }

        public override void BeginPass(int passId)
        {
            throw new NotSupportedException();
        }

        public override void EndPass()
        {
            throw new NotSupportedException();
        }

        public override void Setup(MeshMaterial mat, ref RenderOperation op)
        {
            throw new NotSupportedException();
        }

        #endregion


        Effect effect;

        EffectHandle ehMVP;

        public DefaultSMGenEffect(Device device)
            : base(false, "DefaultSMGenEffect")
        {
            FileLocation fl = FileSystem.Instance.Locate(FileSystem.CombinePath(Paths.Effects, "HardwareShadowMap.fx"), FileLocateRules.Default);
            ContentStreamReader sr = new ContentStreamReader(fl);

            string err;
            string code = sr.ReadToEnd();
            effect = Effect.FromString(device, code, null, IncludeHandler.Instance, null, ShaderFlags.OptimizationLevel3, null, out err);
            sr.Close();

            effect.Technique = new EffectHandle("GenerateShadowMap");

            ehMVP = new EffectHandle("mvp");
        }

        public override void BeginShadowPass()
        {
            effect.Begin(FX.DoNotSaveSamplerState | FX.DoNotSaveShaderState | FX.DoNotSaveState);
            effect.BeginPass(0);
        }

        public override void EndShadowPass()
        {
            effect.EndPass();
            effect.End();
        }



        public override void SetupShadowPass(MeshMaterial mat, ref RenderOperation op)
        {
            effect.SetValue(ehMVP, op.Transformation * EffectParams.ShadowMap.ViewProj);

            effect.CommitChanges();
        }

        protected override void Dispose(bool disposing)
        {
            if (disposing)
            {
                effect.Dispose();
                ehMVP.Dispose();
            }
            ehMVP = null;
            effect = null;
        }
    }
}
