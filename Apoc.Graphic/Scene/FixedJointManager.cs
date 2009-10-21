using System;
using System.Collections.Generic;
using System.Text;
using Apoc.Collections;
using Apoc.Physics;

namespace Apoc.Scene
{
    public class FixedJointManager
    {
        //Queue<FixedJoint> pool;
        //ExistTable<FixedJoint> existTable;

        //PhysicsWorld phyWorld;

        //JointGroup jointGroup;

        //const int MaxJoint = 500;

        //public FixedJointManager(PhysicsWorld world) 
        //{
        //    this.phyWorld = world;
        //    this.pool = new Queue<FixedJoint>();
        //    this.existTable = new ExistTable<FixedJoint>();
        //    this.jointGroup = new JointGroup();
        //}

        //public FixedJoint CreateJoint()
        //{
        //    if (pool.Count > 0)
        //    {
        //        FixedJoint joint = pool.Dequeue();
        //        existTable.Remove(joint);

        //        return joint;
        //    }
        //    FixedJoint j = new FixedJoint(phyWorld, jointGroup);

        //    return j;
        //}
        //public void DestoryJoint(FixedJoint joint)
        //{
        //    joint.Attach(null, null);
        //    if (pool.Count <= MaxJoint)
        //    {
        //        if (!existTable.Exists(joint))
        //        {
        //            existTable.Add(joint);
        //            pool.Enqueue(joint);
                   
        //            return;
        //        }
        //    }

        //    joint.Dispose();
        //}
    }
}
