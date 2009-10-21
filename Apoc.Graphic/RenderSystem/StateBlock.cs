using System;
using System.Collections.Generic;
using System.Text;

namespace Apoc.RenderSystem
{
    public abstract class StateBlock : IDisposable
    {
        public RenderSystem RenderSystem
        {
            get;
            private set;
        }

        protected StateBlock(RenderSystem rs, StateBlockType type)
        {
            this.RenderSystem = rs;
            this.Type = type;
        }

        public StateBlockType Type
        {
            get;
            private set;
        }

        public abstract void Capture();
        public abstract void Apply();


        #region IDisposable 成员

        protected virtual void Dispose(bool disposing)
        {
            if (disposing)
            {
                this.RenderSystem = null;
            }
        }

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

        #endregion

        ~StateBlock()
        {
            if (!Disposed)
            {
                Dispose(false);
                Disposed = true;
            }
        }
    }
}
