using System;
using System.Collections.Generic;
using System.Text;

namespace Apoc.RenderSystem
{
    public abstract class HardwareBuffer : IDisposable
    {
       

        public BufferUsage Usage
        {
            get;
            private set;
        }

        public bool UseSystemMemory
        {
            get;
            private set;
        }

        public int Size
        {
            get;
            private set;
        }

        public bool IsLocked
        {
            get;
            private set;
        }

        protected int LockOffset
        {
            get;
            private set;
        }
        protected int LockSize
        {
            get;
            private set;
        }


        protected HardwareBuffer(BufferUsage usage, int sizeInBytes, bool useSysMem)
        {
            Usage = usage;
            Size = sizeInBytes;
            UseSystemMemory = useSysMem;
        }

        protected abstract IntPtr @lock(int offset, int size, LockMode mode);


        public IntPtr Lock(LockMode mode)
        {
            if (!IsLocked)
            {
                IntPtr res = @lock(0, Size, mode);
                IsLocked = true;
                return res;
            }
            throw new InvalidOperationException();
        }
        public unsafe DataStream LockStream(LockMode mode)
        {
            if (!IsLocked)
            {
                IntPtr res = @lock(0, Size, mode);
                IsLocked = true;
                return new DataStream(res.ToPointer(), Size, mode == LockMode.ReadOnly);
            }
            throw new InvalidOperationException();
        }
        public IntPtr Lock(int offset, int size, LockMode mode)
        {
            if (!IsLocked)
            {
                IntPtr res = @lock(offset, size, mode);
                IsLocked = true;
                return res;
            }
            throw new InvalidOperationException();
        }
        public unsafe DataStream LockStream(int offset, int size, LockMode mode)
        {
            if (!IsLocked)
            {
                IntPtr res = @lock(offset, size, mode);
                IsLocked = true;
                return new DataStream(res.ToPointer(), Size, mode == LockMode.ReadOnly);
            }
            throw new InvalidOperationException();
        }
        public void Unlock() 
        {
            if (IsLocked)
            {
                unlock();
                IsLocked = false;
            }
            throw new InvalidOperationException();
        }

        protected abstract void unlock();


        

        #region IDisposable 成员

        public bool Disposed
        {
            get;
            private set;
        }

        public abstract void Dispose(bool disposing);

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
    }
}
