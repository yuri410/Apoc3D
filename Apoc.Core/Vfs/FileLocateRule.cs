using System;
using System.Collections.Generic;
using System.Text;

namespace Apoc.Vfs
{
    /// <summary>
    ///  检查点包括若干路径
    ///  
    ///  查找文件时逐项检查
    /// </summary>
    public class LocateCheckPoint
    {
        struct Entry
        {
            public string Path;
            public string ArchivePath;

            public Entry(string path, string ap)
            {
                Path = path;
                ArchivePath = ap;
            }
        }

        List<Entry> pathList = new List<Entry>();

        /// <summary>
        ///  添加检查路径
        /// </summary>
        /// <param name="path"></param>
        public void AddPath(string path)
        {
            List<string> fullPath;
            List<string> arcPath;

            if (FileSystem.Instance.Exists(path, out fullPath, out arcPath))
            {
                for (int i = 0; i < fullPath.Count; i++)
                {
                    pathList.Add(new Entry(fullPath[i], arcPath[i]));
                }
            }
        }

        public void Clear()
        {
            pathList.Clear();
        }

        /// <summary>
        ///  索引为index的检查路径是否指向一个文件包
        /// </summary>
        /// <param name="index"></param>
        /// <returns></returns>
        public bool HasArchivePath(int index)
        {
            return !string.IsNullOrEmpty(pathList[index].ArchivePath);
        }
        /// <summary>
        ///  获得索引为index的检查路径
        /// </summary>
        /// <param name="index"></param>
        /// <returns></returns>
        public string GetPath(int index)        
        {
            return pathList[index].Path;
        }

        public string GetArchivePath(int index)
        {
            return pathList[index].ArchivePath;
        }

        /// <summary>
        ///  获得路径的数量
        /// </summary>
        public int Count
        {
            get { return pathList.Count; }
        }


        public bool SearchesCurrectArchiveSet
        {
            get;
            set;
        }
    }


    /// <summary>
    ///  定义查找文件的规则，就是检查目录或者文件包的次序
    /// </summary>
    public class FileLocateRule
    {
        List<LocateCheckPoint> pathChkPt;

       
        public FileLocateRule()
        {
            pathChkPt = new List<LocateCheckPoint>();
        }


        public FileLocateRule(LocateCheckPoint[] checkPoints)
        {
            pathChkPt = new List<LocateCheckPoint>(checkPoints.Length + 1);
            for (int i = 0; i < checkPoints.Length; i++)
            {
                pathChkPt.Add(checkPoints[i]);
            }
        }

        public void AddCheckPoint(LocateCheckPoint coll)
        {
            pathChkPt.Add(coll);
        }

        public int Count
        {
            get { return pathChkPt.Count; }
        }
        public LocateCheckPoint this[int index]
        {
            get { return pathChkPt[index]; }
        }
    }
}
