using System;
using System.Collections.Generic;
using System.Text;

namespace Apoc
{
    public abstract class Singleton : IDisposable
    {
        static Dictionary<Type, Singleton> singletons;

        static Singleton()
        {
            singletons = new Dictionary<Type, Singleton>();
        }

        public static int SingletonCount
        {
            get;
            private set;
        }

        public static Singleton GetSingleton(Type type)
        {
            return singletons[type];
        }

        public static void Destroy(Type type)
        {
            Singleton sgt = singletons[type];
            sgt.Dispose();
        }

        public static void DestoryAll() 
        {
            Dictionary<Type, Singleton>.ValueCollection vals = singletons.Values;

            foreach (Singleton s in vals)            
            {
                s.Dispose();
            }
        }


        protected Singleton()         
        {
            singletons.Add(this.GetType(), this);
            SingletonCount++;
        }


        #region IDisposable 成员

        public bool Disposed
        {
            get;
            private set;
        }

        protected abstract void dispose();

        public void Dispose()
        {
            if (!Disposed)
            {
                SingletonCount--;
                singletons.Remove(this.GetType());
                dispose();
                Disposed = true;
            }   
            else
            {
                throw new ObjectDisposedException(ToString());
            }
        }

        #endregion
    }

    //public abstract class Singleton<T> : IDisposable
    //    where T : class
    //{
    //    protected static T singleton;

    //    public static T Instance
    //    {
    //        get { return singleton; }
    //    }

    //    #region IDisposable 成员

    //    public abstract void Dispose(bool disposing);

    //    public bool Disposed
    //    {
    //        get;
    //        private set;
    //    }

    //    public void Dispose()
    //    {
    //        if (!Disposed)
    //        {
    //            Dispose(true);
    //            Destory();
    //            Disposed = true;
    //        }
    //        else
    //        {
    //            throw new ObjectDisposedException(ToString());
    //        }
    //    }

    //    #endregion

    //    public void Destory()
    //    {
    //        //singleton.Dispose();
    //        singleton = null;
    //    }
    //}


    //public abstract class Singleton<T> : IDisposable 
    //    where T : class
    //{
    //    public Singleton()
    //    {
    //        if (SingletonFactory.instance != null && !IntPtr.ReferenceEquals(this, SingletonFactory.instance))
    //            throw new Exception(String.Format("Cannot create instances of the {0} class. Use the static Instance property instead.", this.GetType().Name));
    //    }

    //    public virtual bool Initialize(params object[] args)
    //    {
    //        return true;
    //    }

    //    public static T Instance
    //    {
    //        get
    //        {
    //            try
    //            {
    //                return SingletonFactory.instance;
    //            }
    //            catch (TypeInitializationException)
    //            {
    //                throw new Exception("Singleton<T> sublasses must implement a private parameterless constructor.");
    //            }
    //        }
    //    }

    //    class SingletonFactory
    //    {
    //        static SingletonFactory()
    //        {

    //        }

    //        internal static T instance = (T)typeof(T).InvokeMember(typeof(T).Name,
    //                                                                  BindingFlags.CreateInstance | BindingFlags.Instance | BindingFlags.NonPublic,
    //                                                                  null, null, null);
    //    }

    //    public static void Destroy()
    //    {
    //        SingletonFactory.instance = null;
    //    }

    //    #region IDisposable Implementation


    //    #region isDisposed Property

    //    private bool _disposed = false;
    //    /// <summary>
    //    /// Determines if this instance has been disposed of already.
    //    /// </summary>
    //    protected bool isDisposed
    //    {
    //        get
    //        {
    //            return _disposed;
    //        }
    //        set
    //        {
    //            _disposed = value;
    //        }
    //    }

    //    #endregion isDisposed Property

    //    /// <summary>
    //    /// Class level dispose method
    //    /// </summary>
    //    /// <remarks>
    //    /// When implementing this method in an inherited class the following template should be used;
    //    /// protected override void dispose( bool disposeManagedResources )
    //    /// {
    //    /// 	if ( !isDisposed )
    //    /// 	{
    //    /// 		if ( disposeManagedResources )
    //    /// 		{
    //    /// 			// Dispose managed resources.
    //    /// 		}
    //    /// 
    //    /// 		// There are no unmanaged resources to release, but
    //    /// 		// if we add them, they need to be released here.
    //    /// 	}
    //    /// 	isDisposed = true;
    //    ///
    //    /// 	// If it is available, make the call to the
    //    /// 	// base class's Dispose(Boolean) method
    //    /// 	base.dispose( disposeManagedResources );
    //    /// }
    //    /// </remarks>
    //    /// <param name="disposeManagedResources">True if Unmanaged resources should be released.</param>
    //    protected virtual void dispose(bool disposeManagedResources)
    //    {
    //        if (!isDisposed)
    //        {
    //            if (disposeManagedResources)
    //            {
    //                Singleton<T>.Destroy();
    //            }

    //            // There are no unmanaged resources to release, but
    //            // if we add them, they need to be released here.
    //        }
    //        isDisposed = true;
    //    }

    //    public void Dispose()
    //    {
    //        dispose(false);
    //        GC.SuppressFinalize(this);
    //    }
    //    #endregion IDisposable Implementation

    //}
}
