using System;
using System.Collections.Generic;
using System.Text;
using Apoc.CollisionModel;
using Apoc.CollisionModel.Shapes;
using Apoc.Physics;
using Apoc.Physics.Dynamics;
using PM = Apoc.Physics.MathLib;

namespace Apoc.Scene
{
    public abstract class DynamicObject : Entity
    {
        //protected DefaultMotionState motionState;

        #region 构造函数

        public DynamicObject()
            : base(false)
        {
        }

        public DynamicObject(bool hasSubObjects)
            : base(hasSubObjects)
        {
        }

        #endregion

        #region 方法

        public override void Update(float dt)
        {
            if (RigidBody != null)
            {
                position = RigidBody.CenterOfMassPosition;
                orientation = RigidBody.Orientation;

                isTransformDirty = true;
            }

            base.Update(dt);
        }
        public override void UpdateTransform()
        {
            base.UpdateTransform(); //Transformation = ((DefaultMotionState)RigidBody.MotionState).GraphicsWorldTransform;

            BoundingSphere.Center = position + BoundingSphereOffset;

            RequiresUpdate = true;
        }

        #endregion

        #region 物理相关
        public override bool HasPhysicsModel
        {
            get { return true; }
        }
        public override void BuildPhysicsModel(DynamicsWorld world)
        {
            UpdateTransform();

            BoxShape shape = new BoxShape(new Vector3(1));
            MotionState motionState = new DefaultMotionState(Transformation);

            PM.Vector3 inertia;
            shape.CalculateLocalInertia(1, out inertia);

            RigidBody = new RigidBody(1, motionState, shape, inertia, 0, 0, 0.5f, 0.5f);

            if (world != null)
            {
                world.AddRigidBody(RigidBody);
            }
        }

        #endregion
    }
}
