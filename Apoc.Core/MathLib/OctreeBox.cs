using System;
using System.Collections.Generic;
using System.Text;

namespace Apoc.MathLib
{
    public struct OctreeBox
    {
        public Vector3 Center;

        public float Length;

        public OctreeBox(float length)
        {
            this.Length = length;
            this.Center.X = length * 0.5f;
            this.Center.Y = this.Center.X;
            this.Center.Z = this.Center.X;
        }
        public OctreeBox(float length, float posY)
        {
            this.Length = length;
            this.Center.X = length * 0.5f;
            this.Center.Y = posY;
            this.Center.Z = this.Center.X;
        }
        public OctreeBox(BoundingBox aabb)
        {
            Length = MathEx.Distance(ref aabb.Minimum, ref aabb.Maximum) / MathEx.Root3;
            Vector3.Add(ref aabb.Minimum, ref aabb.Maximum, out Center);
            Vector3.Multiply(ref Center, 0.5f, out Center);
        }

        public OctreeBox(ref BoundingSphere sph)
        {
            Center = sph.Center;
            Length = sph.Radius * 2;
        }

        public void GetBoundingSphere(out BoundingSphere sp)
        {
            sp.Center = Center;
            sp.Radius = Length * (MathEx.Root3 / 2f);  // 0.5f;
        }
    }
}
