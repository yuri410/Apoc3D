using System;
using System.Collections.Generic;
using System.Text;
using SlimDX.Direct3D9;
using Apoc.Core;
using Apoc.IO;
using VBC = Apoc.Core;

namespace Apoc.Graphics
{
    public class ModelManager : Apoc.Core.ResourceManager
    {
        static ModelManager singleton;

        public static ModelManager Instance
        {
            get
            {
                return singleton;
            }
        }

        public static void Initialize()
        {
            if (singleton == null)
            {
                singleton = new ModelManager(1048576 * 92);
            }
            EngineConsole.Instance.Write("模型管理器初始化完毕。内存使用上限" + Math.Round(singleton.TotalCacheSize / 1048576.0, 2).ToString() + "MB。", ConsoleMessageType.Information);
        }

        public ModelManager() { }
        public ModelManager(int cacheSize)
            : base(cacheSize)
        {
        }
        public Model CreateInstance(Device device, ResourceLocation rl)
        {
            VBC.Resource retrived = base.Exists(rl.Name);
            if (retrived == null)
            {
                Model mdl = new Model(device, rl);// Model.FromFile(device, rl);
                retrived = mdl;
                base.NewResource(mdl, CacheType.Static);
            }
            else
            {
                retrived.Use();
            }
            return new Model(device, (Model)retrived);


            //Entry ent;
            //if (instances.TryGetValue(rl.Name, out ent))
            //{
            //    ent.ReferenceCount++;
            //    return new Model(rs, ent.Model);
            //}

            //Model model = Model.FromFile(rs, rl);

            //ent = new Entry(model);
            //instances.Add(rl.Name, ent);
            //hashTable.Add(model, ent);

            //return model;
        }

        //protected override void dispose()
        //{
        //    Dictionary<string, Entry>.ValueCollection vals = instances.Values;
        //    foreach (Entry mdl in vals)
        //    {
        //        mdl.Model.Dispose();
        //    }
        //    instances = null;
        //}

        public void DestoryInstance(Model model)
        {
            base.DestoryResource(model);
            //Entry ent;
            //if (hashTable.TryGetValue(model, out ent))
            //{
            //    ent.ReferenceCount--;
            //    if (ent.ReferenceCount == 0)
            //    {
            //        ent.Model.Dispose();
            //    }
            //}
            //model.Dispose();
        }
    }
}
