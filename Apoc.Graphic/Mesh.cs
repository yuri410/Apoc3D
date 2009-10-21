using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;
using SlimDX;
using SlimDX.Direct3D9;
using Apoc.Graphics.Animation;
using Apoc.IO;

namespace Apoc.Graphics
{
    /// <summary>
    /// mesh的最小组成部分-三角形
    /// </summary>
    public struct MeshFace
    {
        #region Fields
        int a;
        int b;
        int c;

        int materialIdx;
        #endregion

        #region Constructors
        public MeshFace(int A, int B, int C)
        {
            a = A;
            b = B;
            c = C;
            materialIdx = -1;
        }
        public MeshFace(int A, int B, int C, int matId)
        {
            a = A;
            b = B;
            c = C;
            materialIdx = matId;
        }
        #endregion

        #region Properties
        public int IndexA
        {
            get { return a; }
            set { a = value; }
        }
        public int IndexB
        {
            get { return b; }
            set { b = value; }
        }
        public int IndexC
        {
            get { return c; }
            set { c = value; }
        }

        public int MaterialIndex
        {
            get { return materialIdx; }
            set { materialIdx = value; }
        }
        #endregion
    }

    public interface IMeshTriangleCallBack
    {
        void Process(Vector3 a, Vector3 b, Vector3 c);
    }

    public abstract unsafe class MeshData<MType>
        where MType : class
    {
        #region 常量

        public const int MeshId = ((byte)'M' << 24) | ((byte)'E' << 16) | ((byte)'S' << 8) | ((byte)'H');

        protected static readonly string MaterialCountTag = "MaterialCount";
        protected static readonly string MaterialsTag = "Materials";

        protected static readonly string MaterialAnimationTag = "MaterialAnimation";
        protected static readonly string FaceCountTag = "FaceCount";
        protected static readonly string FacesTag = "Faces";
        protected static readonly string VertexFormatTag = "VertexFormat";
        protected static readonly string VertexCountTag = "VertexCount";
        protected static readonly string VertexSizeTag = "VertexSize";

        protected static readonly string DataTag = "VertexData";

        protected static readonly string NameTag = "Name";

        #endregion

        byte[] buffer;
        Device device;

        protected MeshData(Device dev)
        {
            device = dev;
        }

        #region 属性

        [Browsable(false)]
        public Device Device
        {
            get { return device; }
            protected set { device = value; }
        }

        public MType[][] Materials
        {
            get;
            set;
        }

        /// <summary>
        ///  先entity后frameIndex
        /// </summary>
        public MaterialAnimationInstance[] MaterialAnimation
        {
            get;
            set;
        }

        public string Name
        {
            get;
            set;
        }

        public VertexFormat Format
        {
            get;
            set;
        }

        [Browsable(false)]
        public IntPtr Data
        {
            get
            {
                IntPtr res;
                fixed (byte* ptr = &buffer[0])
                {
                    res = new IntPtr(ptr);
                }
                return res;
            }
        }

        public int VertexSize
        {
            get;
            set;
        }

        public int VertexCount
        {
            get;
            set;
        }

        public MeshFace[] Faces
        {
            get;
            set;
        }
        [Browsable(false)]
        public int TextureCoordCount
        {
            get
            {
                int result = 0;

                if (HasTexCoord1)
                    result++;
                if (HasTexCoord2)
                    result++;
                if (HasTexCoord3)
                    result++;
                if (HasTexCoord4)
                    result++;
                if (HasTexCoord5)
                    result++;
                if (HasTexCoord6)
                    result++;
                if (HasTexCoord7)
                    result++;
                if (HasTexCoord8)
                    result++;
                return result;
            }
        }

        [Browsable(false)]
        public VertexElement[] VertexElements
        {
            get;
            set;
        }

        [Browsable(false)]
        public bool HasTexCoord1
        {
            get { return (Format & VertexFormat.Texture1) == VertexFormat.Texture1; }
        }
        [Browsable(false)]
        public bool HasTexCoord2
        {
            get { return (Format & VertexFormat.Texture2) == VertexFormat.Texture2; }
        }
        [Browsable(false)]
        public bool HasTexCoord3
        {
            get { return (Format & VertexFormat.Texture3) == VertexFormat.Texture3; }
        }
        [Browsable(false)]
        public bool HasTexCoord4
        {
            get { return (Format & VertexFormat.Texture4) == VertexFormat.Texture4; }
        }
        [Browsable(false)]
        public bool HasTexCoord5
        {
            get { return (Format & VertexFormat.Texture5) == VertexFormat.Texture5; }
        }
        [Browsable(false)]
        public bool HasTexCoord6
        {
            get { return (Format & VertexFormat.Texture6) == VertexFormat.Texture6; }
        }
        [Browsable(false)]
        public bool HasTexCoord7
        {
            get { return (Format & VertexFormat.Texture7) == VertexFormat.Texture7; }
        }
        [Browsable(false)]
        public bool HasTexCoord8
        {
            get { return (Format & VertexFormat.Texture8) == VertexFormat.Texture8; }
        }

        #endregion

        #region 方法

        public static void BuildFromMesh(Mesh mesh, MeshData<MType> data, MType[][] mats)
        {
            void* src = mesh.LockVertexBuffer(LockFlags.None).DataPointer.ToPointer();

            byte[] buffer = new byte[mesh.VertexCount * mesh.BytesPerVertex];

            fixed (byte* dst = &buffer[0])
            {
                Memory.Copy(src, dst, buffer.Length);
                data.SetData(dst, buffer.Length);
            }

            mesh.UnlockVertexBuffer();

            data.device = mesh.Device;
            data.Format = mesh.VertexFormat;
            data.Materials = mats;
            data.MaterialAnimation = new MaterialAnimationInstance[mats.Length]; //{ new MaterialAnimationInstance(matAnimData) };
            for (int i = 0; i < mats.Length; i++)
            {
                MaterialAnimation matAnimData = new MaterialAnimation(mats[i].Length, 0.025f);
                data.MaterialAnimation[i] = new MaterialAnimationInstance(matAnimData);
            }
            data.VertexSize = mesh.BytesPerVertex;
            data.VertexCount = mesh.VertexCount;

            VertexElement[] elements = D3DX.DeclaratorFromFVF(mesh.VertexFormat);

            data.VertexElements = new VertexElement[elements.Length];
            Array.Copy(elements, data.VertexElements, elements.Length);

            int faceCount = mesh.FaceCount;

            data.Faces = new MeshFace[faceCount];

            uint* ab = (uint*)mesh.LockAttributeBuffer(LockFlags.ReadOnly).DataPointer.ToPointer();

            if ((mesh.CreationOptions & MeshFlags.Use32Bit) == MeshFlags.Use32Bit)
            {
                uint* ib = (uint*)mesh.LockIndexBuffer(LockFlags.ReadOnly).DataPointer.ToPointer();
                for (int i = 0; i < faceCount; i++)
                {
                    int idxId = i * 3;

                    data.Faces[i] = new MeshFace((int)ib[idxId], (int)ib[idxId + 1], (int)ib[idxId + 2], (int)ab[i]);
                }
                mesh.UnlockIndexBuffer();
            }
            else
            {
                ushort* ib = (ushort*)mesh.LockIndexBuffer(LockFlags.ReadOnly).DataPointer.ToPointer();
                for (int i = 0; i < faceCount; i++)
                {
                    int idxId = i * 3;

                    data.Faces[i] = new MeshFace(ib[idxId], ib[idxId + 1], ib[idxId + 2], (int)ab[i]);
                }
                mesh.UnlockIndexBuffer();
            }

            mesh.UnlockAttributeBuffer();

        }

        protected abstract MType LoadMaterial(Device device, BinaryDataReader matData);
        protected abstract BinaryDataWriter SaveMaterial(MType mat);

        public void SetData(void* data, int size)
        {
            if (buffer == null || buffer.Length != size)
            {
                buffer = new byte[size];
            }
            Memory.Copy(data, this.Data.ToPointer(), size);
        }

        /// <summary>
        /// 从一个BinaryDataReader对象载入mesh数据
        /// </summary>
        /// <param name="data"></param>
        public virtual void Load(BinaryDataReader data)
        {
            int materialCount = data.GetDataInt32(MaterialCountTag);
            Materials = new MType[materialCount][];

            ContentBinaryReader br = data.GetData(MaterialsTag);
            for (int i = 0; i < materialCount; i++)
            {
                int frameCount = br.ReadInt32();

                Materials[i] = new MType[frameCount];

                for (int j = 0; j < frameCount; j++)
                {
                    BinaryDataReader matData = br.ReadBinaryData();
                    Materials[i][j] = LoadMaterial(device, matData);
                    matData.Close();
                }
            }
            br.Close();

            MaterialAnimation = new MaterialAnimationInstance[materialCount];
            br = data.GetData(MaterialAnimationTag);
            for (int i = 0; i < materialCount; i++)
            {
                int frameCount = br.ReadInt32();
                float frameLength = br.ReadSingle();

                MaterialAnimation animData = new MaterialAnimation(frameCount, frameLength);
                MaterialAnimation[i] = new MaterialAnimationInstance(animData);
            }
            br.Close();

            br = data.GetData(NameTag);
            Name = br.ReadStringUnicode();
            br.Close();

            int faceCount = data.GetDataInt32(FaceCountTag);
            Faces = new MeshFace[faceCount];

            br = data.GetData(FacesTag);
            for (int i = 0; i < faceCount; i++)
            {
                Faces[i].IndexA = br.ReadInt32();
                Faces[i].IndexB = br.ReadInt32();
                Faces[i].IndexC = br.ReadInt32();

                Faces[i].MaterialIndex = br.ReadInt32();
            }
            br.Close();

            Format = (VertexFormat)data.GetDataInt32(VertexFormatTag);
            VertexElements = D3DX.DeclaratorFromFVF(Format);

            if (data.Contains(VertexSizeTag))
            {
                VertexSize = data.GetDataInt32(VertexSizeTag);
            }
            else
            {
                VertexSize = MeshData.ComputeVertexSize(VertexElements);
            }
            VertexCount = data.GetDataInt32(VertexCountTag);

            br = data.GetData(DataTag);
            buffer = br.ReadBytes(VertexSize * VertexCount);
            br.Close();
        }

        /// <summary>
        /// 保存mesh数据到一个BinaryDataWriter对象中
        /// </summary>
        /// <returns></returns>
        public virtual BinaryDataWriter Save()
        {
            int materialCount = Materials.Length;

            BinaryDataWriter data = new BinaryDataWriter();

            data.AddEntry(MaterialCountTag, materialCount);

            ContentBinaryWriter bw = data.AddEntry(MaterialsTag);
            for (int i = 0; i < Materials.Length; i++)
            {
                int frameCount = Materials[i].Length;

                bw.Write(frameCount);
                for (int j = 0; j < frameCount; j++)
                {
                    BinaryDataWriter matData = SaveMaterial(Materials[i][j]);
                    bw.Write(matData);
                    matData.Dispose();
                }
            }
            bw.Close();

            bw = data.AddEntry(MaterialAnimationTag);
            for (int i = 0; i < MaterialAnimation.Length; i++)
            {
                bw.Write(MaterialAnimation[i].Data.FrameCount);
                bw.Write(MaterialAnimation[i].Data.FrameLength);
            }
            bw.Close();


            bw = data.AddEntry(NameTag);
            bw.WriteStringUnicode(Name);
            bw.Close();

            data.AddEntry(FaceCountTag, Faces.Length);

            bw = data.AddEntry(FacesTag);
            for (int i = 0; i < Faces.Length; i++)
            {
                bw.Write(Faces[i].IndexA);
                bw.Write(Faces[i].IndexB);
                bw.Write(Faces[i].IndexC);
                bw.Write(Faces[i].MaterialIndex);
            }
            bw.Close();

            data.AddEntry(VertexFormatTag, (int)Format);

            data.AddEntry(VertexSizeTag, VertexSize);
            data.AddEntry(VertexCountTag, VertexCount);

            if (VertexSize * VertexCount != buffer.Length)
            {
                throw new InvalidDataException();
            }

            bw = data.AddEntry(DataTag);
            bw.Write(buffer);
            bw.Close();

            return data;
        }


        /// <summary>
        /// 从流中读取数据
        /// </summary>
        /// <param name="stm"></param>
        public void Load(Stream stm)
        {
            ContentBinaryReader br = new ContentBinaryReader(stm, Encoding.Default);

            int id = br.ReadInt32();

            if (id == (int)MeshId)
            {
                BinaryDataReader data = br.ReadBinaryData();
                Load(data);
                data.Close();
            }
            else
            {
                br.Close();
                throw new InvalidDataException(stm.ToString());
            }

            br.Close();
        }

        /// <summary>
        /// 保存数据到流中
        /// </summary>
        /// <param name="stm"></param>
        public void Save(Stream stm)
        {
            ContentBinaryWriter bw = new ContentBinaryWriter(stm, Encoding.Default);

            bw.Write((int)MeshId);
            BinaryDataWriter data = Save();
            bw.Write(data);
            data.Dispose();

            bw.Close();
        }

        #endregion
    }

    public unsafe class MeshData : MeshData<MeshMaterial>
    {
        public MeshData(Device dev)
            : base(dev)
        {
        }

        public MeshData(GameMesh mesh)
            : base(mesh.Device)
        {
            this.Format = mesh.Format;
            this.Materials = mesh.Materials;
            this.MaterialAnimation = mesh.MaterialAnimation;
            this.Name = mesh.Name;
            this.VertexCount = mesh.VertexCount;
            this.VertexSize = mesh.VertexSize;

            void* src = mesh.VertexBuffer.Lock(0, 0, LockFlags.ReadOnly).DataPointer.ToPointer();

            SetData(src, VertexSize * VertexCount);

            mesh.VertexBuffer.Unlock();

            GetFaces(mesh);
        }

        public static int ComputeVertexSize(VertexElement[] elements)
        {
            int vertexSize = 0;
            for (int i = 0; i < elements.Length; i++)
            {
                vertexSize += GetVertexElementSize(elements[i]);
            }
            return vertexSize;
        }

        static int[] elementSizeTable = new int[]
        {
            sizeof(float), 
            sizeof(float) * 2, 
            sizeof(float) * 3, 
            sizeof(float) * 4, 
            sizeof(int), 
            sizeof(byte) * 4, 
            sizeof(short) * 2,
            sizeof(short) * 4, 
            0, 0, 0, 0, 0, 0, 0, 
            sizeof(float), 
            sizeof(float) * 2, 
            0 };

        public static int GetVertexElementSize(VertexElement elem)
        {
            return elementSizeTable[(int)elem.Type];
        }

        void GetFaces(GameMesh mesh)
        {
            IndexBuffer[] ibs = mesh.IndexBuffers;

            Faces = new MeshFace[mesh.PrimitiveCount];

            int[] partPrimCount = mesh.PartPrimitiveCount;

            int faceIdx = 0;

            for (int i = 0; i < ibs.Length; i++)
            {
                if (ibs[i].Description.Format == SlimDX.Direct3D9.Format.Index16)
                {
                    ushort* isrc = (ushort*)ibs[i].Lock(0, 0, LockFlags.ReadOnly).DataPointer.ToPointer();

                    for (int j = 0; j < partPrimCount[i]; j++)
                    {
                        int iPos = j * 3;
                        Faces[faceIdx++] = new MeshFace(isrc[iPos], isrc[iPos + 1], isrc[iPos + 2], i);
                    }

                    ibs[i].Unlock();
                }
                else
                {
                    int* isrc = (int*)ibs[i].Lock(0, 0, LockFlags.ReadOnly).DataPointer.ToPointer();

                    for (int j = 0; j < partPrimCount[i]; j++)
                    {
                        int iPos = j * 3;
                        Faces[faceIdx++] = new MeshFace(isrc[iPos], isrc[iPos + 1], isrc[iPos + 2], i);
                    }

                    ibs[i].Unlock();
                }
            }
        }

        protected override MeshMaterial LoadMaterial(Device device, BinaryDataReader matData)
        {
            return MeshMaterial.FromBinary(device, matData);
        }
        protected override BinaryDataWriter SaveMaterial(MeshMaterial mat)
        {
            return MeshMaterial.ToBinary(mat);
        }
    }

    /// <summary>
    /// 游戏中的mesh
    /// </summary>
    public unsafe class GameMesh : IRenderable, IDisposable, IUpdatable
    {
        #region Fields
        VertexDeclaration vtxDecl;
        int vertexSize;

        protected VertexFormat vtxFormat;

        //GeomentryData[] bufferedGm;
        RenderOperation[] bufferedOp;

        protected MeshMaterial[][] materials;
        protected MaterialAnimationInstance[] matAnims;

        protected VertexBuffer vertexBuffer;
        protected IndexBuffer[] indexBuffers;
        protected int[] partPrimCount;
        protected int[] partVtxCount;

        Device dev;
        string name;

        bool disposed;

        int primCount;
        int vertexCount;
        #endregion

        #region APIMesh
        public static Mesh BuildMesh(Device dev, int vertexCount, int faceCount, VertexFormat format)
        {
            bool useIndex16 = vertexCount <= ushort.MaxValue;
            Mesh mesh;
            if (useIndex16)
            {
                mesh = new Mesh(dev, faceCount, vertexCount, MeshFlags.Managed, format);
            }
            else
            {
                mesh = new Mesh(dev, faceCount, vertexCount, MeshFlags.Managed | MeshFlags.Use32Bit, format);
            }
            return mesh;
        }

        /// <summary>
        /// Build a Mesh object from GameMeshDataBase
        /// </summary>
        /// <typeparam name="MType"></typeparam>
        /// <param name="dev"></param>
        /// <param name="sounds"></param>
        /// <returns></returns>
        public static Mesh BuildMeshFromData<MType>(Device dev, MeshData<MType> data)
            where MType : class
        {
            Mesh mesh;

            int matCount = data.Materials.Length;
            int vertexCount = data.VertexCount;
            int faceCount = data.Faces.Length;

            bool useIndex16 = vertexCount <= ushort.MaxValue;

            if (useIndex16)
            {
                mesh = new Mesh(dev, faceCount, vertexCount, MeshFlags.Managed, data.Format);
            }
            else
            {
                mesh = new Mesh(dev, faceCount, vertexCount, MeshFlags.Managed | MeshFlags.Use32Bit, data.Format);
            }

            void* vdst = mesh.LockVertexBuffer(LockFlags.None).DataPointer.ToPointer();

            Memory.Copy(data.Data.ToPointer(), vdst, vertexCount * data.VertexSize);

            mesh.UnlockVertexBuffer();

            

            List<int>[] indices = new List<int>[matCount];
            for (int i = 0; i < matCount; i++)
            {
                indices[i] = new List<int>();
            }


            MeshFace[] faces = data.Faces;
            for (int i = 0; i < faces.Length; i++)
            {
                MeshFace face = faces[i];
                int matId = face.MaterialIndex;

                indices[matId].Add(face.IndexA);
                indices[matId].Add(face.IndexB);
                indices[matId].Add(face.IndexC);
            }


            uint* ab = (uint*)mesh.LockAttributeBuffer(LockFlags.None).DataPointer.ToPointer();

            if (useIndex16)
            {
                ushort* ib = (ushort*)mesh.LockIndexBuffer(LockFlags.None).DataPointer.ToPointer();

                int faceIdx = 0;
                for (int i = 0; i < matCount; i++)
                {
                    List<int> idx = indices[i];
                    for (int j = 0; j < idx.Count; j += 3)
                    {
                        ab[faceIdx] = (uint)i;

                        int ibIdx = faceIdx * 3;
                        ib[ibIdx] = (ushort)idx[j];
                        ib[ibIdx + 1] = (ushort)idx[j + 1];
                        ib[ibIdx + 2] = (ushort)idx[j + 2];

                        faceIdx++;
                    }
                }

                mesh.UnlockIndexBuffer();
            }
            else
            {
                uint* ib = (uint*)mesh.LockIndexBuffer(LockFlags.None).DataPointer.ToPointer();

                int faceIdx = 0;
                for (int i = 0; i < matCount; i++)
                {
                    List<int> idx = indices[i];
                    for (int j = 0; j < idx.Count; j += 3)
                    {
                        ab[faceIdx] = (uint)i;

                        int ibIdx = faceIdx * 3;
                        ib[ibIdx] = (ushort)idx[j];
                        ib[ibIdx + 1] = (ushort)idx[j + 1];
                        ib[ibIdx + 2] = (ushort)idx[j + 2];

                        faceIdx++;
                    }
                }

                mesh.UnlockIndexBuffer();
            }
            mesh.UnlockAttributeBuffer();
            return mesh;
        }
        #endregion

        #region 获取数据

        public VertexBuffer VertexBuffer
        {
            get { return vertexBuffer; }
        }

        public IndexBuffer[] IndexBuffers
        {
            get { return indexBuffers; }
        }

        public int[] GetIndices()
        {
            int[] indices = new int[PrimitiveCount * 3];

            int index = 0;

            for (int i = 0; i < indexBuffers.Length; i++)
            {
                if (indexBuffers[i].Description.Format == SlimDX.Direct3D9.Format.Index16)
                {
                    ushort* isrc = (ushort*)indexBuffers[i].Lock(0, 0, LockFlags.ReadOnly).DataPointer.ToPointer();

                    for (int j = 0; j < partPrimCount[i]; j++)
                    {
                        int iPos = j * 3;

                        indices[index++] = isrc[iPos];
                        indices[index++] = isrc[iPos + 1];
                        indices[index++] = isrc[iPos + 2];

                        //faces[faceIdx++] = new MeshFace(isrc[iPos], isrc[iPos + 1], isrc[iPos + 2], i);
                    }

                    indexBuffers[i].Unlock();
                }
                else
                {
                    int* isrc = (int*)indexBuffers[i].Lock(0, 0, LockFlags.ReadOnly).DataPointer.ToPointer();

                    for (int j = 0; j < partPrimCount[i]; j++)
                    {
                        int iPos = j * 3;

                        indices[index++] = isrc[iPos];
                        indices[index++] = isrc[iPos + 1];
                        indices[index++] = isrc[iPos + 2];
                    }

                    indexBuffers[i].Unlock();
                }
            }
            return indices;
        }

        #endregion

        #region 方法

        public void ProcessAllTriangles(IMeshTriangleCallBack cbk)
        {
            int vertexCount = VertexCount;
            int vertexSize = VertexSize;
            int[] indices = GetIndices();

            VertexBuffer vb = VertexBuffer;

            byte* src = (byte*)vb.Lock(0, 0, LockFlags.ReadOnly).DataPointer.ToPointer();

            for (int i = 0; i < indices.Length; i += 3)
            {
                cbk.Process(*((Vector3*)(src + vertexSize * indices[i])),
                            *((Vector3*)(src + vertexSize * indices[i + 1])),
                            *((Vector3*)(src + vertexSize * indices[i + 2])));
            }

            vb.Unlock();
        }

        public int CalculateSizeInBytes()
        {
            int size = vertexBuffer.Description.SizeInBytes;

            for (int i = 0; i < indexBuffers.Length; i++)
            {
                size += indexBuffers[i].Description.SizeInBytes;
            }

            return size + 256;
        }

        /// <summary>
        ///  从网格数据创建网格
        /// </summary>
        /// <param name="dev"></param>
        /// <param name="data">网格数据</param>
        public unsafe void BuildFromData(Device dev, MeshData data)
        {
            this.name = data.Name;

            this.materials = data.Materials;
            this.matAnims = data.MaterialAnimation;

            int matCount = data.Materials.Length;
            int vertexCount = data.VertexCount;
            int faceCount = data.Faces.Length;

            this.primCount = faceCount;
            this.vertexCount = vertexCount;

            this.vertexSize = data.VertexSize;

            this.vtxDecl = new VertexDeclaration(dev, data.VertexElements);

            this.vertexBuffer = new VertexBuffer(dev, vertexSize * vertexCount, Usage.None, data.Format, Pool.Managed);

            void* vdst = vertexBuffer.Lock(0, 0, LockFlags.None).DataPointer.ToPointer();

            Memory.Copy(data.Data.ToPointer(), vdst, vertexSize * vertexCount);

            vertexBuffer.Unlock();

            bool useIndex16 = vertexCount <= ushort.MaxValue;

            List<int>[] indices = new List<int>[matCount];
            for (int i = 0; i < matCount; i++)
            {
                indices[i] = new List<int>();
            }

            partPrimCount = new int[matCount];
            partVtxCount = new int[matCount];

            MeshFace[] faces = data.Faces;
            for (int i = 0; i < faces.Length; i++)
            {
                MeshFace face = faces[i];
                int matId = face.MaterialIndex;

                indices[matId].Add(face.IndexA);
                indices[matId].Add(face.IndexB);
                indices[matId].Add(face.IndexC);
                partPrimCount[matId]++;
            }

            bool[] passed = new bool[vertexCount];
            if (useIndex16)
            {
                indexBuffers = new IndexBuffer[matCount];

                for (int i = 0; i < matCount; i++)
                {
                    fixed (void* dst = &passed[0])
                    {
                        Memory.Zero(dst, vertexCount);
                    }

                    List<int> idx = indices[i];
                    indexBuffers[i] = new IndexBuffer(dev, idx.Count * sizeof(ushort), Usage.None, Pool.Managed, true);

                    ushort* ib = (ushort*)indexBuffers[i].Lock(0, 0, LockFlags.None).DataPointer.ToPointer();
                    for (int j = 0; j < idx.Count; j++)
                    {
                        ib[j] = (ushort)idx[j];
                        passed[idx[j]] = true;
                    }
                    indexBuffers[i].Unlock();


                    int vtxCount = 0;
                    for (int j = 0; j < vertexCount; j++)
                        if (passed[j])
                            vtxCount++;
                    partVtxCount[i] = vtxCount;
                }
            }
            else
            {
                indexBuffers = new IndexBuffer[matCount];

                for (int i = 0; i < matCount; i++)
                {
                    fixed (void* dst = &passed[0])
                    {
                        Memory.Zero(dst, vertexCount);
                    }
                    List<int> idx = indices[i];
                    indexBuffers[i] = new IndexBuffer(dev, idx.Count * sizeof(uint), Usage.None, Pool.Managed, false);

                    uint* ib = (uint*)indexBuffers[i].Lock(0, 0, LockFlags.None).DataPointer.ToPointer();
                    for (int j = 0; j < idx.Count; j++)
                    {
                        ib[j] = (uint)idx[j];
                    }
                    indexBuffers[i].Unlock();

                    int vtxCount = 0;
                    for (int j = 0; j < vertexCount; j++)
                        if (passed[j])
                            vtxCount++;
                    partVtxCount[i] = vtxCount;
                }
            }

        }

        #endregion

        #region 静态方法

        /// <summary>
        ///  从流创建一个网格(Mesh)
        /// </summary>
        /// <param name="dev"></param>
        /// <param name="stm"></param>
        /// <returns></returns>
        public static GameMesh FromStream(Device dev, Stream stm)
        {
            MeshData data = new MeshData(dev);
            data.Load(stm);
            return new GameMesh(dev, data);
        }

        /// <summary>
        ///  将网格(Mesh)保存到流中
        /// </summary>
        /// <param name="mesh">网格</param>
        /// <param name="stm">要保存到的流</param>
        public static void ToStream(GameMesh mesh, Stream stm)
        {
            MeshData data = new MeshData(mesh);
            data.Save(stm);
        }

        #endregion

        #region 构造函数

        /// <summary>
        /// 引用mesh的数据，但重新创建动画实例
        /// </summary>
        /// <param name="rs"></param>
        /// <param name="mesh"></param>
        public GameMesh(Device dev, GameMesh mesh)
        {
            this.dev = dev;
            this.disposed = mesh.disposed;
            this.indexBuffers = mesh.indexBuffers;
            this.materials = mesh.materials;
            this.name = mesh.name;
            this.partPrimCount = mesh.partPrimCount;
            this.partVtxCount = mesh.partVtxCount;
            this.bufferedOp = mesh.bufferedOp;
            this.vtxFormat = mesh.Format;

            this.vertexBuffer = mesh.vertexBuffer;
            this.vertexSize = mesh.vertexSize;
            this.vtxDecl = mesh.vtxDecl;

            this.primCount = mesh.primCount;
            this.vertexCount = mesh.vertexCount;
            this.matAnims = new MaterialAnimationInstance[mesh.matAnims.Length];

            for (int i = 0; i < matAnims.Length; i++)
            {
                matAnims[i] = new MaterialAnimationInstance(mesh.matAnims[i].Data);
            }
        }
        /// <summary>
        ///  从网格数据创建网格
        /// </summary>
        /// <param name="dev"></param>
        /// <param name="data"></param>
        public GameMesh(Device dev, MeshData data)
        {
            this.dev = dev;

            BuildFromData(dev, data);
        }

        public GameMesh(Device dev, VertexPNT1[] vertices, int[] indices, MeshMaterial[][] materials)
        {
            this.dev = dev;
            this.vtxFormat = VertexPNT1.Format;
            this.materials = materials;

            this.matAnims = new MaterialAnimationInstance[materials.Length];

            this.vtxDecl = new VertexDeclaration(dev, D3DX.DeclaratorFromFVF(VertexPNT1.Format));

            vertexSize = sizeof(VertexPNT1);
            int vbSize = vertexSize * vertices.Length;

            vertexBuffer = new VertexBuffer(dev, vbSize, Usage.None, VertexPNT1.Format, Pool.Managed);

            void* vdst = vertexBuffer.Lock(0, 0, LockFlags.None).DataPointer.ToPointer();

            fixed (VertexPNT1* src = &vertices[0])
            {
                Memory.Copy(src, vdst, vbSize);
            }

            vertexBuffer.Unlock();


            int ibSize = sizeof(uint) * indices.Length;

            indexBuffers = new IndexBuffer[1];
            indexBuffers[0] = new IndexBuffer(dev, ibSize, Usage.None, Pool.Managed, false);

            void* idst = indexBuffers[0].Lock(0, 0, LockFlags.None).DataPointer.ToPointer();

            indexBuffers[0].Lock(0, 0, LockFlags.None);

            fixed (int* src = &indices[0])
            {
                Memory.Copy(src, idst, ibSize);
            }

            indexBuffers[0].Unlock();

            partPrimCount = new int[materials.Length];
            partVtxCount = new int[materials.Length];

            partVtxCount[0] = vertices.Length;
            partPrimCount[0] = indices.Length / 3;

            this.primCount = partPrimCount[0];
            this.vertexCount = vertices.Length;

            for (int i = 0; i < materials.Length; i++)
            {
                this.matAnims[i] = new MaterialAnimationInstance(new MaterialAnimation(1, 1));
            }
        }

        #endregion

        #region 属性
        /// <summary>
        ///  获得网格顶点的格式的VertexFormat表达形式
        /// </summary>
        public VertexFormat Format
        {
            get { return vtxFormat; }
        }

        public int[] PartPrimitiveCount
        {
            get { return partPrimCount; }
        }

        public MaterialAnimationInstance[] MaterialAnimation
        {
            get { return matAnims; }
        }
        public MeshMaterial[][] Materials
        {
            get { return materials; }
        }

        public int VertexSize
        {
            get { return vertexSize; }
        }

        public int VertexCount
        {
            get { return vertexCount; }
        }
        public int PrimitiveCount
        {
            get { return primCount; }
        }

        /// <summary>
        ///  获取或设置网格的名称
        /// </summary>
        public string Name
        {
            get { return name; }
            set { name = value; }
        }

        public Device Device
        {
            get { return dev; }
            set { dev = value; }
        }

        #endregion

        #region IRenderable 成员

        public RenderOperation[] GetRenderOperation()
        {
            if (bufferedOp == null)
            {
                //bufferedGm = new GeomentryData[materials.Length];
                bufferedOp = new RenderOperation[materials.Length];
                for (int i = 0; i < bufferedOp.Length; i++)
                {
                    GeomentryData gd = new GeomentryData(this);
                    gd.Format = vtxFormat;
                    //bufferedGm[i].Material = materials[i];
                    gd.IndexBuffer = indexBuffers[i];
                    gd.PrimCount = partPrimCount[i];
                    gd.PrimitiveType = PrimitiveType.TriangleList;
                    gd.VertexBuffer = vertexBuffer;
                    gd.VertexCount = partVtxCount[i];
                    gd.VertexDeclaration = vtxDecl;
                    gd.VertexSize = vertexSize;

                    bufferedOp[i].Material = materials[i][matAnims[i].CurrentFrame];
                    bufferedOp[i].Geomentry = gd;
                    //bufferedOp[i].Transformation 
                }
            }
            return bufferedOp;
        }

        #endregion

        #region IDisposable 成员

        public bool Disposed
        {
            get { return disposed; }
        }

        public void Dispose()
        {
            if (!disposed)
            {
                bufferedOp = null;
                if (vertexBuffer != null && !vertexBuffer.Disposed)
                {
                    vertexBuffer.Dispose();
                    vertexBuffer = null;
                }
                if (indexBuffers != null)
                {
                    for (int i = 0; i < indexBuffers.Length; i++)
                    {
                        if (!indexBuffers[i].Disposed)
                        {
                            indexBuffers[i].Dispose();
                        }
                    }
                    indexBuffers = null;
                }
                if (vtxDecl != null && !vtxDecl.Disposed)
                    vtxDecl.Dispose();


                for (int i = 0; i < materials.Length; i++)
                {
                    for (int j = 0; j < materials[i].Length; j++)
                    {
                        if (materials[i][j] != MeshMaterial.DefaultMaterial)
                        {
                            if (!materials[i][j].Disposed)
                            {
                                materials[i][j].Dispose();
                            }
                        }
                    }
                }
                disposed = true;
            }
            else
                throw new ObjectDisposedException(ToString());
        }

        #endregion

        #region IUpdatable 成员

        public void Update(float dt)
        {
            for (int i = 0; i < matAnims.Length; i++)
            {
                matAnims[i].Update(dt);
            }
        }

        #endregion

        ~GameMesh()
        {
            if (!disposed)
                Dispose();
        }

    }
}
