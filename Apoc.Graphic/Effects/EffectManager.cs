using System;
using System.Collections.Generic;
using System.Text;
using SlimDX.Direct3D9;

namespace Apoc.Graphics.Effects
{
    public class EffectManager : Singleton
    {
        static EffectManager singleton;

        public static void Initialize(Device device)
        {
            singleton = new EffectManager();

            EngineConsole.Instance.Write("特效系统启动。", ConsoleMessageType.Information);

            singleton.RegisterModelEffectType(StandardEffectFactory.Name, new StandardEffectFactory(device));
            singleton.RegisterModelEffectType(TerrainRenderingEffectFactory.Name, new TerrainRenderingEffectFactory(device));

            singleton.RegisterModelEffectType(RoadEffectFactory.Name, new RoadEffectFactory(device));
        }


        /// <summary>
        ///   Gets the singleton instance of this class.
        /// </summary>
        public static EffectManager Instance
        {
            get
            {
                return singleton;
            }
        }

        private EffectManager()
        {
            Enabled = true;
        }

        Dictionary<string, ModelEffect> modelFX = new Dictionary<string, ModelEffect>(CaseInsensitiveStringComparer.Instance);
        
        Dictionary<string, PostEffect> postFX = new Dictionary<string, PostEffect>(CaseInsensitiveStringComparer.Instance);


        Dictionary<string, ModelEffectFactory> modelFXFac = new Dictionary<string, ModelEffectFactory>(CaseInsensitiveStringComparer.Instance);

        Dictionary<string, PostEffectFactory> postFXFac = new Dictionary<string, PostEffectFactory>(CaseInsensitiveStringComparer.Instance);


        public void RegisterModelEffectType(string name, ModelEffectFactory fac)
        {
            modelFXFac.Add(name, fac);
            EngineConsole.Instance.Write("找到模型特效 " + name, ConsoleMessageType.Information);
        }

        public void RegisterPostEffectType(string name, PostEffectFactory fac)
        {
            postFXFac.Add(name, fac);
            EngineConsole.Instance.Write("找到后期特效 " + name, ConsoleMessageType.Information);
        }

        public void UnregisterModelEffectType(string name)
        {
            modelFXFac.Remove(name);
        }

        public void UnregisterPostEffectType(string name)
        {
            postFXFac.Remove(name);
        }


        public ModelEffect GetModelEffect(string name)
        {
            return Enabled ? modelFX[name] : null;
        }

        public PostEffect GetPostEffect(string name)
        {
            return Enabled ? postFX[name] : null;
        }

        public int ModelEffectCount
        {
            get { return modelFX.Count; }
        }

        public int PostEffectCount
        {
            get { return postFX.Count; }
        }

        public bool HasModelEffect(string name)
        {
            return modelFX.ContainsKey(name);
        }

        public bool HasPostEffect(string name)
        {
            return postFX.ContainsKey(name);
        }

        public void LoadEffects()
        {
            EffectsLoaded = true;
            foreach (KeyValuePair<string, ModelEffectFactory> e in modelFXFac)
            {
                modelFX.Add(e.Key, e.Value.CreateInstance());
            }
            foreach (KeyValuePair<string, PostEffectFactory> e in postFXFac)
            {
                postFX.Add(e.Key, e.Value.CreateInstance());
            }

            string msg = "{0} 种模型特效以及 {1} 种后期特效已经加载。";
            EngineConsole.Instance.Write(
                string.Format(msg, modelFXFac.Count.ToString(), postFXFac.Count.ToString()), ConsoleMessageType.Information);
        }

        public void UnloadEffects()
        {
            EffectsLoaded = false;
            foreach (KeyValuePair<string, ModelEffect> e in modelFX)
            {
                modelFXFac[e.Key].DestroyInstance(e.Value);
            }
            foreach (KeyValuePair<string, PostEffect> e in postFX)
            {
                postFXFac[e.Key].DestroyInstance(e.Value);
            }
            modelFX.Clear();
            postFX.Clear();
        }

        public bool Enabled
        {
            get;
            set;
        }

        public bool EffectsLoaded
        {
            get;
            private set;
        }
        protected override void dispose()
        {
            if (EffectsLoaded)
            {
                UnloadEffects();
            }
            singleton = null;
        }
    }
}
