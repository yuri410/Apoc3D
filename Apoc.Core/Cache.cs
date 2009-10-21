using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using Apoc.Vfs;

namespace Apoc
{
    public enum CacheType
    {
        Static,
        Dynamic
    }

    public struct CacheMemory
    {
        public long Offset;
        public long Size;

        public int StartChunkId;
        public int ChunkCount;

        public CacheType Type;
        public string ExtFile;

        public ResourceLocation ResourceLocation;
    }

    public class CacheMemoryTable
    {
        #region 常量

        public const long ChunkSize = 512 * 1024;
        public const long ChunkCount = (int)((long)1048576 * 1024 * 2 / ChunkSize);

        public const long TotalMemory = ChunkSize * ChunkCount;

        #endregion

        bool[] useStat = new bool[ChunkCount];

        #region 方法

        public long AvailableMemory
        {
            get { return TotalMemory - MemoryUsed; }
            private set { MemoryUsed = TotalMemory - value; }
        }

        public long MemoryUsed
        {
            get;
            private set;
        }

        public bool this[int chunkId]
        {
            get { return useStat[chunkId]; }
        }

        #endregion

        #region 方法

        public bool IsUsed(int offset)
        {
            return useStat[offset / ChunkSize];
        }

        public bool IsAvailable(int startChunk, int count)
        {
            for (int i = 0; i < count; i++)
            {
                if (useStat[startChunk + i])
                {
                    return false;
                }
            }
            return true;
        }

        public void UseMemory(int offset, int size)
        {
            int chunkCount = (int)(size / ChunkSize);
            int rem = (int)(size % ChunkSize);
            if (rem > 0)
            {
                chunkCount++;
            }

            int startChunk = (int)(offset / ChunkSize);
            for (int i = 0; i < chunkCount; i++)
            {
                useStat[startChunk + i] = true;
            }
            MemoryUsed += chunkCount * ChunkSize;
        }

        public void UseMemoryC(int startChunk, int count)
        {
            for (int i = 0; i < count; i++)
            {
                useStat[startChunk + i] = true;
            }
            MemoryUsed += count * ChunkSize;
        }

        public void UnuseMemoryC(int startChunk, int count)
        {
            for (int i = 0; i < count; i++)
            {
                useStat[startChunk + i] = false;
            }
            MemoryUsed -= count * ChunkSize;
        }

        #endregion
    }

    /// <summary>
    ///  负责将临时不使用的资源 存到硬盘中，使用这些资源时再重新建立
    /// </summary>
    public class Cache : Singleton
    {
        static readonly string CacheFile = "cachefile.dat";

        readonly string CacheDir;

        #region 单例相关

        static Cache singleton;

        public static Cache Instance
        {
            get { return singleton; }
        }

        public static void Initialize()
        {
            singleton = new Cache();
        }

        #endregion

        #region 字段

        CacheMemoryTable table;
        FileStream fileStream;

        #endregion

        #region 方法

        private Cache()
        {
            CacheDir = Application.StartupPath;
            string path = Path.Combine(CacheDir, CacheFile);

            FileInfo fi = new FileInfo(path);
            if (!fi.Exists)
            {
                EngineConsole.Instance.Write("缓存文件丢失。重新建立……", ConsoleMessageType.Exclamation);

                FileStream fs = new FileStream(path, FileMode.OpenOrCreate, FileAccess.Write);
                fs.SetLength(CacheMemoryTable.TotalMemory);
                fs.Close();
            }
            else if (fi.Length != CacheMemoryTable.TotalMemory)
            {
                EngineConsole.Instance.Write("缓存文件损坏了。重新建立……", ConsoleMessageType.Exclamation);
                FileStream fs = new FileStream(path, FileMode.OpenOrCreate, FileAccess.Write);
                fs.SetLength(CacheMemoryTable.TotalMemory);
                fs.Close();
            }


            fileStream = new FileStream(path, FileMode.Open, FileAccess.ReadWrite);
            table = new CacheMemoryTable();

            EngineConsole.Instance.Write("缓存初始化完毕。总大小 " + Math.Round(CacheMemoryTable.TotalMemory / 1048576.0, 2).ToString() + "MB。", ConsoleMessageType.Information);


            string tmpDir = Path.Combine(CacheDir, Utils.TempDir);

            try
            {
                if (Directory.Exists(tmpDir))
                {
                    Directory.Delete(tmpDir, true);
                }
                Directory.CreateDirectory(tmpDir);
            }
            catch { }
        }

        public CacheMemory Allocate()
        {
            CacheMemory result;
            result.Offset = 0;
            result.Size = 0;
            result.StartChunkId = 0;
            result.ChunkCount = 0;
            result.Type = CacheType.Dynamic;
            result.ExtFile = Path.Combine(CacheDir, Utils.GetTempFileName());

            FileStream fs = new FileStream(result.ExtFile, FileMode.OpenOrCreate, FileAccess.ReadWrite);
            result.ResourceLocation = new StreamedLocation(fs);

            return result;
        }

        public CacheMemory Allocate(CacheType type, int size)
        {
            if (type == CacheType.Static)
            {
                int chunkCount = (int)(size / CacheMemoryTable.ChunkSize);
                int rem = (int)(size % CacheMemoryTable.ChunkSize);
                if (rem > 0)
                {
                    chunkCount++;
                }

                for (int i = 0; i < CacheMemoryTable.ChunkCount; i++)
                {
                    if (!table[i])
                    {
                        if (table.IsAvailable(i, chunkCount))
                        {
                            CacheMemory result;
                            result.Offset = i * CacheMemoryTable.ChunkSize;
                            result.Size = size;
                            result.StartChunkId = i;
                            result.ChunkCount = chunkCount;
                            result.Type = CacheType.Static;
                            result.ResourceLocation = new StreamedLocation(new VirtualStream(fileStream, result.Offset, size));
                            result.ExtFile = string.Empty;

                            table.UseMemoryC(i, chunkCount);

                            double totalMem = Math.Round(CacheMemoryTable.TotalMemory / 1048576.0, 2);
                            EngineConsole.Instance.Write("分配静态缓存空间中。 " +
                                Math.Round(table.MemoryUsed / 1048576.0, 2).ToString() + "MB / " + totalMem.ToString() + "MB 已经使用。", ConsoleMessageType.Information);
                            return result;
                        }
                    }
                }

                throw new OutOfMemoryException();
            }
            else
            {
                CacheMemory result;
                result.Offset = 0;
                result.Size = size;
                result.StartChunkId = 0;
                result.ChunkCount = 0;
                result.Type = CacheType.Dynamic;
                result.ExtFile = Path.Combine(CacheDir, Utils.GetTempFileName());

                FileStream fs = new FileStream(result.ExtFile, FileMode.OpenOrCreate, FileAccess.ReadWrite);
                result.ResourceLocation = new StreamedLocation(fs);

                return result;
            }
        }

        public CacheMemory Allocate(int size)
        {
            return Allocate(CacheType.Static, size);
        }

        public void Release(CacheMemory cm)
        {
            if (cm.Type == CacheType.Static)
            {
                cm.ResourceLocation.GetStream.Close();
                table.UnuseMemoryC(cm.StartChunkId, cm.ChunkCount);
            }
            else
            {
                cm.ResourceLocation.GetStream.Close();
                File.Delete(cm.ExtFile);
            }
        }

        protected override void dispose()
        {
            throw new NotImplementedException();
        }

        #endregion
    }
}
