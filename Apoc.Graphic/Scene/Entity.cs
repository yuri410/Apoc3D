using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Text;
using Apoc.CollisionModel;
using Apoc.Graphics;
using Apoc.MathLib;
using Apoc.Physics;
using Apoc.Physics.Dynamics;
using PM = Apoc.Physics.MathLib;

namespace Apoc.Scene
{
    /// <summary>
    ///  表示有位置信息的物体
    /// </summary>
    public interface IPositionedObject
    {
        /// <summary>
        ///  获取或设置物体的位置
        /// </summary>
        Vector3 Position { get; set; }

        /// <summary>
        ///  更新变换矩阵。
        /// </summary>
        void UpdateTransform();

        /// <summary>
        ///  获取一个布尔值，表示物体是否可以在编辑器中操作。
        /// </summary>
        [Browsable(false)]
        bool EditorMovable { get; }
    }

    /// <summary>
    ///  表示可以旋转的物体
    /// </summary>
    public interface IRoatableObject
    {
        /// <summary>
        ///  获取或设置一个表示物体朝向的四元数
        /// </summary>
        Quaternion Orientation { get; set; }

        /// <summary>
        ///  更新变换矩阵。
        /// </summary>
        void UpdateTransform();

        /// <summary>
        ///  获取一个布尔值，表示物体是否可以在编辑器中操作。
        /// </summary>
        [Browsable(false)]
        bool EditorMovable { get; }
    }

    /// <summary>
    ///  表示有带有的场景物体
    /// </summary>
    public abstract class Entity : SceneObject, IUpdatable, IPositionedObject, IRoatableObject
    {
        #region 字段

        /// <summary>
        ///  物体的位置
        /// </summary>
        protected Vector3 position;

        /// <summary>
        ///  物体的朝向
        /// </summary>
        protected Quaternion orientation = Quaternion.Identity;

        /// <summary>
        ///  变换矩阵是否需要重新计算
        /// </summary>
        protected bool isTransformDirty;

        bool useLodModel;
        bool dontDraw;

        Vector3 boundingSphereOffset;

        /// <summary>
        ///  这个物体的LOD界别模型，索引0为LOD级别（仅次于最精细的）1的。
        /// </summary>
        protected Model[] lodModels;

        #endregion

        #region 构造函数

        /// <summary>
        ///  
        /// </summary>
        /// <param name="hasSubObjects">表示该物体是否含有子物体</param>
        protected Entity(bool hasSubObjects)
            : base(hasSubObjects)
        {
        }

        #endregion

        #region 属性

        /// <summary>
        ///  获取一个布尔值，表示这个物体是否有不同LOD级别的模型。
        /// </summary>
        protected bool HasLodModel
        {
            get { return lodModels != null; }
        }

        /// <summary>
        ///  Gets or sets a bool value which indicates the entity object is visible.
        ///  获取或设置一个布尔值，表示该物体是否可见
        /// </summary>
        public bool Visible
        {
            get;
            set;
        }

        /// <summary>
        ///  获取一个四元数，表示物体的朝向
        /// </summary>
        public Quaternion Orientation
        {
            get { return orientation; }
            set
            {
                if (RigidBody != null)
                {
                    RigidBody.Orientation = value;
                }
                orientation = value;
                isTransformDirty = true;
            }
        }

        /// <summary>
        ///  获取或设置物体在世界坐标系中的位置
        /// </summary>
        public Vector3 Position
        {
            get { return position; }
            set
            {
                position = value;

                BoundingSphere.Center = value + BoundingSphereOffset;

                if (RigidBody != null)
                {
                    RigidBody.Translate((PM.Vector3)value - RigidBody.CenterOfMassPosition);
                }
                isTransformDirty = true;
            }
        }

        /// <summary>
        ///  获取物体几何中心的坐标
        /// </summary>
        public Vector3 BoundingSphereOffset
        {
            get { return boundingSphereOffset; }
            protected set
            {
                boundingSphereOffset = value;
                BoundingSphere.Center = value + Position;
            }
        }

        /// <summary>
        ///  获取物体在LOD0级别的模型
        ///  Gets the model at level 0 (the detailed one)
        /// </summary>
        public Model ModelL0
        {
            get;
            protected set;
        }

        /// <summary>
        ///  获取物体在LOD1级别的模型，如果不带有这个模型返回Null。
        ///  Gets the model at level 1 (Lod Level 1)
        ///  If the entity doesn't have a lod model at this level, the return value is null.
        /// </summary>
        public Model ModelL1
        {
            get
            {
                if (lodModels != null && lodModels.Length > 1)
                {
                    return lodModels[0];
                }
                return null;
            }
            protected set
            {
                if (lodModels.Length < 1)
                {
                    Array.Resize<Model>(ref lodModels, 1);
                }
                lodModels[0] = value;
            }
        }

        /// <summary>
        ///  获取物体在LOD2级别的模型，如果不带有这个模型返回Null。
        ///  Get the model at level 2 (Lod Level 2)
        ///  If the entity doesn't have a lod model at this level, the return value is null.
        /// </summary>
        public Model ModelL2
        {
            get
            {
                if (lodModels != null && lodModels.Length > 2)
                {
                    return lodModels[1];
                }
                return null;
            }
            protected set
            {
                if (lodModels.Length < 2)
                {
                    Array.Resize<Model>(ref lodModels, 2);
                }
                lodModels[1] = value;
            }
        }

        /// <summary>
        ///  获取一个布尔值，表示物体是否可以在编辑器中操作。
        /// </summary>
        [Browsable(false)]
        public virtual bool EditorMovable
        {
            get { return true; }
        }

        #endregion

        #region 方法

        /// <summary>
        ///  当物体换到一个新的Cluster时被引擎调用。重写的方法应调用基类中的方法。
        /// </summary>
        /// <param name="newCluster">新的Cluster</param>
        public override void NotifyClusterChanged(Cluster newCluster)
        {
            float ofsX = newCluster.WorldX - OffsetX;
            float ofsY = newCluster.WorldY - OffsetY;
            float ofsZ = newCluster.WorldZ - OffsetZ;

            BoundingSphere.Center.X -= ofsX;
            BoundingSphere.Center.Y -= ofsY;
            BoundingSphere.Center.Z -= ofsZ;

            position.X -= ofsX;
            position.Y -= ofsY;
            position.Z -= ofsZ;

        }

        /// <summary>
        ///  更新变换矩阵。
        /// </summary>
        public virtual void UpdateTransform()
        {
            // T = r * t

            Matrix rotation;
            Matrix.RotationQuaternion(ref orientation, out rotation);

            Matrix.Translation(ref position, out Transformation);
            Matrix.Multiply(ref rotation, ref Transformation, out Transformation);


            RequiresUpdate = true;

        }

        /// <summary>
        ///  获取该物体的所有渲染操作
        /// </summary>
        /// <returns>包含所有渲染操作的数组，其中有的元素的GeomentryData成员为null，这些渲染操作无效，会被忽略</returns>
        public override RenderOperation[] GetRenderOperation()
        {
            if (HasLodModel)
            {
                if (ModelL1 != null)
                {
                    return ModelL1.GetRenderOperation();
                }
            }
            if (ModelL0 != null)
            {
                return ModelL0.GetRenderOperation();
            }

            return null;
        }

        /// <summary>
        ///  释放场景物体所使用的所有资源。应在派生类中重写。重写的方法应调用基类中的方法。
        /// </summary>
        /// <param name="disposing">表示是否需要释放该物体所引用的其他资源，当为真时，调用那些资源的Dispose方法</param>
        protected override void Dispose(bool disposing)
        {
            base.Dispose(disposing);

            ModelL0 = null;
            ModelL1 = null;
            ModelL2 = null;
        }

        #endregion

        #region IUpdatable 成员

        /// <summary>
        ///  更新该物体的状态，每一帧如果可见，则被引擎调用
        /// </summary>
        /// <param name="dt">时间间隔，以秒为单位</param>
        public override void Update(float dt)
        {
            if (ModelL0 != null)
            {
                ModelL0.Update(dt);
            }

            if (ModelL1 != null)
            {
                ModelL1.Update(dt);
            }

            if (GameScene != null)
            {
                ICamera camera = GameScene.CurrentCamera;

                float dist = Vector3.Distance(camera.Position, BoundingSphere.Center);

            //    HasLodModel = dist > BoundingSphere.Radius * 15f;

                dontDraw = (dist - BoundingSphere.Radius) > camera.FarPlane * 0.75f;
            }
            //else
            //{
            //    HasLodModel = false;
            //}

            if (isTransformDirty)
            {
                UpdateTransform();
                isTransformDirty = false;
            }
        }

        #endregion
    }
}
