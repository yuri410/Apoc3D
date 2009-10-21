using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Text;
using SlimDX;
using SlimDX.Direct3D9;
using Apoc.CollisionModel;
using Apoc.CollisionModel.Dispatch;
using Apoc.CollisionModel.Shapes;
using Apoc.Graphics;
using Apoc.MathLib;
using Apoc.Physics;
using Apoc.Physics.Dynamics;
using PM = Apoc.Physics.MathLib;

namespace Apoc.Scene
{
    /// <summary>
    ///  表示静态物体
    /// </summary>
    public abstract class StaticObject : Entity
    {
        #region 字段

        DefaultMotionState motionState;

        bool isPhyBuilt;

        CollisionMesh cdMesh;

        #endregion

        #region 构造函数

        public StaticObject()
            : base(false)
        {
        }

        public StaticObject(bool hasSubObjects)
            : base(hasSubObjects)
        {

        }

        public StaticObject(Vector3 position, Quaternion orientation, bool hasSubObjects)
            : base(hasSubObjects)
        {
            base.position = position;
            base.orientation = orientation;

            base.UpdateTransform();
        }

        #endregion

       
        //public override bool IntersectsSelectionRay(ref Ray ray)
        //{
        //    if (RigidBody == null)
        //    {
        //        return base.IntersectsSelectionRay(ref ray);                       
        //    }

        //    BvhTriMeshResShape shape = RigidBody.CollisionShape as BvhTriMeshResShape;

        //    LineSegment line = new LineSegment(ray.Position, ray.Direction * 1000 + ray.Position);

        //    if (shape != null)
        //    {
        //        CollisionWorld.ClosestRayResultCallback cbk = new CollisionWorld.ClosestRayResultCallback(line.Start, line.End);

        //        PM.Matrix worldTocollisionObject = MathUtil.InvertMatrix(RigidBody.WorldTransform);

        //        PM.Vector3 rayFromLocal = PM.Vector3.TransformNormal(line.Start, worldTocollisionObject);
        //        PM.Vector3 rayToLocal = PM.Vector3.TransformNormal(line.End, worldTocollisionObject);

        //        BridgeTriangleRaycastCallback rcb = new BridgeTriangleRaycastCallback(rayFromLocal, rayToLocal, cbk, RigidBody, shape);
        //        rcb.HitFraction = cbk.ClosestHitFraction;

        //        PM.Vector3 rayAabbMinLocal = rayFromLocal;
        //        MathUtil.SetMin(ref rayAabbMinLocal, rayToLocal);
        //        PM.Vector3 rayAabbMaxLocal = rayFromLocal;
        //        MathUtil.SetMax(ref rayAabbMaxLocal, rayToLocal);

        //        shape.ProcessAllTriangles(rcb, rayAabbMinLocal, rayAabbMaxLocal);

        //        return (cbk.CollisionObject == RigidBody);
        //    }

        //    return false;
        //}

        #region 物理相关

        [Browsable(false)]
        public override bool HasPhysicsModel
        {
            get { return true; }
        }


        public unsafe override void BuildPhysicsModel(DynamicsWorld world)
        {
            if (!isPhyBuilt)
            {
                UpdateTransform();

                Matrix trans = Transformation;
                trans.M41 = 0;
                trans.M42 = 0;
                trans.M43 = 0;

                cdMesh = CollisionMeshManager.Instance.CreateInstance(ModelL0, trans);
                BvhTriMeshResShape shape = new BvhTriMeshResShape(cdMesh);


                motionState = new DefaultMotionState(Matrix.Translation(position));

                RigidBody = new RigidBody(0, motionState, shape);
                RigidBody.CollisionFlags |= CollisionOptions.StaticObject;

                PM.Vector3 aabbMin;
                PM.Vector3 aabbMax;
                shape.GetAabb(PM.Matrix.Identity, out aabbMin, out aabbMax);

                float rad = PM.Vector3.Distance(aabbMin, aabbMax) * 0.5f;

                BoundingSphereOffset = 0.5f * (aabbMin + aabbMax);
                BoundingSphere.Radius = rad;

                if (world != null)
                {
                    world.AddRigidBody(RigidBody);
                }
                isPhyBuilt = true;
            }
        }

        protected override void Dispose(bool disposing)
        {
            base.Dispose(disposing);

            if (disposing)
            {
                CollisionMeshManager.Instance.DestoryInstance(cdMesh);
            }
            cdMesh = null;
        }
        #endregion
    }
}
