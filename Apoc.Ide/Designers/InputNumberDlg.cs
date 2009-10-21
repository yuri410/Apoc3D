using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace VirtualBicycle.Ide.Designers
{
    public partial class InputNumberDlg : Form
    {
        public InputNumberDlg()
        {
            InitializeComponent();
            DialogResult = DialogResult.Cancel;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.OK;
            this.Close();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        public decimal Value
        {
            get { return numericUpDown1.Value; }
            set { numericUpDown1.Value = value; }
        }

    }
}
