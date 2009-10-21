using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using VirtualBicycle.IO;

namespace VirtualBicycle.Ide.Designers
{
    public class GeneralDocumentBase : DocumentBase
    {
        DesignerAbstractFactory factory;

        ResourceLocation resLoc;

        bool saved;

        protected virtual string NewFileName
        {
            get { return "新文件"; }
        }


        protected void Init(DesignerAbstractFactory fac, ResourceLocation rl)
        {
            factory = fac;
            resLoc = rl;

            if (resLoc != null)
            {
                this.Text = resLoc.ToString();

                if (resLoc is FileLocation)
                    this.Text = Path.GetFileName(this.Text);

                if (resLoc.IsReadOnly)
                    this.Text += "只读";
            }
            else
            {
                this.Text = NewFileName;
            }

            this.TabText = this.Text;

        }

        public override bool IsReadOnly
        {
            get { return resLoc == null ? false : resLoc.IsReadOnly; }
        }

        public override int GetHashCode()
        {
            if (resLoc != null && resLoc.Name != null)
            {
                return resLoc.GetHashCode();
            }
            return 0;
        }
        public override string ToString()
        {
            if (resLoc != null)
                return resLoc.ToString();
            return Text;
        }
        public override ResourceLocation ResourceLocation
        {
            get { return resLoc; }
            set { resLoc = value; }
        }
        public override bool Saved
        {
            get { return saved; }
            protected set
            {
                if (value != saved)
                {
                    saved = value;
                    if (saveChanged != null)
                    {
                        saveChanged(this);
                    }
                }
            }
        }
        public override DesignerAbstractFactory Factory
        {
            get { return factory; }
        }
    }
}
