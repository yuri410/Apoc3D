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
using SlimDX.Direct3D9;
using VirtualBicycle.Ide.Designers;
using VirtualBicycle;
using VirtualBicycle.Design;
using VirtualBicycle.UI;

namespace VirtualBicycle.Ide.Editors
{
    public partial class TextureEditControl : UserControl, IEditControl<Texture>
    {
        public TextureEditControl()
        {
            InitializeComponent();
            LanguageParser.ParseLanguage(DevStringTable.Instance, this);
        }

        Texture image;
        IWindowsFormsEditorService service;
        #region IEditControl<Texture> 成员

        public unsafe Texture Value
        {
            get
            {
                return image;
            }
            set
            {
                image = value;


                widthLabel.Text = DevStringTable.Instance["PROP:IMGWIDTH"];
                heightLabel.Text = DevStringTable.Instance["PROP:IMGHEIGHT"];
                formatLabel.Text = DevStringTable.Instance["PROP:IMGPixFmt"];

                if (value != null)
                {
                    try
                    {
                        //RawImage rawImg = RawImage.FromTexture (tex
                        Bitmap bmp = DevUtils.Texture2Bitmap(value);
                        pictureBox1.Image = bmp;
                    }
                    catch (NotSupportedException)
                    {
                        pictureBox1.Image = null;
                    }
                    finally
                    {
                        SurfaceDescription desc = value.GetLevelDescription(0);
                        widthLabel.Text += desc.Width.ToString();
                        heightLabel.Text += desc.Height.ToString();
                        formatLabel.Text += desc.Format.ToString();
                    }
                    
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
            openFileDialog1.ShowReadOnly = false;
            openFileDialog1.Filter = TextureEditor.GetFilter(); // ImageManager.Instance.GetFilter();
            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                if (image != null)
                {
                    image.Dispose();
                }

                image = Texture.FromFile(GraphicsDevice.Instance.Device, openFileDialog1.FileName);

                //image = Texture.FromFile(GraphicsDevice.Instance.Device, openFileDialog1.FileName, Usage.None, Pool.Managed);
                //ImageBase img = ImageManager.Instance.CreateInstaceUnmanaged(new DevFileLocation(openFileDialog1.FileName));

                //image = img.GetTexture(GraphicsDevice.Instance.Device, Usage.None, Pool.Managed);

                //img.Dispose();
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

        private void button3_Click(object sender, EventArgs e)
        {

        }
    }
}
