using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.Windows.Forms;

namespace VirtualBicycle.Ide.Tools
{
    public delegate void ToolBoxItemActivatedHandler();

    public abstract class ToolBoxItem
    {
        public abstract Image Icon
        {
            get;
        }

        public abstract string Name
        {
            get;
        }

        public abstract ToolBoxCategory Category
        {
            get;
        }

        public abstract void NotifyMouseDown(MouseEventArgs e);
        public abstract void NotifyMouseUp(MouseEventArgs e);
        public abstract void NotifyMouseMove(MouseEventArgs e);
        public abstract void NotifyMouseClick(MouseEventArgs e);
        public abstract void NotifyMouseDoubleClick(MouseEventArgs e);
        public abstract void NotifyMouseWheel(MouseEventArgs e);


        public event ToolBoxItemActivatedHandler Activated;

        public virtual void NotifyActivated()
        {
            if (Activated != null)
            {
                Activated();
            }
        }
    }
}
