using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using VirtualBicycle.Ide.Tools;
using VirtualBicycle.IO;
using WeifenLuo.WinFormsUI.Docking;

namespace VirtualBicycle.Ide.Designers
{
    public delegate void PropertyUpdateHandler(object sender, object[] allObjects);
    public delegate void SaveStateChangedHandler(object sender);
    public delegate void ToolBoxItemsChangedHandler(ToolBoxItem[] items, ToolBoxCategory[] cates);

    public class DocumentBase : DockContent, IDocument
    {
        protected PropertyUpdateHandler propertyUpdated;
        protected SaveStateChangedHandler saveChanged;
        protected ToolBoxItemsChangedHandler tbitemsChanged;

        bool activated;

        [Browsable(false)]
        [ReadOnly(true)]
        public event ToolBoxItemsChangedHandler TBItemsChanged
        {
            add { tbitemsChanged += value; }
            remove { tbitemsChanged -= value; }
        }

        protected virtual void OnTBItemsChanged(ToolBoxItem[] items, ToolBoxCategory[] cates)
        {
            if (tbitemsChanged != null)
            {
                tbitemsChanged(items, cates);
            }
        }


        /// <summary>
        /// 更新属性窗格
        /// </summary>
        [Browsable(false)]
        [ReadOnly(true)]
        public event PropertyUpdateHandler PropertyUpdate
        {
            add { propertyUpdated += value; }
            remove { propertyUpdated -= value; }
        }

        protected virtual void OnPropertyUpdated(object[] objects)
        {
            if (propertyUpdated != null)
            {
                propertyUpdated(this, objects);
            }
        }

        [Browsable(false)]
        [ReadOnly(true)]
        public event SaveStateChangedHandler SavedStateChanged
        {
            add { saveChanged += value; }
            remove { saveChanged -= value; }
        }

        public virtual Icon GetIcon()
        {
            return Program.DefaultIcon;
        }

        [Browsable(false)]
        [EditorBrowsable(EditorBrowsableState.Never)]
        [ReadOnly(true)]
        public virtual DesignerAbstractFactory Factory
        {
            get { throw new NotImplementedException(); }
        }
        public virtual bool LoadRes()
        {
            throw new NotImplementedException();
        }
        public virtual bool SaveRes()
        {
            throw new NotImplementedException();
        }
        [Browsable(false)]
        [EditorBrowsable(EditorBrowsableState.Never)]
        [ReadOnly(true)]
        public virtual ToolStrip[] ToolStrips
        {
            get { throw new NotImplementedException(); }
        }
        [Browsable(false)]
        [EditorBrowsable(EditorBrowsableState.Never)]
        [ReadOnly(true)]
        public virtual ResourceLocation ResourceLocation
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }
        [Browsable(false)]
        [EditorBrowsable(EditorBrowsableState.Never)]
        [ReadOnly(true)]
        public virtual bool IsReadOnly
        {
            get { throw new NotImplementedException(); }
        }
        [Browsable(false)]
        [EditorBrowsable(EditorBrowsableState.Never)]
        [ReadOnly(true)]
        public virtual bool Saved
        {
            get { throw new NotImplementedException(); }
            protected set
            {
                throw new NotImplementedException();
            }
        }

        protected virtual void active() { }
        protected virtual void deactive() { }

        void IDocument.DocActivate()
        {
            if (!activated)
            {
                active();
                activated = true;
            }
        }

        void IDocument.DocDeactivate()
        {
            if (activated)
            {
                deactive();
                activated = false;
            }
        }
        [Browsable(false)]
        [EditorBrowsable(EditorBrowsableState.Never)]
        [ReadOnly(true)]
        bool IDocument.IsActivated
        {
            get { return activated; }
        }
    }
}
