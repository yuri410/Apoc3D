using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.IO.Compression;
using System.Text;
using SlimDX.Direct3D9;
using Apoc.IO;

namespace Apoc.Graphics
{
    public static class TextureLoader
    {
        public static Texture LoadUITexture(Device device, FileLocation fl)
        {
            Stream stream = fl.GetStream;
            Bitmap bmp = new Bitmap(stream);

            stream.Close();

            Rectangle rect = new Rectangle(0, 0, bmp.Width, bmp.Height);

            BitmapData data = bmp.LockBits(rect, ImageLockMode.ReadOnly, PixelFormat.Format32bppArgb);

            Texture result = new Texture(device, bmp.Width, bmp.Height, 1, Usage.None, Format.A8R8G8B8, Pool.Managed);
            IntPtr dest = result.LockRectangle(0, rect, LockFlags.None).Data.DataPointer;

            Memory.Copy(data.Scan0, dest, sizeof(int) * bmp.Width * bmp.Height);

            result.UnlockRectangle(0);
            bmp.UnlockBits(data);
            return result;
        }
        public static Texture LoadUITexture(Device device, ResourceLocation fl)
        {
            Stream stream = fl.GetStream;
            Bitmap bmp = new Bitmap(stream);

            stream.Close();

            Rectangle rect = new Rectangle(0, 0, bmp.Width, bmp.Height);

            BitmapData data = bmp.LockBits(rect, ImageLockMode.ReadOnly, PixelFormat.Format32bppArgb);

            Texture result = new Texture(device, bmp.Width, bmp.Height, 1, Usage.None, Format.A8R8G8B8, Pool.Managed);
            IntPtr dest = result.LockRectangle(0, rect, LockFlags.None).Data.DataPointer;

            Memory.Copy(data.Scan0, dest, sizeof(int) * bmp.Width * bmp.Height);

            result.UnlockRectangle(0);
            bmp.UnlockBits(data);
            return result;
        }
        public static void SaveUITexture(Device device, Texture texture, Stream stream)
        {
            SurfaceDescription desc = texture.GetLevelDescription(0);

            Rectangle rect = new Rectangle(0, 0, desc.Width, desc.Height);

            IntPtr src = texture.LockRectangle(0, rect, LockFlags.ReadOnly).Data.DataPointer;

            Bitmap bmp = new Bitmap(desc.Width, desc.Height);
            BitmapData data = bmp.LockBits(rect, ImageLockMode.WriteOnly, PixelFormat.Format32bppArgb);

            Memory.Copy(src, data.Scan0, sizeof(int) * rect.Width * rect.Height);

            texture.UnlockRectangle(0);
            bmp.UnlockBits(data);

            bmp.Save(stream, ImageFormat.Png);
        }
        


        public static void SaveDisplacementMap(Device device, Texture texture, string file)
        {
            FileStream fs = new FileStream(file, FileMode.OpenOrCreate, FileAccess.Write);
            ContentBinaryWriter bw = new ContentBinaryWriter(fs);
            byte[] data = TextureLoader.SaveDisplacementMap(device, texture);
            bw.Write(data);

            bw.Close();
        }
        public unsafe static byte[] SaveDisplacementMap(Device device, Texture texture)
        {
            System.IO.MemoryStream ms = new System.IO.MemoryStream();

            GZipStream gzipStm = new GZipStream(ms, CompressionMode.Compress);

            ContentBinaryWriter tbw = new ContentBinaryWriter(gzipStm);

            SurfaceDescription desc = texture.GetLevelDescription(0);

            tbw.Write(desc.Width);
            tbw.Write(desc.Height);

            float* dst = (float*)texture.LockRectangle(0, LockFlags.None).Data.DataPointer.ToPointer();

            for (int i = 0; i < desc.Height; i++)
            {
                for (int j = 0; j < desc.Width; j++)
                {
                    tbw.Write(dst[i * desc.Width + j]);
                }
            }

            texture.UnlockRectangle(0);
            tbw.Close();
            return ms.ToArray();
        }
        public unsafe static Texture LoadDisplacementMap(Device device, ResourceLocation fl)
        {
            Stream stm = fl.GetStream;

            GZipStream gzstm = new GZipStream(stm, CompressionMode.Decompress);

            ContentBinaryReader br = new ContentBinaryReader(gzstm);

            int width = br.ReadInt32();
            int height = br.ReadInt32();

            Texture texture = new Texture(device, width, height, 1, Usage.None, Format.R32F, Pool.Managed);

            float* dst = (float*)texture.LockRectangle(0, LockFlags.None).Data.DataPointer.ToPointer();

            for (int i = 0; i < height; i++)
            {
                for (int j = 0; j < width; j++)
                {
                    dst[i * width + j] = br.ReadSingle();
                }
            }

            texture.UnlockRectangle(0);

            br.Close();
            return texture;
        }
    }
}
