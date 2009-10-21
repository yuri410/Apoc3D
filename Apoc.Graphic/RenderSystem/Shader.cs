using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace Apoc.RenderSystem
{
    /// <summary>
    /// Describes the location for the include file.
    /// </summary>
    public enum IncludeType
    {
        /// <summary>
        /// Look in the local project for the include file.
        /// </summary>
        Local = 0,
        /// <summary>
        /// Look in the system path for the include file.
        /// </summary>
        System = 1
    }

    public interface Include
    {
        void Close(Stream stream);
        void Open(IncludeType includeType, string fileName, out Stream stream);
    }

    public struct Macro
    {
        string name;
        string defination;

        public string Name
        {
            get { return name; }
        }
        public string Defination
        {
            get { return defination; }
        }
        public Macro(string name, string def)
        {
            this.name = name;
            this.defination = def;
        }
    }


    public abstract class Shader : IDisposable
    {
        protected Shader(RenderSystem rs)
        {
            RenderSystem = rs;
        }

        public RenderSystem RenderSystem
        {
            get;
            private set;
        }


        public ConstantTable ConstantTable
        {
            get;
            protected set;
        }

        #region IDisposable 成员

        public bool Disposed
        {
            get;
            private set;
        }

        public virtual void Dispose(bool disposing)
        {
            RenderSystem = null;
            ConstantTable = null;
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

        ~Shader()
        {
            if (!Disposed)
            {
                Dispose(false);
                Disposed = true;
            }
        }
        #endregion
    }

    public abstract class VertexShader : Shader
    {
        protected VertexShader(RenderSystem rs)
            : base(rs)
        {
        }
    }

    public abstract class PixelShader : Shader
    {
        protected PixelShader(RenderSystem rs)
            : base(rs)
        {
        }
    }
}
