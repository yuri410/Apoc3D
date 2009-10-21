using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using Apoc.MathLib;

namespace Apoc.Vfs
{

    public class ContentBinaryReader : BinaryReader
    {
        //bool closeStream = true;

        public ContentBinaryReader(ResourceLocation fl)
            : this(fl.GetStream, Encoding.Default)
        { }

        public ContentBinaryReader(Stream src)
            : base(src)
        { }

        public ContentBinaryReader(Stream src, Encoding enc)
            : base(src, enc)
        { }

        //public bool AutoCloseStream
        //{
        //    get { return closeStream; }
        //    set { closeStream = value; }
        //}

        //public override void Close()
        //{
        //    base.Close();
        //}

        public void ReadMatrix(out Matrix mat)
        {
            mat.M11 = ReadSingle();
            mat.M12 = ReadSingle();
            mat.M13 = ReadSingle();
            mat.M14 = ReadSingle();
            mat.M21 = ReadSingle();
            mat.M22 = ReadSingle();
            mat.M23 = ReadSingle();
            mat.M24 = ReadSingle();
            mat.M31 = ReadSingle();
            mat.M32 = ReadSingle();
            mat.M33 = ReadSingle();
            mat.M34 = ReadSingle();
            mat.M41 = ReadSingle();
            mat.M42 = ReadSingle();
            mat.M43 = ReadSingle();
            mat.M44 = ReadSingle();
        }

        public string ReadStringUnicode()
        {
            int len = ReadInt32();
            char[] chars = new char[len];
            for (int i = 0; i < len; i++)
            {
                chars[i] = (char)ReadUInt16();
            }
            //char[] chars = ReadChars(len);
            return new string(chars);
        }

        public Matrix ReadMatrix()
        {
            Matrix mat;
            mat.M11 = ReadSingle();
            mat.M12 = ReadSingle();
            mat.M13 = ReadSingle();
            mat.M14 = ReadSingle();
            mat.M21 = ReadSingle();
            mat.M22 = ReadSingle();
            mat.M23 = ReadSingle();
            mat.M24 = ReadSingle();
            mat.M31 = ReadSingle();
            mat.M32 = ReadSingle();
            mat.M33 = ReadSingle();
            mat.M34 = ReadSingle();
            mat.M41 = ReadSingle();
            mat.M42 = ReadSingle();
            mat.M43 = ReadSingle();
            mat.M44 = ReadSingle();
            return mat;
        }

        //public void ReadMaterial(out Material mat)
        //{
        //    mat = new Material();

        //    float alpha = ReadSingle();
        //    float red = ReadSingle();
        //    float green = ReadSingle();
        //    float blue = ReadSingle();
        //    mat.Ambient = new Color4(alpha, red, green, blue);

        //    alpha = ReadSingle();
        //    red = ReadSingle();
        //    green = ReadSingle();
        //    blue = ReadSingle();
        //    mat.Diffuse = new Color4(alpha, red, green, blue);

        //    alpha = ReadSingle();
        //    red = ReadSingle();
        //    green = ReadSingle();
        //    blue = ReadSingle();
        //    mat.Specular = new Color4(alpha, red, green, blue);

        //    alpha = ReadSingle();
        //    red = ReadSingle();
        //    green = ReadSingle();
        //    blue = ReadSingle();
        //    mat.Emissive = new Color4(alpha, red, green, blue);
            
        //    mat.Power = ReadSingle();
        //}

        //public MeshMaterial.Data ReadMaterial()
        //{
        //    MeshMaterial.Data mat = new MeshMaterial.Data();

        //    if (ReadInt32() == (int)FileID.Material)
        //    {
        //        mat.Flags = (MaterialFlags)ReadInt32();
        //        ReadMaterial(out mat.material);

        //        int len = ReadInt32();
        //        if (len > 0)
        //        {
        //            char[] chars = ReadChars(len);

        //            mat.TextureName = new string(chars);
        //        }
        //        return mat;
        //    }
        //    else
        //    {
        //        throw new InvalidDataException();
        //    }
        //}
        //public MeshMaterial.DataTextureEmbeded ReadMaterialEx()
        //{
        //    MeshMaterial.DataTextureEmbeded mat = new MeshMaterial.DataTextureEmbeded();

        //    if (ReadInt32() == (int)FileID.Material)
        //    {
        //        mat.Flags = (MaterialFlags)ReadInt32();
        //        ReadMaterial(out mat.material);

        //        bool hasTexture = ReadBoolean();
        //        if (hasTexture)
        //        {
        //            mat.Texture = LoadImage();
        //        }
        //        return mat;
        //    }
        //    else
        //    {
        //        throw new InvalidDataException();
        //    }
        //}
        //public BlockMaterial ReadBlockMaterial()
        //{
        //    BlockMaterial mat = new BlockMaterial();

        //    if (ReadInt32() == (int)FileID.Material)
        //    {
        //        mat.Flags = (MaterialFlags)ReadInt32();
        //        ReadMaterial(out mat.material);

        //        bool hasTexture = ReadBoolean();
        //        if (hasTexture)
        //        {
        //            mat.Texture = LoadImage();
        //        }
        //        return mat;
        //    }
        //    else
        //    {
        //        throw new InvalidDataException();
        //    }
        //}
        //public unsafe ImageBase ReadEmbededImage()
        //{
        //    int imgWidth = ReadUInt16();
        //    int imgHeight = ReadUInt16();

        //    int bytesPerPixel = ReadByte();

        //    RawImage image = new RawImage(imgWidth, imgHeight, bytesPerPixel == 4 ? ImagePixelFormat.A8R8G8B8 : ImagePixelFormat.R8G8B8);

        //    byte[] buffer = ReadBytes(imgHeight * imgWidth * bytesPerPixel);

        //    fixed (byte* src = &buffer[0])
        //    {
        //        Helper.MemCopy(image.GetData(), src, buffer.Length);
        //    }

        //    return image;
        //}

        /// <summary>
        ///  读取一个BinaryDataReader数据块。
        /// </summary>
        /// <returns></returns>
        public BinaryDataReader ReadBinaryData()
        {
            int size = ReadInt32();

            VirtualStream vs = new VirtualStream(BaseStream, BaseStream.Position, size);
            return new BinaryDataReader(vs);
        }

        public void Close(bool closeBaseStream)
        {
            base.Dispose(closeBaseStream);
        }
    }

    public class ContentBinaryWriter : BinaryWriter
    {
        //bool closeStream = true;

        public ContentBinaryWriter(ResourceLocation rl) :
            this(rl.GetStream, Encoding.Default)
        { }
        public ContentBinaryWriter(Stream output) : base(output) { }

        public ContentBinaryWriter(Stream output, Encoding encoding) : base(output, encoding) { }

        //public bool AutoCloseStream
        //{
        //    get { return closeStream; }
        //    set { closeStream = value; }
        //}
        //public override void Close()
        //{
        //    base.Close();
        //}

        public void WriteStringUnicode(string str)
        {
            if (str == null)
                str = string.Empty;
            int len = str.Length;
            Write(len);

            for (int i = 0; i < len; i++)
            {
                Write((ushort)str[i]);
            }
        }

        public void Write(ref Matrix mat)
        {
            Write(mat.M11);
            Write(mat.M12);
            Write(mat.M13);
            Write(mat.M14);
            Write(mat.M21);
            Write(mat.M22);
            Write(mat.M23);
            Write(mat.M24);
            Write(mat.M31);
            Write(mat.M32);
            Write(mat.M33);
            Write(mat.M34);
            Write(mat.M41);
            Write(mat.M42);
            Write(mat.M43);
            Write(mat.M44);
        }
        public void Write(Matrix mat)
        {
            Write(mat.M11);
            Write(mat.M12);
            Write(mat.M13);
            Write(mat.M14);
            Write(mat.M21);
            Write(mat.M22);
            Write(mat.M23);
            Write(mat.M24);
            Write(mat.M31);
            Write(mat.M32);
            Write(mat.M33);
            Write(mat.M34);
            Write(mat.M41);
            Write(mat.M42);
            Write(mat.M43);
            Write(mat.M44);
        }

        //public void Write(ref Material mat)
        //{
        //    Write(mat.Ambient.Alpha);
        //    Write(mat.Ambient.Red);
        //    Write(mat.Ambient.Green);
        //    Write(mat.Ambient.Blue);

        //    Write(mat.Diffuse.Alpha);
        //    Write(mat.Diffuse.Red);
        //    Write(mat.Diffuse.Green);
        //    Write(mat.Diffuse.Blue);

        //    Write(mat.Specular.Alpha);
        //    Write(mat.Specular.Red);
        //    Write(mat.Specular.Green);
        //    Write(mat.Specular.Blue);

        //    Write(mat.Emissive.Alpha);
        //    Write(mat.Emissive.Red);
        //    Write(mat.Emissive.Green);
        //    Write(mat.Emissive.Blue);

        //    Write(mat.Power);
        //}

        //public void Write(MeshMaterial.Data mat)
        //{
        //    Write((int)FileID.Material);
        //    Write((int)mat.Flags);
        //    Write(ref mat.material);
        //    if (string.IsNullOrEmpty(mat.TextureName))
        //    {
        //        Write((int)0);
        //    }
        //    else
        //    {
        //        string name = mat.TextureName;
        //        Write(name.Length);
        //        for (int i = 0; i < name.Length; i++)
        //        {
        //            Write(name[i]);
        //        }
        //    }
        //}
        //public void Write(MeshMaterial.DataTextureEmbeded mat)
        //{
        //    Write((int)FileID.Material);
        //    Write((int)mat.Flags);
        //    Write(ref mat.material);
        //    if (mat.Texture == null)
        //    {
        //        Write(false);
        //    }
        //    else
        //    {
        //        Write(true);
        //        SaveImage(mat.Texture);
        //    }
        //}

        //public unsafe void Write(ImageBase image)
        //{
        //    Write((UInt16)image.Width);
        //    Write((UInt16)image.Height);

        //    Write((byte)image.BytesPerPixel);

        //    //bw.Write(image.InternalData);

        //    byte* src = (byte*)image.GetData();
        //    for (int i = 0; i < image.ContentSize; i++)
        //    {
        //        Write(src[i]);
        //    }
        //}

        /// <summary>
        ///  写入一个BinaryDataWriter数据块。
        /// </summary>
        /// <returns></returns>
        public void Write(BinaryDataWriter data)
        {
            Write(0); //占个位置
            Flush();

            long start = BaseStream.Position;

            data.Save(new VirtualStream(BaseStream, BaseStream.Position));           

            long end = BaseStream.Position;
            int size = (int)(end - start);

            BaseStream.Position = start - 4;
            Write(size);
            BaseStream.Position = end;
        }
    }

 
}
