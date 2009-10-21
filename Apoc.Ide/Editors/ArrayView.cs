using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using VirtualBicycle.UI;


namespace VirtualBicycle.Ide.Editors
{
    public partial class ArrayView<T> : Form
    {
        static int[] res;
        static DialogResult dr;
        public ArrayView()
        {
            InitializeComponent();

            LanguageParser.ParseLanguage(DevStringTable.Instance, this);
        }

        public static DialogResult ShowDialog(string text, T[] array, out int[] result)
        {
            ArrayView<T> f = new ArrayView<T>();

            f.label1.Text = text;
            if (array != null)
            {
                for (int i = 0; i < array.Length; i++)
                {
                    f.listBox1.Items.Add(i.ToString() + ' ' + array[i].ToString());
                }
            }

            f.ShowDialog();

            result = res;
            res = null;
            return dr;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            dr = DialogResult.OK;
            res = new int[listBox1.CheckedItems.Count];
            for (int i = 0; i < res.Length; i++)
            {
                res[i] = listBox1.CheckedIndices[i];
            }
            Array.Sort<int>(res);
            this.Close();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            dr = DialogResult.Cancel;
            this.Close();
        }
    }
}
