using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using SlimDX.Direct3D9;
using VirtualBicycle.Ide.Editors.EditableObjects;

namespace VirtualBicycle.Ide.Designers
{
    public partial class VertexElementDlg : Form
    {
        VertexElement[] value;

        public VertexElementDlg()
        {
            InitializeComponent();
            DialogResult = DialogResult.Cancel;
        }
        public VertexElementDlg(VertexElement[] elements)
        {
            InitializeComponent();
            DialogResult = DialogResult.Cancel;
            value = elements;

            for (int i = 0; i < elements.Length; i++)
            {
                if (elements[i].Type != DeclarationType.Unused)
                {
                    checkedListBox1.Items.Add(elements[i].Usage.ToString());
                }
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            List<VertexElement> newElements = new List<VertexElement>();

            CheckedListBox.CheckedIndexCollection c = checkedListBox1.CheckedIndices;
            for (int i = 0; i < c.Count; i++)
            {
//                newElements.Add(value[checkedListBox1.Items.Count - c[i] - 1]);
                newElements.Add(value[c[i]]);
            }
            value = newElements.ToArray();

            DialogResult = DialogResult.OK;
            this.Close();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            this.Close();
        }


        public VertexElement[] Elements
        {
            get { return value; }
        }
    }
}
