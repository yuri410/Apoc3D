using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Drawing.Design;
using System.IO;
using System.Text;
using SlimDX;
using SlimDX.Direct3D9;
using VirtualBicycle.Ide.Converters;
using VirtualBicycle.Ide.Designers;
using VirtualBicycle;
using VirtualBicycle.Graphics;
using VirtualBicycle.Graphics.Animation;
using VirtualBicycle.IO;
using VirtualBicycle.MathLib;

namespace VirtualBicycle.Ide.Editors.EditableObjects
{
    public unsafe class EditableMesh : MeshData<EditableMeshMaterial>, IDisposable
    {
        bool disposed;

        Mesh mesh;

        int[] texCoordOffset = new int[8];
        int[] texCoordDemi = new int[8];

        #region 属性

        public bool HasPosition
        {
            get { return (Format & VertexFormat.Position) == VertexFormat.Position; }
        }
        public bool HasNormal
        {
            get { return (Format & VertexFormat.Normal) == VertexFormat.Normal; }
        }

        public int PositionOffset
        {
            get;
            private set;
        }

        public int PositionDemi
        {
            get;
            private set;
        }

        public int NormalOffset
        {
            get;
            private set;
        }

        private void UpdateVertexFormatInfo()
        {
            byte* srcData = (byte*)Data.ToPointer();

            VertexElement[] newElems = D3DX.DeclaratorFromFVF(Format);
            int newVtxSize = VirtualBicycle.Graphics.MeshData.ComputeVertexSize(VertexElements);

            for (int i = 0; i < newElems.Length; i++)
            {
                if (newElems[i].Type != DeclarationType.Unused)
                {
                    switch (newElems[i].Usage)
                    {
                        case DeclarationUsage.Position:
                            PositionOffset = newElems[i].Offset;
                            if (newElems[i].Type == DeclarationType.Float3)
                                PositionDemi = 3;
                            else if (newElems[i].Type == DeclarationType.Float4)
                                PositionDemi = 4;

                            break;
                        case DeclarationUsage.Normal:
                            NormalOffset = newElems[i].Offset;
                            break;
                        case DeclarationUsage.TextureCoordinate:
                            texCoordOffset[newElems[i].UsageIndex] = newElems[i].Offset;
                            texCoordDemi[newElems[i].UsageIndex] = VirtualBicycle.Graphics.MeshData.GetVertexElementSize(newElems[i]) / sizeof(float);

                            break;
                    }
                }
            }
            this.VertexSize = newVtxSize;
            this.VertexElements = newElems;
        }

        [Editor(typeof(VertexFormatEditor), typeof(UITypeEditor))]
        [TypeConverter(typeof(VertexFormatConverter))]
        public new VertexFormat Format
        {
            get { return base.Format; }
            set
            {
                if (base.Format != value)
                {
                    base.Format = value;

                    byte* srcData = (byte*)Data.ToPointer();

                    VertexElement[] newElems = D3DX.DeclaratorFromFVF(Format);
                    int newVtxSize = VirtualBicycle.Graphics.MeshData.ComputeVertexSize(VertexElements);

                    for (int i = 0; i < newElems.Length; i++)
                    {
                        if (newElems[i].Type != DeclarationType.Unused)
                        {
                            switch (newElems[i].Usage)
                            {
                                case DeclarationUsage.Position:
                                    PositionOffset = newElems[i].Offset;
                                    if (newElems[i].Type == DeclarationType.Float3)
                                        PositionDemi = 3;
                                    else if (newElems[i].Type == DeclarationType.Float4)
                                        PositionDemi = 4;

                                    break;
                                case DeclarationUsage.Normal:
                                    NormalOffset = newElems[i].Offset;
                                    break;
                                case DeclarationUsage.TextureCoordinate:
                                    texCoordOffset[newElems[i].UsageIndex] = newElems[i].Offset;
                                    texCoordDemi[newElems[i].UsageIndex] = VirtualBicycle.Graphics.MeshData.GetVertexElementSize(newElems[i]) / sizeof(float);

                                    break;
                            }
                        }
                    }

                    byte[] newBuffer = new byte[VertexCount * newVtxSize];

                    fixed (byte* dstData = &newBuffer[0])
                    {
                        for (int i = 0; i < VertexCount; i++)
                        {
                            int vtxSrcOfs = i * VertexSize;
                            int vtxDstOfs = i * newVtxSize;

                            for (int j = 0; j < VertexElements.Length; j++)
                            {
                                VertexElement targetElement = new VertexElement();
                                bool found = false;

                                for (int k = 0; k < newElems.Length; k++)
                                {
                                    if (newElems[k].Usage == VertexElements[j].Usage &&
                                        newElems[k].UsageIndex == VertexElements[j].UsageIndex)
                                    {
                                        found = true;
                                        targetElement = newElems[k];
                                        break;
                                    }
                                }

                                if (found)
                                {
                                    int size = Math.Min(
                                        VirtualBicycle.Graphics.MeshData.GetVertexElementSize(targetElement),
                                        VirtualBicycle.Graphics.MeshData.GetVertexElementSize(VertexElements[i]));

                                    Memory.Copy(srcData + vtxSrcOfs + VertexElements[i].Offset, dstData + vtxDstOfs + targetElement.Offset, size);
                                }
                            }
                        }

                        this.SetData(dstData, newVtxSize * VertexCount);

                        this.VertexSize = newVtxSize;
                        this.VertexElements = newElems;
                    }

                    MeshUpdate();
                }
            }
        }

        public void SetVertexFormat(VertexFormat format)
        {
            base.Format = format;
        }

        protected Mesh Mesh
        {
            get
            {
                if (IsD3DMeshDirty || mesh == null)
                {
                    MeshUpdate();
                }
                return mesh;
            }
            private set
            {
                if (value != mesh && !IsReleased(mesh))
                {
                    mesh.Dispose();
                }
                mesh = value;
            }
        }
        #endregion

        public EditableMesh()
            : base(GraphicsDevice.Instance.Device)
        { }
        public EditableMesh(string name, Mesh mesh, EditableMeshMaterial[][] mats)
            : base(GraphicsDevice.Instance.Device)
        {
            this.Materials = mats;
            this.Name = name;

            BuildFromMesh(mesh, this, mats);
            this.mesh = mesh;
        }

        public override void Load(BinaryDataReader data)
        {
            base.Load(data);
            UpdateVertexFormatInfo();
        }

        protected override EditableMeshMaterial LoadMaterial(Device device, BinaryDataReader matData)
        {
            return EditableMeshMaterial.FromBinary(matData);
        }
        protected override BinaryDataWriter SaveMaterial(EditableMeshMaterial mat)
        {
            return EditableMeshMaterial.ToBinary(mat);
        }

        static bool IsReleased(Mesh m)
        {
            return m == null || m.Disposed;
        }

        #region 使用D3DMesh实现的
        public void ComputeNormals()
        {
            Mesh.ComputeNormals();
            BuildFromMesh(mesh, this, this.Materials);
        }

        public void Optmize(MeshOptimizeFlags flags)
        {
            Mesh.OptimizeInPlace(flags);

            BuildFromMesh(mesh, this, this.Materials);
        }

        public void Subdevide(float tessellation)
        {
            if (tessellation < 1f)
            {
                return;
            }
            if (tessellation > 32f)
            {
                tessellation = 32f;
            }

            PatchMesh patch = new PatchMesh(Mesh);

            float cubicTess = tessellation * tessellation * tessellation;
            int faceCount = (int)(mesh.FaceCount * cubicTess);
            int vtxCount = (int)(mesh.VertexCount * cubicTess);

            Mesh newMesh = GameMesh.BuildMesh(Device, vtxCount, faceCount, mesh.VertexFormat);

            patch.Tessellate(tessellation, newMesh);

            patch.Dispose();

            Mesh = newMesh;
            BuildFromMesh(mesh, this, this.Materials);
        }
        #endregion

        #region 网格简化
        public void Simplify(MeshSimplifier ms, int[] map, int vtxCount)
        {
            System.IO.MemoryStream memStream = new System.IO.MemoryStream(VertexCount * VertexSize);

            BinaryWriter bw = new BinaryWriter(memStream);

            List<MeshFace> faceList = new List<MeshFace>(Faces.Length);
            Pair<bool, int>[] useState = new Pair<bool, int>[VertexCount];

            int index = 0;
            byte[] vtxBuffer = new byte[VertexSize];

            byte* data = (byte*)Data;

            for (int i = 0; i < Faces.Length; i++)
            {
                int p0 = ms.Map(map, Faces[i].IndexA, vtxCount);
                int p1 = ms.Map(map, Faces[i].IndexB, vtxCount);
                int p2 = ms.Map(map, Faces[i].IndexC, vtxCount);

                if (p0 == p1 || p1 == p2 || p2 == p0)
                    continue;

                if (!useState[p0].first)
                {
                    useState[p0].second = index;
                    Faces[i].IndexA = index;

                    fixed (byte* dst = &vtxBuffer[0])
                    {
                        Memory.Copy(data + p0 * VertexSize, dst, VertexSize);
                    }

                    bw.Write(vtxBuffer);

                    index++;

                }
                else
                {
                    Faces[i].IndexA = useState[p0].second;
                }

                if (!useState[p1].first)
                {
                    useState[p1].second = index;
                    Faces[i].IndexB = index;

                    fixed (byte* dst = &vtxBuffer[0])
                    {
                        Memory.Copy(data + p1 * VertexSize, dst, VertexSize);
                    }

                    bw.Write(vtxBuffer);

                    index++;
                }
                else
                {
                    Faces[i].IndexB = useState[p1].second;
                }

                if (!useState[p2].first)
                {
                    useState[p2].second = index;
                    Faces[i].IndexC = index;

                    fixed (byte* dst = &vtxBuffer[0])
                    {
                        Memory.Copy(data + p2 * VertexSize, dst, VertexSize);
                    }

                    bw.Write(vtxBuffer);

                    index++;
                }
                else
                {
                    Faces[i].IndexC = useState[p2].second;
                }

                faceList.Add(Faces[i]);
            }

            bw.Close();


            if (faceList.Count > 0)
            {
                Faces = faceList.ToArray();


                VertexCount = index;

                byte[] newBuffer = memStream.ToArray();

                Debug.Assert(newBuffer.Length == VertexCount * VertexSize);

                fixed (byte* newDataPtr = &newBuffer[0])
                {
                    SetData(newDataPtr, newBuffer.Length);
                }

                MeshUpdate();
            }
        }

        //public SimplificationMesh GetSimplificationMesh()
        //{
        //    Mesh.GenerateAdjacency(float.Epsilon);

        //    Mesh cleaned = mesh.Clean(CleanType.Simplification);

        //    SimplificationMesh sm = new SimplificationMesh(cleaned);
        //    cleaned.Dispose();
        //    return sm;
        //}
        //public SimplificationMesh GetSimplificationMesh(AttributeWeights[] weights)
        //{
        //    Mesh.GenerateAdjacency(float.Epsilon);

        //    Mesh cleaned = mesh.Clean(CleanType.Simplification);

        //    SimplificationMesh sm = new SimplificationMesh(cleaned, weights);
        //    cleaned.Dispose();
        //    return sm;
        //}
        #endregion

        #region 法线处理
        public void InverseNormals()
        {
            if (!HasNormal)
            {
                throw new NotSupportedException();
            }

            byte* data = (byte*)Data;

            for (int i = 0; i < VertexCount; i++)
            {
                *((Vector3*)(data + NormalOffset + i * VertexSize)) = -(*((Vector3*)(data + NormalOffset + i * VertexSize)));
            }
            MeshUpdate();
        }
        public void InverseNormalX()
        {
            if (!HasNormal)
            {
                throw new NotSupportedException();
            }

            byte* data = (byte*)Data;

            for (int i = 0; i < VertexCount; i++)
            {
                (*((Vector3*)(data + NormalOffset + i * VertexSize))).X = -(*((Vector3*)(data + NormalOffset + i * VertexSize))).X;
            }
            MeshUpdate();
        }
        public void InverseNormalY()
        {
            if (!HasNormal)
            {
                throw new NotSupportedException();
            }

            byte* data = (byte*)Data;

            for (int i = 0; i < VertexCount; i++)
            {
                (*((Vector3*)(data + NormalOffset + i * VertexSize))).Y = -(*((Vector3*)(data + NormalOffset + i * VertexSize))).Y;
            }
            MeshUpdate();
        }
        public void InverseNormalZ()
        {
            if (!HasNormal)
            {
                throw new NotSupportedException();
            }

            byte* data = (byte*)Data;

            for (int i = 0; i < VertexCount; i++)
            {
                (*((Vector3*)(data + NormalOffset + i * VertexSize))).Z = -(*((Vector3*)(data + NormalOffset + i * VertexSize))).Z;
            }
            MeshUpdate();
        }

        public void ComputeFlatNormal()
        {
            Dictionary<string, int> table = new Dictionary<string, int>(Faces.Length * 3);

            if (!HasPosition)
            {
                throw new NotSupportedException();
            }
            if (PositionDemi == 4)
            {
                throw new NotSupportedException();
            }
            if (!HasNormal)
            {
                Format |= VertexFormat.Normal;
            }

            System.IO.MemoryStream newData = new System.IO.MemoryStream();

            BinaryWriter bw = new BinaryWriter(newData);


            byte[] vtxBuf = new byte[VertexSize];

            byte* data = (byte*)Data;

            for (int i = 0; i < Faces.Length; i++)
            {
                int a = Faces[i].IndexA;
                int b = Faces[i].IndexB;
                int c = Faces[i].IndexC;

                byte* ptra = data + a * VertexSize;
                byte* ptrb = data + b * VertexSize;
                byte* ptrc = data + c * VertexSize;

                Vector3 pa = *(Vector3*)(ptra + PositionOffset);
                Vector3 pb = *(Vector3*)(ptrb + PositionOffset);
                Vector3 pc = *(Vector3*)(ptrc + PositionOffset);

                Vector3 n;
                MathEx.ComputePlaneNormal(ref pa, ref pb, ref pc, out n);

                *(Vector3*)(ptra + NormalOffset) = n;
                *(Vector3*)(ptrb + NormalOffset) = n;
                *(Vector3*)(ptrc + NormalOffset) = n;

                int idx;
                string desc = GetVertexDescription(a);
                if (!table.TryGetValue(desc, out idx))
                {
                    idx = table.Count;
                    table.Add(desc, idx);

                    fixed (byte* vtxDataPtr = &vtxBuf[0])
                    {
                        Memory.Copy(ptra, vtxDataPtr, VertexSize);
                    }
                    bw.Write(vtxBuf);
                }
                Faces[i].IndexA = idx;

                desc = GetVertexDescription(b);
                if (!table.TryGetValue(desc, out idx))
                {
                    idx = table.Count;
                    table.Add(desc, idx);

                    fixed (byte* vtxDataPtr = &vtxBuf[0])
                    {
                        Memory.Copy(ptrb, vtxDataPtr, VertexSize);
                    }
                    bw.Write(vtxBuf);
                }
                Faces[i].IndexB = idx;

                desc = GetVertexDescription(c);
                if (!table.TryGetValue(desc, out idx))
                {
                    idx = table.Count;
                    table.Add(desc, idx);

                    fixed (byte* vtxDataPtr = &vtxBuf[0])
                    {
                        Memory.Copy(ptrc, vtxDataPtr, VertexSize);
                    }
                    bw.Write(vtxBuf);
                }
                Faces[i].IndexC = idx;
            }

            bw.Close();

            byte[] newBuffer = newData.ToArray();

            VertexCount = table.Count;
            Debug.Assert(newBuffer.Length == VertexCount * VertexSize);

            fixed (byte* src = &newBuffer[0])
            {
                SetData(src, newBuffer.Length);
            }

            MeshUpdate();
        }
        #endregion

        public bool Intersects(Vector3 start, Vector3 end, out float dist)
        {
            return Mesh.Intersects(new Ray(start, end), out dist);
        }

        #region 顶点哈希
        public string GetVertexDescription(int index)
        {
            return GetVertexDescription(index, VertexElements);
        }
        public string GetVertexDescription(int index, VertexElement[] elements)
        {
            byte* data = (byte*)Data + index * VertexSize;

            StringBuilder sb = new StringBuilder();

            for (int i = 0; i < elements.Length; i++)
            {
                if (elements[i].Type != DeclarationType.Unused)
                {
                    sb.Append(elements[i].Usage.ToString());

                    switch (elements[i].Type)
                    {
                        case DeclarationType.Color:
                            sb.Append((*(int*)(data + elements[i].Offset)).ToString());
                            break;
                        case DeclarationType.Float1:
                            sb.Append((*(float*)(data + elements[i].Offset)).ToString());
                            break;
                        case DeclarationType.Float2:
                            sb.Append((*(Vector2*)(data + elements[i].Offset)).ToString());
                            break;
                        case DeclarationType.Float3:
                            sb.Append((*(Vector3*)(data + elements[i].Offset)).ToString());
                            break;
                        case DeclarationType.Float4:
                            sb.Append((*(Vector4*)(data + elements[i].Offset)).ToString());
                            break;
                        default:
                            throw new NotSupportedException(elements[i].Type.ToString());
                    }
                }
            }
            return sb.ToString();
        }

        #endregion

        #region 合并
        public void WeldVertices(bool pos, bool n, bool uv1)
        {
            VertexElement[] elems = D3DX.DeclaratorFromFVF(VertexFormat.PositionNormal | VertexFormat.Texture1);
            WeldVertices(elems);
        }

        public void WeldVertices(VertexElement[] elem)
        {
            Dictionary<string, int> table = new Dictionary<string, int>(VertexCount);

            // vertices[i]表示新顶点数据中，顶点i在旧定点数据中的索引
            List<int> vertices = new List<int>(VertexCount);

            for (int i = 0; i < Faces.Length; i++)
            {
                int index;

                string desc = GetVertexDescription(Faces[i].IndexA, elem);
                if (!table.TryGetValue(desc, out index))
                {
                    index = vertices.Count;

                    table.Add(desc, index);
                    vertices.Add(Faces[i].IndexA);
                }
                Faces[i].IndexA = index;


                desc = GetVertexDescription(Faces[i].IndexB, elem);
                if (!table.TryGetValue(desc, out index))
                {
                    index = vertices.Count;

                    table.Add(desc, index);
                    vertices.Add(Faces[i].IndexB);
                }
                Faces[i].IndexB = index;


                desc = GetVertexDescription(Faces[i].IndexC, elem);
                if (!table.TryGetValue(desc, out index))
                {
                    index = vertices.Count;

                    table.Add(desc, index);
                    vertices.Add(Faces[i].IndexC);
                }
                Faces[i].IndexC = index;
            }

            int newVertexCount = vertices.Count;

            byte[] newBuffer = new byte[VertexSize * newVertexCount];

            fixed (byte* dst = &newBuffer[0])
            {
                int offset = 0;
                byte* src = (byte*)Data;

                for (int i = 0; i < newVertexCount; i++)
                {
                    Memory.Copy(src + vertices[i] * VertexSize, dst + offset, VertexSize);

                    offset += VertexSize;
                }

                SetData(dst, newBuffer.Length);
                VertexCount = newVertexCount;
            }
            IsD3DMeshDirty = true;
        }
        #endregion

        bool IsD3DMeshDirty
        {
            get;
            set;
        }

        void MeshUpdate()
        {
            if (!IsReleased(mesh))
            {
                mesh.Dispose();
            }
            mesh = GameMesh.BuildMeshFromData<EditableMeshMaterial>(Device, this);
        }

        public void Render()
        {
            if (Mesh != null)
            {
                for (int j = 0; j < MaterialAnimation.Length; j++)
                {
                    MaterialAnimation[j].Update(0.25f);
                }

                for (int i = 0; i < Materials.Length; i++)
                {
                    DevUtils.SetMaterial(Device, Materials[i][MaterialAnimation[i].CurrentFrame]);
                    Mesh.DrawSubset(i);
                }
            }
        }

        public void ExportAsObj(string file)
        {

            if (!HasTexCoord1)
            {
                throw new NotSupportedException();
            }
            if (PositionDemi != 3)
            {
                throw new NotSupportedException();
            }


            FileStream fs = new FileStream(file, FileMode.OpenOrCreate, FileAccess.Write);
            fs.SetLength(0);
            StreamWriter sw = new StreamWriter(fs, Encoding.Default);


            byte* data = (byte*)Data;

            sw.WriteLine('g');

            for (int i = 0; i < VertexCount; i++)
            {
                Vector3* vtxPtr = (Vector3*)(data + i * VertexSize + PositionOffset);

                sw.Write("v ");
                sw.Write(vtxPtr->X.ToString());
                sw.Write(' ');
                sw.Write(vtxPtr->Y.ToString());
                sw.Write(' ');
                sw.WriteLine(vtxPtr->Z.ToString());

            }

            for (int i = 0; i < VertexCount; i++)
            {
                Vector2* vtxPtr = (Vector2*)(data + i * VertexSize + PositionOffset);

                sw.Write("vt ");
                sw.Write(vtxPtr->X.ToString());
                sw.Write(' ');
                sw.Write(vtxPtr->Y.ToString());
                sw.WriteLine(" 0.0");
            }

            for (int i = 0; i < VertexCount; i++)
            {
                Vector3* vtxPtr = (Vector3*)(data + i * VertexSize + PositionOffset);

                sw.Write("vn ");
                sw.Write(vtxPtr->X.ToString());
                sw.Write(' ');
                sw.Write(vtxPtr->Y.ToString());
                sw.Write(' ');
                sw.WriteLine(vtxPtr->Z.ToString());
            }
            sw.WriteLine('g');
            for (int i = 0; i < Faces.Length; i++)
            {
                int a = Faces[i].IndexA + 1;
                int b = Faces[i].IndexB + 1;
                int c = Faces[i].IndexC + 1;

                string sa = a.ToString();
                string sb = b.ToString();
                string sc = c.ToString();


                sw.Write("f ");
                sw.Write(sa + "/" + sa + "/" + sa);
                sw.Write(' ');
                sw.Write(sb + "/" + sb + "/" + sb);
                sw.Write(' ');
                sw.WriteLine(sc + "/" + sc + "/" + sc);
            }
            sw.WriteLine('g');
            sw.Close();
        }
        //public static EditableMesh ImportFromXml(string file)
        //{
        //    Xml2ModelConverter conv = new Xml2ModelConverter();
        //    System.IO.MemoryStream ms = new System.IO.MemoryStream(65536);
        //    conv.Convert(new FileLocation(file), new StreamedLocation(new VirtualStream(ms, 0)));

        //    ms.Position = 0;


        //    EditableModel sounds = EditableModel.FromFile(new StreamedLocation(ms));// new EditableModel();                

        //    //content.Texture1 = sounds.Entities[0].Texture1;
        //    //content.Positions = sounds.Entities[0].Positions;
        //    //content.Normals = sounds.Entities[0].Normals;
        //    //content.Materials = sounds.Entities[0].Materials;
        //    //content.Faces = sounds.Entities[0].Faces;
        //    //content.Format = sounds.Entities[0].Format;

        //    return sounds.Entities[0];
        //}

        public EditableMesh Clone()
        {
            EditableMesh copy = new EditableMesh();
            copy.Device = Device;
            copy.Faces = new MeshFace[Faces.Length];
            Array.Copy(Faces, copy.Faces, Faces.Length);

            copy.Materials = new EditableMeshMaterial[Materials.Length][];
            for (int i = 0; i < Materials.Length; i++)
            {
                copy.Materials[i] = new EditableMeshMaterial[Materials[i].Length];
                Array.Copy(Materials[i], copy.Materials[i], Materials[i].Length);
            }

            copy.MaterialAnimation = new MaterialAnimationInstance[MaterialAnimation.Length];
            for (int i = 0; i < copy.MaterialAnimation.Length; i++)
            {
                copy.MaterialAnimation[i] = new MaterialAnimationInstance(MaterialAnimation[i].Data);
            }

            copy.VertexSize = VertexSize;
            copy.VertexElements = new VertexElement[VertexElements.Length];
            Array.Copy(VertexElements, copy.VertexElements, VertexElements.Length);
            copy.VertexCount = VertexCount;
            copy.texCoordDemi = new int[texCoordDemi.Length];
            Array.Copy(texCoordDemi, copy.texCoordDemi, texCoordDemi.Length);
            copy.texCoordOffset = new int[texCoordOffset.Length];
            Array.Copy(texCoordOffset, copy.texCoordOffset, texCoordOffset.Length);
            copy.PositionDemi = PositionDemi;
            copy.PositionOffset = PositionOffset;
            copy.NormalOffset = NormalOffset;


            copy.SetData(this.Data.ToPointer(), VertexSize * VertexCount);

            copy.SetVertexFormat(Format);

            copy.Name = Name;

            return copy;
        }
        public void CloneTo(EditableMesh m)
        {
            m.Faces = new MeshFace[Faces.Length];
            Array.Copy(Faces, m.Faces, Faces.Length);

            m.Materials = new EditableMeshMaterial[Materials.Length][];
            for (int i = 0; i < m.Materials.Length; i++)
            {
                m.Materials[i] = new EditableMeshMaterial[Materials[i].Length];
                Array.Copy(Materials[i], m.Materials[i], Materials[i].Length);
            }

            m.MaterialAnimation = new MaterialAnimationInstance[MaterialAnimation.Length];
            for (int i = 0; i < m.MaterialAnimation.Length; i++)
            {
                m.MaterialAnimation[i] = new MaterialAnimationInstance(MaterialAnimation[i].Data);
            }

            m.VertexSize = VertexSize;
            m.VertexElements = new VertexElement[VertexElements.Length];
            Array.Copy(VertexElements, m.VertexElements, VertexElements.Length);
            m.VertexCount = VertexCount;
            m.texCoordDemi = new int[texCoordDemi.Length];
            Array.Copy(texCoordDemi, m.texCoordDemi, texCoordDemi.Length);
            m.texCoordOffset = new int[texCoordOffset.Length];
            Array.Copy(texCoordOffset, m.texCoordOffset, texCoordOffset.Length);
            m.PositionDemi = PositionDemi;
            m.PositionOffset = PositionOffset;
            m.NormalOffset = NormalOffset;

            m.SetData(this.Data.ToPointer(), VertexSize * VertexCount);

            m.SetVertexFormat(Format);

            m.Name = Name;
        }

        #region IDisposable 成员

        public bool Disposed
        {
            get { return disposed; }
        }

        public void Dispose()
        {
            if (disposed)
            {
                if (!IsReleased(mesh))
                {
                    mesh.Dispose();
                    mesh = null;
                }
                if (Materials != null)
                {
                    for (int i = 0; i < Materials.Length; i++)
                    {
                        for (int j = 0; j < Materials[i].Length; j++)
                        {
                            Materials[i][j].Dispose();
                            Materials[i][j] = null;
                        }
                    }
                    Materials = null;
                }
                disposed = true;
            }
        }

        #endregion

        ~EditableMesh()
        {
            if (!disposed)
                Dispose();
        }

        public void Dispose(bool p)
        {
            if (!disposed)
            {
                if (!IsReleased(mesh))
                {
                    mesh.Dispose();
                    mesh = null;
                }
                if (p)
                {
                    if (Materials != null)
                    {
                        for (int i = 0; i < Materials.Length; i++)
                        {
                            for (int j = 0; j < Materials[i].Length; j++)
                            {
                                Materials[i][j].Dispose();
                                Materials[i] = null;
                            }
                        }
                        Materials = null;
                    }
                }
                disposed = true;
            }

        }

        public override string ToString()
        {
            return string.Format("网格：{0}", Name);
        }
    }
}
