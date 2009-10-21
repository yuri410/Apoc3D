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
    public unsafe class Xml2ModelConverter2 : ConverterBase
    {
        const string CsfKey = "GUI:Xml2Mesh2";

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
            XmlModelParser parser = new XmlModelParser();

            ParsedXmlModel model = parser.Parse(source.GetStream);



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
