using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace Apoc.Vfs
{
    /// <summary>
    ///  定义一种由若干“键”—“值”组成的集合的存储方式  的读取器
    ///  “键”为字符串，“值”为二进制数据块。
    ///  
    ///  意义：可以不按先后顺序将数据存储，可以方便增添或减少存储的项目。
    /// </summary>
    public unsafe class BinaryDataReader
    {
        /// <summary>
        ///  定义一个“键”—“值”的存储项
        /// </summary>
        struct Entry
        {
            public string name;
            public int offset;
            public int size;

            public Entry(string name, int offset, int size)
            {
                this.name = name;
                this.offset = offset;
                this.size = size;
            }
        }


        int sectCount;
        Dictionary<string, Entry> positions;
        Stream stream;

        byte[] buffer;

        public BinaryDataReader(Stream stm)
        {
            stream = stm;
            buffer = new byte[sizeof(decimal)];

            ContentBinaryReader br = new ContentBinaryReader(stm, Encoding.Default);

            // 读出所有块
            sectCount = br.ReadInt32();
            positions = new Dictionary<string, Entry>(sectCount);

            for (int i = 0; i < sectCount; i++)
            {
                string name = br.ReadStringUnicode();
                int size = br.ReadInt32();

                positions.Add(name, new Entry(name, (int)br.BaseStream.Position, size));

                br.BaseStream.Position += size;
            }
            br.Close();
        }

        public bool Contains(string name)
        {
            return positions.ContainsKey(name);
        }

        public ContentBinaryReader TryGetData(string name) 
        {
            Entry ent;
            if (positions.TryGetValue(name, out ent))
            {
                return new ContentBinaryReader(new VirtualStream(stream, ent.offset, ent.size));
            }
            return null;
        }
        public ContentBinaryReader GetData(string name)
        {
            Entry ent = positions[name];
            return new ContentBinaryReader(new VirtualStream(stream, ent.offset, ent.size));
        }
        public Stream GetDataStream(string name)
        {
            Entry ent = positions[name];
            return new VirtualStream(stream, ent.offset, ent.size);
        }
        public int GetDataInt32(string name)
        {
            Entry ent = positions[name];

            stream.Position = ent.offset;
            stream.Read(buffer, 0, sizeof(int));

            return buffer[0] | (buffer[1] << 8) | (buffer[2] << 16) | (buffer[3] << 24);

        }
        //[CLSCompliant(false)]
        public uint GetDataUInt32(string name)
        {
            Entry ent = positions[name];

            stream.Position = ent.offset;
            stream.Read(buffer, 0, sizeof(uint));

            return (uint)(buffer[0] | (buffer[1] << 8) | (buffer[2] << 16) | (buffer[3] << 24));
        }

        public short GetDataInt16(string name)
        {
            Entry ent = positions[name];

            stream.Position = ent.offset;
            stream.Read(buffer, 0, sizeof(short));

            return (short)(buffer[0] | (buffer[1] << 8));
        }
        //[CLSCompliant(false)]
        public ushort GetDataUInt16(string name)
        {
            Entry ent = positions[name];

            stream.Position = ent.offset;
            stream.Read(buffer, 0, sizeof(ushort));

            return (ushort)(buffer[0] | (buffer[1] << 8));
        }

        public long GetDataInt64(string name)
        {
            Entry ent = positions[name];

            stream.Position = ent.offset;
            stream.Read(buffer, 0, sizeof(long));

            uint num = (uint)(buffer[0] | (buffer[1] << 8) | (buffer[2] << 16) | (buffer[3] << 24));
            uint num2 = (uint)(buffer[4] | (buffer[5] << 8) | (buffer[6] << 16) | (buffer[7] << 24));
            return (long)((num2 << 32) | num);
        }
        //[CLSCompliant(false)]
        public ulong GetDataUInt64(string name)
        {
            Entry ent = positions[name];

            stream.Position = ent.offset;
            stream.Read(buffer, 0, sizeof(ulong));

            uint num = (uint)(((buffer[0] | (buffer[1] << 8)) | (buffer[2] << 16)) | (buffer[3] << 24));
            uint num2 = (uint)(((buffer[4] | (buffer[5] << 8)) | (buffer[6] << 16)) | (buffer[7] << 24));
            return ((num2 << 32) | num);
        }

        public bool GetDataBool(string name)
        {
            Entry ent = positions[name];

            stream.Position = ent.offset;
            stream.Read(buffer, 0, sizeof(bool));

            return (buffer[0] != 0);
        }

        public float GetDataSingle(string name)
        {
            Entry ent = positions[name];

            stream.Position = ent.offset;
            stream.Read(buffer, 0, sizeof(float));

            uint num = (uint)(((buffer[0] | (buffer[1] << 8)) | (buffer[2] << 16)) | (buffer[3] << 24));
            return *(((float*)&num));
        }
        public float GetDataDouble(string name)
        {
            Entry ent = positions[name];

            stream.Position = ent.offset;
            stream.Read(buffer, 0, sizeof(float));

            uint num = (uint)(((buffer[0] | (buffer[1] << 8)) | (buffer[2] << 16)) | (buffer[3] << 24));
            uint num2 = (uint)(((buffer[4] | (buffer[5] << 8)) | (buffer[6] << 16)) | (buffer[7] << 24));
            ulong num3 = (num2 << 32) | num;
            return *(((float*)&num3));

        }




        public int GetDataInt32(string name, int def)
        {
            Entry ent;
            if (positions.TryGetValue(name, out ent))
            {//= positions[name];

                stream.Position = ent.offset;
                stream.Read(buffer, 0, sizeof(int));

                return buffer[0] | (buffer[1] << 8) | (buffer[2] << 16) | (buffer[3] << 24);
            }
            return def;
        }
        public uint GetDataUInt32(string name, uint def)
        {
            Entry ent;
            if (positions.TryGetValue(name, out ent))
            {
                stream.Position = ent.offset;
                stream.Read(buffer, 0, sizeof(uint));

                return (uint)(buffer[0] | (buffer[1] << 8) | (buffer[2] << 16) | (buffer[3] << 24));
            }
            return def;
        }

        public short GetDataInt16(string name, short def)
        {
            Entry ent;
            if (positions.TryGetValue(name, out ent))
            {
                stream.Position = ent.offset;
                stream.Read(buffer, 0, sizeof(short));

                return (short)(buffer[0] | (buffer[1] << 8));
            }
            return def;
        }
        public ushort GetDataUInt16(string name, ushort def)
        {
            Entry ent;
            if (positions.TryGetValue(name, out ent))
            {

                stream.Position = ent.offset;
                stream.Read(buffer, 0, sizeof(ushort));

                return (ushort)(buffer[0] | (buffer[1] << 8));
            }
            return def;
        }

        public long GetDataInt64(string name, long def)
        {
            Entry ent;
            if (positions.TryGetValue(name, out ent))
            {
                stream.Position = ent.offset;
                stream.Read(buffer, 0, sizeof(long));

                uint num = (uint)(buffer[0] | (buffer[1] << 8) | (buffer[2] << 16) | (buffer[3] << 24));
                uint num2 = (uint)(buffer[4] | (buffer[5] << 8) | (buffer[6] << 16) | (buffer[7] << 24));
                return (long)((num2 << 32) | num);
            }
            return def;
        }
        public ulong GetDataUInt64(string name, ulong def)
        {
            Entry ent;
            if (positions.TryGetValue(name, out ent))
            {
                stream.Position = ent.offset;
                stream.Read(buffer, 0, sizeof(ulong));

                uint num = (uint)(((buffer[0] | (buffer[1] << 8)) | (buffer[2] << 16)) | (buffer[3] << 24));
                uint num2 = (uint)(((buffer[4] | (buffer[5] << 8)) | (buffer[6] << 16)) | (buffer[7] << 24));
                return ((num2 << 32) | num);
            }
            return def;
        }

        public bool GetDataBool(string name, bool def)
        {
            Entry ent;
            if (positions.TryGetValue(name, out ent))
            {
                stream.Position = ent.offset;
                stream.Read(buffer, 0, sizeof(bool));

                return (buffer[0] != 0);
            }
            return def;
        }

        public float GetDataSingle(string name, float def)
        {
            Entry ent;
            if (positions.TryGetValue(name, out ent))
            {
                stream.Position = ent.offset;
                stream.Read(buffer, 0, sizeof(float));

                uint num = (uint)(((buffer[0] | (buffer[1] << 8)) | (buffer[2] << 16)) | (buffer[3] << 24));
                return *(((float*)&num));
            }
            return def;
        }
        public float GetDataDouble(string name, float def)
        {
            Entry ent;
            if (positions.TryGetValue(name, out ent))
            {
                stream.Position = ent.offset;
                stream.Read(buffer, 0, sizeof(float));

                uint num = (uint)(((buffer[0] | (buffer[1] << 8)) | (buffer[2] << 16)) | (buffer[3] << 24));
                uint num2 = (uint)(((buffer[4] | (buffer[5] << 8)) | (buffer[6] << 16)) | (buffer[7] << 24));
                ulong num3 = (num2 << 32) | num;
                return *(((float*)&num3));
            }
            return def;
        }



        public void Close()
        {
            stream.Close();
        }


        public int GetChunkOffset(string name)
        {
            Entry ent = positions[name];
            return ent.offset;
        }
        public Stream BaseStream
        {
            get { return stream; }
        }
    }

    /// <summary>
    ///   定义一种由若干“键”—“值”组成的集合的存储方式  的写入器
    /// </summary>
    public unsafe class BinaryDataWriter : IDisposable
    {
        class Entry
        {
            public string name;
            public System.IO.MemoryStream buffer;

            public Entry(string name)
            {
                this.name = name;
                buffer = new System.IO.MemoryStream();
            }

        }

        bool disposed;
        Dictionary<string, Entry> positions = new Dictionary<string, Entry>();
        byte[] buffer = new byte[sizeof(decimal)];

        public ContentBinaryWriter AddEntry(string name)
        {
            Entry ent = new Entry(name);
            positions.Add(name, ent);
            return new ContentBinaryWriter(new VirtualStream(ent.buffer, 0));
        }
        public Stream AddEntryStream(string name)
        {
            Entry ent = new Entry(name);
            positions.Add(name, ent);
            return new VirtualStream(ent.buffer, 0);
        }

        //public int GetSize()
        //{
        //    int size = 0;
        //    Dictionary<string, Entry>.ValueCollection vals = positions.Values;
        //    foreach (Entry e in vals)
        //    {
        //        size += (int)e.buffer.Length;
        //    }
        //    return size;
        //}

        public void AddEntry(string name, int value)
        {
            Entry ent = new Entry(name);
            positions.Add(name, ent);

            buffer[0] = (byte)value;
            buffer[1] = (byte)(value >> 8);
            buffer[2] = (byte)(value >> 16);
            buffer[3] = (byte)(value >> 24);

            ent.buffer.Write(buffer, 0, sizeof(int));
        }
        public void AddEntry(string name, uint value)
        {
            Entry ent = new Entry(name);
            positions.Add(name, ent);

            buffer[0] = (byte)value;
            buffer[1] = (byte)(value >> 8);
            buffer[2] = (byte)(value >> 16);
            buffer[3] = (byte)(value >> 24);

            ent.buffer.Write(buffer, 0, sizeof(uint));
        }
        public void AddEntry(string name, short value)
        {
            Entry ent = new Entry(name);
            positions.Add(name, ent);

            buffer[0] = (byte)value;
            buffer[1] = (byte)(value >> 8);

            ent.buffer.Write(buffer, 0, sizeof(short));
        }
        public void AddEntry(string name, ushort value)
        {
            Entry ent = new Entry(name);
            positions.Add(name, ent);

            buffer[0] = (byte)value;
            buffer[1] = (byte)(value >> 8);

            ent.buffer.Write(buffer, 0, sizeof(ushort));
        }
        public void AddEntry(string name, long value)
        {
            Entry ent = new Entry(name);
            positions.Add(name, ent);

            buffer[0] = (byte)value;
            buffer[1] = (byte)(value >> 8);
            buffer[2] = (byte)(value >> 16);
            buffer[3] = (byte)(value >> 24);
            buffer[4] = (byte)(value >> 32);
            buffer[5] = (byte)(value >> 40);
            buffer[6] = (byte)(value >> 48);
            buffer[7] = (byte)(value >> 56);

            ent.buffer.Write(buffer, 0, sizeof(long));
        }
        public void AddEntry(string name, ulong value)
        {
            Entry ent = new Entry(name);
            positions.Add(name, ent);

            buffer[0] = (byte)value;
            buffer[1] = (byte)(value >> 8);
            buffer[2] = (byte)(value >> 16);
            buffer[3] = (byte)(value >> 24);
            buffer[4] = (byte)(value >> 32);
            buffer[5] = (byte)(value >> 40);
            buffer[6] = (byte)(value >> 48);
            buffer[7] = (byte)(value >> 56);

            ent.buffer.Write(buffer, 0, sizeof(ulong));
        }
        public void AddEntry(string name, float value)
        {
            Entry ent = new Entry(name);
            positions.Add(name, ent);

            uint num = *((uint*)&value);
            buffer[0] = (byte)num;
            buffer[1] = (byte)(num >> 8);
            buffer[2] = (byte)(num >> 16);
            buffer[3] = (byte)(num >> 24);

            ent.buffer.Write(buffer, 0, sizeof(float));
        }
        public void AddEntry(string name, double value)
        {
            Entry ent = new Entry(name);
            positions.Add(name, ent);

            ulong num = *((ulong*)&value);
            buffer[0] = (byte)num;
            buffer[1] = (byte)(num >> 8);
            buffer[2] = (byte)(num >> 16);
            buffer[3] = (byte)(num >> 24);
            buffer[4] = (byte)(num >> 32);
            buffer[5] = (byte)(num >> 40);
            buffer[6] = (byte)(num >> 48);
            buffer[7] = (byte)(num >> 56);

            ent.buffer.Write(buffer, 0, sizeof(float));
        }
        public void AddEntry(string name, bool value)
        {
            Entry ent = new Entry(name);
            positions.Add(name, ent);

            buffer[0] = value ? ((byte)1) : ((byte)0);

            ent.buffer.Write(buffer, 0, sizeof(bool));
        }

        public ContentBinaryWriter GetData(string name)
        {
            Entry ent = positions[name];
            return new ContentBinaryWriter(new VirtualStream(ent.buffer, 0));
        }

        public void SetData(string name, int value)
        {
            Entry ent = positions[name];

            buffer[0] = (byte)value;
            buffer[1] = (byte)(value >> 8);
            buffer[2] = (byte)(value >> 16);
            buffer[3] = (byte)(value >> 24);

            ent.buffer.Position = 0;
            ent.buffer.Write(buffer, 0, sizeof(int));
        }
        public void SetData(string name, uint value)
        {
            Entry ent = positions[name];

            buffer[0] = (byte)value;
            buffer[1] = (byte)(value >> 8);
            buffer[2] = (byte)(value >> 16);
            buffer[3] = (byte)(value >> 24);

            ent.buffer.Position = 0;
            ent.buffer.Write(buffer, 0, sizeof(uint));
        }
        public void SetData(string name, short value)
        {
            Entry ent = positions[name];

            buffer[0] = (byte)value;
            buffer[1] = (byte)(value >> 8);

            ent.buffer.Position = 0;
            ent.buffer.Write(buffer, 0, sizeof(short));
        }
        public void SetData(string name, ushort value)
        {
            Entry ent = positions[name];

            buffer[0] = (byte)value;
            buffer[1] = (byte)(value >> 8);

            ent.buffer.Position = 0;
            ent.buffer.Write(buffer, 0, sizeof(ushort));
        }
        public void SetData(string name, long value)
        {
            Entry ent = positions[name];

            buffer[0] = (byte)value;
            buffer[1] = (byte)(value >> 8);
            buffer[2] = (byte)(value >> 16);
            buffer[3] = (byte)(value >> 24);
            buffer[4] = (byte)(value >> 32);
            buffer[5] = (byte)(value >> 40);
            buffer[6] = (byte)(value >> 48);
            buffer[7] = (byte)(value >> 56);

            ent.buffer.Position = 0;
            ent.buffer.Write(buffer, 0, sizeof(long));
        }
        public void SetData(string name, ulong value)
        {
            Entry ent = positions[name];

            buffer[0] = (byte)value;
            buffer[1] = (byte)(value >> 8);
            buffer[2] = (byte)(value >> 16);
            buffer[3] = (byte)(value >> 24);
            buffer[4] = (byte)(value >> 32);
            buffer[5] = (byte)(value >> 40);
            buffer[6] = (byte)(value >> 48);
            buffer[7] = (byte)(value >> 56);

            ent.buffer.Position = 0;
            ent.buffer.Write(buffer, 0, sizeof(ulong));
        }
        public void SetData(string name, float value)
        {
            Entry ent = positions[name];

            uint num = *((uint*)&value);
            buffer[0] = (byte)num;
            buffer[1] = (byte)(num >> 8);
            buffer[2] = (byte)(num >> 16);
            buffer[3] = (byte)(num >> 24);

            ent.buffer.Position = 0;
            ent.buffer.Write(buffer, 0, sizeof(float));
        }
        public void SetData(string name, double value)
        {
            Entry ent = positions[name];

            ulong num = *((ulong*)&value);
            buffer[0] = (byte)num;
            buffer[1] = (byte)(num >> 8);
            buffer[2] = (byte)(num >> 16);
            buffer[3] = (byte)(num >> 24);
            buffer[4] = (byte)(num >> 32);
            buffer[5] = (byte)(num >> 40);
            buffer[6] = (byte)(num >> 48);
            buffer[7] = (byte)(num >> 56);


            ent.buffer.Position = 0;
            ent.buffer.Write(buffer, 0, sizeof(float));
        }
        public void SetData(string name, bool value)
        {
            Entry ent = positions[name];

            buffer[0] = value ? ((byte)1) : ((byte)0);

            ent.buffer.Position = 0;
            ent.buffer.Write(buffer, 0, sizeof(bool));
        }


        public void Save(Stream stm)
        {
            ContentBinaryWriter bw = new ContentBinaryWriter(stm, Encoding.Default);

            bw.Write(positions.Count);

            foreach (KeyValuePair<string, Entry> e in positions)
            {
                bw.WriteStringUnicode(e.Key);
                bw.Write((int)e.Value.buffer.Length);
                bw.Flush();
                e.Value.buffer.WriteTo(stm);
            }
            bw.Close();
        }

        #region IDisposable 成员

        public void Dispose()
        {
            if (!disposed)
            {
                foreach (KeyValuePair<string, Entry> e in positions)
                {
                    e.Value.buffer.Dispose();
                }
                positions.Clear();
                disposed = true;
            }
            else
            {
                throw new ObjectDisposedException(this.ToString());
            }
        }

        #endregion

        ~BinaryDataWriter()
        {
            if (!disposed)
                Dispose();
        }
    }
}
