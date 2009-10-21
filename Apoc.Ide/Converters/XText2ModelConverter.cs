using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Windows.Forms;
using SlimDX;
using SlimDX.Direct3D9;
using VirtualBicycle.Ide.Converters;
using VirtualBicycle.Ide.Designers;
using VirtualBicycle.Ide.Editors.EditableObjects;
using VirtualBicycle.Graphics.Animation;
using VirtualBicycle.IO;
using VirtualBicycle.MathLib;

namespace VirtualBicycle.Ide.Converters
{
    public class XText2ModelConverter : ConverterBase
    {
        const string CsfKey = "GUI:X2Mesh";

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

        public override void Convert(ResourceLocation source, ResourceLocation dest)
        {
            Mesh mesh = Mesh.FromStream(GraphicsDevice.Instance.Device, source.GetStream, MeshFlags.Managed);

            ExtendedMaterial[] mats = mesh.GetMaterials();
            EditableMeshMaterial[][] outMats = new EditableMeshMaterial[mats.Length][];
            for (int i = 0; i < mats.Length; i++)
            {
                outMats[i] = new EditableMeshMaterial[1];
                outMats[i][0] = new EditableMeshMaterial();
                outMats[i][0].D3DMaterial = mats[i].MaterialD3D;
                outMats[i][0].TextureFile1 = mats[i].TextureFileName;
            }


            string name = string.Empty;
            FileLocation fl = source as FileLocation;
            if (fl != null)
            {
                name = Path.GetFileNameWithoutExtension(fl.Path);
            }
            EditableMesh outMesh = new EditableMesh(name, mesh, outMats);

            EditableModel outMdl = new EditableModel();

            mesh.Dispose();

            outMdl.Entities = new EditableMesh[] { outMesh };

            TransformAnimation transAnim = new TransformAnimation(1);
            transAnim.Nodes[0].Transforms = new Matrix[1] { Matrix.RotationX(-MathEx.PIf / 2) };

            outMdl.SetTransformAnimInst(new TransformAnimationInstance(transAnim));            

            EditableModel.ToFile(outMdl, dest);


            outMdl.Dispose();
        }

        public override string Name
        {
            get { return DevStringTable.Instance[CsfKey]; }
        }

        public override string[] SourceExt
        {
            get { return new string[] { ".x" }; }
        }
        public override string[] DestExt
        {
            get { return new string[] { ".mesh" }; }
        }

        public override string SourceDesc
        {
            get { return DevStringTable.Instance["DOCS:MeshDesc"]; }
        }

        public override string DestDesc
        {
            get { return DevStringTable.Instance["Docs:XTextDesc"]; }
        }
    }
}
