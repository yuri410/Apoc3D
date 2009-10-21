using System;
using System.Collections.Generic;
using System.Text;

namespace Apoc.MathLib
{
    public struct DirectDetectData
    {
        public Vector3 Position;
        public Vector3 Normal;
        public float Depth;

        public DirectDetectData(Vector3 p, Vector3 n, float depth)
        {
            Position = p;
            Normal = n;
            Depth = depth;
        }
    }

}
