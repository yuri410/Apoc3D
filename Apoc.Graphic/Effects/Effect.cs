using System;
using System.Collections.Generic;
using System.Text;
using SlimDX;
using SlimDX.Direct3D9;
using Apoc.IO;
using Apoc.Scene;

namespace Apoc.Graphics.Effects
{
    public abstract class ModelEffectFactory
    {
        public abstract ModelEffect CreateInstance();

        public abstract void DestroyInstance(ModelEffect fx);

    }
    public abstract class PostEffectFactory
    {
        public abstract PostEffect CreateInstance();
        public abstract void DestroyInstance(PostEffect fx);
        //public abstract string Name
        //{
        //    get;
        //}
    }


    public abstract class EffectBase : IDisposable
    {

        public EffectBase(string name)
        {
            this.Name = name;
        }

        public string Name
        {
            get;
            private set;
        }
 
        public bool Disposed
        {
            get;
            private set;
        }
        protected abstract void Dispose(bool disposing);

        #region IDisposable 成员

        public void Dispose()
        {
            if (!Disposed)
            {
                Dispose(true);
                Disposed = true;
            }
            else
                throw new ObjectDisposedException(ToString());
        }

        #endregion

        ~EffectBase()
        {
            if (!Disposed)
                Dispose();
        }
    }

    public abstract class ModelEffect : EffectBase
    {

        public static Effect LoadEffect(Device device, string filename)
        {
            FileLocation fl = FileSystem.Instance.Locate(FileSystem.CombinePath(Paths.Effects, filename), FileLocateRules.Default);
            ContentStreamReader sr = new ContentStreamReader(fl);

            string err;
            string code = sr.ReadToEnd();

            Effect effect = Effect.FromString(device, code, null, IncludeHandler.Instance, null, ShaderFlags.OptimizationLevel3, null, out err);
            sr.Close();
            return effect;
        }

        bool begun;
        public ModelEffect(bool supportsInstancing, string name)
            : base(name)
        {
            SupportsInstancing = supportsInstancing;
        }


        //public abstract Camera GetPassCamera(int passId);

        protected abstract int begin();
        protected abstract void end();
        public abstract void BeginPass(int passId);
        public abstract void EndPass();

        public abstract void BeginShadowPass();
        public abstract void EndShadowPass();

        public int Begin()
        {
            if (!begun)
            {
                begun = true;
                return begin();
            }
            return -1;
        }

        public void End()
        {
            if (begun)
            {
                end();
                begun = false;
            }
        }

        protected virtual int beginInst() 
        {
            throw new NotSupportedException();
        }
        protected virtual void endInst()
        {
            throw new NotSupportedException();
        }
        public virtual void BeginPassInst(int passId)
        {
            throw new NotSupportedException();
        }
        public virtual void EndPassInst()
        {
            throw new NotSupportedException();
        }

        public int BeginInst()
        {
            if (!begun)
            {
                begun = true;
                return beginInst();
            }
            return -1;
        }
        public void EndInst() 
        {
            if (begun)
            {
                endInst();
                begun = false;
            }
        }

        
        public bool SupportsInstancing
        {
            get;
            protected set;
        }

        public virtual void SetupInstancing(MeshMaterial mat) 
        {
            throw new NotSupportedException();
        }

        public abstract void Setup(MeshMaterial mat, ref RenderOperation op);
        public abstract void SetupShadowPass(MeshMaterial mat, ref RenderOperation op);

    }

    public abstract class PostEffect : EffectBase
    {
        public PostEffect(string name)
            : base(name)
        { }
    }
}
