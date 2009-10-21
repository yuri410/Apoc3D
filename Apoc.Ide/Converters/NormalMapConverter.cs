using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Text;
using System.Windows.Forms;
using VirtualBicycle.IO;

namespace VirtualBicycle.Ide.Converters
{
    public unsafe class NormalMapConverter : ConverterBase
    {
        public enum ConversionType
        {
            SwapXY,
            SwapYZ,
            SwapXZ,
        }



        public NormalMapConverter()
        {
            Type = ConversionType.SwapYZ;
        }

        public ConversionType Type
        {
            get;
            set;
        }

        public override void ShowDialog(object sender, EventArgs e)
        {
            string[] files;
            string path;
            if (ConvDlg.Show("", GetOpenFilter(), out files, out path) == DialogResult.OK)
            {
                if (NormalMapConvDlg.ShowDialog(null) == DialogResult.OK)
                {
                    this.Type = NormalMapConvDlg.ConversionType;

                    ProgressDlg pd = new ProgressDlg(DevStringTable.Instance["GUI:Converting"]);

                    pd.MinVal = 0;
                    pd.Value = 0;
                    pd.MaxVal = files.Length;

                    pd.Show();
                    for (int i = 0; i < files.Length; i++)
                    {
                        string dest = Path.Combine(path, Path.GetFileNameWithoutExtension(files[i]) + ".png");

                        Convert(new DevFileLocation(files[i]), new DevFileLocation(dest));
                        pd.Value = i;
                    }
                    pd.Close();
                    pd.Dispose();
                }
            }
        }

        public override void Convert(ResourceLocation source, ResourceLocation dest)
        {
            Stream srcStm = source.GetStream;
            Bitmap src = new Bitmap(srcStm);
            srcStm.Close();

            int width = src.Width;
            int height = src.Height;

            Bitmap dst = new Bitmap(width, height);

            BitmapData srcData = src.LockBits(new Rectangle(0, 0, width, height), ImageLockMode.ReadOnly, PixelFormat.Format32bppArgb);
            BitmapData dstData = dst.LockBits(new Rectangle(0, 0, width, height), ImageLockMode.WriteOnly, PixelFormat.Format32bppArgb);


            uint* srcPtr = (uint*)srcData.Scan0.ToPointer();
            uint* dstPtr = (uint*)dstData.Scan0.ToPointer();

            

            switch (Type)
            {
                case ConversionType.SwapXY:
                    for (int i = 0; i < height; i++)
                    {
                        for (int j = 0; j < width; j++)
                        {
                            int index = i * width + j;
                            uint value = srcPtr[index];

                            uint a = value & 0xff000000;
                            uint r = (value >> 16) & 0xff;
                            uint g = (value >> 8) & 0xff;
                            uint b = (value & 0xff);

                            dstPtr[index] = a | (g << 16) | (r << 8) | b;
                        }
                    }
                    break;
                case ConversionType.SwapXZ:
                    for (int i = 0; i < height; i++)
                    {
                        for (int j = 0; j < width; j++)
                        {
                            int index = i * width + j;
                            uint value = srcPtr[index];

                            uint a = value & 0xff000000;
                            uint r = (value >> 16) & 0xff;
                            uint g = (value >> 8) & 0xff;
                            uint b = (value & 0xff);

                            dstPtr[index] = a | (b << 16) | (g << 8) | r;
                        }
                    }
                    break;
                case ConversionType.SwapYZ:
                    for (int i = 0; i < height; i++)
                    {
                        for (int j = 0; j < width; j++)
                        {
                            int index = i * width + j;
                            uint value = srcPtr[index];

                            uint a = value & 0xff000000;
                            uint r = (value >> 16) & 0xff;
                            uint g = (value >> 8) & 0xff;
                            uint b = (value & 0xff);

                            dstPtr[index] = a | (r << 16) | (b << 8) | g;
                        }
                    }
                    break;
            }
            


            src.UnlockBits(srcData);
            dst.UnlockBits(dstData);

            Stream dstStm = dest.GetStream;
            dst.Save(dstStm, ImageFormat.Png);
            dstStm.Close();
        }

        public override string Name
        {
            get { return DevStringTable.Instance["GUI:NormalMapConverter"]; }
        }

        public override string[] SourceExt
        {
            get { return new string[] { ".bmp", ".png", }; }
        }

        public override string[] DestExt
        {
            get { return new string[] { ".png" }; }
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
