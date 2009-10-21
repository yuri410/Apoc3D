using System;
using System.Collections.Generic;
using System.Text;
using SlimDX.Direct3D9;
using SlimDX;

namespace Apoc.Graphics
{
    /// <summary>
    /// 一个渲染操作由下列三个元素组成:
    ///     变换
    ///     材质
    ///     几何信息
    /// </summary>
    public struct RenderOperation
    {
        public Matrix Transformation;

        public MeshMaterial Material;

        public GeomentryData Geomentry;        
    }

    public class GeomentryData
    {
        //public delegate void RenderedHandler(RenderOperation op);

        IRenderable sender;
        //public Matrix Transformation;


        //RenderedHandler renderHandler;
        /// <summary>
        /// 渲染操作的来源
        /// </summary>
        public IRenderable Sender
        {
            get { return sender; }
        }

        /// <summary>
        ///  获取或设置顶点格式
        /// </summary>
        public VertexFormat Format
        {
            get;
            set;
        }
        public int VertexSize
        {
            get;
            set;
        }
        public VertexDeclaration VertexDeclaration
        {
            get;
            set;
        }



        public VertexBuffer VertexBuffer
        {
            get;
            set;
        }
        public IndexBuffer IndexBuffer
        {
            get;
            set;
        }
        public bool UseIndices
        {
            get { return IndexBuffer != null; }
        }
       
        public PrimitiveType PrimitiveType
        {
            get;
            set;
        }
        public int PrimCount
        {
            get;
            set;
        }
        public int VertexCount
        {
            get;
            set;
        }

        public GeomentryData(IRenderable obj)
        {
            sender = obj;
        }

        public int BaseVertex
        {
            get;
            set;
        }
        public int BaseIndexStart
        {
            get;
            set;
        }


        //public bool IsLastOperation
        //{
        //    get;
        //    set;
        //}

        //public event RenderedHandler Rendered
        //{
        //    add { renderHandler += value; }
        //    remove { renderHandler -= value; }
        //}

        //public void OnRendered()
        //{
        //    if (renderHandler != null)
        //        renderHandler(this);
        //}

        //~RenderOperation()
        //{
        //    renderHandler = null;
        //}
    }
}
