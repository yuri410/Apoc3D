using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace VirtualBicycle.Ide.Converters
{
    public partial class ProgressDlg : Form
    {

        public ProgressDlg(string message)
        {
            InitializeComponent();
            Message = message;
        }

        public string Message
        {
            get { return label1.Text; }
            set
            {
                label1.Text = value;
                Application.DoEvents();
            }
        }

        public int Value
        {
            get { return progressBar1.Value; }
            set
            {
                progressBar1.Value = value;
                Application.DoEvents();
            }
        }
        public int MaxVal
        {
            get { return progressBar1.Maximum; }
            set { progressBar1.Maximum = value; }
        }
        public int MinVal
        {
            get { return progressBar1.Minimum; }
            set { progressBar1.Minimum = value; }
        }
    }
}
