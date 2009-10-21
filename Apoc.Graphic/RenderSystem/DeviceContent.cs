using System;
using System.Collections.Generic;
using System.Text;
using Apoc.Collections;
using Apoc.Media;
using System.Windows.Forms;

namespace Apoc.RenderSystem
{
    public struct PresentParameters
    {
        bool isWindowed;
        bool isFullForm;

        int bbWidth;
        int bbHeight;

        PresentInterval presentInterval;

        DepthFormat depthFormat;

        PixelFormat pixelFormat;

        Control ctl;

        public Control Control
        {
            get { return ctl; }
            set { ctl = value; }
        }
        public PixelFormat BackBufferFormat
        {
            get { return pixelFormat; }
            set { pixelFormat = value; }
        }

        public DepthFormat DepthFormat
        {
            get { return depthFormat; }
            set { depthFormat = value; }
        }

        public PresentInterval PresentInterval
        {
            get { return presentInterval; }
            set { presentInterval = value; }
        }

        public bool IsWindowed
        {
            get { return isWindowed; }
            set { isWindowed = value; }
        }

        public bool IsFullForm
        {
            get { return isFullForm; }
            set { isFullForm = value; }
        }

        public int BackBufferWidth
        {
            get { return bbWidth; }
            set { bbWidth = value; }
        }

        public int BackBufferHeight
        {
            get { return bbHeight; }
            set { bbHeight = value; }
        }


    }

    public abstract class DeviceContent
    {
        FastList<RenderControl> renderVps;

        public bool SupportsRenderControl
        {
            get;
            private set;
        }

        protected DeviceContent(bool supportsRenderCtrl)
        {
            renderVps = new FastList<RenderControl>();
            SupportsRenderControl = supportsRenderCtrl;
        }

        protected abstract RenderControl create(PresentParameters pm);
     

        public RenderControl Create(PresentParameters pm)
        {
            RenderControl rc = create(pm);
            renderVps.Add(rc);
            return rc;
        }

        

        public void Destroy(RenderControl rc)
        {
            renderVps.Remove(rc);
            rc.Dispose();
        }

        public abstract RenderSystem RenderSystem
        {
            get;
        }
    }
}
