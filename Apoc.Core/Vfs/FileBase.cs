using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace Apoc.Vfs
{
    public abstract class FileBase
    {

        protected bool isInArchive;
        protected int fileSize;
        protected string fileName;
        protected string filePath;

        protected FileBase(string file, int size, bool isinMix)
        {
            isInArchive = isinMix;
            fileName = Path.GetFileName(file);
            filePath = file;
            fileSize = size;
        }

        public bool IsInArchive
        {
            get { return isInArchive; }
        }
        public int FileSize
        {
            get { return fileSize; }
        }
        public string FileName
        {
            get { return fileName; }
        }
        public string FilePath
        {
            get { return filePath; }
        }

    }

}
