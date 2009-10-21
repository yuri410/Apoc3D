using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Text;
using SlimDX;
using Apoc.Collections;
using Apoc.IO;

namespace Apoc.Graphics.Animation
{
    [TypeConverter(typeof(ExpandableObjectConverter))]
    public class TransformAnimationInstance : AnimationInstance
    {
        Matrix[] currentTransform;
        TransformAnimation data;

        EntityNode[] rootNodes;

        public TransformAnimationInstance(TransformAnimation data)
            : base(data)
        {
            this.data = data;

            currentTransform = new Matrix[data.EntityCount];
            for (int i = 0; i < data.EntityCount; i++)
            {
                currentTransform[i] = Matrix.Identity;
            }

            rootNodes = data.RootNodes;
        }

        void Pass(EntityNode node)
        {
            if (node.Parent == null)
            {
                currentTransform[node.Index] = node.Transforms[CurrentFrame];// frames[CurrentFrame][node.Index];
                //node.CurrentTransform = currentTransform[node.Index];
            }
            else
            {
                currentTransform[node.Index] = node.Transforms[CurrentFrame] * node.Parent.Transforms[CurrentFrame];// frames[CurrentFrame][node.Index] * node.Parent;
                //node.CurrentTransform = currentTransform[node.Index];
            }

            EntityNodeCollection children = node.Children;

            if (children != null && children.Count > 0)
            {
                for (int i = 0; i < children.Count; i++)
                {
                    Pass(children[i]);
                }
            }
        }
        void Pass(EntityNode node, float lerp)
        {
            if (node.Parent == null)
            {
                Matrix.Lerp(ref node.Transforms[CurrentFrame], ref node.Transforms[CurrentFrame - 1], lerp, out  currentTransform[node.Index]);

                currentTransform[node.Index] = node.Transforms[CurrentFrame];
                //node.CurrentTransform = currentTransform[node.Index];
            }
            else
            {
                Matrix tmp;

                Matrix.Lerp(ref node.Transforms[CurrentFrame], ref node.Transforms[CurrentFrame - 1], lerp, out  currentTransform[node.Index]);
                Matrix.Lerp(ref node.Parent.Transforms[CurrentFrame], ref node.Parent.Transforms[CurrentFrame - 1], lerp, out  tmp);

                currentTransform[node.Index] *= tmp;

                //currentTransform[node.Index] = node.Transforms[CurrentFrame] * node.Parent.Transforms[CurrentFrame];// frames[CurrentFrame][node.Index] * node.Parent;
                //node.CurrentTransform = currentTransform[node.Index];
            }

            EntityNodeCollection children = node.Children;

            if (children != null && children.Count > 0)
            {
                for (int i = 0; i < children.Count; i++)
                {
                    Pass(children[i]);
                }
            }
        }

        public TransformAnimation Data
        {
            get { return data; }
        }

        public override void Update(float dt)
        {
            base.Update(dt);

            if (data.FrameLength > float.Epsilon)
            {
                if (CurrentFrame > 0)
                {
                    float lerpAmount = (CurrentTime - CurrentFrame * data.FrameLength) / data.FrameLength;
                    for (int i = 0; i < rootNodes.Length; i++)
                    {
                        Pass(rootNodes[i], lerpAmount);
                    }
                }
                else
                {
                    for (int i = 0; i < rootNodes.Length; i++)
                    {
                        Pass(rootNodes[i]);
                    }
                }
            }
        }

        public Matrix GetTransform(int entity)
        {
            return currentTransform[entity];
        }
    }

    [TypeConverter(typeof(ExpandableObjectConverter))]
    public class TransformAnimation : AnimationData
    {
        static readonly string FrameCountTag = "FrameCount";
        static readonly string FrameLengthTag = "FrameLength";
        static readonly string EntityNodeTag = "Node";

        static readonly string NodeChildrenCount = "ChildrenCount";
        static readonly string NodeChildren = "Children";
        static readonly string NodeParent = "Parent";

        public int EntityCount
        {
            get;
            private set;
        }

        EntityNode[] rootNodes;

        EntityNode[] nodes;

        public TransformAnimation(int entityCount)
        {
            EntityCount = entityCount;
            base.FrameLength = 0.025f;

            nodes = new EntityNode[EntityCount];

            for (int i = 0; i < EntityCount; i++)
            {
                nodes[i] = new EntityNode(i);
                nodes[i].Transforms = new Matrix[] { Matrix.Identity };
            }
            rootNodes = nodes;
            
        }


        public void Load(BinaryDataReader data)
        {
            FrameCount = data.GetDataInt32(FrameCountTag);
            FrameLength = data.GetDataSingle(FrameLengthTag);

            List<EntityNode> roots = new List<EntityNode>();

            for (int i = 0; i < EntityCount; i++)
            {   
                Matrix[] frameTB = new Matrix[FrameCount];

                ContentBinaryReader br = data.GetData(EntityNodeTag + i.ToString());

                for (int j = 0; j < FrameCount; j++)
                {
                    br.ReadMatrix(out frameTB[j]);
                }
                br.Close();

                nodes[i].Transforms = frameTB;

                int childrenCount = data.GetDataInt32(NodeChildrenCount + i.ToString());
                br = data.GetData(NodeChildren + i.ToString());
                for (int j = 0; j < childrenCount; j++)
                {
                    int id = br.ReadInt32();

                    nodes[i].Children.Add(nodes[id]);
                }
                br.Close();

                int parentId = data.GetDataInt32(NodeParent + i.ToString());
                if (parentId >= 0)
                {
                    nodes[i].Parent = nodes[parentId];
                }
                else
                {
                    roots.Add(nodes[i]);
                }

            }

            rootNodes = roots.ToArray();
        }
        public BinaryDataWriter Save()
        {
            BinaryDataWriter data = new BinaryDataWriter();

            FrameCount = int.MaxValue;
            for (int i = 0; i < EntityCount; i++)
            {
                if (nodes[i].Transforms.Length < FrameCount)
                {
                    FrameCount = nodes[i].Transforms.Length;
                }
            }
            if (FrameCount == int.MaxValue) 
                FrameCount = 1;

            data.AddEntry(FrameCountTag, FrameCount);
            data.AddEntry(FrameLengthTag, FrameLength);

            for (int i = 0; i < EntityCount; i++)
            {
                ContentBinaryWriter bw = data.AddEntry(EntityNodeTag + i.ToString());

                for (int j = 0; j < FrameCount; j++)
                {
                    bw.Write(nodes[i].Transforms[j]);
                }
                bw.Close();

                int nodeChCount = nodes[i].Children == null ? 0 : nodes[i].Children.Count;

                data.AddEntry(NodeChildrenCount + i.ToString(), nodeChCount);

                bw = data.AddEntry(NodeChildren + i.ToString());
                for (int j = 0; j < nodeChCount; j++)
                {
                    bw.Write(nodes[i].Children[j].Index);
                }
                bw.Close();

                if (nodes[i].Parent == null)
                {
                    data.AddEntry(NodeParent + i.ToString(), (int)-1);
                }
                else
                {
                    data.AddEntry(NodeParent + i.ToString(), nodes[i].Parent.Index);
                }
            }
            return data;
        }

        public EntityNode[] Nodes
        {
            get { return nodes; }
        }

        public EntityNode[] RootNodes
        {
            get { return rootNodes; }
        }
    }
}
