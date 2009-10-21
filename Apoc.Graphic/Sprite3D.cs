using System;
using System.Collections.Generic;
using System.Drawing;
using System.Text;
using SlimDX;
using SlimDX.Direct3D9;
using Apoc.MathLib;
using Apoc.UI;

namespace Apoc.Graphics
{
    public unsafe class Sprite3D
    {
        struct PTVertex 
        {
            public Vector3 pos;
            public float dummy;
            public int diffuse;
            public Vector2 texCoord;

            public static VertexFormat Format
            {
                get { return VertexFormat.PositionRhw | VertexFormat.Diffuse | VertexFormat.Texture1; }
            }

            public static int Size
            {
                get { return Vector4.SizeInBytes + sizeof(int) + Vector2.SizeInBytes; }
            }
        }

        Device device;
        Game game;

        Matrix proj;

        float nearPlane = 1;
        float farPlane = 100;

        VertexBuffer orgBuffer;
        VertexBuffer prjBuffer;

        IndexBuffer idxBuffer;

        public Sprite3D(Game game, Device dev)
        {
            this.device = dev;
            this.game = game;

            idxBuffer = new IndexBuffer(dev, 2 * 4, Usage.WriteOnly, Pool.Managed, true);

            ushort* ib = (ushort*)idxBuffer.Lock(0, 0, LockFlags.None).DataPointer.ToPointer();
            ib[0] = 0;
            ib[1] = 3;
            ib[2] = 2;
            ib[3] = 1;
            idxBuffer.Unlock();

            prjBuffer = new VertexBuffer(dev, VertexPCT.Size, Usage.Dynamic, VertexPCT.Format, Pool.Managed);

            VertexPCT* vb1 = (VertexPCT*)prjBuffer.Lock(0, 0, LockFlags.None).DataPointer.ToPointer();
            vb1[0].texCoord = new Vector2(0, 0);
            vb1[1].texCoord = new Vector2(1, 0);
            vb1[2].texCoord = new Vector2(1, 1);
            vb1[3].texCoord = new Vector2(0, 1);
            prjBuffer.Unlock();

            orgBuffer = new VertexBuffer(dev, PTVertex.Size, Usage.Dynamic, PTVertex.Format, Pool.Managed);
            PTVertex* vb2 = (PTVertex*)orgBuffer.Lock(0, 0, LockFlags.None).DataPointer.ToPointer();

            vb2[0].texCoord = new Vector2(0, 0);
            vb2[1].texCoord = new Vector2(1, 0);
            vb2[2].texCoord = new Vector2(1, 1);
            vb2[3].texCoord = new Vector2(0, 1);
            orgBuffer.Unlock();

        }

        void UpdateProjection()
        {
            proj = Matrix.PerspectiveFovRH(MathEx.PIf / 4f, (float)game.ClientSize.Width / (float)game.ClientSize.Height, nearPlane, farPlane);
        }

        public float NearPlane
        {
            get { return nearPlane; }
            set
            {
                nearPlane = value;
                UpdateProjection();
            }
        }

        public float FarPlane
        {
            get { return farPlane; }
            set
            {
                farPlane = value;
                UpdateProjection();
            }
        }

        public void Draw(Texture texture, RectangleF rect, float z, int color)
        {
            device.VertexShader = null;
            device.PixelShader = null;


            device.SetTransform(TransformState.Projection, proj);
            device.SetTransform(TransformState.View, Matrix.Identity);
            device.SetTransform(TransformState.World, Matrix.Identity);


            VertexPCT* vb1 = (VertexPCT*)prjBuffer.Lock(0, 0, LockFlags.None).DataPointer.ToPointer();
            vb1[0].diffuse = color;
            vb1[0].pos = new Vector3(rect.Left, rect.Top, z);
            vb1[1].diffuse = color;
            vb1[1].pos = new Vector3(rect.Left, rect.Bottom, z);
            vb1[2].diffuse = color;
            vb1[2].pos = new Vector3(rect.Right, rect.Bottom, z);
            vb1[3].diffuse = color;
            vb1[3].pos = new Vector3(rect.Right, rect.Top, z);
            prjBuffer.Unlock();


            device.SetTexture(0, texture);
            device.Indices = idxBuffer;
            device.SetStreamSource(0, orgBuffer, 0, sizeof(PTVertex));
            device.DrawIndexedPrimitives(PrimitiveType.TriangleStrip, 0, 0, 4, 0, 2);
        }

        public void Draw(Texture texture, int color)
        {
            Draw(texture, 0, 0, color);
        }

        public void Draw(Texture texture, RectangleF rect, int color)
        {
            device.VertexShader = null;
            device.PixelShader = null;

            PTVertex* vb2 = (PTVertex*)orgBuffer.Lock(0, 0, LockFlags.None).DataPointer.ToPointer();
            vb2[0].diffuse = color;
            vb2[0].pos = new Vector3(rect.Left, rect.Top, 0);
            vb2[1].diffuse = color;
            vb2[1].pos = new Vector3(rect.Left, rect.Bottom, 0);
            vb2[2].diffuse = color;
            vb2[2].pos = new Vector3(rect.Right, rect.Bottom, 0);
            vb2[3].diffuse = color;
            vb2[3].pos = new Vector3(rect.Right, rect.Top, 0);
            orgBuffer.Unlock();

            device.SetTexture(0, texture);
            device.Indices = idxBuffer;
            device.SetStreamSource(0, orgBuffer, 0, sizeof(PTVertex));
            device.DrawIndexedPrimitives(PrimitiveType.TriangleStrip, 0, 0, 4, 0, 2);

        }

        public void Draw(Texture texture, float x, float y, int color)
        {
            SurfaceDescription sd = texture.GetLevelDescription(0);

            Draw(texture, new RectangleF(x, y, sd.Width, sd.Height), color);
        }



        public Matrix Projection
        {
            get
            {
                return proj;
            }
        }



    }
}
