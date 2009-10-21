using System;
using System.Collections.Generic;
using System.Text;
using VirtualBicycle.IO;
using System.IO;
using System.Windows.Forms;
using VirtualBicycle.Graphics;
using VirtualBicycle.Ide.Designers;

namespace VirtualBicycle.Ide.Converters
{
    public class Mesh2MeshConverter : ConverterBase
    {
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

        public override void Convert(ResourceLocation source, ResourceLocation dest)
        {
            Model model = ModelManager.Instance.CreateInstance(GraphicsDevice.Instance.Device, source);

            Model.ToStream(model, dest.GetStream);

            ModelManager.Instance.DestoryInstance(model);
        }

        public override string Name
        {
            get { return "Mesh Updater"; }
        }

        public override string[] SourceExt
        {
            get { return new string[] { ".mesh" }; }
        }

        public override string[] DestExt
        {
            get { return new string[] { ".mesh" }; }
        }

        public override string SourceDesc
        {
            get { return "mesh"; }
        }

        public override string DestDesc
        {
            get { return "mesh"; }
        }
    }
}
