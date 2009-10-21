using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;
using System.IO;
using Apoc.Vfs;

namespace Apoc.Core
{
    public abstract class ResourceManager
    {
        /// <summary>
        ///  受管理的资源哈希表
        /// </summary>
        Dictionary<string, Resource> hashTable;

        /// <summary>
        ///  受管理的资源的列表
        /// </summary>
        List<Resource> objects;

        /// <summary>
        ///  最大允许的缓存大小
        /// </summary>
        int totalCacheSize;

        /// <summary>
        ///  已经使用的缓存大小
        /// </summary>
        int curUsedCache;


        /// <summary>
        ///  记录管理资源的频率
        /// </summary>
        int manageFrequency;

        /// <summary>
        ///  记录管理资源的次数
        /// </summary>
        int manageTimes;


        /// <summary>
        ///  是否允许资源的动态加载
        /// </summary>
        bool allowDynLd = true;

        /// <summary>
        ///  以默认缓存大小创建一个资源管理器
        /// </summary>
        protected ResourceManager()
        {
            objects = new List<Resource>();
            hashTable = new Dictionary<string, Resource>();
            totalCacheSize = 8 * 1048576;  // 8mb
            manageFrequency = 4;
        }

        /// <summary>
        ///  以特定缓存大小创建一个资源管理器
        /// </summary>
        /// <param name="cacheSize">缓存大小</param>
        protected ResourceManager(int cacheSize)
        {
            objects = new List<Resource>();
            hashTable = new Dictionary<string, Resource>();
            totalCacheSize = cacheSize;
            manageFrequency = 4;
        }


        protected ResourceManager(int cacheSize, int manageFreq)
        {
            objects = new List<Resource>();
            hashTable = new Dictionary<string, Resource>();
            totalCacheSize = cacheSize;
            manageFrequency = manageFreq;
        }

        /// <summary>
        ///  获取或设置该资源管理器是否允许动态加载/释放
        /// </summary>
        public bool AllowDynamicLoading
        {
            get { return allowDynLd; }
            set { allowDynLd = value; }
        }

        /// <summary>
        ///  获取或设置最大允许的缓存大小
        /// </summary>
        public int TotalCacheSize
        {
            get { return totalCacheSize; }
            set
            {
                if (value < totalCacheSize)
                {
                    totalCacheSize = value;
                    Manage();
                }
                else
                {
                    totalCacheSize = value;
                }
            }
        }

        /// <summary>
        ///  获取已经使用的缓存大小
        /// </summary>
        public int UsedCacheSize
        {
            get { return curUsedCache; }
        }

        #region ICachedObjectManager 成员

        /// <summary>
        ///  比较器
        /// </summary>
        /// <param name="a"></param>
        /// <param name="b"></param>
        /// <returns></returns>
        int Comparison(Resource a, Resource b)
        {
            return a.UseFrequency.CompareTo(b.UseFrequency);
        }

        /// <summary>
        ///  管理资源，将不常使用的销毁掉以释放内存
        /// </summary>
        public void Manage()
        {
            if (allowDynLd)
            {
                manageTimes++;

                if (manageTimes >= manageFrequency)
                {
                    manageTimes = 0;
                    if (curUsedCache > totalCacheSize)
                    {
                        objects.Sort(Comparison);

                        int oc = objects.Count;
                        int k = oc - 1;
                        while (curUsedCache > totalCacheSize && k >= 0)
                        {
                            if (objects[k].State == ResourceState.Loaded && objects[k].IsUnloadable)
                            {
                                objects[k].Unload();
                            }
                            k--;
                        }
                    }
                }
            }
        }

        /// <summary>
        ///  提示资源管理器一个资源已经加载
        /// </summary>
        /// <param name="res">资源</param>
        public void NotifyResourceLoaded(Resource res)
        {
            if (allowDynLd)
            {
                curUsedCache += res.GetSize();
            }
        }

        /// <summary>
        ///  提示资源管理器一个资源已经释放
        /// </summary>
        /// <param name="res">资源</param>
        public void NotifyResourceUnloaded(Resource res)
        {
            if (allowDynLd)
            {
                curUsedCache -= res.GetSize();
            }
        }

        /// <summary>
        ///  提示资源管理器一个资源已经销毁
        /// </summary>
        /// <param name="res">资源</param>
        public void NotifyResourceFinalizing(Resource res)
        {
            hashTable.Remove(res.HashString);
            objects.Remove(res);
        }

        #endregion


        /// <summary>
        ///  提示资源管理器已经创建了一个新资源，这个资源在创建时不考虑先前创建的资源
        /// </summary>
        /// <param name="res"></param>
        protected void NewCached(Resource res, CacheType ctype)
        {
            objects.Add(res);

            if (!res.AllowDynamicLoading)
            {
                allowDynLd = false;
            }

            int size = res.GetSize();

            CacheMemory cm;
            if (size == 0)
            {
                cm = Cache.Instance.Allocate();
            }
            else
            {
                if (ctype == CacheType.Dynamic)
                {
                    cm = Cache.Instance.Allocate(CacheType.Dynamic, size);
                }
                else
                {
                    cm = Cache.Instance.Allocate(CacheType.Static, size);
                }
            }
            res.SetCache(cm);
        }

        /// <summary>
        ///  提示资源管理器已经创建了一个新资源，将它放入管理范围
        /// </summary>
        /// <param name="res"></param>
        protected void NewResource(Resource res, CacheType ctype)
        {
            hashTable.Add(res.HashString, res);
            objects.Add(res);

            if (!res.AllowDynamicLoading)
            {
                allowDynLd = false;
            }

            int size = res.GetSize();

            CacheMemory cm;
            if (size == 0)
            {
                cm = Cache.Instance.Allocate();
            }
            else
            {
                if (ctype == CacheType.Dynamic)
                {
                    cm = Cache.Instance.Allocate(CacheType.Dynamic, size);
                }
                else
                {
                    EngineConsole.Instance.Write("正在为资源" + res.HashString + "分为静态缓存空间。");
                    cm = Cache.Instance.Allocate(CacheType.Static, size);
                }
            }
            res.SetCache(cm);
        }

        /// <summary>
        ///  销毁一个资源对象
        ///  即使该资源不受管理也照常销毁
        /// </summary>
        /// <param name="res"></param>
        protected void DestoryResource(Resource res)
        {
            if (res == null)
            {
                throw new ArgumentNullException("res");
            }


            if (!res.IsUnmamaged)
            {
                Cache.Instance.Release(res.GetCache());
                res.ResetCache();

                res.Dispose();
            }
            else
            {
                if (res.ResourceEntity != null)
                {
                    res.ResourceEntity.ReferenceCount--;
                    if (res.ResourceEntity.ReferenceCount == 0)
                    {
                        if (res.ResourceEntity.IsResourceEntity)
                        {
                            hashTable.Remove(res.ResourceEntity.HashString);
                            objects.Remove(res.ResourceEntity);
                        }
                        DestoryResource(res.ResourceEntity);
                    }
                }
                //res.Dispose(false);
            }
        }

        /// <summary>
        ///  检查一个资源
        /// </summary>
        /// <param name="hashcode">资源的哈希代码</param>
        /// <returns>如果找到，则返回该资源。否则返回null</returns>
        public Resource Exists(string hashString)
        {
            Resource res;
            if (hashTable.TryGetValue(hashString, out res))
            {
                return res;
            }
            return null;
        }


    }

}
