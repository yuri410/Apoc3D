using System;
using System.Collections.Generic;
using System.Drawing;
using System.Text;
using SlimDX;
using SlimDX.Direct3D9;
using Apoc.Collections;
using Apoc.CollisionModel.Shapes;
using Apoc.Graphics;
using Apoc.Graphics.Effects;
using Apoc.MathLib;

namespace Apoc.Scene
{
    /// <summary>
    ///  为场景管理器提供抽象基类
    /// </summary>
    public unsafe abstract class SceneManagerBase : IDisposable
    {
        #region Fields

        Device device;

        Cluster parentCluster;

        List<SceneObject> objects = new List<SceneObject>();

        #endregion

        #region Properties

        /// <summary>
        ///  获取一个System.Boolean，表示该对象是否已经释放
        /// </summary>
        public bool Disposed
        {
            get;
            private set;
        }

        public Cluster ParentCluster
        {
            get { return parentCluster; }
        }

        public List<SceneObject> SceneObjects
        {
            get { return objects; }
        }

        public float OffsetX
        {
            get;
            private set;
        }

        public float OffsetY
        {
            get;
            private set;
        }

        public float OffsetZ
        {
            get;
            private set;
        }
        #endregion

        #region Constructor
        protected SceneManagerBase(Device dev, Cluster cluster)
        {
            this.device = dev;

            this.OffsetX = cluster.WorldX;
            this.OffsetY = cluster.WorldY;
            this.OffsetZ = cluster.WorldZ;

            this.parentCluster = cluster;

            BuildSceneManager();
        }
        #endregion

        #region Methods

        /// <summary>
        /// 建立SceneManager的结构
        /// </summary>
        protected abstract void BuildSceneManager();

        /// <summary>
        ///  把物体加入到场景管理器中，并将它附到一个合适的节点
        /// </summary>
        /// <param name="obj">加入的物体</param>
        public virtual void AddObjectToScene(SceneObject obj)
        {
            obj.OffsetX = OffsetX;
            obj.OffsetY = OffsetY;
            obj.OffsetZ = OffsetZ;

            obj.ParentCluster = parentCluster;

            objects.Add(obj);
        }


        /// <summary>
        ///  把物体从场景管理器中移除
        /// </summary>
        /// <param name="obj">移除的物体</param>
        public virtual void RemoveObjectFromScene(SceneObject obj)
        {
            objects.Remove(obj);
        }

        /// <summary>
        /// 在场景中查找和视见体相交的物体
        /// </summary>
        /// <param name="objects"></param>
        /// <param name="frus"></param>
        public abstract void FindObjects(FastList<SceneObject> objects, Frustum frus);

        /// <summary>
        ///  超找符合IObjectFilter提供的条件的物体
        /// </summary>
        /// <param name="callBack">回调</param>
        /// <returns></returns>
        public List<SceneObject> FindObjects(IObjectFilter callBack)
        {
            List<SceneObject> result = new List<SceneObject>();
            for (int i = 0; i < objects.Count; i++)
            {
                if (callBack.Check(objects[i]))
                {
                    result.Add(objects[i]);
                }
            }
            return result;
        }

        /// <summary>
        ///  在场景中查找和射线相交的物体
        /// </summary>
        /// <param name="ray"></param>
        /// <returns>找到的物体，如果没找到返回null</returns>
        public abstract SceneObject FindObject(Ray ray);

        /// <summary>
        ///  在场景中查找和射线相交并符合回调的物体
        /// </summary>
        /// <param name="ray"></param>
        /// <param name="cbk"></param>
        /// <returns>找到的物体，如果没找到返回null</returns>
        public virtual SceneObject FindObject(Ray ray, IObjectFilter callBack)
        {
            SceneObject result = null;
            float nearest = float.MaxValue;

            for (int i = 0; i < objects.Count; i++)
            {
                SceneObject curObj = objects[i];
                if (callBack.Check(curObj) &&
                    curObj.IntersectsSelectionRay(ref ray))
                {
                    float dist = MathEx.DistanceSquared(ref curObj.BoundingSphere.Center, ref ray.Position);
                    if (dist < nearest)
                    {
                        nearest = dist;
                        result = curObj;
                    }
                }
            }
            return result;
        }

        /// <summary>
        /// 根据摄像机的视见体准备可见物体
        /// </summary>
        /// <param name="camera"></param>
        public abstract void PrepareVisibleObjects(ICamera camera, PassInfo batchHelper);

        /// <summary>
        /// 添加可见物体，准备渲染
        /// </summary>
        /// <param name="obj">要添加的物体</param>
        /// <remarks>用于渲染批次优化</remarks>
        protected void AddVisibleObject(SceneObject obj, PassInfo batchHelper)
        {
            batchHelper.RenderedObjectCount++;

            batchHelper.visibleObjects.Add(obj);

            RenderOperation[] ops = obj.GetRenderOperation();
            if (ops != null)
            {
                for (int k = 0; k < ops.Length; k++)
                {
                    if (ops[k].Geomentry != null)
                    {
                        Matrix ofsTrans = Matrix.Translation(obj.OffsetX, obj.OffsetY, obj.OffsetZ);

                        Matrix.Multiply(ref ops[k].Transformation, ref ofsTrans, out ops[k].Transformation);
                        Matrix.Multiply(ref ops[k].Transformation, ref obj.Transformation, out ops[k].Transformation);

                        MeshMaterial mate = ops[k].Material;
                        GeomentryData geoData = ops[k].Geomentry;

                        if (mate != null)
                        {
                            string desc;
                            bool supportsInst;
                            if (mate.Effect == null)
                            {
                                desc = string.Empty;
                                // if effect is null, instancing is supported by defualt
                                supportsInst = true;
                            }
                            else
                            {
                                supportsInst = mate.Effect.SupportsInstancing;
                                desc = mate.Effect.Name + "_" + mate.BatchIndex.ToString();
                            }


                            if (supportsInst && ops[k].Geomentry.UseIndices)
                            {
                                ModelEffect effect;
                                if (!batchHelper.effects.TryGetValue(desc, out effect))
                                {
                                    batchHelper.effects.Add(desc, mate.Effect);
                                }

                                Dictionary<MeshMaterial, Dictionary<GeomentryData, FastList<RenderOperation>>> matTable;
                                if (!batchHelper.instanceTable.TryGetValue(desc, out matTable))
                                {
                                    matTable = new Dictionary<MeshMaterial, Dictionary<GeomentryData, FastList<RenderOperation>>>();
                                    batchHelper.instanceTable.Add(desc, matTable);
                                }

                                Dictionary<GeomentryData, FastList<RenderOperation>> geoDataTbl;
                                if (!matTable.TryGetValue(mate, out geoDataTbl))
                                {
                                    geoDataTbl = new Dictionary<GeomentryData, FastList<RenderOperation>>();
                                    matTable.Add(mate, geoDataTbl);
                                }

                                FastList<RenderOperation> instOpList;
                                if (!geoDataTbl.TryGetValue(geoData, out instOpList))
                                {
                                    instOpList = new FastList<RenderOperation>();
                                    geoDataTbl.Add(geoData, instOpList);
                                }

                                instOpList.Add(ops[k]);
                            }
                            else
                            {
                                ModelEffect effect;
                                FastList<RenderOperation> opList;

                                if (!batchHelper.effects.TryGetValue(desc, out effect))
                                {
                                    batchHelper.effects.Add(desc, mate.Effect);
                                }

                                if (!batchHelper.batchTable.TryGetValue(desc, out opList))
                                {
                                    opList = new FastList<RenderOperation>();
                                    batchHelper.batchTable.Add(desc, opList);
                                }

                                //Matrix.Multiply(ref ops[k].Transformation, ref obj.Transformation, out ops[k].Transformation);
                                //ops[k].Transformation = obj.Transformation;
                                opList.Add(ops[k]);
                            }
                        }
                    }
                }
            }
        }


        public virtual void Update(float dt)
        {
            for (int i = 0; i < objects.Count; i++)
            {
                objects[i].Update(dt);
            }
        }


        #region IDisposable 成员



        protected virtual void Dispose(bool disposing)
        {
            if (disposing)
            {
                for (int i = 0; i < objects.Count; i++)
                {
                    if (!objects[i].Disposed)
                        objects[i].Dispose();
                }
                objects.Clear();
            }
            objects = null;
        }

        public void Dispose()
        {
            if (!Disposed)
            {
                Dispose(true);
                Disposed = true;
            }
            else
            {
                throw new ObjectDisposedException(ToString());
            }
        }

        #endregion
        #endregion
    }

    /// <summary>
    ///  实现一个简单的的场景管理器
    ///  
    /// </summary>
    public unsafe class SceneManager : SceneManagerBase
    {
        #region 字段

        protected SceneNode rootNode;

        List<SceneNode> sceneNodes;

        #endregion

        #region 构造函数

        public SceneManager(Device device, Cluster cluster, Atmosphere atmos)
            : base(device, cluster)
        {
        }

        #endregion

        #region 方法

        protected override void BuildSceneManager()
        {
            sceneNodes = new List<SceneNode>();
            // the default sceMgr is simply made up of one node
            SceneNode node = new SceneNode(this, null);
            sceneNodes.Add(node);

        }

        public override void AddObjectToScene(SceneObject obj)
        {
            base.AddObjectToScene(obj);
            if (sceneNodes.Count > 0)
            {
                sceneNodes[0].AttchedObjects.Add(obj);
            }
            //obj.OnAddedToScene(this, this);
        }

        public override void RemoveObjectFromScene(SceneObject obj)
        {
            base.RemoveObjectFromScene(obj);
            if (sceneNodes.Count > 0)
            {
                sceneNodes[0].RemoveObject(obj);
            }
            //obj.OnRemovedFromScene(this, this);
        }

        public override void FindObjects(FastList<SceneObject> objects, Frustum frus)
        {
            for (int i = 0; i < sceneNodes.Count; i++)
            {
                SceneNode node = sceneNodes[i];
                for (int j = 0; j < node.AttchedObjects.Count; j++)
                {
                    SceneObject curObj = node.AttchedObjects[j];
                    if (frus.IntersectsSphere(ref  curObj.BoundingSphere.Center, curObj.BoundingSphere.Radius))
                    {
                        objects.Add(curObj);
                    }
                }
            }
        }



        public override SceneObject FindObject(Ray ray)
        {
            SceneObject result = null;
            float nearest = float.MaxValue;
            for (int i = 0; i < sceneNodes.Count; i++)
            {
                SceneNode node = sceneNodes[i];
                for (int j = 0; j < node.AttchedObjects.Count; j++)
                {
                    SceneObject curObj = node.AttchedObjects[j];
                    if (curObj.IntersectsSelectionRay(ref ray))
                    {
                        float dist = MathEx.DistanceSquared(ref curObj.BoundingSphere.Center, ref ray.Position);
                        if (dist < nearest)
                        {
                            nearest = dist;
                            result = curObj;
                        }
                    }
                }
            }
            return result;
        }

        public override void PrepareVisibleObjects(ICamera camera, PassInfo batchHelper)
        {
            batchHelper.visibleObjects.FastClear();
            for (int i = 0; i < sceneNodes.Count; i++)
            {
                FastList<SceneObject> objs = sceneNodes[i].AttchedObjects;
                for (int j = 0; j < objs.Count; j++)
                {
                    if (objs[j].HasSubObjects)
                    {
                        objs[j].PrepareVisibleObjects(camera);
                    }

                    AddVisibleObject(objs[j], batchHelper);
                }
            }
        }

        protected override void Dispose(bool disposing)
        {
            base.Dispose(disposing);
            if (disposing)
            {
                sceneNodes.Clear();
            }
            sceneNodes = null;
        }

        #endregion
    }
}
