using System;
using System.Collections.Generic;
using System.Text;
using Apoc.IO;

namespace Apoc.RenderSystem
{
    public unsafe class DataStream : MemoryStream
    {
        bool writable = true;


        public DataStream(IntPtr ptr, int size, bool @readonly)
            : this(ptr.ToPointer(), size, @readonly)
        { }

        public DataStream(IntPtr ptr, int size)
            : this(ptr.ToPointer(), size)
        { }

        public DataStream(void* ptr, int size, bool @readonly)
            : base(ptr, size)
        {
            writable = !@readonly;
        }

        public DataStream(void* ptr, int size)
            : base(ptr, size)
        { 
        }


        public IntPtr DataPointer
        {
            get { return InternalPointer; }
        }
        
        public override bool CanWrite
        {
            get
            {
                return writable;
            }
        }

        public override void Write(byte[] buffer, int offset, int count)
        {
            if (writable)
            {
                base.Write(buffer, offset, count);
            }
            else
            {
                throw new InvalidOperationException();
            }
        }
        public override void WriteByte(byte value)
        {
            if (writable)
            {
                base.WriteByte(value);
            }
            else
            {
                throw new InvalidOperationException();
            }
        }
    }
}
