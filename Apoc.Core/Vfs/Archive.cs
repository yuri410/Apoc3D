using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Runtime.InteropServices;

namespace Apoc.Vfs
{
    /// <summary>
    ///  为文件包定义抽象基类
    /// </summary>
    public abstract class Archive : FileBase
    {
        protected Archive(string file, int size, bool isinArchive)
            : base(file, size, isinArchive)
        { }

        public abstract Dictionary<uint, ArchiveFileEntry> Files
        {
            get;
        }
        public abstract int FileCount
        {
            get;
        }
        public abstract bool Find(string file, out ArchiveFileEntry entry);

        public abstract Stream ArchiveStream
        {
            get;
        }

        public abstract void Dispose(bool disposing);

        public bool Disposed
        {
            get;
            private set;
        }

        public void Dispose()
        {
            if (!Disposed)
            {
                Dispose(true);

                Disposed = true;
            }
            else
            {
                throw new ObjectDisposedException(ToString());
            }
        }
        
    }


    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct ArchiveFileEntry
    {
        public uint id;
        public int offset;
        public int size;

        public override string ToString()
        {
            StringBuilder sb = new StringBuilder(8);
            sb.Append("{ ID= ");
            sb.Append(Convert.ToString(id, 16));
            sb.Append(", Offset= ");
            sb.Append(offset.ToString());
            sb.Append('}');
            return sb.ToString();
        }
    }

    /// <summary>
    ///  为文件包定义抽象工厂
    /// </summary>
    public abstract class ArchiveFactory : IAbstractFactory<Archive, FileLocation>
    {

        #region IAbstractFactory<Archive,FileLocation> 成员

        public abstract Archive CreateInstance(string file);

        public abstract Archive CreateInstance(FileLocation fl);

        public abstract string Type
        {
            get;
        }

        #endregion
    }


    
}
