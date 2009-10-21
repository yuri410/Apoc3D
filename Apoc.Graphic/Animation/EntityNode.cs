using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Text;
using SlimDX;
using Apoc.Collections;
using Apoc.Design;

namespace Apoc.Graphics.Animation
{
    [TypeConverter(typeof(ExpandableObjectConverter))]
    public class EntityNode
    {
        public EntityNode(int index)
        {
            //this.Entity = mesh;
            this.Index = index;
        }

        public EntityNode(int index, EntityNodeCollection children, EntityNode parent, Matrix[] transforms)
        {
            this.Index = index;
            this.Parent = parent;
            this.Children = children;
            //this.Entity = mesh;
            this.Transforms = transforms;
        }

        public EntityNodeCollection Children
        {
            get;
            private set;
        }


        public EntityNode Parent
        {
            get;
             set;
        }

        //public Mesh Entity
        //{
        //    get;
        //    private set;
        //}

        public int Index
        {
            get;
            private set;
        }

        [TypeConverter(typeof(ArrayConverter<MatrixEditor, Matrix>))]
        public Matrix[] Transforms
        {
            get;
            set;
        }

    //    public Matrix CurrentTransform
    //    {
    //        get;
    //        set;
    //    }
    }
}
