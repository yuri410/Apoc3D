using System;
using System.Collections.Generic;
using System.Text;
using SlimDX;
using Apoc.MathLib;

namespace Apoc.Scene
{
    /// <summary>
    ///  八叉树场景节点
    /// </summary>
    public class OctreeSceneNode : SceneNodeBase
    {
        //const float MinBVSize = 10f;

        enum Extend
        {
            None,
            PxPyPz,
            NxPyPz,
            PxPyNz,
            NzPyNz,
            PxNyPz,
            NxNyPz,
            PxNyNz,
            NxNyNz
        }

        OctreeSceneNode[][][] childTable;
        Extend[][][] nodeExtends;


        //new List<OctreeSceneNode> children;
        OctreeSceneNode parent;

        public OctreeBox BoundingVolume;

        public BoundingSphere BoundingSphere;


        public OctreeSceneManager Manager
        {
            get;
            private set;
        }


        public OctreeSceneNode(OctreeSceneManager mgr, OctreeSceneNode parent)
        {
            Manager = mgr;

            this.parent = parent;


            //children = new List<OctreeSceneNode>();
            childTable = new OctreeSceneNode[2][][];

            childTable[0] = new OctreeSceneNode[2][];
            childTable[1] = new OctreeSceneNode[2][];

            childTable[0][0] = new OctreeSceneNode[2];
            childTable[1][0] = new OctreeSceneNode[2];
            childTable[0][1] = new OctreeSceneNode[2];
            childTable[1][1] = new OctreeSceneNode[2];

            nodeExtends = new Extend[2][][];

            nodeExtends[0] = new Extend[2][];
            nodeExtends[1] = new Extend[2][];

            nodeExtends[0][0] = new Extend[2];
            nodeExtends[1][0] = new Extend[2];
            nodeExtends[0][1] = new Extend[2];
            nodeExtends[1][1] = new Extend[2];
        }


        public override void AddObject(SceneObject obj)
        {
            // check the obj's bv size, and decide if this is the level to add it.
            // if obj's size is small enough, put it in to child nodes

            float ofLen = BoundingVolume.Length / 4f;

            if (ofLen > Manager.MinimumBVSize && obj.BoundingSphere.Radius <= (1f / 16f) * BoundingVolume.Length)
            {
                // if the child node is more suitable, add obj to the child node 
                int i = obj.BoundingSphere.Center.X - BoundingVolume.Center.X > 0 ? 1 : 0;
                int j = obj.BoundingSphere.Center.Y - BoundingVolume.Center.Y > 0 ? 1 : 0;
                int k = obj.BoundingSphere.Center.Z - BoundingVolume.Center.Z > 0 ? 1 : 0;

                if (childTable[i][j][k] == null)
                {
                    childTable[i][j][k] = new OctreeSceneNode(Manager, this);

                    childTable[i][j][k].BoundingVolume.Length = BoundingVolume.Length / 2f;

                    float ofx = (i == 0 ? -1f : 1f) * ofLen;
                    float ofy = (j == 0 ? -1f : 1f) * ofLen;
                    float ofz = (k == 0 ? -1f : 1f) * ofLen;

                    childTable[i][j][k].BoundingVolume.Center = new Vector3(BoundingVolume.Center.X + ofx, BoundingVolume.Center.Y + ofy, BoundingVolume.Center.Z + ofz);

                    childTable[i][j][k].BoundingVolume.GetBoundingSphere(out childTable[i][j][k].BoundingSphere);

                }
                childTable[i][j][k].AddObject(obj);

                // change the extend if necessary

            }
            else
            {
                obj.ParentSceneNode = this;
                AttchedObjects.Add(obj);
            }
        }

        private bool RemoveAttachedObject(SceneObject obj)
        {
            for (int i = AttchedObjects.Count - 1; i >= 0; i--)
            {
                if (AttchedObjects[i] == obj)
                {
                    AttchedObjects.RemoveAt(i);
                    obj.ParentSceneNode = null;
                    return true;
                }
            }
            return false;
        }

        private bool RemoveObjectInternal(SceneObject obj)
        {
            for (int i = 0; i < 2; i++)
                for (int j = 0; j < 2; j++)
                    for (int k = 0; k < 2; k++)
                    {
                        if (childTable[i][j][k] != null &&
                            MathEx.BoundingSphereIntersects(ref childTable[i][j][k].BoundingSphere, ref obj.BoundingSphere))
                        {
                            if (childTable[i][j][k].RemoveObjectInternal(obj))
                            {
                                return true;
                            }
                        }
                    }

            return RemoveAttachedObject(obj);
        }

        public override void RemoveObject(SceneObject obj)
        {
            if (obj.ParentSceneNode != null) 
            {
                OctreeSceneNode node = (OctreeSceneNode)obj.ParentSceneNode;

                node.RemoveObjectInternal(obj);
            }
            else
            {
                RemoveObjectInternal(obj);
            }
        }

        public OctreeSceneNode this[int i, int j, int k]
        {
            get { return childTable[i][j][k]; }
        }

        [Obsolete()]
        protected override void Eject(SceneObject obj)
        {
            if (!EjectTestObject(obj) && parent != null)
            {
                parent.Eject(obj);
            }
        }

        [Obsolete()]
        bool EjectContains(SceneObject obj)
        {
            float distSqr = MathEx.DistanceSquared(ref BoundingSphere.Center, ref obj.BoundingSphere.Center);
            return (distSqr < BoundingSphere.Radius * BoundingSphere.Radius);
        }
        [Obsolete()]
        bool EjectTestObject(SceneObject obj)
        {
            float ofLen = BoundingVolume.Length / 4f;
            if (parent != null)
            {
                if (ofLen > Manager.MinimumBVSize && obj.BoundingSphere.Radius <= (1f / 16f) * BoundingVolume.Length)
                {
                    float distSqr = MathEx.DistanceSquared(ref BoundingSphere.Center, ref obj.BoundingSphere.Center);
                    if (distSqr < BoundingSphere.Radius * BoundingSphere.Radius)
                    {
                        int i = obj.BoundingSphere.Center.X - BoundingVolume.Center.X > 0 ? 1 : 0;
                        int j = obj.BoundingSphere.Center.Y - BoundingVolume.Center.Y > 0 ? 1 : 0;
                        int k = obj.BoundingSphere.Center.Z - BoundingVolume.Center.Z > 0 ? 1 : 0;

                        if (childTable[i][j][k] == null)
                        {
                            childTable[i][j][k] = new OctreeSceneNode(Manager, this);

                            childTable[i][j][k].BoundingVolume.Length = BoundingVolume.Length / 2f;

                            float ofx = (i == 0 ? -1f : 1f) * ofLen;
                            float ofy = (j == 0 ? -1f : 1f) * ofLen;
                            float ofz = (k == 0 ? -1f : 1f) * ofLen;

                            childTable[i][j][k].BoundingVolume.Center = new Vector3(BoundingVolume.Center.X + ofx, BoundingVolume.Center.Y + ofy, BoundingVolume.Center.Z + ofz);

                            childTable[i][j][k].BoundingVolume.GetBoundingSphere(out childTable[i][j][k].BoundingSphere);

                        }
                        childTable[i][j][k].AddObject(obj);
                        return true;
                    }
                }
            }
            return false;
        }

        public override void Update()
        {
            if (childTable != null)
            {
                for (int i = 0; i < 2; i++)
                    for (int j = 0; j < 2; j++)
                        for (int k = 0; k < 2; k++)
                        {
                            if (childTable[i][j][k] != null)
                            {
                                childTable[i][j][k].Update();
                            }
                        }
            }

            //for (int i = AttchedObjects.Count - 1; i >= 0; i--)
            //{
            //    if (AttchedObjects[i].RequiresUpdate)
            //    {
            //        AttchedObjects[i].RequiresUpdate = false;
            //        if (!EjectContains(AttchedObjects[i]))
            //        {
            //            Eject(AttchedObjects[i]);
            //            AttchedObjects.RemoveAt(i--);                        
            //        }

            //    }
            //}
        }


        //public Extend GetExtend(int i, int j, int k)
        //{
        //    return nodeExtends[i][j][k];
        //}

        //public override void AddChild(OctreeSceneNode ch)
        //{

        //}

        //public void FindNode(ref BoundingBox box)
        //{

        //}

        //public override void AddChild(SceneNode ch)
        //{
        //    OctreeSceneNode och = (OctreeSceneNode)ch;

        //    // wtf?if the node is not inside this node, enlarge this node's bv.



        //}

        //public override void Changed()
        //{
        //    base.Changed();
        //}        
    }
}
