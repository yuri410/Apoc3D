using System;
using System.Collections.Generic;
using System.Text;

namespace Apoc.RenderSystem
{


    public class PresetedVertex
    {
        public struct VertexPNT1
        {
            internal static VertexDeclaration vtxDecl;

            //[FieldOffset(0)]
            public Vector3 pos;
            //[FieldOffset(12)]
            public Vector3 n;

            //[FieldOffset(24)]
            public float u;
            //[FieldOffset(28)]
            public float v;
            //[FieldOffset(24)]
            //public Vector2 tex1;

            //public static VertexFormat Format
            //{
            //    get { return VertexFormat.Position | VertexFormat.Normal | VertexFormat.Texture1; }
            //}


            public static VertexDeclaration Declaration
            {
                get { return vtxDecl; }
            }

            public override int GetHashCode()
            {
                return pos.GetHashCode() ^ n.GetHashCode() ^ u.GetHashCode() ^ v.GetHashCode();
            }

            public override string ToString()
            {
                return "Pos: " + pos.ToString() + "N: " + n.ToString() + "uv: " + u.ToString() + "," + v.ToString();
            }
        }
        public struct VertexPNT2
        {
            internal static VertexDeclaration vtxDecl;

            public Vector3 pos;
            public Vector3 n;
            public float u1, v1;
            public float u2, v2;


            public static VertexDeclaration Declaration
            {
                get { return vtxDecl; }
            }
            //public static VertexFormat Format
            //{
            //    get { return VertexFormat.Position | VertexFormat.Normal | VertexFormat.Texture2; }
            //}


            public override string ToString()
            {
                return "Pos: " + pos.ToString() + "N: " + n.ToString()
                    + "uv1: " + u1.ToString() + "," + v1.ToString()
                    + "uv2: " + u2.ToString() + "," + v2.ToString();
            }
        }
        public struct VertexPNT3
        {
            internal static VertexDeclaration vtxDecl;

            public Vector3 pos;
            public Vector3 n;
            public float u1, v1;
            public float u2, v2;
            public float u3, v3;


            public static VertexDeclaration Declaration
            {
                get { return vtxDecl; }
            }
            //public static VertexFormat Format
            //{
            //    get { return VertexFormat.Position | VertexFormat.Normal | VertexFormat.Texture3; }
            //}

            public override string ToString()
            {
                return "Pos: " + pos.ToString() + "N: " + n.ToString()
                    + "uv1: " + u1.ToString() + "," + v1.ToString()
                    + "uv2: " + u2.ToString() + "," + v2.ToString()
                    + "uv3: " + u3.ToString() + "," + v3.ToString();
            }
        }
        public struct VertexPNT4
        {
            internal static VertexDeclaration vtxDecl;

            public Vector3 pos;
            public Vector3 n;
            public float u1, v1;
            public float u2, v2;
            public float u3, v3;
            public float u4, v4;


            public static VertexDeclaration Declaration
            {
                get { return vtxDecl; }
            }
            //public static VertexFormat Format
            //{
            //    get { return VertexFormat.Position | VertexFormat.Normal | VertexFormat.Texture4; }
            //}

            public override string ToString()
            {
                return "Pos: " + pos.ToString() + "N: " + n.ToString()
                    + "uv1: " + u1.ToString() + "," + v1.ToString()
                    + "uv2: " + u2.ToString() + "," + v2.ToString()
                    + "uv3: " + u3.ToString() + "," + v3.ToString()
                    + "uv4: " + u4.ToString() + "," + v4.ToString();
            }
        }

        public struct VertexPT1
        {
            internal static VertexDeclaration vtxDecl;

            public Vector3 pos;
            public float u1, v1;


            public static VertexDeclaration Declaration
            {
                get { return vtxDecl; }
            }
            //public static VertexFormat Format
            //{
            //    get { return VertexFormat.Position | VertexFormat.Texture1; }
            //}

            public override string ToString()
            {
                return "Pos: " + pos.ToString() + "uv: " + u1.ToString() + "," + v1.ToString();
            }
        }
        public struct VertexPT2
        {
            internal static VertexDeclaration vtxDecl;

            public Vector3 pos;
            public float u1, v1;
            public float u2, v2;


            public static VertexDeclaration Declaration
            {
                get { return vtxDecl; }
            }
            //public static VertexFormat Format
            //{
            //    get { return VertexFormat.Position | VertexFormat.Texture2; }
            //}

            public override string ToString()
            {
                return "Pos: " + pos.ToString()
                    + "uv1: " + u1.ToString() + "," + v1.ToString()
                    + "uv2: " + u2.ToString() + "," + v2.ToString();
            }
        }
        public struct VertexPT3
        {
            internal static VertexDeclaration vtxDecl;

            public Vector3 pos;
            public float u1, v1;
            public float u2, v2;
            public float u3, v3;


            public static VertexDeclaration Declaration
            {
                get { return vtxDecl; }
            }
            //public static VertexFormat Format
            //{
            //    get { return VertexFormat.Position | VertexFormat.Texture3; }
            //}

            public override string ToString()
            {
                return "Pos: " + pos.ToString()
                     + "uv1: " + u1.ToString() + "," + v1.ToString()
                     + "uv2: " + u2.ToString() + "," + v2.ToString()
                     + "uv3: " + u3.ToString() + "," + v3.ToString();
            }
        }
        public struct VertexPT4
        {
            internal static VertexDeclaration vtxDecl;

            public Vector3 pos;
            public float u1;
            public float v1;
            public float u2;
            public float v2;
            public float u3;
            public float v3;

            public float u4;
            public float v4;


            public static VertexDeclaration Declaration
            {
                get { return vtxDecl; }
            }
            //public static VertexFormat Format
            //{
            //    get { return VertexFormat.Position | VertexFormat.Texture4; }
            //}

            public override string ToString()
            {
                return "Pos: " + pos.ToString()
                   + "uv1: " + u1.ToString() + "," + v1.ToString()
                   + "uv2: " + u2.ToString() + "," + v2.ToString()
                   + "uv3: " + u3.ToString() + "," + v3.ToString()
                   + "uv4: " + u4.ToString() + "," + v4.ToString();
            }
        }

        public struct VertexPC
        {
            internal static VertexDeclaration vtxDecl;

            public Vector3 pos;
            public int diffuse;

            public static VertexDeclaration Declaration
            {
                get { return vtxDecl; }
            }
            //public static VertexFormat Format
            //{
            //    get { return VertexFormat.Position | VertexFormat.Diffuse; }
            //}
            public override string ToString()
            {
                return "Pos: " + pos.ToString() + "clr: " + diffuse.ToString();
            }
        }
        public struct VertexP
        {
            internal static VertexDeclaration vtxDecl;

            public Vector3 pos;

            //public VertexFormat Format
            //{
            //    get { return VertexFormat.Position; }
            //}

            public static VertexDeclaration Declaration
            {
                get { return vtxDecl; }
            }

            public override string ToString()
            {
                return pos.ToString();
            }
        }


        public static void Initialize(ObjectFactory factory)
        {
            VertexElement[] elements = new VertexElement[3];

            elements[0] = new VertexElement(0, VertexElementFormat.Vector3, VertexElementUsage.Position);
            elements[1] = new VertexElement(elements[0].Size, VertexElementFormat.Vector3, VertexElementUsage.Normal);
            elements[2] = new VertexElement(elements[1].Offset + elements[1].Size, VertexElementFormat.Vector2, VertexElementUsage.TextureCoordinate, 0);

            VertexPNT1.vtxDecl = factory.CreateVertexDeclaration(elements);

            // ------------------------------------------

            elements = new VertexElement[4];

            elements[0] = new VertexElement(0, VertexElementFormat.Vector3, VertexElementUsage.Position);
            elements[1] = new VertexElement(elements[0].Size, VertexElementFormat.Vector3, VertexElementUsage.Normal);
            elements[2] = new VertexElement(elements[1].Offset + elements[1].Size, VertexElementFormat.Vector2, VertexElementUsage.TextureCoordinate, 0);
            elements[3] = new VertexElement(elements[2].Offset + elements[2].Size, VertexElementFormat.Vector2, VertexElementUsage.TextureCoordinate, 1);

            VertexPNT2.vtxDecl = factory.CreateVertexDeclaration(elements);


            // ------------------------------------------

            elements = new VertexElement[5];

            elements[0] = new VertexElement(0, VertexElementFormat.Vector3, VertexElementUsage.Position);
            elements[1] = new VertexElement(elements[0].Size, VertexElementFormat.Vector3, VertexElementUsage.Normal);
            elements[2] = new VertexElement(elements[1].Offset + elements[1].Size, VertexElementFormat.Vector2, VertexElementUsage.TextureCoordinate, 0);
            elements[3] = new VertexElement(elements[2].Offset + elements[2].Size, VertexElementFormat.Vector2, VertexElementUsage.TextureCoordinate, 1);
            elements[4] = new VertexElement(elements[3].Offset + elements[3].Size, VertexElementFormat.Vector2, VertexElementUsage.TextureCoordinate, 2);
            VertexPNT3.vtxDecl = factory.CreateVertexDeclaration(elements);

            // ------------------------------------------

            elements = new VertexElement[3];

            elements[0] = new VertexElement(0, VertexElementFormat.Vector3, VertexElementUsage.Position);
            elements[1] = new VertexElement(elements[0].Size, VertexElementFormat.Vector3, VertexElementUsage.Normal);
            elements[2] = new VertexElement(elements[1].Offset + elements[1].Size, VertexElementFormat.Vector2, VertexElementUsage.TextureCoordinate, 0);
            elements[3] = new VertexElement(elements[2].Offset + elements[2].Size, VertexElementFormat.Vector2, VertexElementUsage.TextureCoordinate, 1);
            elements[4] = new VertexElement(elements[3].Offset + elements[3].Size, VertexElementFormat.Vector2, VertexElementUsage.TextureCoordinate, 2);
            elements[5] = new VertexElement(elements[4].Offset + elements[4].Size, VertexElementFormat.Vector2, VertexElementUsage.TextureCoordinate, 3);
            VertexPNT4.vtxDecl = factory.CreateVertexDeclaration(elements);

            // ------------------------------------------

            elements = new VertexElement[2];

            elements[0] = new VertexElement(0, VertexElementFormat.Vector3, VertexElementUsage.Position);
            elements[1] = new VertexElement(elements[0].Size, VertexElementFormat.Vector2, VertexElementUsage.TextureCoordinate, 0);
            VertexPT1.vtxDecl = factory.CreateVertexDeclaration(elements);
            // ------------------------------------------

            elements = new VertexElement[3];

            elements[0] = new VertexElement(0, VertexElementFormat.Vector3, VertexElementUsage.Position);
            elements[1] = new VertexElement(elements[0].Size, VertexElementFormat.Vector2, VertexElementUsage.TextureCoordinate, 0);
            elements[2] = new VertexElement(elements[1].Offset + elements[1].Size, VertexElementFormat.Vector2, VertexElementUsage.TextureCoordinate, 1);
            VertexPT2.vtxDecl = factory.CreateVertexDeclaration(elements);

            // ------------------------------------------

            elements = new VertexElement[4];

            elements[0] = new VertexElement(0, VertexElementFormat.Vector3, VertexElementUsage.Position);
            elements[1] = new VertexElement(elements[0].Size, VertexElementFormat.Vector2, VertexElementUsage.TextureCoordinate, 0);
            elements[2] = new VertexElement(elements[1].Offset + elements[1].Size, VertexElementFormat.Vector2, VertexElementUsage.TextureCoordinate, 1);
            elements[3] = new VertexElement(elements[2].Offset + elements[2].Size, VertexElementFormat.Vector2, VertexElementUsage.TextureCoordinate, 2);
            VertexPT3.vtxDecl = factory.CreateVertexDeclaration(elements);

            // ------------------------------------------

            elements = new VertexElement[5];

            elements[0] = new VertexElement(0, VertexElementFormat.Vector3, VertexElementUsage.Position);
            elements[1] = new VertexElement(elements[0].Size, VertexElementFormat.Vector2, VertexElementUsage.TextureCoordinate, 0);
            elements[2] = new VertexElement(elements[1].Offset + elements[1].Size, VertexElementFormat.Vector2, VertexElementUsage.TextureCoordinate, 1);
            elements[3] = new VertexElement(elements[2].Offset + elements[2].Size, VertexElementFormat.Vector2, VertexElementUsage.TextureCoordinate, 2);
            elements[4] = new VertexElement(elements[3].Offset + elements[3].Size, VertexElementFormat.Vector2, VertexElementUsage.TextureCoordinate, 3);
            VertexPT4.vtxDecl = factory.CreateVertexDeclaration(elements);

            // ------------------------------------------

            elements = new VertexElement[2];

            elements[0] = new VertexElement(0, VertexElementFormat.Vector3, VertexElementUsage.Position);
            elements[1] = new VertexElement(elements[0].Size, VertexElementFormat.Vector3, VertexElementUsage.Color);
            VertexPC.vtxDecl = factory.CreateVertexDeclaration(elements);

            // ------------------------------------------

            elements = new VertexElement[1];

            elements[0] = new VertexElement(0, VertexElementFormat.Vector3, VertexElementUsage.Position);
            VertexP.vtxDecl = factory.CreateVertexDeclaration(elements);
        }
    }

}
