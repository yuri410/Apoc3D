using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Drawing.Imaging;
using System.Text;
using System.Windows.Forms;
using System.Windows.Forms.Design;
using SlimDX;
using VirtualBicycle.Design;
using VirtualBicycle.UI;

namespace VirtualBicycle.Ide.Editors
{
    public partial class Color4EditControl : UserControl, IEditControl<Color4>
    {      
        Color4 value;

        public Color4EditControl()
        {
            InitializeComponent();
            LanguageParser.ParseLanguage(DevStringTable.Instance, this);

            redBar.ValueChanged += this.refreshPreview;
            alphaBar.ValueChanged += this.refreshPreview;
            greenBar.ValueChanged += this.refreshPreview;
            blueBar.ValueChanged += this.refreshPreview;

        }

        #region IEditControl<Color4> 成员

        public Color4 Value
        {
            get { return value; }
            set
            {
                this.value = value;

                alphaBar.Value = (int)(value.Alpha * 255);
                redBar.Value = (int)(value.Red * 255);
                greenBar.Value = (int)(value.Green * 255);
                blueBar.Value = (int)(value.Blue * 255);

            }
        }

        public IWindowsFormsEditorService Service
        {
            get { return null; }
            set {  }
        }

        #endregion

        private void pictureBox1_Paint(object sender, PaintEventArgs e)
        {
            System.Drawing.Graphics g = e.Graphics;

            g.Clear(Color.White);


            Size cs = pictureBox1.ClientSize;
            Bitmap bmp = VirtualBicycle.Ide.Properties.Resources.gird;

            for (int y = 0; y < cs.Height; y += bmp.Height)
            {
                for (int x = 0; x < cs.Width; x += bmp.Width)
                {
                    g.DrawImageUnscaled(bmp, x, y);
                }
            }

            SolidBrush brush = new SolidBrush(value.ToColor());
            g.FillRectangle(brush, new Rectangle(Point.Empty, pictureBox1.ClientSize));

            brush.Dispose();
        }

        private void redBar_ValueChanged(object sender, EventArgs e)
        {
            value.Red = redBar.Value / 255f;
            redLabel.Text = redBar.Value.ToString();
            if (checkBox1.Checked)
            {
                greenBar.Value = redBar.Value;
                blueBar.Value = redBar.Value;
            }
        }

        private void alphaBar_ValueChanged(object sender, EventArgs e)
        {
            value.Alpha = alphaBar.Value / 255f;
            alphaLabel.Text = alphaBar.Value.ToString();
        }

        private void greenBar_ValueChanged(object sender, EventArgs e)
        {
            value.Green = greenBar.Value / 255f;
            greenLabel.Text = greenBar.Value.ToString();
            if (checkBox1.Checked)
            {
                redBar.Value = greenBar.Value;
                blueBar.Value = greenBar.Value;
            }           
        }

        private void blueBar_ValueChanged(object sender, EventArgs e)
        {
            value.Blue = blueBar.Value / 255f;
            blueLabel.Text = blueBar.Value.ToString();
            if (checkBox1.Checked)
            {
                redBar.Value = blueBar.Value;
                greenBar.Value = blueBar.Value;
            }            
        }

        private void refreshPreview(object sender, EventArgs e)
        {
            pictureBox1.Refresh();
        }


    }
}
