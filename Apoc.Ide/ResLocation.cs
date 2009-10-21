using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using VirtualBicycle.Ide.Designers;
using VirtualBicycle.IO;

namespace VirtualBicycle.Ide
{
    public unsafe class DevFileLocation : FileLocation
    {
        public DevFileLocation(string filePath)
            : base(filePath, 0)
        {
            if (File.Exists(filePath))
            {
                size = (int)new FileInfo(filePath).Length;
            }
        }

        public override Stream GetStream
        {
            get { return new FileStream(base.Path, FileMode.OpenOrCreate, FileAccess.ReadWrite, FileShare.ReadWrite); }
        }

        public override bool IsReadOnly
        {
            get { return false; }
        }

    }
    public unsafe class DevMemoryLocation : MemoryLocation
    {
        DocumentBase parent;

        public DevMemoryLocation(DocumentBase parent, void* data, int size) :
            base(data, size)        
        {
            this.parent = parent;
        }


    }
 
}
