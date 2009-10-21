using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using VirtualBicycle.UI;

namespace VirtualBicycle.Ide.Converters
{
    public partial class ConvDlg : Form
    {

        static DialogResult dr;
        static string[] files;
        static string outPath;

        public ConvDlg()
        {
            InitializeComponent();

            LanguageParser.ParseLanguage(DevStringTable.Instance, this);
            
            files = null;
            outPath = null;
        }

        private void button5_Click(object sender, EventArgs e)
        {
            if (folderBrowserDialog1.ShowDialog() == DialogResult.OK)
            {
                textBox1.Text = folderBrowserDialog1.SelectedPath;
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                string[] files = openFileDialog1.FileNames;
                
                for (int i = 0; i < files.Length; i++)
                {
                    listView1.Items.Add(files[i]);
                }
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            for (int i = listView1.SelectedItems.Count - 1; i >= 0; i++)
            {
                listView1.SelectedItems[i].Remove();
            }
        }

        private void button3_Click(object sender, EventArgs e)
        {
            dr = DialogResult.Cancel;
            this.Close();
        }

        private void button4_Click(object sender, EventArgs e)
        {
            dr = DialogResult.OK;
            files = new string[listView1.Items.Count];
            for (int i = 0; i < listView1.Items.Count; i++)
            {
                files[i] = listView1.Items[i].Text;
            }
            outPath = textBox1.Text;
            this.Close();
        }

        public static DialogResult Show(string title, string filter, out string[] files, out string path)
        {
            ConvDlg f = new ConvDlg();
            f.Text = title;
            f.openFileDialog1.Filter = filter;
            f.ShowDialog();            
            if (dr == DialogResult.OK)
            {
                files = ConvDlg.files;
                path = ConvDlg.outPath;
            }
            else
            {
                path = null;
                files = null;
            }
            f.Dispose();
            return dr;
        }
    }
}
