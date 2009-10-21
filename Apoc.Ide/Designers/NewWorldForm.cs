using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using VirtualBicycle.Scene;
using VirtualBicycle.UI;

namespace VirtualBicycle.Ide.Designers
{
    public partial class NewWorldForm : Form
    {
        public static int SceneWidth
        {
            get;
            private set;
        }
        public static int SceneHeight
        {
            get;
            private set;
        }

        public NewWorldForm()
        {
            InitializeComponent();

            LanguageParser.ParseLanguage(DevStringTable.Instance, this);

            textBox1.Text = (513).ToString();
            textBox2.Text = (513).ToString();

            DialogResult = DialogResult.Cancel;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            int sw = int.Parse(textBox1.Text);
            int sh = int.Parse(textBox2.Text);

            if ((sw % Cluster.ClusterLength) == 1 && (sh % Cluster.ClusterLength) == 1)
            {
                SceneWidth = sw;
                SceneHeight = sh;
                DialogResult = DialogResult.OK;
                this.Close();
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
            this.Close();
        }




    }
}
