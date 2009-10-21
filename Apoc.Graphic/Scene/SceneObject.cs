using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Text;
using SlimDX;
using Apoc.Collections;
using Apoc.CollisionModel.Dispatch;
using Apoc.CollisionModel.Shapes;
using Apoc.Graphics;
using Apoc.IO;
using Apoc.MathLib;
using Apoc.Physics;
using Apoc.Physics.Dynamics;

namespace Apoc.Scene
{
    /// <summary>
    /// 场景中的对象
    /// </summary>
    public abstract class SceneObject : IRenderable, IUpdatable, IDisposable
    {
        #region 物理相关

        /// <summary>
        ///  【见方法】
        /// </summary>
        RigidBody rigidBody;

        /// <summary>
        ///  获取该物体在物理引擎中的的刚体对象
        /// </summary>
        [Browsable(false)]
        public RigidBody RigidBody
        {
            get { return rigidBody; }
            protected set { rigidBody = value; }
        }

        /// <summary>
        ///  获取一个布尔值，表示该物体是否有物理方面的资源需要创建。
        ///  当值为真时，引擎会调用BuildPhysicsModel方法
        /// </summary>
        [Browsable(false)]
        public virtual bool HasPhysicsModel
        {
            get { return false; }
        }

        /// <summary>
        ///  加载该物体所有与物体有关的资源，当场景中所有物体均被加载后，引擎才会开始调用该方法。
        /// </summary>
        /// <param name="world"></param>
        public virtual void BuildPhysicsModel(DynamicsWorld world)
        {
            throw new NotSupportedException();
        }

        /// <summary>
        ///  【无信息】物理引擎未提供信息。可能是刚体所受的力矩回乘上这个比率。
        /// </summary>
        public float AngularFactor
        {
            get
            {
                if (RigidBody == null)
                    return 0;
                return rigidBody.AngularFactor;
            }
            set { rigidBody.AngularFactor = value; }
        }

        /// <summary>
        ///  【无信息】物理引擎未提供信息。
        /// </summary>
        public ContactSolverType FrictionSolverType
        {
            get
            {
                if (RigidBody == null)
                    return ContactSolverType.Default;
                return rigidBody.FrictionSolverType;
            }
            set { rigidBody.FrictionSolverType = value; }
        }

        /// <summary>
        ///  【无信息】物理引擎未提供信息。
        /// </summary>
        public ContactSolverType ContactSolverType
        {
            get
            {
                if (RigidBody == null)
                    return ContactSolverType.Default;
                return RigidBody.ContactSolverType;
            }
            set { rigidBody.ContactSolverType = value; }
        }

        /// <summary>
        ///  【无信息】物理引擎未提供信息。获取或设置一个浮点数，表示刚体碰撞时的恢复系数。
        /// </summary>
        public float Restitution
        {
            get
            {
                if (RigidBody == null)
                    return 0;
                return RigidBody.Restitution;
            }
            set { RigidBody.Restitution = value; }
        }
        /// <summary>
        ///  【无信息】物理引擎未提供信息。获取或设置一个浮点数，表示刚体碰撞或接触时受到的摩擦因数。
        /// </summary>
        public float Friction
        {
            get
            {
                if (RigidBody == null)
                    return 0; 
                return RigidBody.Friction;
            }
            set { RigidBody.Friction = value; }
        }
        /// <summary>
        ///  【无信息】物理引擎未提供信息。设置刚体运动的阻尼
        /// </summary>
        /// <param name="linDamping">线性运动阻尼</param>
        /// <param name="angDamping">角运动阻尼</param>
        public void SetDamping(float linDamping, float angDamping)
        {
            RigidBody.SetDamping(linDamping, angDamping);
        }

        #endregion

        #region 字段

        /// <summary>
        ///  【见属性】
        /// </summary>
        Cluster parentCluster;

        /// <summary>
        ///  该物体的包围球
        /// </summary>
        public BoundingSphere BoundingSphere;

        /// <summary>
        ///  该物体在世界坐标系中的变换矩阵
        /// </summary>
        public Matrix Transformation = Matrix.Identity;

        #endregion

        #region 属性

        /// <summary>
        ///  由Cluster造成的X偏移，世界坐标单位
        /// </summary>
        [Browsable(false)]
        public float OffsetX
        {
            get;
            set;
        }
        /// <summary>
        ///  由Cluster造成的Y偏移，世界坐标单位
        /// </summary>
        [Browsable(false)]
        public float OffsetY
        {
            get;
            set;
        }
        /// <summary>
        ///  由Cluster造成的Z偏移，世界坐标单位
        /// </summary>
        [Browsable(false)]
        public float OffsetZ
        {
            get;
            set;
        }

        /// <summary>
        ///  获取或设置一个布尔值，表示该物体是否需要更新
        /// </summary>
        /// <remarks>当变换矩阵变化以后为true</remarks>
        [Browsable(false)]
        public bool RequiresUpdate
        {
            get;
            set;
        }

        /// <summary>
        ///  获取场景物体所在的GameScene对象
        /// </summary>
        [Browsable(false)]
        public GameScene GameScene
        {
            get;
            private set;
        }

        /// <summary>
        ///  获取场景物体所在的Scene Node
        /// </summary>
        [Browsable(false)]
        public SceneNodeBase ParentSceneNode
        {
            get;
            protected internal set;
        }

        /// <summary>
        ///  获取场景物体所在的Cluster
        /// </summary>
        [Browsable(false)]
        public Cluster ParentCluster
        {
            get { return parentCluster; }
            set
            {
                parentCluster = value;
                if (value != null)
                {
                    GameScene = value.GameScene;
                }
            }
        }

        /// <summary>
        /// 获取该SceneObject是否包含子物体。当它为真时，渲染时会引擎回调用
        /// </summary>
        [Browsable(false)]
        public bool HasSubObjects
        {
            get;
            protected set;
        }

        #endregion

        #region 构造函数

        /// <summary>
        ///  SceneObject的构造函数。
        /// </summary>
        /// <param name="hasSubObject">表示该物体是否含有子物体，当为真时，引擎在渲染时会调用PrepareVisibleObjects</param>
        protected SceneObject(bool hasSubObject)
        {
            HasSubObjects = hasSubObject;
        }

        #endregion


        /// <summary>
        ///  如果该SceneObject包含子物体，准备摄像机的可见物体
        /// </summary>
        /// <param name="cam">
        ///  渲染到的摄像机
        /// </param>
        public virtual void PrepareVisibleObjects(ICamera cam)
        {
            throw new NotSupportedException();
        }

        /// <summary>
        ///  当物体加入到场景管理器时被调用
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="sceneMgr">加入到的场景管理器</param>
        public virtual void OnAddedToScene(object sender, OctreeSceneManager sceneMgr) { }

        /// <summary>
        ///  当物体从场景管理器中移除时被调用
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="sceneMgr">源场景管理器</param>
        public virtual void OnRemovedFromScene(object sender, OctreeSceneManager sceneMgr) { }

        /// <summary>
        ///  当物体换到一个新的Cluster时被引擎调用。重写的方法应调用基类中的方法。
        /// </summary>
        /// <param name="newCluster">新的Cluster</param>
        public virtual void NotifyClusterChanged(Cluster newCluster)
        {
            float ofsX = newCluster.WorldX - OffsetX;
            float ofsY = newCluster.WorldY - OffsetY;
            float ofsZ = newCluster.WorldZ - OffsetZ;

            BoundingSphere.Center.X -= ofsX;
            BoundingSphere.Center.Y -= ofsY;
            BoundingSphere.Center.Z -= ofsZ;
        }

        /// <summary>
        ///  计算物体是否与拾取射线相交
        /// </summary>
        /// <param name="ray"></param>
        /// <returns></returns>
        public virtual bool IntersectsSelectionRay(ref Ray ray)
        {
            return MathEx.BoundingSphereIntersects(ref BoundingSphere, ref ray);
        }


        #region IRenderable 成员

        /// <summary>
        ///  获取该物体的所有渲染操作
        /// </summary>
        /// <returns></returns>
        public abstract RenderOperation[] GetRenderOperation();


        #endregion

        #region IUpdatable 成员

        /// <summary>
        ///  更新该物体的状态，每一帧如果可见，则被引擎调用
        /// </summary>
        /// <param name="dt">帧时间间隔，以秒为单位</param>
        public abstract void Update(float dt);

        #endregion

        #region SceneObject 序列化

        /// <summary>
        ///  获取一个布尔值，表示该物体是否可序列化为二进制数据
        /// </summary>
        [Browsable(false)]
        public abstract bool IsSerializable
        {
            get;
        }

        /// <summary>
        ///  序列化这个物体，并将数据存入 二进制层次块data。
        /// </summary>
        /// <param name="data"></param>
        public virtual void Serialize(BinaryDataWriter data)
        {
            throw new NotSupportedException();
        }

        /// <summary>
        ///  获取一个字符串，表示该物体的类型。
        /// </summary>
        [Browsable(false)]
        public virtual string TypeTag
        {
            get { return "Unknown"; }
        }
        #endregion

        #region IDisposable 成员

        /// <summary>
        ///  释放场景物体所使用的所有资源。应在派生类中重写。
        /// </summary>
        /// <param name="disposing">表示是否需要释放该物体所引用的其他资源，当为真时，调用那些资源的Dispose方法</param>
        protected virtual void Dispose(bool disposing)
        {

        }

        /// <summary>
        ///  获取一个布尔值，表示该场景物体是否已经释放了资源。
        /// </summary>
        [Browsable(false)]
        public bool Disposed
        {
            get;
            private set;
        }

        /// <summary>
        ///  释放场景物体所使用的所有资源。
        /// </summary>
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
    }
}
