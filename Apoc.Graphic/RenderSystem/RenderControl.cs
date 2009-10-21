using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using SD = System.Drawing;

namespace Apoc.RenderSystem
{
    public abstract class RenderControl : IDisposable
    {
        RenderTarget renderTarget;

        PresentParameters presentParams;
        RenderSystem renderSystem;

        FpsCounter fpsCounter;
        Control control;

        protected RenderControl(RenderSystem rs, Control ctl, PresentParameters pm)
            : this(rs, pm)
        {
            this.control = ctl;
        }
        protected RenderControl(RenderSystem rs, Control ctl, PresentParameters pm, RenderTarget rt)
            : this(rs, pm, rt)
        {
            this.control = ctl;
        }
        protected RenderControl(RenderSystem rs, PresentParameters pm, RenderTarget rt)
            : this(rs, pm)
        {
            this.renderTarget = rt;
        }
        protected RenderControl(RenderSystem rs, PresentParameters pm)
        {
            this.presentParams = pm;
            this.renderSystem = rs;
            this.fpsCounter = new FpsCounter();
            this.control = pm.Control;

        }

        public Control TargetControl
        {
            get { return control; }
        }
        public RenderTarget RenderTarget
        {
            get { return renderTarget; }
        }

        protected abstract RenderTarget CreateRenderTarget(RenderSystem rs, PresentParameters pm);

        public virtual void Present()
        {
            fpsCounter.Update();
        }

        public float FPS
        {
            get { return fpsCounter.FPS; }
        }

        public void NotifyBind()
        {
            if (renderTarget == null)
            {
                renderTarget = CreateRenderTarget(renderSystem, presentParams);
            }
        }

        #region IDisposable 成员

        public bool Disposed
        {
            get;
            private set;
        }


        protected virtual void Dispose(bool disposing)
        {
            if (disposing)
            {

            }
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

        ~RenderControl()
        {
            if (!Disposed)
            {
                Dispose(false);
                Disposed = false;
            }
        }

        #endregion
    }

    public abstract class RenderWindow : RenderControl
    {
        public class RenderForm : Form
        {
            public RenderForm(SD.Size size)
            {
                this.ClientSize = size;
                this.SetStyle(ControlStyles.AllPaintingInWmPaint | ControlStyles.Opaque, true);
            }
            //public IntPtr Handle
            //{
            //    get { return base.Handle; }
            //}
            
        }

        protected RenderWindow(RenderSystem rs, Form form, PresentParameters pm, RenderTarget rt)
            : base(rs, form, pm, rt)
        {
            this.form = form;
        }
        protected RenderWindow(RenderSystem rs, Form form, PresentParameters pm)
            : base(rs, form, pm)
        {
            this.form = form;
        }
        protected RenderWindow(RenderSystem rs, PresentParameters pm)
            : base(rs, new RenderForm(new SD.Size(pm.BackBufferWidth, pm.BackBufferHeight)), pm)
        {
            this.form = (Form)TargetControl;
        }

        Form form;

        public Form Window 
        {
            get { return form; }
        }
    }
}
