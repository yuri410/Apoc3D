using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Drawing.Imaging;
using System.Text;
using System.Windows.Forms;
using System.Windows.Forms.Design;
using R3D;
using R3D.Design;
using R3D.IO;
using R3D.Media;
using R3D.UI;
using R3D.Base;

namespace Ra2Develop.Editors
{
    public partial class ImageEditControl : UserControl, IEditControl<ImageBase>
    {
        public ImageEditControl()
        {
            InitializeComponent();
            LanguageParser.ParseLanguage(Program.StringTable, this);
        }

        ImageBase image;
        IWindowsFormsEditorService service;
        #region IEditControl<RawImage> 成员

        public unsafe ImageBase Value
        {
            get
            {
                return image;
            }
            set
            {
                image = value;
                widthLabel.Text = Program.StringTable["PROP:IMGWIDTH"];
                heightLabel.Text = Program.StringTable["PROP:IMGHEIGHT"];
                formatLabel.Text = Program.StringTable["PROP:IMGPixFmt"];

                if (value != null)
                {
                    Bitmap bmp = new Bitmap(image.Width, image.Height);
                    BitmapData data = bmp.LockBits(new Rectangle(0, 0, bmp.Width, bmp.Height), ImageLockMode.WriteOnly, PixelFormat.Format32bppArgb);

                    void* dst = data.Scan0.ToPointer();
                    image.CopyDataARGB(dst);

                    bmp.UnlockBits(data);
                    pictureBox1.Image = bmp;

                    widthLabel.Text += bmp.Width.ToString();
                    heightLabel.Text += bmp.Height.ToString();
                    formatLabel.Text += image.PixelFormat.ToString();
                }
            }
        }

        public IWindowsFormsEditorService Service
        {
            get { return service; }
            set { service = value; }
        }

        #endregion

        private unsafe void button1_Click(object sender, EventArgs e)
        {
            openFileDialog1.Filter = ImageManager.Instance.GetFilter();
            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                ImageBase img = ImageManager.Instance.CreateInstaceUnmanaged(new FileLocation(openFileDialog1.FileName));

                image = new RawImage(img.Width, img.Height, img.PixelFormat);


                void* src = img.GetData();
                void* dst = image.GetData();
                Memory.Copy(src, dst, image.ContentSize);

                img.Dispose();
            }
            service.CloseDropDown();
            service = null;
        }

        private void pictureBox1_Paint(object sender, PaintEventArgs e)
        {
            if (image == null)
            {
                e.Graphics.Clear(SystemColors.Window);

                Pen cross = new Pen(Color.Red);                

                Size cs = pictureBox1.ClientSize;
                e.Graphics.DrawLine(cross, Point.Empty, new Point(cs));
                e.Graphics.DrawLine(cross, new Point(cs.Width, 0), new Point(0, cs.Height));

                cross.Dispose();
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            image = null;
            service.CloseDropDown();
            service = null;
            if (pictureBox1.Image != null)
            {
                pictureBox1.Image.Dispose();
                pictureBox1.Image = null;
            }
        }
    }
}
