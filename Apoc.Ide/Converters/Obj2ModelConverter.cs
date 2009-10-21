using System;
using System.Collections.Generic;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;
using System.Windows.Forms;
using VirtualBicycle.Ide.Editors.EditableObjects;
using SlimDX;
using SlimDX.Direct3D9;
using VirtualBicycle.Graphics;
using VirtualBicycle.MathLib;
using VirtualBicycle.IO;
using VirtualBicycle.Ide.Designers;

namespace VirtualBicycle.Ide.Converters
{
    [Obsolete ()]
    class Obj2ModelConverter : ConverterBase
    {
        class ObjFace
        {
            public bool bHasNormal;
            public Vector3 CacNormal;

            public string sAssignedMat;
            public int iAssignedMatIdx;

            public int iNormalA, iNormalB, iNormalC;

            public int iVertexA, iVertexB, iVertexC;

            public int iTexCoordA, iTexCoordB, iTexCoordC;

            public ObjFace()
            {
                iTexCoordA = 1; iTexCoordB = 1; iTexCoordC = 1;
            }
        }
        class ObjMaterial : EditableMeshMaterial
        {
            public ObjMaterial()
            { }

            public string sName;
            //public string sTextureFile;
            public Vector3 vTiling;
            public bool bHasTexture;

            public List<int> iDervedFaces;

        }
        enum FaceVertex
        {
            A, B, C
        }

        sealed class ObjMesh
        {

            const string VertexTag = "v";
            const string TexCoordTag = "vt";
            const string NormalTag = "vn";
            const string GruopTag = "g";
            const string FaceTag = "f";
            const string MaterialTag = "usemtl";
            const string MtlTag = "mtllib";
            const string Comment = "#";

            const string Group = "g";


            const string NewMaterialTag = "newmtl";
            const string AmbientTag = "ka";
            const string DiffuseTag = "kd";
            const string SpecularTag = "ks";
            //const string IllumTag = "illum";
            const string ShininessTag = "ns";
            const string MapKaTag = "map_ka";
            const string MapKdTag = "map_kd";
            const string MapKsTag = "map_ks";
            const string MapBumpTag = "map_bump";
            //const string DissolveTag = "d";
            //const string RefractionTag = "Ni";


            List<Vector3> vVertex = new List<Vector3>();
            List<Vector3> vNormal = new List<Vector3>();
            List<Vector2> vTexCoords = new List<Vector2>();
            List<ObjFace> fFaces = new List<ObjFace>();
            List<ObjMaterial> mMaterial = new List<ObjMaterial>();
            List<int> iUngroupedMatFace = new List<int>();

            string name;

            string sFilePath;
            string sCurMaterial = "";

            public ObjMesh(string name, string path)
            {
                this.name = name;
                sFilePath = path;
            }

            public void ReadLine(string[] arg)
            {
                string pf = arg[0];
                switch (pf)
                {
                    case VertexTag:
                        vVertex.Add(new Vector3(float.Parse(arg[1]),
                                                float.Parse(arg[2]),
                                                float.Parse(arg[3])));
                        break;
                    case TexCoordTag:
                        vTexCoords.Add(new Vector2(float.Parse(arg[1]),
                                                   float.Parse(arg[2])));
                        break;
                    case NormalTag:
                        Vector3 n = new Vector3(float.Parse(arg[1]),
                                                float.Parse(arg[2]),
                                                float.Parse(arg[3]));
                        n.Normalize();
                        vNormal.Add(n);

                        break;
                    case FaceTag:
                        fFaces.Add(new ObjFace());
                        int c = fFaces.Count - 1;
                        AddFace(arg[1], c, FaceVertex.A);
                        AddFace(arg[2], c, FaceVertex.B);
                        AddFace(arg[3], c, FaceVertex.C);
                        fFaces[c].sAssignedMat = sCurMaterial;// a[3];

                        break;
                    case MtlTag:
                        LoadMtl(Path.Combine(sFilePath, arg[1]));
                        break;
                    case MaterialTag:
                        sCurMaterial = arg[1];
                        break;

                }
            }
            private void AddFace(string part, int faceID, FaceVertex v)
            {
                int[] numArray = new int[3];
                string[] textArray = part.Split('/');
                int upperBound = textArray.GetUpperBound(0);
                for (int i = 0; i <= upperBound; i++)
                {
                    if (textArray[i].Length > 0)
                    {
                        numArray[i] = int.Parse(textArray[i]);
                    }
                }
                if (!part.Contains("//"))
                {
                    if (textArray.GetUpperBound(0) == 1)
                    {
                        fFaces[faceID].bHasNormal = false;
                        switch (v)
                        {
                            case FaceVertex.A:
                                fFaces[faceID].iVertexA = numArray[0];
                                fFaces[faceID].iTexCoordA = numArray[1];
                                return;

                            case FaceVertex.B:
                                fFaces[faceID].iVertexB = numArray[0];
                                fFaces[faceID].iTexCoordB = numArray[1];
                                return;

                            case FaceVertex.C:
                                fFaces[faceID].iVertexC = numArray[0];
                                fFaces[faceID].iTexCoordC = numArray[1];
                                return;
                        }
                        return;
                    }
                    if (!part.Contains("/"))
                    {
                        switch (v)
                        {
                            case FaceVertex.A:
                                fFaces[faceID].iVertexA = numArray[0];
                                fFaces[faceID].bHasNormal = false;
                                return;

                            case FaceVertex.B:
                                fFaces[faceID].iVertexB = numArray[0];
                                fFaces[faceID].bHasNormal = false;
                                return;

                            case FaceVertex.C:
                                fFaces[faceID].iVertexC = numArray[0];
                                fFaces[faceID].bHasNormal = false;
                                return;
                        }
                        return;
                    }

                    switch (v)
                    {
                        case FaceVertex.A:
                            fFaces[faceID].iVertexA = numArray[0];
                            fFaces[faceID].iTexCoordA = numArray[1];
                            fFaces[faceID].iNormalA = numArray[2];
                            fFaces[faceID].bHasNormal = true;
                            return;

                        case FaceVertex.B:
                            fFaces[faceID].iVertexB = numArray[0];
                            fFaces[faceID].iTexCoordB = numArray[1];
                            fFaces[faceID].iNormalB = numArray[2];
                            fFaces[faceID].bHasNormal = true;
                            return;

                        case FaceVertex.C:
                            fFaces[faceID].iVertexC = numArray[0];
                            fFaces[faceID].iTexCoordC = numArray[1];
                            fFaces[faceID].iNormalC = numArray[2];
                            fFaces[faceID].bHasNormal = true;
                            return;
                    }
                }
                else
                {
                    fFaces[faceID].bHasNormal = true;
                    switch (v)
                    {
                        case FaceVertex.A:
                            fFaces[faceID].iVertexA = numArray[0];
                            fFaces[faceID].iNormalA = numArray[2];
                            break;

                        case FaceVertex.B:
                            fFaces[faceID].iVertexB = numArray[0];
                            fFaces[faceID].iNormalB = numArray[2];
                            break;

                        case FaceVertex.C:
                            fFaces[faceID].iVertexC = numArray[0];
                            fFaces[faceID].iNormalC = numArray[2];
                            break;
                    }
                    return;
                }
            }

            private void LoadMtl(string file)
            {
                FileStream fs = new FileStream(file, FileMode.Open, FileAccess.Read, FileShare.Read);
                StreamReader sr = new StreamReader(file, Encoding.Default);

                while (!sr.EndOfStream)
                {
                    string line = sr.ReadLine().Trim();
                    if (line.Length > 0)
                    {
                        ReadMtlLine(GetObjLineArg(line));
                    }
                }
                sr.Close();
                sr.Dispose();
            }
            private void ReadMtlLine(string[] args)
            {
                string pf = args[0];
                int c = mMaterial.Count - 1;

                switch (pf)
                {
                    case NewMaterialTag:
                        mMaterial.Add(new ObjMaterial());
                        mMaterial[mMaterial.Count - 1].sName = args[1];
                        break;
                    case AmbientTag:
                        mMaterial[c].Ambient = new Color4(float.Parse(args[1]), float.Parse(args[2]), float.Parse(args[3]));
                        break;
                    case DiffuseTag:
                        mMaterial[c].Diffuse = new Color4(float.Parse(args[1]), float.Parse(args[2]), float.Parse(args[3]));
                        break;
                    case SpecularTag:
                        mMaterial[c].Specular = new Color4(float.Parse(args[1]), float.Parse(args[2]), float.Parse(args[3]));
                        break;
                    case ShininessTag:
                        float s = float.Parse(args[1]);
                        mMaterial[c].Power = s * 1.2812812812812812812812812812813f;
                        break;
                    case MapKdTag:
                        if (args[1] == "-s")
                        {
                            mMaterial[c].vTiling = new Vector3(float.Parse(args[2]),
                                                       float.Parse(args[3]),
                                                       float.Parse(args[4]));
                            mMaterial[c].TextureFile1 = args[5];
                            mMaterial[c].bHasTexture = true;
                        }
                        else
                        {
                            mMaterial[c].vTiling = new Vector3(1, 1, 1);
                            mMaterial[c].TextureFile1 = args[1];
                            mMaterial[c].bHasTexture = true;
                        }

                        break;
                    case MapKsTag:
                        break;
                }

            }

            public VertexFormat Format
            {
                get { return VertexPNT1.Format; }
            }

            public string Name
            {
                get { return name; }
            }
            public List<Vector3> Positions
            {
                get { return vVertex; }
            }
            public List<Vector3> Normals
            {
                get { return vNormal; }
            }
            public List<Vector2> TextureCoords
            {
                get { return vTexCoords; }
            }
            public List<ObjFace> Faces
            {
                get { return fFaces; }
            }
            public List<ObjMaterial> Materials
            {
                get { return mMaterial; }
            }
            public List<int> UngroupedFaces
            {
                get { return iUngroupedMatFace; }
            }

            public void PostProcess(bool flatNormals)
            {
                int iMatCount = mMaterial.Count;


                for (int i = 0; i < fFaces.Count; i++)
                {
                    if (!flatNormals)
                    {
                        if (!fFaces[i].bHasNormal)
                        {
                            fFaces[i].CacNormal = MathEx.ComputePlaneNormal(vVertex[fFaces[i].iVertexA - 1], vVertex[fFaces[i].iVertexB - 1], vVertex[fFaces[i].iVertexC - 1]);
                        }
                    }
                    else
                    {
                        fFaces[i].CacNormal = MathEx.ComputePlaneNormal(vVertex[fFaces[i].iVertexA - 1], vVertex[fFaces[i].iVertexB - 1], vVertex[fFaces[i].iVertexC - 1]);
                        fFaces[i].bHasNormal = false;

                    }
                    if (fFaces[i].sAssignedMat.Length > 0)
                    {
                        for (int j = 0; j < iMatCount; j++)
                        {
                            if (fFaces[i].sAssignedMat == mMaterial[j].sName)
                            {
                                fFaces[i].iAssignedMatIdx = j;
                                if (mMaterial[j].iDervedFaces == null)
                                    mMaterial[j].iDervedFaces = new List<int>();
                                mMaterial[j].iDervedFaces.Add(i);
                                break;
                            }
                        }
                    }
                    else
                    {
                        iUngroupedMatFace.Add(i);
                    }
                }
            }

        }
        sealed class ObjReader
        {
            List<ObjMesh> meshes = new List<ObjMesh>();

            string sFile;
            string sFilePath;

            bool flatNormals;

            public ObjReader(ResourceLocation rl)
            {
                FileLocation fl = rl as FileLocation;
                if (fl != null)
                {
                    sFile = fl.Path;
                }
                else
                {
                    throw new NotSupportedException(rl.GetType().ToString());
                }
            }

            public void Load()
            {
                sFilePath = Path.GetDirectoryName(sFile);
                StreamReader sr = new StreamReader(sFile, Encoding.Default);

                ObjMesh commonData = new ObjMesh(string.Empty, sFilePath);
                meshes.Add(commonData);
                //ObjMesh commonData = new ObjMesh("", sFilePath);

                ObjMesh objMesh = null;
                while (!sr.EndOfStream)
                {
                    string line = sr.ReadLine().Trim();
                    //commonData.ReadLine(GetObjLineArg(line));

                    if (line.Length > 0)
                    {
                        string[] args = GetObjLineArg(line);

                        if (args[0] == "g")
                        {
                            string meshName;
                            if (args.Length > 1)
                            {
                                meshName = args[1];
                            }
                            else
                            {
                                meshName = string.Empty;
                            }

                            bool found = false;
                            for (int i = 0; i < meshes.Count; i++)
                            {
                                if (meshName == meshes[i].Name)
                                {
                                    objMesh = meshes[i];
                                    found = true;
                                    break;
                                }
                            }
                            if (!found)
                            {
                                objMesh = new ObjMesh(meshName, sFilePath);
                                objMesh.Materials.AddRange(commonData.Materials);
                                meshes.Add(objMesh);
                            }
                        }
                        else
                        {
                            if (objMesh == null)
                            {
                                objMesh = commonData;
                            }
                            objMesh.ReadLine(args);
                        }
                    }
                }

                for (int i = 0; i < meshes.Count; i++)
                {
                    if (meshes[i] != commonData)
                    {
                        if (meshes[i].Positions.Count == 0)
                        {
                            meshes[i].Positions.AddRange(commonData.Positions);
                        }
                        if (meshes[i].Normals.Count == 0)
                        {
                            meshes[i].Normals.AddRange(commonData.Normals);
                        }
                        if (meshes[i].TextureCoords.Count == 0)
                        {
                            meshes[i].TextureCoords.AddRange(commonData.TextureCoords);
                        }
                        meshes[i].PostProcess(flatNormals);
                    }
                }
                sr.Close();
                sr.Dispose();
            }

            public int EntityCount
            {
                get { return meshes.Count; }
            }
            public List<ObjMesh> Entities
            {
                get { return meshes; }
            }
            public bool FlatNormals
            {
                get { return flatNormals; }
                set { flatNormals = value; }
            }
        }
        //sealed class SortedFaceVertex
        //{
        //    public ObjFace face;
        //    public FaceVertex vtx;
        //    public string vtxDesc;

        //}

        bool flatNormals = true;



        public bool FlatNormals
        {
            get { return flatNormals; }
            set { flatNormals = value; }
        }

        static string[] GetObjLineArg(string line)
        {
            return line.ToLower().Split((char[])null, StringSplitOptions.RemoveEmptyEntries);
        }

        public override void ShowDialog(object sender, EventArgs e)
        {
            string[] files;
            string path;
            if (ConvDlg.Show("", GetOpenFilter(), out files, out path) == DialogResult.OK)
            {
                ProgressDlg pd = new ProgressDlg(DevStringTable.Instance["GUI:Converting"]);

                pd.MinVal = 0;
                pd.Value = 0;
                pd.MaxVal = files.Length;

                pd.Show();
                for (int i = 0; i < files.Length; i++)
                {
                    string dest = Path.Combine(path, Path.GetFileNameWithoutExtension(files[i]) + ".mesh");

                    Convert(new DevFileLocation(files[i]), new DevFileLocation(dest));
                    pd.Value = i;
                }
                pd.Close();
                pd.Dispose();
            }
        }

        //string GetFaceDesc(ObjFace face, FaceVertex vtx)
        //{
        //    StringBuilder sb = new StringBuilder();
        //    if (vtx == FaceVertex.A)
        //    {
        //        sb.Append(face.iVertexA.ToString());
        //        sb.Append('-');
        //        sb.Append(face.iTexCoordA.ToString());
        //        if (face.bHasNormal)
        //        {
        //            sb.Append('-');
        //            sb.Append(face.iNormalA);
        //        }
        //        else
        //        {
        //            sb.Append('-');
        //            sb.Append(face.CacNormal.GetHashCode());
        //        }
        //    }
        //    else if (vtx == FaceVertex.B)
        //    {
        //        sb.Append(face.iVertexB.ToString());
        //        sb.Append('-');
        //        sb.Append(face.iTexCoordB.ToString());
        //        if (face.bHasNormal)
        //        {
        //            sb.Append('-');
        //            sb.Append(face.iNormalB);
        //        }
        //        else
        //        {
        //            sb.Append('-');
        //            sb.Append(face.CacNormal.GetHashCode());
        //        }
        //    }
        //    else
        //    {
        //        sb.Append(face.iVertexC.ToString());
        //        sb.Append('-');
        //        sb.Append(face.iTexCoordC.ToString());
        //        if (face.bHasNormal)
        //        {
        //            sb.Append('-');
        //            sb.Append(face.iNormalC);
        //        }
        //        else
        //        {
        //            sb.Append('-');
        //            sb.Append(face.CacNormal.GetHashCode());
        //        }
        //    }
        //    return sb.ToString();
        //}

        //int FaceVertexComparison(SortedFaceVertex a, SortedFaceVertex b)
        //{
        //    return a.vtxDesc.CompareTo(b.vtxDesc);
        //}

        public unsafe override void Convert(ResourceLocation source, ResourceLocation dest)
        {
            ObjReader obj = new ObjReader(source);
            obj.FlatNormals = flatNormals;
            obj.Load();

            List<ObjMesh> meshes = obj.Entities;

            EditableModel model = new EditableModel();
            model.Entities = new EditableMesh[meshes.Count];

            int i = 0;
            for (int k = 0; k < meshes.Count; k++)
            {
                ObjMesh mesh = meshes[i];
                if (mesh.Name.Length > 0)
                {
                    List<ObjFace> objFaces = mesh.Faces;

                    VertexPNT1[] vertices = new VertexPNT1[objFaces.Count * 3];
                    MeshFace[] faces = new MeshFace[objFaces.Count];

                    ObjMaterial[] mats = mesh.Materials.ToArray();

                    for (int j = 0; j < objFaces.Count; j++)
                    {
                        ObjFace face = objFaces[j];
                        int j3 = j * 3;

                        int matIdx = face.iAssignedMatIdx;
                        faces[j] = new MeshFace(j3, j3 + 1, j3 + 2);
                        faces[j].MaterialIndex = matIdx;

                        vertices[j3].pos = mesh.Positions[face.iVertexA - 1];
                        vertices[j3 + 1].pos = mesh.Positions[face.iVertexB - 1];
                        vertices[j3 + 2].pos = mesh.Positions[face.iVertexC - 1];

                        if (face.bHasNormal)
                        {
                            vertices[j3].n = mesh.Normals[face.iNormalA - 1];
                            vertices[j3 + 1].n = mesh.Normals[face.iNormalB - 1];
                            vertices[j3 + 2].n = mesh.Normals[face.iNormalC - 1];
                        }
                        else
                        {
                            vertices[j3].n = face.CacNormal;
                            vertices[j3 + 1].n = face.CacNormal;
                            vertices[j3 + 2].n = face.CacNormal;
                        }

                        Vector3 tilling = mats[matIdx].vTiling;

                        Vector2 texCoord = mesh.TextureCoords[face.iTexCoordA - 1];
                        texCoord.X *= tilling.X;
                        texCoord.Y *= tilling.Y;

                        vertices[j3].u = texCoord.X;
                        vertices[j3].v = texCoord.Y;


                        texCoord = mesh.TextureCoords[face.iTexCoordB - 1];
                        texCoord.X *= tilling.X;
                        texCoord.Y *= tilling.Y;
                        vertices[j3 + 1].u = texCoord.X;
                        vertices[j3 + 1].v = texCoord.Y;

                        texCoord = mesh.TextureCoords[face.iTexCoordC - 1];
                        texCoord.X *= tilling.X;
                        texCoord.Y *= tilling.Y;
                        vertices[j3 + 2].u = texCoord.X;
                        vertices[j3 + 2].v = texCoord.Y;
                    }

                    EditableMesh data = new EditableMesh();

                    fixed (VertexPNT1* src = &vertices[0])
                    {
                        data.SetData(src, vertices.Length * VertexPNT1.Size);
                    }

                    data.Faces = faces;
                    data.Materials = new EditableMeshMaterial[mesh.Materials.Count][];
                    for (int j = 0; j < data.Materials.Length; j++)
                    {
                        data.Materials[j] = new EditableMeshMaterial[] { mesh.Materials[j] };
                    }
                    data.SetVertexFormat(VertexPNT1.Format);

                    model.Entities[i++] = data;
                }
            }
            //for (int i = 0; i < meshes.Count; i++)
            //{
            //    ObjMesh mesh = meshes[i];
            //    if (mesh.Name.Length > 0)
            //    {
            //        //列出所有顶点并排序
            //        List<SortedFaceVertex> faceVtx = new List<SortedFaceVertex>();
            //        for (int j = 0; j < mesh.Faces.Count; j++)
            //        {
            //            ObjFace face = mesh.Faces[j];

            //            SortedFaceVertex vtx;
            //            vtx = new SortedFaceVertex();
            //            vtx.vtxDesc = GetFaceDesc(face, FaceVertex.A);
            //            vtx.vtx = FaceVertex.A;
            //            vtx.face = face;
            //            faceVtx.Add(vtx);

            //            vtx = new SortedFaceVertex();
            //            vtx.vtxDesc = GetFaceDesc(face, FaceVertex.B);
            //            vtx.vtx = FaceVertex.B;
            //            vtx.face = face;
            //            faceVtx.Add(vtx);

            //            vtx = new SortedFaceVertex();
            //            vtx.vtxDesc = GetFaceDesc(face, FaceVertex.C);
            //            vtx.vtx = FaceVertex.C;
            //            vtx.face = face;
            //            faceVtx.Add(vtx);
            //        }
            //        faceVtx.Sort(FaceVertexComparison);

            //        List<SortedFaceVertex> cleaned = new List<SortedFaceVertex>();
            ////剔除相同顶点
            //        SortedFaceVertex lastItem = null;
            //        for (int j = 0; j < faceVtx.Count; j++)
            //        {
            //            if (lastItem == null)
            //            {
            //                lastItem = faceVtx[j];
            //            }
            //            else
            //            {
            //                if (faceVtx[j].vtxDesc != lastItem.vtxDesc)
            //                {
            //                    cleaned.Add(lastItem);
            //                    lastItem = faceVtx[j];
            //                }
            //            }
            //        }
            //        cleaned.Add(lastItem);

            //        Vector3[] pos = new Vector3[cleaned.Count];
            //        Vector3[] n = new Vector3[cleaned.Count];
            //        Vector2[] texc = new Vector2[cleaned.Count];
            //        for (int j = 0; j < cleaned.Count; j++)
            //        {
            //            ObjFace face = cleaned[j].face;
            //            if (cleaned[j].vtx == FaceVertex.A)
            //            {
            //                pos[j] = mesh.Positions[face.iVertexA - 1];
            //                if (face.bHasNormal)
            //                    n[j] = mesh.Normals[face.iNormalA - 1];
            //                else
            //                    n[j] = face.CacNormal;

            //                texc[j] = mesh.TextureCoords[face.iTexCoordA - 1];

            //                face.iVertexA = j;
            //            }
            //            else if (cleaned[j].vtx == FaceVertex.B)
            //            {
            //                pos[j] = mesh.Positions[face.iVertexB - 1];
            //                if (face.bHasNormal)
            //                    n[j] = mesh.Normals[face.iNormalB - 1];
            //                else
            //                    n[j] = face.CacNormal;
            //                texc[j] = mesh.TextureCoords[face.iTexCoordB - 1];

            //                face.iVertexB = j;
            //            }
            //            else
            //            {
            //                pos[j] = mesh.Positions[face.iVertexC - 1];
            //                if (face.bHasNormal)
            //                    n[j] = mesh.Normals[face.iNormalC - 1];
            //                else
            //                    n[j] = face.CacNormal;
            //                texc[j] = mesh.TextureCoords[face.iTexCoordC - 1];

            //                face.iVertexC = j;
            //            }

            //        }

            //        MeshFace[] faces = new MeshFace[mesh.Faces.Count];
            //        for (int j = 0; j < mesh.Faces.Count; j++)
            //        {
            //            faces[j].IndexA = mesh.Faces[j].iVertexA;
            //            faces[j].IndexB = mesh.Faces[j].iVertexB;
            //            faces[j].IndexC = mesh.Faces[j].iVertexC;
            //            faces[j].MaterialIndex = mesh.Faces[j].iAssignedMatIdx;
            //        }

            //        R3D.Graphics.MeshMaterial.Data[] mats = mesh.Materials.ToArray();

            //        GameMesh.Data sounds = new GameMesh.Data();
            //        sounds.Positions = pos;
            //        sounds.Faces = faces;
            //        sounds.Normals = n;
            //        sounds.Texture1 = texc;
            //        sounds.Materials = mats;
            //        sounds.Format = StdMeshVertex.Format;
            //        model.Entities.Add(sounds);
            //    }
            //}

            ContentBinaryWriter bw = new ContentBinaryWriter(dest);
            BinaryDataWriter mdata = EditableModel.ToBinary(model);
            bw.Write(mdata);
            mdata.Dispose();
            bw.Close();
        }

        public override string Name
        {
            get { return "Obj转Mesh"; }// Program.StringTable["GUI:OBJ2MESH"]; }
        }
        public override string[] SourceExt
        {
            get { return new string[] { ".obj" }; }
        }
        public override string[] DestExt
        {
            get { return new string[] { ".mesh" }; }
        }

        public override string SourceDesc
        {
            get { throw new NotImplementedException(); }
        }

        public override string DestDesc
        {
            get { throw new NotImplementedException(); }
        }
    }
}
