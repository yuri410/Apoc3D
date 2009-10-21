using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Windows.Forms;
using System.Xml;
using SlimDX;
using SlimDX.Direct3D9;
using VirtualBicycle.Ide;
using VirtualBicycle.Ide.Converters;
using VirtualBicycle.Ide.Designers;
using VirtualBicycle.Ide.Editors.EditableObjects;
using VirtualBicycle.Collections;
using VirtualBicycle.Graphics;
using VirtualBicycle.Graphics.Animation;
using VirtualBicycle.IO;

namespace VirtualBicycle.Ide.Converters
{
    public unsafe class Xml2ModelConverter : ConverterBase
    {
        enum ModelObjectType
        {
            Dummy,
            Mesh,
            TapeHelper,
            TapeTarget
        }

        struct ModelObject
        {            
            public EditableMesh Mesh;

            public Matrix LocalTransform;

            public int ParentId;

            public string ParentName;

            public int Index;
        }
        struct TexIndex
        {
            public int a, b, c;

            public TexIndex(int a, int b, int c)
            {
                this.a = a;
                this.b = b;
                this.c = c;
            }
        }

        const string CsfKey = "GUI:Xml2Mesh";

        public override void ShowDialog(object sender, EventArgs e)
        {
            string[] files;
            string path;
            if (ConvDlg.Show(DevStringTable.Instance[CsfKey], GetOpenFilter(), out files, out path) == DialogResult.OK)
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

        Color4 ParseColor(string exp)
        {
            string[] vals = exp.Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);
            vals[3] = vals[3].Substring(0, vals[3].Length - 1);
            return new Color4(1, float.Parse(vals[1]) / 255f, float.Parse(vals[2]) / 255f, float.Parse(vals[3]) / 255f);
        }
        Vector4 ParseVector4(string v)
        {
            string[] vals = v.Split(',');
            vals[0] = vals[0].Substring(1);
            vals[3] = vals[3].Substring(0, vals[3].Length - 1);
            return new Vector4(float.Parse(vals[0]), float.Parse(vals[1]), float.Parse(vals[2]), float.Parse(vals[3]));
        }
        Vector3 ParseVector3(string v)
        {
            string[] vals = v.Split(',');
            vals[0] = vals[0].Substring(1);
            vals[2] = vals[2].Substring(0, vals[2].Length - 1);
            return new Vector3(float.Parse(vals[0]), float.Parse(vals[1]), float.Parse(vals[2]));
        }
        Vector2 ParseVector2(string v)
        {
            string[] vals = v.Split(',');
            vals[0] = vals[0].Substring(1);
            vals[1] = vals[1].Substring(0, vals[1].Length - 1);
            return new Vector2(float.Parse(vals[0]), float.Parse(vals[1]));
        }
        Vector2[] ParseVector2Array(XmlReader xml)
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
        Vector3[] ParseVector3Array(XmlReader xml)
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

        Matrix ParseMatrix(XmlReader xml)
        {
            int depth = xml.Depth;
            Matrix res = new Matrix();

            int lineId = 1;
            while (xml.Read() && xml.Depth > depth)
            {
                if (xml.IsStartElement() && !xml.IsEmptyElement)
                {
                    if (xml.Name == "Value")
                    {
                        Vector4 line = ParseVector4(xml.ReadString());
                        switch (lineId)
                        {
                            case 1:
                                res.M11 = line.X;
                                res.M12 = line.Y;
                                res.M13 = line.Z;
                                res.M14 = line.W;
                                break;
                            case 2:
                                res.M21 = line.X;
                                res.M22 = line.Y;
                                res.M23 = line.Z;
                                res.M24 = line.W;
                                break;
                            case 3:
                                res.M31 = line.X;
                                res.M32 = line.Y;
                                res.M33 = line.Z;
                                res.M34 = line.W;
                                break;
                            case 4:
                                res.M41 = line.X;
                                res.M42 = line.Y;
                                res.M43 = line.Z;
                                res.M44 = line.W;
                                break;
                        }
                        lineId++;
                    }
                }
            }
            return res;
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
                        string matId = xml.GetAttribute("MaterialID");
                        string[] val = xml.ReadString().Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries);
                        val[0] = val[0].Substring(1);
                        val[2] = val[2].Substring(0, val[2].Length - 1);

                        MeshFace face = new MeshFace(int.Parse(val[0]), int.Parse(val[1]), int.Parse(val[2]));


                        face.MaterialIndex = int.Parse(matId);

                        data.Add(face);
                    }
                }
            }
            
            return data.ToArray();
        }

        void ParseTexIndex(XmlReader xml, TexIndex[] texIdx)
        {
            int index = 0;
            int subDepth = xml.Depth;
            while (xml.Read() && xml.Depth > subDepth)
            {
                if (xml.IsStartElement() && !xml.IsEmptyElement)
                {
                    if (xml.Name == "Value")
                    {
                        //int index = int.Parse(xml.GetAttribute("Index"));
                        string[] val = xml.ReadString().Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries);
                        val[0] = val[0].Substring(1);
                        val[2] = val[2].Substring(0, val[2].Length - 1);

                        texIdx[index++] = new TexIndex(int.Parse(val[0]), int.Parse(val[1]), int.Parse(val[2]));
                    }
                }
            }
        }

        ModelObject ParseMeshData(XmlReader xml)
        {
            Vector3[] positions = null;
            Vector3[] normals = null;
            MeshFace[] faces = null;
            //Vector2[] tex1 = null;

            Vector2[] texVtx = null;
            TexIndex[] texIdx = null;

            ModelObject result;
            result.ParentId  = -1;
            result.Mesh = null;
            result.LocalTransform = Matrix.Identity;
            result.ParentName = string.Empty;
            result.Index = -1;
  
            int depth = xml.Depth;
            while (xml.Read() && xml.Depth > depth)
            {
                if (xml.IsStartElement() && !xml.IsEmptyElement)
                {
                    switch (xml.Name)
                    {
                        case "Parent":
                            result.ParentId = int.Parse(xml.GetAttribute("ID"));
                            result.ParentName = xml.GetAttribute("Name");
                            break;
                        case "WorldTM":
                            result.LocalTransform = ParseMatrix(xml);
                            break;
                        case "Vertex":
                            positions = ParseVector3Array(xml);
                            break;
                        case "VertexNormal":
                            normals = ParseVector3Array(xml);
                            for (int i = 0; i < normals.Length; i++)
                            {
                                normals[i].Normalize();
                            }
                            break;
                        case "TriIndex":
                            faces = ParseMeshFaces(xml);
                            break;
                        case "TexVertex":
                            texVtx = ParseVector2Array(xml);
                            break;
                        case "TexIndex":
                            texIdx = new TexIndex[faces.Length];

                            ParseTexIndex(xml, texIdx);
                            break;
                    }
                }
            }

            Dictionary<string, int> table = new Dictionary<string, int>(faces.Length * 3);

            FastList<VertexPNT1> vertices = new FastList<VertexPNT1>(faces.Length * 3);

            for (int i = 0; i < faces.Length; i++)
            {
                int index;
                VertexPNT1 vtx;

                vtx.pos = positions[faces[i].IndexA];
                vtx.n = normals[faces[i].IndexA];
                vtx.u = texVtx[texIdx[i].a].X;
                vtx.v = texVtx[texIdx[i].a].Y;

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

                vtx.pos = positions[faces[i].IndexB];
                vtx.n = normals[faces[i].IndexB];
                vtx.u = texVtx[texIdx[i].b].X;
                vtx.v = texVtx[texIdx[i].b].Y;

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
                vtx.u = texVtx[texIdx[i].c].X;
                vtx.v = texVtx[texIdx[i].c].Y;

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


            EditableMesh data = new EditableMesh();

            data.Faces = faces;
            data.SetVertexFormat(VertexPNT1.Format);
            data.VertexSize = VertexPNT1.Size;
            data.VertexCount = vertices.Count;

            fixed (VertexPNT1* src = &vertices.Elements[0])
            {
                data.SetData(src, VertexPNT1.Size * vertices.Count);
            }

            result.Mesh = data;

            return result;
        }

        Vector3 ParseTapeTarget(XmlReader xml)
        {
            Matrix worldTrans = Matrix.Identity;

            int depth = xml.Depth;
            while (xml.Read() && xml.Depth > depth)
            {
                if (xml.IsStartElement() && !xml.IsEmptyElement)
                {
                    if (xml.Name == "WorldTM")
                    {
                        worldTrans = ParseMatrix(xml);
                    }
                }
            }

            Vector3 position = Vector3.Zero;
            Vector4 pos = Vector3.Transform(position, worldTrans);
            position.X = pos.X;
            position.Y = pos.Y;
            position.Z = pos.Z;

            return position;
        }

        EditableMeshMaterial ParseMaterial(XmlReader xml, ResourceLocation dest, string srcPath)
        {
            float opacity = 1;
            EditableMeshMaterial mat = new EditableMeshMaterial();
            int depth = xml.Depth;

            Color4 diffuse = new Color4();
            Color4 ambient = new Color4();
            Color4 specular = new Color4();
            Color4 emissive = new Color4();


            while (xml.Read() && xml.Depth > depth)
            {
                if (xml.IsStartElement() && !xml.IsEmptyElement)
                {
                    switch (xml.Name)
                    {
                        case "Diffuse":
                            diffuse = ParseColor(xml.ReadString());
                            break;
                        case "Ambient":
                            ambient = ParseColor(xml.ReadString());
                            break;
                        case "Specular":
                            specular = ParseColor(xml.ReadString());
                            break;
                        case "Emissive":
                            emissive = ParseColor(xml.ReadString());
                            break;
                        case "Opacity":
                            opacity = float.Parse(xml.ReadString()) / 100f;
                            break;
                        case "Power":
                            mat.Power = float.Parse(xml.ReadString());
                            break;
                        case "DiffuseMap":
                            string texFile = xml.ReadString();
                            string texFileName = Path.GetFileName(texFile);
                            mat.TextureFile1 = texFileName;

                            FileLocation fl = dest as FileLocation;
                            if (fl != null)
                            {
                                string dstTex = Path.Combine(Path.GetDirectoryName(fl.Path), texFileName);
                                if (!File.Exists(dstTex) && File.Exists(texFile))
                                {
                                    File.Copy(Path.Combine(srcPath, texFile), dstTex, false);
                                }
                            }
                            break;

                    }
                }

            }
            mat.IsTransparent = opacity < 1;
            ambient.Alpha = opacity;
            diffuse.Alpha = opacity;
            emissive.Alpha = opacity;
            specular.Alpha = opacity;

            mat.Ambient = ambient;
            mat.Diffuse = diffuse;
            mat.Emissive = emissive;
            mat.Specular = specular;
            

            return mat;
        }

        public override void Convert(ResourceLocation source, ResourceLocation dest)
        {
            XmlReader xml = XmlReader.Create(source.GetStream);

            EditableMesh[] entities = null;
            ModelObject[] modelObjects = null;

            List<EditableMeshMaterial> materials = new List<EditableMeshMaterial>();
            List<TapeHelper> targetHelpers = new List<TapeHelper>();

            xml.Read();

            string srcPath = Path.GetDirectoryName(((FileLocation)source).Path);

            int depth;
            int index = 0;

            while (xml.Read())
            {
                if (xml.IsStartElement() && !xml.IsEmptyElement)
                {
                    switch (xml.Name)
                    {
                        case "Info":
                            depth = xml.Depth;
                            while (xml.Read() && xml.Depth > depth)
                            {
                                if (xml.IsStartElement() && !xml.IsEmptyElement)
                                {
                                    if (xml.Name == "MeshCount")
                                    {
                                        int meshCount = int.Parse(xml.ReadString());
                                        entities = new EditableMesh[meshCount];
                                        modelObjects = new ModelObject[meshCount];
                                    }
                                }
                            }
                            break;
                        case "Material":
                            //XmlReader xmlMats = xml.ReadSubtree();
                            depth = xml.Depth;
                            while (xml.Read() && xml.Depth > depth)
                            {
                                if (xml.IsStartElement() && !xml.IsEmptyElement)
                                {
                                    if (xml.Name == "Slot")
                                    {
                                        materials.Add(ParseMaterial(xml, dest, srcPath));
                                    }
                                }
                            }

                            EditableMeshMaterial defMat = new EditableMeshMaterial();
                            defMat.D3DMaterial = MeshMaterial.DefaultMatColor;
                            materials.Add(defMat);
                            //materialArray = materials.ToArray();
                            //xmlMats.Close();
                            break;
                        case "Object":
                            string objName = xml.GetAttribute("Name");
                            string objClass = xml.GetAttribute("Class");
                            int oindex = int.Parse(xml.GetAttribute("ID"));

                            switch (objClass)
                            {
                                case "Editable_mesh":
                                    modelObjects[index] = ParseMeshData(xml);

                                    modelObjects[index].Index = oindex;

                                    entities[index] = modelObjects[index].Mesh;
                                    entities[index].Name = objName;

                                    index++;
                                    break;

                                //case "Tape":
                                //    Vector3 pos = ParseTapeTarget(xml);


                                //    break;
                                //case "Targetobject":
                                //    pos = ParseTapeTarget(xml);


                                //    break;
                                case "BoneGeometry":

                                    break;
                            }
                            break;
                    }
                }
            }

            xml.Close();

            for (int i = 0; i < entities.Length; i++)
            {
                bool[] useState = new bool[materials.Count];
                for (int j = 0; j < entities[i].Faces.Length; j++)
                {
                    int mId = entities[i].Faces[j].MaterialIndex;
                    if (mId == -1)
                    {
                        mId = materials.Count - 1;
                        entities[i].Faces[j].MaterialIndex = mId;
                    }
                    useState[mId] = true;
                }

                int[] matIdxShift = new int[materials.Count];
                int shifts = 0;
                List<EditableMeshMaterial> entMats = new List<EditableMeshMaterial>();
                for (int j = 0; j < materials.Count; j++)
                {
                    if (useState[j])
                    {
                        entMats.Add(materials[j]);
                        matIdxShift[j] = shifts;
                    }
                    else
                    {
                        shifts++;
                    }
                }

                entities[i].Materials = new EditableMeshMaterial[entMats.Count][];  //entMats.ToArray();
                entities[i].MaterialAnimation = new MaterialAnimationInstance[entMats.Count];
                
                for (int j = 0; j < entMats.Count; j++)
                {
                    entities[i].Materials[j] = new EditableMeshMaterial[] { entMats[j] };

                    entities[i].MaterialAnimation[j] = new MaterialAnimationInstance(new MaterialAnimation(1, 1));
                }
                    
                for (int j = 0; j < entities[i].Faces.Length; j++)
                {
                    entities[i].Faces[j].MaterialIndex -= matIdxShift[entities[i].Faces[j].MaterialIndex];
                }
            }

            EditableModel mdl = new EditableModel();
            mdl.Entities = entities;

            #region
            TransformAnimation tranAnim = new TransformAnimation(entities.Length);
            for (int i = 0; i < entities.Length; i++) 
            {
                tranAnim.Nodes[i].Transforms[0] = modelObjects[i].LocalTransform;
            }
            #endregion

            mdl.SetTransformAnimInst(
                new TransformAnimationInstance(tranAnim));
//#warning impl skeleton
//            mdl.ModelAnimation = new NoAnimation(GraphicsDevice.Instance.Device, trans);

            EditableModel.ToFile(mdl, dest);

            mdl.Dispose();
        }

        public override string Name
        {
            get { return DevStringTable.Instance[CsfKey]; }
        }

        public override string[] SourceExt
        {
            get { return new string[] { ".xml" }; }
        }
        public override string[] DestExt
        {
            get { return new string[] { ".mesh" }; }
        }

        public override string SourceDesc
        {
            get { return DevStringTable.Instance["Docs:XMLDesc"]; }
        }

        public override string DestDesc
        {
            get { return DevStringTable.Instance["DOCS:MeshDesc"]; }
        }
    }
}
