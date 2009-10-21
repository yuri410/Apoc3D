using System;
using System.Collections.Generic;
using System.Text;
using Apoc.Collections;
using Apoc.Physics;

namespace Apoc.Scene
{
    public class RigidBodyManager
    {
        //Queue<RigidBody> pool;
        //ExistTable<RigidBody> existTable;

        //PhysicsWorld phyWorld;

        //const int MaxBody = 500;

        //public RigidBodyManager(PhysicsWorld phyWorld)
        //{
        //    this.pool = new Queue<RigidBody>();
        //    this.existTable = new ExistTable<RigidBody>();

        //    this.phyWorld = phyWorld;
        //}

        //public RigidBody CreateBody()
        //{
        //    if (pool.Count > 0)
        //    {
        //        RigidBody body = pool.Dequeue();
        //        existTable.Remove(body);

        //        body.Enabled = true;
        //        return body;
        //    }
        //    return new RigidBody(phyWorld);
        //}

        //public void DestoryBody(RigidBody body)
        //{
        //    if (pool.Count <= MaxBody)
        //    {
        //        if (!existTable.Exists(body))
        //        {
        //            existTable.Add(body);
        //            pool.Enqueue(body);

        //            body.Enabled = false;
        //            return;
        //        }
        //    }

        //    body.Dispose();
        //}


    }
}
