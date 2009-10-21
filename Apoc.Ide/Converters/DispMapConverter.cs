using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.IO.Compression;
using System.Text;
using System.Windows.Forms;
using SlimDX;
using SlimDX.Direct3D9;
using VirtualBicycle.Ide.Designers;
using VirtualBicycle;
using VirtualBicycle.Graphics;
using VirtualBicycle.IO;
using VirtualBicycle.Scene;

namespace VirtualBicycle.Ide.Converters
{
    public class DispMapConverter : ConverterBase
    {
        public DispMapConverter()
        {
            HeightScale = 35f / 256f;
        }

        public float HeightScale
        {
            get;
            private set;
        }

        public override void ShowDialog(object sender, EventArgs e)
        {
            string[] files;
            string path;
            if (ConvDlg.Show("", GetOpenFilter(), out files, out path) == DialogResult.OK)
            {
                ProgressDlg pd = new ProgressDlg(DevStringTable.Instance["GUI:Converting"]);

                pd.MinVal = 0;
                pd.Value = 0;
                pd.MaxVal = files.Length;

                pd.Show();
                for (int i = 0; i < files.Length; i++)
                {
                    string dest = Path.Combine(path, Path.GetFileNameWithoutExtension(files[i]) + ".dmp");

                    Convert(new DevFileLocation(files[i]), new DevFileLocation(dest));
                    pd.Value = i;
                }
                pd.Close();
                pd.Dispose();
            }
        }

        public unsafe override void Convert(ResourceLocation source, ResourceLocation dest)
        {
            Stream srcStm = source.GetStream;
            Bitmap bmp = new Bitmap(srcStm);

            srcStm.Close();

            int width = bmp.Width;
            int height = bmp.Height;
            Rectangle rect = new Rectangle(0, 0, width, height);
            BitmapData data = bmp.LockBits(rect, ImageLockMode.ReadOnly, PixelFormat.Format32bppArgb);

            int* src = (int*)data.Scan0.ToPointer();

            Stream fs = dest.GetStream;
            fs.SetLength(0);
            GZipStream gzipStm = new GZipStream(fs, CompressionMode.Compress);
            ContentBinaryWriter bw = new ContentBinaryWriter(gzipStm);

            bw.Write(width);
            bw.Write(height);

            for (int i = 0; i < height; i++)
            {
                for (int j = 0; j < width; j++)
                {
                    bw.Write((float)((src[i * width + j] & 0xff) * HeightScale) / SceneData.MaxTerrainHeight);
                }
            }


            bmp.UnlockBits(data);

            bw.Close();
        }

        public override string Name
        {
            get { return DevStringTable.Instance["GUI:DispMapConverter"]; }
        }

        public override string[] SourceExt
        {
            get { return new string[] { ".bmp", ".png" }; }
        }

        public override string[] DestExt
        {
            get { return new string[] { ".dmp" }; }
        }

        public override string SourceDesc
        {
            get { return string.Empty; }
        }

        public override string DestDesc
        {
            get { return string.Empty; }
        }
    }
}
