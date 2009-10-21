using System;
using System.Collections.Generic;
using System.Text;
using SlimDX;
using Apoc.Collections;

namespace Apoc.Graphics.Animation
{
    public class Bone
    {
        public Bone(int index)
        {
            this.Index = index;
        }

        public Bone(int index, Matrix[] transforms, Bone[] children, Bone parent)
        {
            this.Index = index;
            this.Transforms = transforms;
            this.Children = new BoneCollection(children);
            this.Parent = parent;
        }

        public Matrix[] Transforms
        {
            get;
            set;
        }

        public Bone Parent
        {
            get;
            set;
        }

        public BoneCollection Children
        {
            get;
            private set;
        }

        public int Index
        {
            get;
            set;
        }
    }
}
