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
    public partial class NormalMapConvDlg : Form
    {
        static NormalMapConvDlg()
        {
            ConversionType = NormalMapConverter.ConversionType.SwapYZ;
        }

        public static NormalMapConverter.ConversionType ConversionType
        {
            get;
            private set;
        }

        public NormalMapConvDlg()
        {
            InitializeComponent();

            LanguageParser.ParseLanguage(DevStringTable.Instance, this);

            this.DialogResult = DialogResult.Cancel;
        }

        public static DialogResult ShowDialog(string caption)
        {
            NormalMapConvDlg frm = new NormalMapConvDlg();

            if (!string.IsNullOrEmpty(caption))
            {
                frm.Text = caption;
            }

            frm.ShowDialog();

            return frm.DialogResult;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            if (radioButton1.Checked)
            {
                ConversionType = NormalMapConverter.ConversionType.SwapXY;
            }
            else if (radioButton2.Checked)            
            {
                ConversionType = NormalMapConverter.ConversionType.SwapXZ;
            }
            else if (radioButton3.Checked)
            {
                ConversionType = NormalMapConverter.ConversionType.SwapYZ;
            }

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
