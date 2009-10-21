using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Text;
using System.Windows.Forms;

namespace VirtualBicycle.Ide.Converters
{
    public partial class DemConvDlg : Form
    {
        DemConverter converter;
        DemParameters conParams;

        public DemConvDlg(DemConverter converter)
        {
            InitializeComponent();

            this.converter = converter;
            this.conParams = new DemParameters();
            this.propertyGrid1.SelectedObject = conParams;
        }


        Point GetTileCoord(string fileName)
        {
            string[] v = fileName.Split('_');
            int len = v.Length;

            return new Point(int.Parse(v[len - 2]), int.Parse(v[len - 1]));
        }

        private void addButton_Click(object sender, EventArgs e)
        {
            openFileDialog1.Filter = converter.GetOpenFilter();

            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                string[] files = openFileDialog1.FileNames;

                for (int i = 0; i < files.Length; i++)
                {
                    string fileName = Path.GetFileName(files[i]);

                    ListViewItem item = fileListView.Items.Add(fileName);

                    Point p = GetTileCoord(Path.GetFileNameWithoutExtension(fileName));

                    item.SubItems.Add(p.X.ToString() + ", " + p.Y.ToString());
                    item.Tag = files[i];
                }
            }
        }

        private void okButton_Click(object sender, EventArgs e)
        {
            propertyGrid1.Enabled = false;
            fileListView.Enabled = false;

            addButton.Enabled = false;
            removeButton.Enabled = false;
            okButton.Enabled = false;
            cancelButton.Enabled = false;

            for (int i = 0; i < fileListView.Items.Count; i++)
            {
                string fileName = (string)fileListView.Items[i].Tag;

                DevFileLocation sfl = new DevFileLocation(fileName);

                converter.Convert(sfl, null);


            }

            propertyGrid1.Enabled = true;
            fileListView.Enabled = true;

            addButton.Enabled = true;
            removeButton.Enabled = true;
            okButton.Enabled = true;
            cancelButton.Enabled = true;
        }

        private void cancelButton_Click(object sender, EventArgs e)
        {
            this.Close();
        }

    }
}
