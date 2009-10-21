using System;
using System.Collections.Generic;
using System.Text;
using SlimDX;
using SlimDX.Direct3D9;
using Apoc.MathLib;

namespace Apoc.Graphics
{
    public struct VertexPBNT1
    {
        public Vector3 pos;
        public Vector4 blend;
        public byte boneId1;
        public byte boneId2;
        public byte boneId3;
        public byte boneId4;

        public Vector3 n;

        public float u, v;

        public static VertexFormat Format
        {
            get { return VertexFormat.PositionBlend4 | VertexFormat.Normal | VertexFormat.Texture1; }
        }

        public override int GetHashCode()
        {
            return pos.GetHashCode() ^ n.GetHashCode();
        }
        /// <summary>
        ///  获取该顶点的System.String表达形式
        /// </summary>
        /// <returns>该顶点的System.String表达形式</returns>
        public override string ToString()
        {
            return "Pos: " + pos.ToString() + "N: " + n.ToString();
        }

    }

    public struct VertexPN
    {
        /// <summary>
        ///  获取或设置顶点的位置
        /// </summary>
        public Vector3 pos;

        /// <summary>
        ///  获取或设置顶点的法向量
        /// </summary>
        public Vector3 n;
        /// <summary>
        ///  获取该顶点的格式的VertexFormat表达形式
        /// </summary>
        public static VertexFormat Format
        {
            get { return VertexFormat.Position | VertexFormat.Normal; }
        }

        public override int GetHashCode()
        {
            return pos.GetHashCode() ^ n.GetHashCode();
        }
        /// <summary>
        ///  获取该顶点的System.String表达形式
        /// </summary>
        /// <returns>该顶点的System.String表达形式</returns>
        public override string ToString()
        {
            return "Pos: " + pos.ToString() + "N: " + n.ToString();
        }
    }

    /// <summary>
    ///  定义具有位置、法向量和一组二维纹理坐标的顶点
    /// </summary>
    public struct VertexPNT1
    {
        /// <summary>
        ///  获取或设置顶点的位置
        /// </summary>
        public Vector3 pos;

        /// <summary>
        ///  获取或设置顶点的法向量
        /// </summary>
        public Vector3 n;

        /// <summary>
        ///  获取或设置纹理坐标u
        /// </summary>
        public float u;

        /// <summary>
        ///  获取或设置纹理坐标v
        /// </summary>
        public float v;

        /// <summary>
        ///  获取该顶点的格式的VertexFormat表达形式
        /// </summary>
        public static VertexFormat Format
        {
            get { return VertexFormat.Position | VertexFormat.Normal | VertexFormat.Texture1; }
        }
        public unsafe static int Size
        {
            get { return sizeof(VertexPNT1); }
        }

        public override int GetHashCode()
        {
            return pos.GetHashCode() ^ n.GetHashCode() ^ u.GetHashCode() ^ v.GetHashCode();        }


        /// <summary>
        ///  获取该顶点的System.String表达形式
        /// </summary>
        /// <returns>该顶点的System.String表达形式</returns>
        public override string ToString()
        {
            return "Pos: " + pos.ToString() + "N: " + n.ToString() + "uv: " + u.ToString() + "," + v.ToString();
        }
    }

    /// <summary>
    ///  定义具有位置、法向量和两组二维纹理坐标的顶点
    /// </summary>
    public struct VertexPNT2
    {
        /// <summary>
        ///  获取或设置顶点的位置
        /// </summary>
        public Vector3 pos;

        /// <summary>
        ///  获取或设置顶点的法向量
        /// </summary>
        public Vector3 n;

        /// <summary>
        ///  获取或设置第一组的纹理坐标u
        /// </summary>
        public float u1;

        /// <summary>
        ///  获取或设置第一组的纹理坐标v
        /// </summary>
        public float v1;


        /// <summary>
        ///  获取或设置第二组的纹理坐标u
        /// </summary>
        public float u2;

        /// <summary>
        ///  获取或设置第二组的纹理坐标v
        /// </summary>
        public float v2;

        /// <summary>
        ///  获取该顶点的格式的VertexFormat表达形式
        /// </summary>
        public static VertexFormat Format
        {
            get { return VertexFormat.Position | VertexFormat.Normal | VertexFormat.Texture2; }
        }

        /// <summary>
        ///  获取该顶点的System.String表达形式
        /// </summary>
        /// <returns>该顶点的System.String表达形式</returns>
        public override string ToString()
        {
            return "Pos: " + pos.ToString() + "N: " + n.ToString()
                + "uv1: " + u1.ToString() + "," + v1.ToString()
                + "uv2: " + u2.ToString() + "," + v2.ToString();
        }
    }

    /// <summary>
    ///  定义具有位置、法向量和三组二维纹理坐标的顶点
    /// </summary>
    public struct VertexPNT3
    {
        /// <summary>
        ///  获取或设置顶点的位置
        /// </summary>
        public Vector3 pos;

        /// <summary>
        ///  获取或设置顶点的法向量
        /// </summary>
        public Vector3 n;

        /// <summary>
        ///  获取或设置第一组的纹理坐标u
        /// </summary>
        public float u1;
        /// <summary>
        ///  获取或设置第一组的纹理坐标v
        /// </summary>
        public float v1;
        /// <summary>
        ///  获取或设置第二组的纹理坐标u
        /// </summary>
        public float u2;
        /// <summary>
        ///  获取或设置第二组的纹理坐标v
        /// </summary>
        public float v2;
        /// <summary>
        ///  获取或设置第三组的纹理坐标u
        /// </summary>
        public float u3;
        /// <summary>
        ///  获取或设置第三组的纹理坐标v
        /// </summary>
        public float v3;

        /// <summary>
        ///  获取该顶点的格式的VertexFormat表达形式
        /// </summary>
        public static VertexFormat Format
        {
            get { return VertexFormat.Position | VertexFormat.Normal | VertexFormat.Texture3; }
        }

        /// <summary>
        ///  获取该顶点的System.String表达形式
        /// </summary>
        /// <returns>该顶点的System.String表达形式</returns>
        public override string ToString()
        {
            return "Pos: " + pos.ToString() + "N: " + n.ToString()
                + "uv1: " + u1.ToString() + "," + v1.ToString()
                + "uv2: " + u2.ToString() + "," + v2.ToString()
                + "uv3: " + u3.ToString() + "," + v3.ToString();
        }
    }

    /// <summary>
    ///  定义具有位置、法向量和四组二维纹理坐标的顶点
    /// </summary>
    public struct VertexPNT4
    {
        /// <summary>
        ///  获取或设置顶点的位置
        /// </summary>
        public Vector3 pos;

        /// <summary>
        ///  获取或设置第一组的纹理坐标u
        /// </summary>
        public Vector3 n;

        /// <summary>
        ///  获取或设置第一组的纹理坐标u
        /// </summary>
        public float u1;
        /// <summary>
        ///  获取或设置第一组的纹理坐标v
        /// </summary>
        public float v1;
        /// <summary>
        ///  获取或设置第二组的纹理坐标u
        /// </summary>
        public float u2;
        /// <summary>
        ///  获取或设置第二组的纹理坐标v
        /// </summary>
        public float v2;
        /// <summary>
        ///  获取或设置第三组的纹理坐标u
        /// </summary>
        public float u3;
        /// <summary>
        ///  获取或设置第三组的纹理坐标v
        /// </summary>
        public float v3;
        /// <summary>
        ///  获取或设置第四组的纹理坐标u
        /// </summary>
        public float u4;
        /// <summary>
        ///  获取或设置第四组的纹理坐标v
        /// </summary>
        public float v4;

        /// <summary>
        ///  获取该顶点的格式的VertexFormat表达形式
        /// </summary>
        public static VertexFormat Format
        {
            get { return VertexFormat.Position | VertexFormat.Normal | VertexFormat.Texture4; }
        }

        /// <summary>
        ///  获取该顶点的System.String表达形式
        /// </summary>
        /// <returns>该顶点的System.String表达形式</returns>
        public override string ToString()
        {
            return "Pos: " + pos.ToString() + "N: " + n.ToString()
                + "uv1: " + u1.ToString() + "," + v1.ToString()
                + "uv2: " + u2.ToString() + "," + v2.ToString()
                + "uv3: " + u3.ToString() + "," + v3.ToString()
                + "uv4: " + u4.ToString() + "," + v4.ToString();
        }
    }

    /// <summary>
    ///  定义具有位置和一组二维纹理坐标的顶点
    /// </summary>
    public struct VertexPT1
    {
        /// <summary>
        ///  获取或设置顶点的位置
        /// </summary>
        public Vector3 pos;

        /// <summary>
        ///  获取或设置纹理坐标u
        /// </summary>
        public float u1;

        /// <summary>
        ///  获取或设置纹理坐标v
        /// </summary>
        public float v1;

        /// <summary>
        ///  获取该顶点的格式的VertexFormat表达形式
        /// </summary>
        public static VertexFormat Format
        {
            get { return VertexFormat.Position | VertexFormat.Texture1; }
        }

        /// <summary>
        ///  获取该顶点的System.String表达形式
        /// </summary>
        /// <returns>该顶点的System.String表达形式</returns>
        public override string ToString()
        {
            return "Pos: " + pos.ToString() + "uv: " + u1.ToString() + "," + v1.ToString();
        }
    }
    public struct VertexPT2
    {
        /// <summary>
        ///  获取或设置顶点的位置
        /// </summary>
        public Vector3 pos;
        /// <summary>
        ///  获取或设置第一组的纹理坐标u
        /// </summary>
        public float u1;

        /// <summary>
        ///  获取或设置第一组的纹理坐标v
        /// </summary>
        public float v1;


        /// <summary>
        ///  获取或设置第二组的纹理坐标u
        /// </summary>
        public float u2;

        /// <summary>
        ///  获取或设置第二组的纹理坐标v
        /// </summary>
        public float v2;

        /// <summary>
        ///  获取该顶点的格式的VertexFormat表达形式
        /// </summary>
        public static VertexFormat Format
        {
            get { return VertexFormat.Position | VertexFormat.Texture2; }
        }

        /// <summary>
        ///  获取该顶点的System.String表达形式
        /// </summary>
        /// <returns>该顶点的System.String表达形式</returns>
        public override string ToString()
        {
            return "Pos: " + pos.ToString()
                + "uv1: " + u1.ToString() + "," + v1.ToString()
                + "uv2: " + u2.ToString() + "," + v2.ToString();
        }
    }
    public struct VertexPT3
    {
        /// <summary>
        ///  获取或设置顶点的位置
        /// </summary>
        public Vector3 pos;

        /// <summary>
        ///  获取或设置第一组的纹理坐标u
        /// </summary>
        public float u1;
        /// <summary>
        ///  获取或设置第一组的纹理坐标v
        /// </summary>
        public float v1;
        /// <summary>
        ///  获取或设置第二组的纹理坐标u
        /// </summary>
        public float u2;
        /// <summary>
        ///  获取或设置第二组的纹理坐标v
        /// </summary>
        public float v2;
        /// <summary>
        ///  获取或设置第三组的纹理坐标u
        /// </summary>
        public float u3;
        /// <summary>
        ///  获取或设置第三组的纹理坐标v
        /// </summary>
        public float v3;

        /// <summary>
        ///  获取该顶点的格式的VertexFormat表达形式
        /// </summary>
        public static VertexFormat Format
        {
            get { return VertexFormat.Position | VertexFormat.Texture3; }
        }

        /// <summary>
        ///  获取该顶点的System.String表达形式
        /// </summary>
        /// <returns>该顶点的System.String表达形式</returns>
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
        /// <summary>
        ///  获取或设置顶点的位置
        /// </summary>
        public Vector3 pos;
        /// <summary>
        ///  获取或设置第一组的纹理坐标u
        /// </summary>
        public float u1;
        /// <summary>
        ///  获取或设置第一组的纹理坐标v
        /// </summary>
        public float v1;
        /// <summary>
        ///  获取或设置第二组的纹理坐标u
        /// </summary>
        public float u2;
        /// <summary>
        ///  获取或设置第二组的纹理坐标v
        /// </summary>
        public float v2;
        /// <summary>
        ///  获取或设置第三组的纹理坐标u
        /// </summary>
        public float u3;
        /// <summary>
        ///  获取或设置第三组的纹理坐标v
        /// </summary>
        public float v3;
        /// <summary>
        ///  获取或设置第四组的纹理坐标u
        /// </summary>
        public float u4;
        /// <summary>
        ///  获取或设置第四组的纹理坐标v
        /// </summary>
        public float v4;

        /// <summary>
        ///  获取该顶点的格式的VertexFormat表达形式
        /// </summary>
        public static VertexFormat Format
        {
            get { return VertexFormat.Position | VertexFormat.Texture4; }
        }

        /// <summary>
        ///  获取该顶点的System.String表达形式
        /// </summary>
        /// <returns>该顶点的System.String表达形式</returns>
        public override string ToString()
        {
            return "Pos: " + pos.ToString()
               + "uv1: " + u1.ToString() + "," + v1.ToString()
               + "uv2: " + u2.ToString() + "," + v2.ToString()
               + "uv3: " + u3.ToString() + "," + v3.ToString()
               + "uv4: " + u4.ToString() + "," + v4.ToString();
        }
    }


    /// <summary>
    ///  定义具有位置和颜色的顶点
    /// </summary>
    public struct VertexPC
    {
        /// <summary>
        ///  获取或设置顶点的位置
        /// </summary>
        public Vector3 pos;

        /// <summary>
        ///  获取或设置顶点颜色
        /// </summary>
        public int diffuse;

        public VertexPC(Vector3 pos, int diffuse)
        {
            this.pos = pos;
            this.diffuse = diffuse;
        }

        /// <summary>
        ///  获取该顶点的格式的VertexFormat表达形式
        /// </summary>
        public static VertexFormat Format
        {
            get { return VertexFormat.Position | VertexFormat.Diffuse; }
        }

        /// <summary>
        ///  获取该顶点的System.String表达形式
        /// </summary>
        /// <returns>该顶点的System.String表达形式</returns>
        public override string ToString()
        {
            return "Pos: " + pos.ToString() + "clr: " + diffuse.ToString();
        }
    }

    public struct VertexPCT
    {
        public Vector3 pos;
        public float shit;
        public int diffuse;
        public Vector2 texCoord;

        public VertexPCT(float x, float y, float z, float rhw, int color, float u, float v)
        {
            this.pos.X = x;
            this.pos.Y = y;
            this.pos.Z = z;
            this.shit = rhw;
            this.diffuse = color;
            this.texCoord.X = u;
            this.texCoord.Y = v;
        }

        /// <summary>
        ///  获取该顶点的格式的VertexFormat表达形式
        /// </summary>
        public static VertexFormat Format
        {
            get { return VertexFormat.PositionRhw | VertexFormat.Diffuse | VertexFormat.Texture1; }
        }
        /// <summary>
        ///  获取该顶点的大小
        /// </summary>
        public static int Size
        {
            get { return sizeof(float) * 7; }
        }
    }


    /// <summary>
    ///   定义具有位置的顶点
    /// </summary>
    public struct VertexP
    {
        /// <summary>
        ///  获取或设置顶点的位置
        /// </summary>
        public Vector3 pos;

        /// <summary>
        ///  获取该顶点的格式的VertexFormat表达形式
        /// </summary>
        public static VertexFormat Format
        {
            get { return VertexFormat.Position; }
        }

        /// <summary>
        ///  获取该顶点的System.String表达形式
        /// </summary>
        /// <returns>该顶点的System.String表达形式</returns>
        public override string ToString()
        {
            return pos.ToString();
        }
    }
}
