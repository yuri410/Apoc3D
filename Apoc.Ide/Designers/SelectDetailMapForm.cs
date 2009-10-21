using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using VirtualBicycle.Ide.Designers.WorldBuilder;
using VirtualBicycle;
using VirtualBicycle.Scene;
using VirtualBicycle.UI;
using VirtualBicycle.Graphics;

namespace VirtualBicycle.Ide.Designers
{
    public partial class SelectDetailMapForm : Form
    {
        string[] currentDms;
        CheckedListBox[] lists;

        public SelectDetailMapForm(string[] curDms)
        {
            InitializeComponent();

            LanguageParser.ParseLanguage(DevStringTable.Instance, this);

            lists = new CheckedListBox[] { checkedListBox1, checkedListBox2, checkedListBox3, checkedListBox4 };

            this.currentDms = curDms;

            //TerrainTextureSet texSet = WorldDesigner.TerrainTextureSet;

            string[] names = TextureLibrary.Instance.GetNames();

            for (int i = 0; i < names.Length; i++)
            {
                int index = checkedListBox1.Items.Add(names[i]);
                checkedListBox2.Items.Add(names[i]);
                checkedListBox3.Items.Add(names[i]);
                checkedListBox4.Items.Add(names[i]);

                for (int j = 0; j < curDms.Length; j++)
                {
                    if (CaseInsensitiveStringComparer.Compare(names[i], curDms[j]))
                    {
                        lists[j].SetItemChecked(i, true);
                        break;
                    }
                }
            }
            this.DialogResult = DialogResult.Cancel;
            SelectedDetailMaps = curDms;
        }

        private void checkedListBox1_ItemCheck(object sender, ItemCheckEventArgs e)
        {
            if (e.CurrentValue != CheckState.Checked)
            {
                if (checkedListBox1.CheckedItems.Count >= 1)
                {
                    checkedListBox1.SetItemChecked(checkedListBox1.CheckedIndices[0], false);
                }
            }
        }
        private void checkedListBox2_ItemCheck(object sender, ItemCheckEventArgs e)
        {
            if (e.CurrentValue != CheckState.Checked)
            {
                if (checkedListBox2.CheckedItems.Count >= 1)
                {
                    checkedListBox2.SetItemChecked(checkedListBox2.CheckedIndices[0], false);
                }
            }
        }
        private void checkedListBox3_ItemCheck(object sender, ItemCheckEventArgs e)
        {
            if (e.CurrentValue != CheckState.Checked)
            {
                if (checkedListBox3.CheckedItems.Count >= 1)
                {
                    checkedListBox3.SetItemChecked(checkedListBox3.CheckedIndices[0], false);
                }
            }
        }
        private void checkedListBox4_ItemCheck(object sender, ItemCheckEventArgs e)
        {
            if (e.CurrentValue != CheckState.Checked)
            {
                if (checkedListBox4.CheckedItems.Count >= 1)
                {
                    checkedListBox4.SetItemChecked(checkedListBox4.CheckedIndices[0], false);
                }
            }
        }

        public static string[] SelectedDetailMaps
        {
            get;
            private set;
        }

        public static DialogResult Show(string[] currentDms)
        {
            SelectDetailMapForm frm = new SelectDetailMapForm(currentDms);

            frm.ShowDialog();

            return frm.DialogResult;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            string[] result = new string[4];

            for (int i = 0; i < 4; i++)
            {
                if (lists[i].CheckedItems.Count > 0)
                {
                    result[i] = lists[i].CheckedItems[0].ToString();
                }
            }

            SelectedDetailMaps = result;

            this.DialogResult = DialogResult.OK;
            this.Close();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.Cancel;
            this.Close();
        }


    }
}
