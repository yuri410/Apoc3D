using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;
using Apoc.Collections;
using Apoc.MathLib;

namespace Apoc.RenderSystem
{
    public class Sprite : IDisposable
    {
        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        struct SpriteVertex
        {
            internal static VertexDeclaration vtxDecl;

            public Vector3 position;
            float dummy;
            public Vector2 texCoord;

            public SpriteVertex(Vector3 position, Vector2 texCoord)
            {
                this.position = position;
                this.texCoord = texCoord;
                dummy = 0;
            }

            //public static D3D.VertexFormat Format
            //{
            //    get { return D3D.VertexFormat.Position | D3D.VertexFormat.Diffuse; }
            //}

            public static VertexDeclaration Declaration
            {
                get { return vtxDecl; }
            }

            public static unsafe int Size
            {
                get { return sizeof(SpriteVertex); }
            }
        }

        unsafe class Entry : IDisposable
        {
            VertexBuffer buffer;
            Material material;
            GeomentryData geoData;

            public Material Material
            {
                get { return material; }
            }

            public GeomentryData Geomentry
            {
                get { return geoData; }
            }

            public Entry(RenderSystem rs, Sprite spr)
            {
                ObjectFactory fac = rs.ObjectFactory;

                buffer = fac.CreateVertexBuffer(5, SpriteVertex.Declaration, BufferUsage.Dynamic | BufferUsage.WriteOnly);

                material = new Material();
                material.Ambient = new Color4F(1, 1, 1, 1);

                geoData = new GeomentryData(spr);
                geoData.VertexBuffer = buffer;
                geoData.VertexSize = SpriteVertex.Size;
                geoData.VertexDeclaration = SpriteVertex.vtxDecl;
                geoData.VertexCount = 5;
                geoData.PrimitiveType = PrimitiveType.TriangleStrip;
                geoData.PrimCount = 2;
            }

            public void SetVertex(Vector3 tl, Vector3 tr, Vector3 bl, Vector3 br)
            {
                SpriteVertex* dst = (SpriteVertex*)buffer.Lock(LockMode.None).ToPointer();

                dst->position = tl;
                dst->texCoord = new Vector2(0, 0);
                dst++;

                dst->position = tr;
                dst->texCoord = new Vector2(1, 0);
                dst++;

                dst->position = bl;
                dst->texCoord = new Vector2(0, 1);
                dst++;

                dst->position = br;
                dst->texCoord = new Vector2(1, 1);

                buffer.Unlock();
            }

            #region IDisposable 成员

            public void Dispose()
            {
                buffer.Dispose();
            }

            #endregion
        }

        FastList<Entry> buffers;
        FastList<RenderOperation> bufferedOp;
        int curIndex;

        StateBlock block;

        Matrix transform;

        public RenderSystem RenderSystem
        {
            get;
            private set;
        }

        public Sprite(RenderSystem rs)
        {
            ObjectFactory fac = rs.ObjectFactory;

            if (SpriteVertex.vtxDecl == null)
            {

                VertexElement[] elements = new VertexElement[2];
                elements[0] = new VertexElement(0, VertexElementFormat.Vector4, VertexElementUsage.Position);
                elements[1] = new VertexElement(elements[0].Size, VertexElementFormat.Vector2, VertexElementUsage.TextureCoordinate);

                SpriteVertex.vtxDecl = fac.CreateVertexDeclaration(elements);
            }

            block = fac.CreateStateBlock(StateBlockType.All);

            RenderSystem = rs;
            bufferedOp = new FastList<RenderOperation>();
            buffers = new FastList<Entry>();


            transform = Matrix.Identity;
        }

        #region IRenderable 成员

        public RenderOperation[] GetRenderOperation()
        {
            return bufferedOp.Elements;
        }

        #endregion

        public void Begin()        
        {
            bufferedOp.Clear();
            curIndex = 0;
            block.Capture();
        }

        public void End()
        {
            block.Apply();
        }

        public void Draw(Texture texture, int color)
        {
            Draw(texture, 0, 0, color);
        }
        public void Draw(Texture texture, int x, int y, int color)
        {
            Entry buf;
            if (curIndex >= buffers.Count)
            {
                buf = new Entry(RenderSystem, this);
                buffers.Add(buf);
            }
            else
            {
                buf = buffers[curIndex];
            }

            buf.SetVertex(new Vector3(x, y, 0),
                new Vector3(x + texture.Width, y, 0), 
                new Vector3(x, y + texture.Height, 0),
                new Vector3(x + texture.Width, y + texture.Height, 0));
            buf.Material.SetTexture(0, texture);
            
            curIndex++;

            RenderOperation op;
            op.Material = buf.Material;
            op.Geomentry = buf.Geomentry;
            op.Transformation = transform;
            bufferedOp.Add(ref op);
        }

        public void Draw(Texture texture, Rectangle rect, int color)
        {
            Entry buf;
            if (curIndex >= buffers.Count)
            {
                buf = new Entry(RenderSystem, this);
                buffers.Add(buf);
            }
            else
            {
                buf = buffers[curIndex];
            }

            buf.SetVertex(new Vector3(rect.X, rect.Y, 0),
               new Vector3(rect.X + rect.Width, rect.Y, 0),
               new Vector3(rect.X, rect.Y + rect.Height, 0),
               new Vector3(rect.X + rect.Width, rect.Y + texture.Height, 0));
            buf.Material.SetTexture(0, texture);

            curIndex++;

            RenderOperation op;
            op.Material = buf.Material;
            op.Geomentry = buf.Geomentry;
            op.Transformation = transform;
            bufferedOp.Add(ref op);
        }


        public Matrix Transform
        {
            get { return transform; }
            set { transform = value; }
        }



        #region IDisposable 成员

        public void Dispose()
        {
            for (int i = 0; i < buffers.Count; i++)
            {
                buffers[i].Dispose();
            }
            bufferedOp.Clear();
            buffers.Clear();
            bufferedOp = null;
            buffers = null;
        }

        #endregion
    }
}
