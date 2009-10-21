using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Text;
using System.Xml;
using SlimDX;
using SlimDX.Direct3D9;
using VirtualBicycle.Ide.Designers;
using VirtualBicycle.Ide.Editors.EditableObjects;
using VirtualBicycle.Graphics;

namespace VirtualBicycle.Ide
{
    public static class DevUtils
    {
        //public unsafe static void DrawEditMesh(Device dev, EditableMesh mesh)
        //{
        //    if (mesh.Format == VertexPNT1.Format)
        //    {
        //        dev.VertexFormat = mesh.Format;
        //        VertexPNT1[] vertics = new VertexPNT1[mesh.Positions.Length];
        //        for (int i = 0; i < vertics.Length; i++)
        //        {
        //            vertics[i].pos = mesh.Positions[i];
        //            vertics[i].n = mesh.Normals[i];
        //            vertics[i].u = mesh.Texture1[i].X;
        //            vertics[i].v = mesh.Texture1[i].Y;
        //            //vertics[i].tex1 = mesh.Texture1[i];
        //        }

        //        int lastMat = -1;
        //        List<ushort> idx = new List<ushort>();

        //        for (int i = 0; i < mesh.Faces.Length; i++)
        //        {
        //            if (mesh.Faces[i].MaterialIndex != lastMat)
        //            {
        //                if (lastMat != -1)
        //                {
        //                    dev.Material = mesh.Materials[lastMat].mat;
        //                    dev.DrawIndexedUserPrimitives<ushort, VertexPNT1>(PrimitiveType.TriangleList, 0, vertics.Length, idx.Count / 3, idx.ToArray(), Format.Index16, vertics, sizeof(VertexPNT1));
        //                    idx.Clear();
        //                }

        //                lastMat = mesh.Faces[i].MaterialIndex;

        //                idx.Add((ushort)mesh.Faces[i].IndexA);
        //                idx.Add((ushort)mesh.Faces[i].IndexB);
        //                idx.Add((ushort)mesh.Faces[i].IndexC);
        //            }
        //            else
        //            {
        //                idx.Add((ushort)mesh.Faces[i].IndexA);
        //                idx.Add((ushort)mesh.Faces[i].IndexB);
        //                idx.Add((ushort)mesh.Faces[i].IndexC);
        //            }
        //        }
        //        //dev.DrawIndexedUserPrimitives<ushort, StdMeshVertex>(PrimitiveType.TriangleList, 0, vertics.Length, idx.Count / 3, idx.ToArray(), Format.Index16, vertics, sizeof(StdMeshVertex));

        //        if (idx.Count > 0 && lastMat != -1)
        //        {
        //            dev.Material = mesh.Materials[lastMat].mat;
        //            dev.DrawIndexedUserPrimitives<ushort, VertexPNT1>(PrimitiveType.TriangleList, 0, vertics.Length, idx.Count / 3, idx.ToArray(), Format.Index16, vertics, sizeof(VertexPNT1));
        //            idx.Clear();
        //        }
        //    }
        //}
        //public static void DrawLight(Device dev, Vector3 lightPos)
        //{
        //    Matrix lgtMat;
        //    Matrix.Translation(ref lightPos, out lgtMat);
        //    dev.SetTransform(TransformState.World, lgtMat);
        //    dev.SetRenderState(RenderState.Lighting, false);
        //    for (int i = 0; i < 4; i++)
        //    {
        //        dev.SetTexture(i, null);
        //    }
        //    GraphicsDevice.Instance.BallMesh.DrawSubset(0);
        //    dev.SetRenderState(RenderState.Lighting, true);

        //}
        //public static void SetMaterial(Device dev, EditableMeshMaterial mate)
        //{
        //    dev.SetRenderState(RenderState.ZWriteEnable, !mate.IsTransparent);
        //    dev.SetRenderState<Cull>(RenderState.CullMode, mate.CullMode);

        //    dev.Material = mate.mat;

        //    for (int j = 0; j < MaterialBase.MaxTexLayers; j++)
        //    {
        //        dev.SetTexture(j, mate.GetTexture(j));
        //    }
        //}
        public static void DrawLight(Device dev, Vector3 lightPos)
        {
            Matrix lgtMat;
            Matrix.Translation(ref lightPos, out lgtMat);
            dev.SetTransform(TransformState.World, lgtMat);
            dev.SetRenderState(RenderState.Lighting, false);
            for (int i = 0; i < 4; i++)
            {
                dev.SetTexture(i, null);
            }
            GraphicsDevice.Instance.BallMesh.DrawSubset(0);
            dev.SetRenderState(RenderState.Lighting, true);

        }


        public static bool XmlMoveToNextElement(XmlTextReader xmlIn)
        {
            if (!xmlIn.Read())
                return false;

            while (xmlIn.NodeType == XmlNodeType.EndElement)
            {
                if (!xmlIn.Read())
                    return false;
            }

            return true;
        }
        public static string GetFilter(string desc, string[] filters)
        {
            StringBuilder sb = new StringBuilder(filters.Length * 3 + 5);
            sb.Append(desc);
            sb.Append("(");
            StringBuilder ext = new StringBuilder();
            for (int i = 0; i < filters.Length; i++)
            {
                ext.Append('*');
                ext.Append(filters[i]);

                if (i < filters.Length - 1)
                {
                    ext.Append(';');
                }
            }

            sb.Append(ext.ToString());
            sb.Append(")|");
            sb.Append(ext.ToString());
            return sb.ToString();

        }
        public static string GetFilter(string[] subFilters) 
        {
            StringBuilder sb = new StringBuilder(subFilters.Length * 2);

            for (int i = 0; i < subFilters.Length; i++)
            {
                sb.Append(subFilters[i]);

                if (i < subFilters.Length - 1)
                {
                    sb.Append('|');
                }
            }

            return sb.ToString();
        }

        public static void SetMaterial(Device dev, EditableMeshMaterial mate)
        {
            dev.SetRenderState(RenderState.ZWriteEnable, !mate.IsTransparent);
            dev.SetRenderState<Cull>(RenderState.CullMode, mate.CullMode);

            dev.Material = mate.D3DMaterial;

            for (int j = 0; j < MaterialBase.MaxTexLayers; j++)
            {
                dev.SetTexture(j, mate.GetTexture(j));
            }
        }

        public static Bitmap Texture2Bitmap(Texture value)
        {
            throw new NotImplementedException();
        }

       
    }
}
