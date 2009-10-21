using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using VirtualBicycle.Ide.Designers;
using VirtualBicycle;
using VirtualBicycle.UI;

namespace VirtualBicycle.Ide
{
    public partial class SaveConfirmationDlg : Form
    {
        public SaveConfirmationDlg()
        {
            InitializeComponent();
            LanguageParser.ParseLanguage(DevStringTable.Instance, this);
            dr = DialogResult.Cancel;
        }


        static DialogResult dr;
        static DocumentBase[] savingDocs;

        static Dictionary<string, DocumentBase> docs;

        public static Pair<DialogResult, DocumentBase[]> Show(IWin32Window parent, DocumentBase[] allDocs)
        {
            docs = new Dictionary<string, DocumentBase>(allDocs.Length);
            SaveConfirmationDlg f = new SaveConfirmationDlg();
            for (int i = 0; i < allDocs.Length; i++)
            {
                docs.Add(allDocs[i].ToString(), allDocs[i]);
                f.listBox1.Items.Add(allDocs[i].ToString());
            }

            f.ShowDialog(parent);

            Pair<DialogResult, DocumentBase[]> res;
            res.first = dr;
            res.second = savingDocs;

            dr = DialogResult.Cancel;
            savingDocs = null;
            docs = null;
            return res;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            List<DocumentBase> res = new List<DocumentBase>(listBox1.Items.Count);
            for (int i = 0; i < listBox1.Items.Count; i++)
            {
                res.Add(docs[(string)listBox1.Items[i]]);
            }
            savingDocs = res.ToArray();
            dr = DialogResult.Yes;
            this.Close();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            dr = DialogResult.No;
            this.Close();
        }

        private void button3_Click(object sender, EventArgs e)
        {
            this.Close();
        }
    }
}
