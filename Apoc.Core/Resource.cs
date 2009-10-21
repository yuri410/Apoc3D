using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;
using System.IO;
using Apoc.Vfs;

namespace Apoc.Core
{
    /// <summary>
    ///  定义一个资源的状态
    /// </summary>
    public enum ResourceState
    {
        /// <summary>
        ///  未加载
        /// </summary>
        Unloaded,
        /// <summary>
        ///  正在加载
        /// </summary>
        Loading,        
        /// <summary>
        ///  已经加载
        /// </summary>
        Loaded,
        /// <summary>
        ///  正在卸载
        /// </summary>
        Unloading
    }

    /// <summary>
    ///  表示一个资源，如纹理、模型等
    /// </summary>
    /// <remarks>
    /// 资源分两种，引用和实体
    /// 引用引用着资源实体
    /// 引用不受管理，但使用时会影响实体的状态
    /// 资源管理器根据实体资源的状态对其进行管理
    /// </remarks>
    public abstract class Resource : IDisposable
    {
        ResourceState resState;
        TimeSpan creationTime;
        TimeSpan lastAccessed;
        int accessTimes;

        float useFrequency;

        ResourceManager manager;

        string hashString;
        //int hashCode;

        bool isUnmanaged;

        Resource resourceEntity;

        int entityRefCount;

        protected CacheMemory cacheMem;

        object syncHelper = new object();

        /// <summary>
        /// 如果是资源实体，获取资源实体的引用计数
        /// </summary>
        [Browsable(false)]
        public int ReferenceCount
        {
            get { return entityRefCount; }
            set { entityRefCount = value; }
        }
        /// <summary>
        ///  如果是引用，获取资源实体
        /// </summary>
        [Browsable(false)]
        public Resource ResourceEntity
        {
            get { return resourceEntity; }
        }

        /// <summary>
        /// 所有资源的名称都统一用该方法计算哈希代码
        /// </summary>
        /// <param name="name"></param>
        /// <returns></returns>
        public static int GetHashCode(string name)
        {
            return name.ToUpperInvariant().GetHashCode();
        }


        public bool HasCache
        {
            get { return cacheMem.ResourceLocation != null; }
        }

        public virtual void SetCache(CacheMemory rl)
        {
            Stream stm = rl.ResourceLocation.GetStream;
            WriteCacheData(new VirtualStream(stm));

            cacheMem = rl;
        }

        public CacheMemory GetCache()
        {
            return cacheMem;
        }

        public virtual void ResetCache()
        {
            cacheMem = new CacheMemory();
        }

        public abstract void ReadCacheData(Stream stream);
        public abstract void WriteCacheData(Stream stream);

        /// <summary>
        ///  获取该资源的状态
        /// </summary>
        [Browsable(false)]
        public ResourceState State
        {
            get
            {
                lock (syncHelper)
                    return resState;
            }
            private set
            {
                lock (syncHelper)
                    resState = value;
            }
        }

        public override int GetHashCode()
        {
            if (hashString != null)
            {
                return GetHashCode(hashString);
            }
            return base.GetHashCode();
        }

        [Browsable(false)]
        public string HashString
        {
            get { return hashString; }
        }

        /// <summary>
        ///  不受管理
        /// </summary>
        protected Resource()
        {
            IsUnmamaged = true;
        }

        /// <summary>
        ///  获取一个System.Boolean，表示该资源是否允许动态加载/卸载
        /// </summary>
        [Browsable(false)]
        public bool AllowDynamicLoading
        {
            get;
            protected set;
        }

        /// <summary>
        ///  创建引用，不受管理
        /// </summary>
        protected Resource(ResourceManager manager, Resource resourceEntity)
        {
            this.hashString = string.Empty;
            this.resourceEntity = resourceEntity;

            this.IsUnmamaged = true;

            this.resourceEntity.ReferenceCount++;
        }

        protected Resource(ResourceManager manager, string hashString)
        {
            this.AllowDynamicLoading = true;
            this.hashString = hashString;
            this.manager = manager;
        }


        protected Resource(ResourceManager manager, string hashString, bool allowdl)
        {
            this.AllowDynamicLoading = allowdl;
            this.hashString = hashString;
            this.manager = manager;
        }

        /// <summary>
        ///  获得管理该资源的资源管理器
        /// </summary>
        [Browsable(false)]
        public ResourceManager Manager
        {
            get { return manager; }
        }
        /// <summary>
        ///  获取该资源是否不受管理
        /// </summary>
        [Browsable(false)]
        public bool IsUnmamaged
        {
            get { return isUnmanaged; }
            private set { isUnmanaged = value; }
        }

        /// <summary>
        ///  获取一个System.Boolean，表示该资源是否是资源实体(ResourceEntity)
        /// </summary>
        [Browsable(false)]
        public bool IsResourceEntity
        {
            get { return resourceEntity == null; }
        }

        /// <summary>
        ///  获取一个System.Boolean，表示该资源是否已经加载
        /// </summary>
        [Browsable(false)]
        public bool IsLoaded
        {
            get { return resState == ResourceState.Loaded; }
        }

        /// <summary>
        ///  获取该资源的使用频率
        /// </summary>
        [Browsable(false)]
        public float UseFrequency
        {
            get { return useFrequency; }
        }

        /// <summary>
        ///  获取该资源所占内存的大小
        /// </summary>
        /// <returns></returns>
        public abstract int GetSize();

        /// <summary>
        ///  获取该资源的创建时间
        /// </summary>
        [Browsable(false)]
        public TimeSpan CreationTime
        {
            get { return creationTime; }
        }

        /// <summary>
        ///  获取上次访问该资源的时间
        /// </summary>
        [Browsable(false)]
        public TimeSpan LastAccessedTime
        {
            get { return lastAccessed; }
        }

        /// <summary>
        ///  加载资源的实现
        /// </summary>
        protected abstract void load();

        /// <summary>
        ///  释放资源的实现
        /// </summary>
        protected abstract void unload();

        /// <summary>
        ///  获取一个System.Boolean，表示该资源是否可被释放
        /// </summary>
        [Browsable(false)]
        public virtual bool IsUnloadable
        {
            get { return true; }
        }

        /// <summary>
        ///  调用该方法表示将要使用这个资源。让资源做好准备（比如已释放的要加载好）
        /// </summary>
        public void Use()
        {
            if (!IsResourceEntity)
            {
                resourceEntity.Use();
            }
            else
            {
                if (!isUnmanaged)
                {
                    lastAccessed = EngineTimer.TimeSpan;
                    accessTimes++;

                    float seconds = (float)((lastAccessed - creationTime).TotalSeconds);
                    useFrequency = seconds < 1 ? float.MaxValue : ((float)accessTimes) / seconds;

                    if (State == ResourceState.Unloaded)
                    {
                        manager.Manage();
                        Load();
                    }
                }
            }
        }

        /// <summary>
        ///  加载资源
        /// </summary>
        public void Load()
        {            
            if (IsResourceEntity)
            {
                if (!isUnmanaged)
                {
                    creationTime = EngineTimer.TimeSpan;
                    State = ResourceState.Loading;

                    if (HasCache)
                    {
                        Stream stream = cacheMem.ResourceLocation.GetStream;
                        ReadCacheData(new VirtualStream(stream));
                    }
                    else
                    {
                        load();
                    }

                    manager.NotifyResourceLoaded(this);
                    State = ResourceState.Loaded;
                }
            }
        }

        /// <summary>
        ///  释放资源
        /// </summary>
        public void Unload()
        {
            if (IsResourceEntity)
            {
                if (!isUnmanaged)
                {
                    State = ResourceState.Unloading;

                    if (HasCache)
                    {
                        Stream stream = cacheMem.ResourceLocation.GetStream;
                        WriteCacheData(new VirtualStream(stream));
                        unload();
#warning review
                    }
                    else
                    {
                        unload();
                    }

                    //manager.NotifyResourceUnloaded(this);
                    State = ResourceState.Unloaded;
                }
            }
        }
        public void Reload()
        {
            if (IsResourceEntity)
            {
                if (!isUnmanaged)
                {
                    if (IsLoaded)
                    {
                        Unload();
                        Load();
                    }
                }
            }
        }


        /// <summary>
        ///  销毁资源
        /// </summary>
        /// <param name="disposing">是否释放</param>
        protected virtual void Dispose(bool disposing)
        {
            if (disposing && manager != null)
            {
                manager.NotifyResourceFinalizing(this);
            }
        }
        /// <summary>
        ///  获取一个System.Boolean，表示该资源是否已经销毁
        /// </summary>
        [Browsable(false)]
        public bool Disposed
        {
            get;
            private set;
        }


        #region IDisposable 成员

        /// <summary>
        ///  销毁资源
        /// </summary>
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

        #endregion

        ~Resource()
        {
            if (!Disposed)
            {
                Dispose(false);
                Disposed = true;
            }
        }

    }

}
