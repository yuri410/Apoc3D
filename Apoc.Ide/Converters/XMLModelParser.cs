using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Xml;
using SlimDX;
using SlimDX.Direct3D9;
using VirtualBicycle.Ide.Designers;
using VirtualBicycle.Ide.Editors.EditableObjects;
using VirtualBicycle;
using VirtualBicycle.Collections;
using VirtualBicycle.Graphics;
using VirtualBicycle.Graphics.Animation;

namespace VirtualBicycle.Ide.Converters
{
    public struct Index3i
    {
        public int A;
        public int B;
        public int C;
    }
    public struct Index2i
    {
        public int A;
        public int B;
    }
    public struct VertexWeight
    {
        public int[] BoneID;
        public float[] Weight;
    }
    public struct XmlBoneKeyFrame
    {
        public int FrameId;

        public Vector3 Scale;
        public Quaternion Rotation;
        public Vector3 Position;
    }

    public enum XmlModelObjectType
    {
        Unknown,
        Dummy,
        Mesh,
        Bone
    }

    public class XmlModelObject
    {
        public string Name;
        public int ParentID;

        public BoundingBox AABB;

        public int ID;
        public XmlModelObjectType Type;

        public Matrix LocalTM;
        public Matrix WorldTM;
        public VirtualBicycle.Graphics.MeshData Mesh;

        public XmlBoneKeyFrame[] BoneData;
    }

    public class ParsedXmlModel
    {
        public ParsedXmlModel()
        {
            ObjectTable = new Dictionary<int, XmlModelObject>();
            BoneTable = new Dictionary<int, int>();
            MaterialTable = new Dictionary<int, MeshMaterial>();
        }

        public string FileName
        {
            get;
            set;
        }

        public int ObjectCount
        {
            get;
            set;
        }

        public int MeshCount
        {
            get;
            set;
        }

        public float AnimationStart
        {
            get;
            set;
        }

        public float AnimationEnd
        {
            get;
            set;
        }

        public int KeyType
        {
            get;
            set;
        }

        public int SkinType
        {
            get;
            set;
        }

        /// <summary>
        ///  ID查询物体
        /// </summary>
        public Dictionary<int, XmlModelObject> ObjectTable
        {
            get;
            set;
        }

        /// <summary>
        ///  BoneID查询ID
        /// </summary>
        public Dictionary<int, int> BoneTable
        {
            get;
            set;
        }

        /// <summary>
        ///  MaterialID查询材质
        /// </summary>
        public Dictionary<int, MeshMaterial> MaterialTable
        {
            get;
            set;
        }
    }

    public unsafe class XmlModelParser
    {
        static readonly char[] DataSep = new char[] { '[', ']', ',', ' ', '\n', '(', ')' };

        #region parsers
        int ParseFrameId(string str)
        {
            return int.Parse(str.Substring(0, str.Length - 1));
        }

        Index2i ParseIndex2(string str)
        {
            Index2i res;

            string[] v = str.Split(DataSep, StringSplitOptions.RemoveEmptyEntries);

            res.A = int.Parse(v[0]);
            res.B = int.Parse(v[1]);

            return res;
        }
        Index3i ParseIndex3(string str)
        {
            Index3i res;

            string[] v = str.Split(DataSep, StringSplitOptions.RemoveEmptyEntries);

            res.A = int.Parse(v[0]);
            res.B = int.Parse(v[1]);
            res.C = int.Parse(v[2]);

            return res;
        }

        Color3 ParseColor3(string str)
        {
            string[] vals = str.Split(DataSep, StringSplitOptions.RemoveEmptyEntries);

            return new Color3(float.Parse(vals[1]) / 255f, float.Parse(vals[2]) / 255f, float.Parse(vals[3]) / 255f);
        }

        Vector2 ParseVector2(string str)
        {
            string[] vals = str.Split(DataSep, StringSplitOptions.RemoveEmptyEntries);

            return new Vector2(float.Parse(vals[0]), float.Parse(vals[1]));
        }
        Vector3 ParseVector3(string str)
        {
            string[] vals = str.Split(DataSep, StringSplitOptions.RemoveEmptyEntries);

            return new Vector3(float.Parse(vals[0]), float.Parse(vals[1]), float.Parse(vals[2]));
        }
        Vector4 ParseVector4(string str)
        {
            string[] vals = str.Split(DataSep, StringSplitOptions.RemoveEmptyEntries);

            return new Vector4(float.Parse(vals[0]), float.Parse(vals[1]), float.Parse(vals[2]), float.Parse(vals[3]));
        }
        Matrix ParseMatrix(XmlReader xml)
        {
            Matrix res = new Matrix();

            int depth = xml.Depth;
            int row = 0;
            while (xml.Read() && xml.Depth > depth)
            {
                if (xml.IsStartElement() && !xml.IsEmptyElement)
                {
                    if (xml.Name == "Value")
                    {
                        Vector4 vec = ParseVector4(xml.ReadString());

                        switch (row)
                        {
                            case 0:
                                res.M11 = vec.X;
                                res.M12 = vec.Y;
                                res.M13 = vec.Z;
                                res.M14 = vec.W;
                                break;
                            case 1:
                                res.M21 = vec.X;
                                res.M22 = vec.Y;
                                res.M23 = vec.Z;
                                res.M24 = vec.W;
                                break;
                            case 2:
                                res.M31 = vec.X;
                                res.M32 = vec.Y;
                                res.M33 = vec.Z;
                                res.M34 = vec.W;
                                break;
                            case 3:
                                res.M41 = vec.X;
                                res.M42 = vec.Y;
                                res.M43 = vec.Z;
                                res.M44 = vec.W;
                                break;
                        }
                        row++;
                    }
                }
            }
            return res;
        }
        BoundingBox ParseBoundingBox(XmlReader xml)
        {
            BoundingBox res = new BoundingBox();
            int depth = xml.Depth;

            while (xml.Read() && xml.Depth > depth)
            {
                if (xml.IsStartElement() && !xml.IsEmptyElement)
                {
                    switch (xml.Name)
                    {
                        case "Min":
                            res.Minimum = ParseVector3(xml.ReadString());
                            break;
                        case "Max":
                            res.Maximum = ParseVector3(xml.ReadString());
                            break;
                    }
                }
            }
            return res;
        }

        #endregion

        void ParseObjectList(XmlReader xml, ParsedXmlModel data)
        {
            int depth = xml.Depth;

            while (xml.Read() && xml.Depth > depth)
            {
                if (xml.IsStartElement() && !xml.IsEmptyElement)
                {
                    if (xml.Name == "Value")
                    {
                        int id = int.Parse(xml.GetAttribute("ID"));

                        XmlModelObject obj = new XmlModelObject();
                        obj.Name = xml.ReadString();

                        data.ObjectTable.Add(id, obj);
                    }
                }
            }
        }
        void ParseBoneList(XmlReader xml, ParsedXmlModel data)
        {
            int depth = xml.Depth;

            while (xml.Read() && xml.Depth > depth)
            {
                if (xml.IsStartElement() && !xml.IsEmptyElement)
                {
                    if (xml.Name == "Value")
                    {
                        int boneId = int.Parse(xml.GetAttribute("BoneID"));
                        int id = int.Parse(xml.GetAttribute("ID"));

                        data.BoneTable.Add(boneId, id);
                    }
                }
            }
        }

        void ParseInfo(XmlReader xml, ParsedXmlModel data)
        {
            int depth = xml.Depth;
            while (xml.Read() && xml.Depth > depth)
            {
                if (xml.IsStartElement() && !xml.IsEmptyElement)
                {
                    switch (xml.Name)
                    {
                        case "MeshCount":
                            data.MeshCount = int.Parse(xml.ReadString());
                            break;
                        case "FileName":
                            data.FileName = xml.ReadString();
                            break;
                        case "ObjectCount":
                            data.ObjectCount = int.Parse(xml.ReadString());
                            break;
                        case "AnimationStart":
                            data.AnimationStart = ParseFrameId(xml.ReadString());
                            break;
                        case "AnimationEnd":
                            data.AnimationEnd = ParseFrameId(xml.ReadString());
                            break;
                        case "KeyType":
                            data.KeyType = 1;
                            break;
                        case "Bone":
                            ParseObjectList(xml, data);
                            break;
                        case "Object":
                            ParseBoneList(xml, data);
                            break;
                    }
                }
            }
        }

        MeshMaterial ParseMaterial(XmlReader xml)
        {
            MeshMaterial res = new MeshMaterial(GraphicsDevice.Instance.Device);
            int depth = xml.Depth;
            float opacity = 1;

            Color3 diffuse = new Color3();
            Color3 ambient = new Color3();
            Color3 specular = new Color3();
            Color3 emissive = new Color3();

            while (xml.Read() && xml.Depth > depth)
            {
                if (xml.IsStartElement() && !xml.IsEmptyElement)
                {
                    switch (xml.Name)
                    {
                        case "Diffuse":
                            diffuse = ParseColor3(xml.ReadString());
                            break;
                        case "Ambient":
                            ambient = ParseColor3(xml.ReadString());
                            break;
                        case "Specular":
                            specular = ParseColor3(xml.ReadString());
                            break;
                        case "Emissive":
                            emissive = ParseColor3(xml.ReadString());
                            break;
                        case "Opacity":
                            opacity = float.Parse(xml.ReadString()) / 100f;
                            break;
                        case "Power":
                            res.Power = float.Parse(xml.ReadString());
                            break;
                        case "DiffuseMap":
                            res.SetTextureFile(0, xml.ReadString());
                            break;
                    }
                }
            }

            res.Diffuse = new Color4(opacity, diffuse.Red, diffuse.Green, diffuse.Blue);
            res.Ambient = new Color4(opacity, ambient.Red, ambient.Green, ambient.Blue);
            res.Specular = new Color4(opacity, specular.Red, specular.Green, specular.Blue);
            res.Emissive = new Color4(opacity, emissive.Red, emissive.Green, emissive.Blue);

            return res;
        }
        void ParseMaterials(XmlReader xml, ParsedXmlModel data)
        {
            int depth = xml.Depth;

            while (xml.Read() && xml.Depth > depth)
            {
                if (xml.IsStartElement() && !xml.IsEmptyElement)
                {
                    if (xml.Name == "Slot")
                    {
                        int index = int.Parse(xml.GetAttribute("Index"));

                        data.MaterialTable.Add(index, ParseMaterial(xml));
                    }
                }
            }

            MeshMaterial defMat = new MeshMaterial(GraphicsDevice.Instance.Device);
            defMat.D3DMaterial = MeshMaterial.DefaultMatColor;
            data.MaterialTable.Add(-1, defMat);
        }

        Vector2[] ParseMeshVector2Array(XmlReader xml)
        {
            List<Vector2> data = new List<Vector2>();

            int depth = xml.Depth;
            while (xml.Read() && xml.Depth > depth)
            {
                if (xml.IsStartElement() && !xml.IsEmptyElement)
                {
                    if (xml.Name == "Value")
                    {
                        data.Add(ParseVector2(xml.ReadString()));
                    }
                }
            }

            return data.ToArray();
        }
        Vector3[] ParseMeshVector3Array(XmlReader xml)
        {
            List<Vector3> data = new List<Vector3>();

            int depth = xml.Depth;
            while (xml.Read() && xml.Depth > depth)
            {
                if (xml.IsStartElement() && !xml.IsEmptyElement)
                {
                    if (xml.Name == "Value")
                    {
                        data.Add(ParseVector3(xml.ReadString()));
                    }
                }
            }

            return data.ToArray();
        }
        MeshFace[] ParseMeshFaces(XmlReader xml)
        {
            List<MeshFace> data = new List<MeshFace>();

            int depth = xml.Depth;
            while (xml.Read() && xml.Depth > depth)
            {
                if (xml.IsStartElement() && !xml.IsEmptyElement)
                {
                    if (xml.Name == "Value")
                    {
                        Index3i idx = ParseIndex3(xml.ReadString());
                        MeshFace face = new MeshFace(idx.A, idx.B, idx.C);

                        face.MaterialIndex = int.Parse(xml.GetAttribute("MaterialID"));

                        data.Add(face);
                    }
                }
            }

            return data.ToArray();
        }

        Index3i[] ParseTexIndex(XmlReader xml, int count)
        {
            Index3i[] texIdx = new Index3i[count];

            int subDepth = xml.Depth;
            while (xml.Read() && xml.Depth > subDepth)
            {
                if (xml.IsStartElement() && !xml.IsEmptyElement)
                {
                    if (xml.Name == "Value")
                    {
                        int index = int.Parse(xml.GetAttribute("Index"));
                        texIdx[index] = ParseIndex3(xml.ReadString());
                    }
                }
            }
            return texIdx;
        }

        VertexWeight ParseVertexWeight(XmlReader xml)
        {
            VertexWeight w = new VertexWeight();

            List<int> bones = new List<int>();
            List<float> weights = new List<float>();

            int subDepth = xml.Depth;
            while (xml.Read() && xml.Depth > subDepth)
            {
                if (xml.IsStartElement() && !xml.IsEmptyElement)
                {
                    if (xml.Name == "Value")
                    {
                        int id = int.Parse(xml.GetAttribute("BoneID"));
                        float weight = float.Parse(xml.ReadString());

                        bones.Add(id);
                        weights.Add(weight);
                    }
                }
            }

            w.BoneID = bones.ToArray();
            w.Weight = weights.ToArray();
            return w;
        }
        VertexWeight[] ParseVertexWeightArray(XmlReader xml, int count)
        {
            VertexWeight[] vtxWeights = new VertexWeight[count];

            int subDepth = xml.Depth;
            while (xml.Read() && xml.Depth > subDepth)
            {
                if (xml.IsStartElement() && !xml.IsEmptyElement)
                {
                    if (xml.Name == "Vertex")
                    {
                        int index = int.Parse(xml.GetAttribute("Index"));

                        vtxWeights[index] = ParseVertexWeight(xml);
                    }
                }
            }

            return vtxWeights;
        }

        void ParseBonePosition(XmlReader xml, Dictionary<int, XmlBoneKeyFrame> frames)
        {
            int depth = xml.Depth;
            while (xml.Read() && xml.Depth > depth)
            {
                if (xml.IsStartElement() && !xml.IsEmptyElement)
                {
                    if (xml.Name == "Value")
                    {
                        int fr = int.Parse(xml.GetAttribute("Frame"));

                        XmlBoneKeyFrame frame;
                        if (!frames.TryGetValue(fr, out frame))
                        {
                            frame.Position = new Vector3();
                            frame.FrameId = fr;
                            frame.Scale = new Vector3(1, 1, 1);
                            frame.Rotation = Quaternion.Identity;
                            frames.Add(fr, frame);
                        }

                        frame.Position = ParseVector3(xml.ReadString());
                        frames[fr] = frame;
                    }
                }
            }
        }
        void ParseBoneRotation(XmlReader xml, Dictionary<int, XmlBoneKeyFrame> frames)
        {
            int depth = xml.Depth;
            while (xml.Read() && xml.Depth > depth)
            {
                if (xml.IsStartElement() && !xml.IsEmptyElement)
                {
                    if (xml.Name == "Value")
                    {
                        int fr = int.Parse(xml.GetAttribute("Frame"));

                        XmlBoneKeyFrame frame;
                        if (!frames.TryGetValue(fr, out frame))
                        {
                            frame.Position = new Vector3();
                            frame.FrameId = fr;
                            frame.Scale = new Vector3(1, 1, 1);
                            frame.Rotation = Quaternion.Identity;
                            frames.Add(fr, frame);
                        }

                        Vector4 q = ParseVector4(xml.ReadString());
                        frame.Rotation.W = q.W;
                        frame.Rotation.X = q.X;
                        frame.Rotation.Y = q.Y;
                        frame.Rotation.Z = q.Z;

                        frames[fr] = frame;
                    }
                }
            }
        }
        void ParseBoneScale(XmlReader xml, Dictionary<int, XmlBoneKeyFrame> frames)
        {
            int depth = xml.Depth;
            while (xml.Read() && xml.Depth > depth)
            {
                if (xml.IsStartElement() && !xml.IsEmptyElement)
                {
                    if (xml.Name == "Value")
                    {
                        int fr = int.Parse(xml.GetAttribute("Frame"));

                        XmlBoneKeyFrame frame;
                        if (!frames.TryGetValue(fr, out frame))
                        {
                            frame.Position = new Vector3();
                            frame.FrameId = fr;
                            frame.Scale = new Vector3(1, 1, 1);
                            frame.Rotation = Quaternion.Identity;
                            frames.Add(fr, frame);
                        }

                        frame.Scale = ParseVector3(xml.ReadString());
                        frames[fr] = frame;
                    }
                }
            }
        }

        

        XmlBoneKeyFrame[] ParseBoneData(XmlReader xml)
        {
            Dictionary<int, XmlBoneKeyFrame> frames = new Dictionary<int, XmlBoneKeyFrame>();

            int subDepth = xml.Depth;
            while (xml.Read() && xml.Depth > subDepth)
            {
                if (xml.IsStartElement() && !xml.IsEmptyElement)
                {
                    switch (xml.Name)
                    {
                        case "Position":
                            ParseBonePosition(xml, frames);
                            break;
                        case "Quaternion":
                            ParseBoneRotation(xml, frames);
                            break;
                        case "Scale":
                            ParseBoneScale(xml, frames);
                            break;
                    }
                }
            }

            Dictionary<int, XmlBoneKeyFrame>.ValueCollection vals = frames.Values;
            XmlBoneKeyFrame[] frameArray = new XmlBoneKeyFrame[frames.Count];

            int index = 0;
            foreach (XmlBoneKeyFrame f in vals)
            {
                frameArray[index++] = f;
            }
            return frameArray;
        }

        byte[] BuildVertexPBNT1Data(Vector3[] positions, Vector3[] normals,
            Vector2[] texVtx, Index3i[] texIdx, VertexWeight[] vtxWeights, MeshFace[] faces)
        {
            Dictionary<string, int> table = new Dictionary<string, int>(faces.Length * 3);

            FastList<VertexPBNT1> vertices = new FastList<VertexPBNT1>(faces.Length * 3);

            float[] blendBuf = new float[4];
            byte[] blendBuf2 = new byte[4];

            for (int i = 0; i < faces.Length; i++)
            {
                int index;
                VertexPBNT1 vtx;

                int vtxIdx = faces[i].IndexA;

                vtx.pos = positions[vtxIdx];
                vtx.n = normals[vtxIdx];
                vtx.u = texVtx[texIdx[i].A].X;
                vtx.v = texVtx[texIdx[i].A].Y;
                for (int j = 0;  j < 4; j++)
                {
                    if (j < vtxWeights[vtxIdx].BoneID.Length)
                    {
                        blendBuf[j] = vtxWeights[vtxIdx].Weight[j];
                        blendBuf2[j] = (byte)vtxWeights[vtxIdx].BoneID[j];
                    }
                    else 
                    {
                        blendBuf2[j] = 0;
                        blendBuf[j] = 0;
                    }
                }
                vtx.blend = new Vector4(blendBuf[0], blendBuf[1], blendBuf[2], blendBuf[3]);
                vtx.boneId1 = blendBuf2[0];
                vtx.boneId2 = blendBuf2[1];
                vtx.boneId3 = blendBuf2[2];
                vtx.boneId4 = blendBuf2[3];


                string desc = vtx.ToString();

                if (!table.TryGetValue(desc, out index))
                {
                    table.Add(desc, vertices.Count);
                    faces[i].IndexA = vertices.Count;
                    vertices.Add(ref vtx);
                }
                else
                {
                    faces[i].IndexA = index;
                }

                // =========================================
                vtxIdx = faces[i].IndexB;

                vtx.pos = positions[vtxIdx];
                vtx.n = normals[vtxIdx];
                vtx.u = texVtx[texIdx[i].B].X;
                vtx.v = texVtx[texIdx[i].B].Y;
                for (int j = 0; j < 4; j++)
                {
                    if (j < vtxWeights[vtxIdx].BoneID.Length)
                    {
                        blendBuf[j] = vtxWeights[vtxIdx].Weight[j];
                        blendBuf2[j] = (byte)vtxWeights[vtxIdx].BoneID[j];
                    }
                    else
                    {
                        blendBuf2[j] = 0;
                        blendBuf[j] = 0;
                    }
                }
                vtx.blend = new Vector4(blendBuf[0], blendBuf[1], blendBuf[2], blendBuf[3]);
                vtx.boneId1 = blendBuf2[0];
                vtx.boneId2 = blendBuf2[1];
                vtx.boneId3 = blendBuf2[2];
                vtx.boneId4 = blendBuf2[3];

                desc = vtx.ToString();

                if (!table.TryGetValue(desc, out index))
                {
                    table.Add(desc, vertices.Count);
                    faces[i].IndexB = vertices.Count;
                    vertices.Add(ref vtx);
                }
                else
                {
                    faces[i].IndexB = index;
                }

                // =========================================

                vtx.pos = positions[faces[i].IndexC];
                vtx.n = normals[faces[i].IndexC];
                vtx.u = texVtx[texIdx[i].C].X;
                vtx.v = texVtx[texIdx[i].C].Y;
                for (int j = 0; j < 4; j++)
                {
                    if (j < vtxWeights[vtxIdx].BoneID.Length)
                    {
                        blendBuf[j] = vtxWeights[vtxIdx].Weight[j];
                        blendBuf2[j] = (byte)vtxWeights[vtxIdx].BoneID[j];
                    }
                    else
                    {
                        blendBuf2[j] = 0;
                        blendBuf[j] = 0;
                    }
                }
                vtx.blend = new Vector4(blendBuf[0], blendBuf[1], blendBuf[2], blendBuf[3]);
                vtx.boneId1 = blendBuf2[0];
                vtx.boneId2 = blendBuf2[1];
                vtx.boneId3 = blendBuf2[2];
                vtx.boneId4 = blendBuf2[3];

                desc = vtx.ToString();

                if (!table.TryGetValue(desc, out index))
                {
                    table.Add(desc, vertices.Count);
                    faces[i].IndexC = vertices.Count;
                    vertices.Add(ref vtx);
                }
                else
                {
                    faces[i].IndexC = index;
                }
            }


            byte[] buffer = new byte[vertices.Count * sizeof(VertexPBNT1)];
            fixed (byte* dst = &buffer[0])
            {
                fixed (VertexPBNT1* src = &vertices.Elements[0])
                {
                    Memory.Copy(src, dst, buffer.Length);
                }
            }
            return buffer;
        }

        byte[] BuildVertexPNT1Data(Vector3[] positions, Vector3[] normals, 
            Vector2[] texVtx, Index3i[] texIdx, MeshFace[] faces)
        {
            Dictionary<string, int> table = new Dictionary<string, int>(faces.Length * 3);

            FastList<VertexPNT1> vertices = new FastList<VertexPNT1>(faces.Length * 3);

            for (int i = 0; i < faces.Length; i++)
            {
                int index;
                VertexPNT1 vtx;

                int vtxIdx = faces[i].IndexA;

                vtx.pos = positions[vtxIdx];
                vtx.n = normals[vtxIdx];
                vtx.u = texVtx[texIdx[i].A].X;
                vtx.v = texVtx[texIdx[i].A].Y;

                string desc = vtx.ToString();

                if (!table.TryGetValue(desc, out index))
                {
                    table.Add(desc, vertices.Count);
                    faces[i].IndexA = vertices.Count;
                    vertices.Add(ref vtx);
                }
                else
                {
                    faces[i].IndexA = index;
                }

                // =========================================
                vtxIdx = faces[i].IndexB;

                vtx.pos = positions[vtxIdx];
                vtx.n = normals[vtxIdx];
                vtx.u = texVtx[texIdx[i].B].X;
                vtx.v = texVtx[texIdx[i].B].Y;

                desc = vtx.ToString();

                if (!table.TryGetValue(desc, out index))
                {
                    table.Add(desc, vertices.Count);
                    faces[i].IndexB = vertices.Count;
                    vertices.Add(ref vtx);
                }
                else
                {
                    faces[i].IndexB = index;
                }

                // =========================================

                vtx.pos = positions[faces[i].IndexC];
                vtx.n = normals[faces[i].IndexC];
                vtx.u = texVtx[texIdx[i].C].X;
                vtx.v = texVtx[texIdx[i].C].Y;

                desc = vtx.ToString();

                if (!table.TryGetValue(desc, out index))
                {
                    table.Add(desc, vertices.Count);
                    faces[i].IndexC = vertices.Count;
                    vertices.Add(ref vtx);
                }
                else
                {
                    faces[i].IndexC = index;
                }
            }


            byte[] buffer = new byte[vertices.Count * sizeof(VertexPNT1)];
            fixed (byte* dst = &buffer[0])
            {
                fixed (VertexPNT1* src = &vertices.Elements[0])
                {
                    Memory.Copy(src, dst, buffer.Length);
                }
            }
            return buffer;
        }

        byte[] BuildVertexPT1Data(Vector3[] positions, Vector2[] texVtx, Index3i[] texIdx, MeshFace[] faces)
        {
            Dictionary<string, int> table = new Dictionary<string, int>(faces.Length * 3);

            FastList<VertexPT1> vertices = new FastList<VertexPT1>(faces.Length * 3);

            for (int i = 0; i < faces.Length; i++)
            {
                int index;
                VertexPT1 vtx;

                int vtxIdx = faces[i].IndexA;

                vtx.pos = positions[vtxIdx];
                vtx.u1 = texVtx[texIdx[i].A].X;
                vtx.v1 = texVtx[texIdx[i].A].Y;

                string desc = vtx.ToString();

                if (!table.TryGetValue(desc, out index))
                {
                    table.Add(desc, vertices.Count);
                    faces[i].IndexA = vertices.Count;
                    vertices.Add(ref vtx);
                }
                else
                {
                    faces[i].IndexA = index;
                }

                // =========================================
                vtxIdx = faces[i].IndexB;

                vtx.pos = positions[vtxIdx];
                vtx.u1 = texVtx[texIdx[i].B].X;
                vtx.v1 = texVtx[texIdx[i].B].Y;

                desc = vtx.ToString();

                if (!table.TryGetValue(desc, out index))
                {
                    table.Add(desc, vertices.Count);
                    faces[i].IndexB = vertices.Count;
                    vertices.Add(ref vtx);
                }
                else
                {
                    faces[i].IndexB = index;
                }

                // =========================================

                vtx.pos = positions[faces[i].IndexC];
                vtx.u1 = texVtx[texIdx[i].C].X;
                vtx.v1 = texVtx[texIdx[i].C].Y;

                desc = vtx.ToString();

                if (!table.TryGetValue(desc, out index))
                {
                    table.Add(desc, vertices.Count);
                    faces[i].IndexC = vertices.Count;
                    vertices.Add(ref vtx);
                }
                else
                {
                    faces[i].IndexC = index;
                }
            }


            byte[] buffer = new byte[vertices.Count * sizeof(VertexPT1)];
            fixed (byte* dst = &buffer[0])
            {
                fixed (VertexPT1* src = &vertices.Elements[0])
                {
                    Memory.Copy(src, dst, buffer.Length);
                }
            }
            return buffer;
        }

        byte[] BuildVertexPNData(Vector3[] positions, Vector3[] normals, MeshFace[] faces) 
        {
            Dictionary<string, int> table = new Dictionary<string, int>(faces.Length * 3);

            FastList<VertexPN> vertices = new FastList<VertexPN>(faces.Length * 3);

            for (int i = 0; i < faces.Length; i++)
            {
                int index;
                VertexPN vtx;

                int vtxIdx = faces[i].IndexA;

                vtx.pos = positions[vtxIdx];
                vtx.n = normals[vtxIdx];
                
                string desc = vtx.ToString();

                if (!table.TryGetValue(desc, out index))
                {
                    table.Add(desc, vertices.Count);
                    faces[i].IndexA = vertices.Count;
                    vertices.Add(ref vtx);
                }
                else
                {
                    faces[i].IndexA = index;
                }

                // =========================================
                vtxIdx = faces[i].IndexB;

                vtx.pos = positions[vtxIdx];
                vtx.n = normals[vtxIdx];

                desc = vtx.ToString();

                if (!table.TryGetValue(desc, out index))
                {
                    table.Add(desc, vertices.Count);
                    faces[i].IndexB = vertices.Count;
                    vertices.Add(ref vtx);
                }
                else
                {
                    faces[i].IndexB = index;
                }

                // =========================================

                vtx.pos = positions[faces[i].IndexC];
                vtx.n = normals[faces[i].IndexC];

                desc = vtx.ToString();

                if (!table.TryGetValue(desc, out index))
                {
                    table.Add(desc, vertices.Count);
                    faces[i].IndexC = vertices.Count;
                    vertices.Add(ref vtx);
                }
                else
                {
                    faces[i].IndexC = index;
                }
            }


            byte[] buffer = new byte[vertices.Count * sizeof(VertexPN)];
            fixed (byte* dst = &buffer[0])
            {
                fixed (VertexPN* src = &vertices.Elements[0])
                {
                    Memory.Copy(src, dst, buffer.Length);
                }
            }
            return buffer;
        }

        byte[] BuildVertexPData(Vector3[] positions, MeshFace[] faces)
        {
            Dictionary<string, int> table = new Dictionary<string, int>(faces.Length * 3);

            FastList<VertexP> vertices = new FastList<VertexP>(faces.Length * 3);

            for (int i = 0; i < faces.Length; i++)
            {
                int index;
                VertexP vtx;

                int vtxIdx = faces[i].IndexA;

                vtx.pos = positions[vtxIdx];

                string desc = vtx.ToString();

                if (!table.TryGetValue(desc, out index))
                {
                    table.Add(desc, vertices.Count);
                    faces[i].IndexA = vertices.Count;
                    vertices.Add(ref vtx);
                }
                else
                {
                    faces[i].IndexA = index;
                }

                // =========================================
                vtxIdx = faces[i].IndexB;

                vtx.pos = positions[vtxIdx];

                desc = vtx.ToString();

                if (!table.TryGetValue(desc, out index))
                {
                    table.Add(desc, vertices.Count);
                    faces[i].IndexB = vertices.Count;
                    vertices.Add(ref vtx);
                }
                else
                {
                    faces[i].IndexB = index;
                }

                // =========================================

                vtx.pos = positions[faces[i].IndexC];

                desc = vtx.ToString();

                if (!table.TryGetValue(desc, out index))
                {
                    table.Add(desc, vertices.Count);
                    faces[i].IndexC = vertices.Count;
                    vertices.Add(ref vtx);
                }
                else
                {
                    faces[i].IndexC = index;
                }
            }


            byte[] buffer = new byte[vertices.Count * sizeof(VertexP)];
            fixed (byte* dst = &buffer[0])
            {
                fixed (VertexP* src = &vertices.Elements[0])
                {
                    Memory.Copy(src, dst, buffer.Length);
                }
            }
            return buffer;
        }

        void ParseObject(XmlReader xml, ParsedXmlModel data, ref XmlModelObject obj)
        {
            int depth = xml.Depth;

            Vector3[] positions = null;
            Vector3[] normals = null;
            MeshFace[] faces = null;

            Vector2[] texVtx = null;
            Index3i[] texIdx = null;

            VertexWeight[] vtxWeights = null;

            #region 从Xml中读取数据
            while (xml.Read() && xml.Depth > depth)
            {
                if (xml.IsStartElement() && !xml.IsEmptyElement)
                {
                    switch (xml.Name)
                    {
                        case "Parent":
                            obj.ID = int.Parse(xml.GetAttribute("ID"));
                            break;
                        case "LocalTM":
                            obj.LocalTM = ParseMatrix(xml);
                            break;
                        case "WorldTM":
                            obj.WorldTM = ParseMatrix(xml);
                            break;
                        case "BoundingBox":
                            obj.AABB = ParseBoundingBox(xml);
                            break;
                        case "Vertex":
                            positions = ParseMeshVector3Array(xml);
                            break;
                        case "VertexNormal":
                            normals = ParseMeshVector3Array(xml);
                            for (int i = 0; i < normals.Length; i++)
                            {
                                normals[i].Normalize();
                            }
                            break;
                        case "TexVertex":
                            texVtx = ParseMeshVector2Array(xml);
                            break;
                        case "TriIndex":
                            faces = ParseMeshFaces(xml);
                            break;
                        case "TexIndex":
                            texIdx = ParseTexIndex(xml, faces.Length);
                            break;
                        case "VertexWeight":
                            vtxWeights = ParseVertexWeightArray(xml, positions.Length);
                            break;
                        case "Key":
                            obj.BoneData = ParseBoneData(xml);
                            break;
                    }
                }
            }

            #endregion

            if (obj.Type == XmlModelObjectType.Mesh)
            {
                VertexFormat format = VertexFormat.None;
                if (positions != null)
                {
                    format |= VertexFormat.Position;
                }
                if (normals != null)
                {
                    format |= VertexFormat.Normal;
                }
                if (texVtx != null)
                {
                    format |= VertexFormat.Texture1;
                }
                if (vtxWeights != null)
                {
                    format |= VertexFormat.PositionBlend3;
                }


                obj.Mesh = new VirtualBicycle.Graphics.MeshData(GraphicsDevice.Instance.Device);

                VirtualBicycle.Graphics.MeshData mesh = obj.Mesh;

                mesh.Faces = faces;
                mesh.Format = format;
                mesh.Name = obj.Name;

                byte[] buffer = null;
                if (format == VertexPBNT1.Format)
                {
                    buffer = BuildVertexPBNT1Data(positions, normals, texVtx, texIdx, vtxWeights, faces);
                    mesh.VertexSize = sizeof(VertexPBNT1);
                    mesh.VertexElements = D3DX.DeclaratorFromFVF(VertexPBNT1.Format);
                }
                else if (format == VertexPNT1.Format) 
                {
                    buffer = BuildVertexPNT1Data(positions, normals, texVtx, texIdx, faces);
                    mesh.VertexSize = sizeof(VertexPNT1);
                    mesh.VertexElements = D3DX.DeclaratorFromFVF(VertexPNT1.Format);
                }
                else if (format == VertexPT1.Format) 
                {
                    buffer = BuildVertexPT1Data(positions, texVtx, texIdx, faces);
                    mesh.VertexSize = sizeof(VertexPT1);
                    mesh.VertexElements = D3DX.DeclaratorFromFVF(VertexPT1.Format);
                }
                else if (format == VertexPN.Format) 
                {
                    buffer = BuildVertexPNData(positions, normals, faces);
                    mesh.VertexSize = sizeof(VertexPN);
                    mesh.VertexElements = D3DX.DeclaratorFromFVF(VertexPN.Format);
                }
                else if (format == VertexP.Format)
                {
                    buffer = BuildVertexPData(positions, faces);
                    mesh.VertexSize = sizeof(VertexP);
                    mesh.VertexElements = D3DX.DeclaratorFromFVF(VertexP.Format);
                }

                if (buffer != null)
                {
                    if (mesh.VertexSize != 0)
                    {
                        mesh.VertexCount = buffer.Length / mesh.VertexSize;
                    }
                    fixed (byte* src = &buffer[0])
                    {
                        mesh.SetData(src, buffer.Length);
                    }
                }
            }
        }

        void ParseBody(XmlReader xml, ParsedXmlModel data)
        {
            int depth = xml.Depth;
            while (xml.Read() && xml.Depth > depth)
            {
                if (xml.IsStartElement() && !xml.IsEmptyElement)
                {
                    switch (xml.Name)
                    {
                        case "Info":
                            ParseInfo(xml, data);
                            break;
                        case "Material":
                            ParseMaterials(xml, data);
                            break;
                        case "Object":
                            int id = int.Parse(xml.GetAttribute("ID"));
                            string name = xml.GetAttribute("Name");
                            string cls = xml.GetAttribute("Class");

                            XmlModelObject obj = data.ObjectTable[id];

                            obj.Name = name;
                            obj.ID = id;

                            switch (cls)
                            {
                                case "Dummy":
                                    obj.Type = XmlModelObjectType.Dummy;
                                    break;
                                case "Editable_mesh":
                                    obj.Type = XmlModelObjectType.Mesh;
                                    break;
                                case "Biped_Object":
                                    obj.Type = XmlModelObjectType.Bone;
                                    break;
                            }

                            ParseObject(xml, data, ref obj);

                            break;
                    }


                    for (int i = 0; i < data.ObjectCount; i++)
                    {
                        Dictionary<int, MeshMaterial> matTable = data.MaterialTable;
                        bool[] useState = new bool[matTable.Count];

                        XmlModelObject obj = data.ObjectTable[i];

                        if (obj.Type == XmlModelObjectType.Mesh)
                        {
                            VirtualBicycle.Graphics.MeshData meshData = obj.Mesh;

                            for (int j = 0; j < meshData.Faces.Length; j++)
                            {
                                int mId = meshData.Faces[j].MaterialIndex;
                                if (mId == -1)
                                {
                                    mId = matTable.Count - 1;
                                    meshData.Faces[j].MaterialIndex = mId;
                                }
                                useState[mId] = true;
                            }

                            int[] matIdxShift = new int[matTable.Count];
                            int shifts = 0;
                            List<MeshMaterial> entMats = new List<MeshMaterial>();
                            for (int j = 0; j < matTable.Count; j++)
                            {
                                if (useState[j])
                                {
                                    entMats.Add(matTable[j]);
                                    matIdxShift[j] = shifts;
                                }
                                else
                                {
                                    shifts++;
                                }
                            }

                            meshData.Materials = new MeshMaterial[entMats.Count][];  //entMats.ToArray();
                            meshData.MaterialAnimation = new MaterialAnimationInstance[entMats.Count];

                            for (int j = 0; j < entMats.Count; j++)
                            {
                                meshData.Materials[j] = new MeshMaterial[] { entMats[j] };

                                meshData.MaterialAnimation[j] = new MaterialAnimationInstance(new MaterialAnimation(1, 1));
                            }

                            for (int j = 0; j < meshData.Faces.Length; j++)
                            {
                                meshData.Faces[j].MaterialIndex -= matIdxShift[meshData.Faces[j].MaterialIndex];
                            }
                        }
                    }

                }
            }
        }

        public ParsedXmlModel Parse(Stream source)
        {
            ParsedXmlModel data = new ParsedXmlModel();

            XmlReader xml = XmlReader.Create(source);

            while (xml.Read())
            {
                if (xml.IsStartElement() && !xml.IsEmptyElement)
                {
                    switch (xml.Name)
                    {
                        case "Body":
                            ParseBody(xml, data);
                            break;
                    }
                }

            }

            xml.Close();

            return data;
        }
    }
}
