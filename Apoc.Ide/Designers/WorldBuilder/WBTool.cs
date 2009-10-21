using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using VirtualBicycle.Ide.Editors.EditableObjects;

namespace VirtualBicycle.Ide.Designers.WorldBuilder
{
    abstract class WBTool
    {
        public bool RequiresRedraw
        {
            get;
            set;
        }

        protected WBTool(WorldDesigner wb, EditableGameScene data)
        {
            this.Scene = data;
            this.WorldBuilder = wb;
        }

        public WorldDesigner WorldBuilder
        {
            get;
            private set;
        }

        public EditableGameScene Scene
        {
            get;
            private set;
        }

        public virtual void Activate() { }
        public virtual void Deactivate() { }

        public abstract void NotifyMouseDown(MouseEventArgs e);
        public abstract void NotifyMouseUp(MouseEventArgs e);
        public abstract void NotifyMouseMove(MouseEventArgs e);
        public abstract void NotifyMouseClick(MouseEventArgs e);
        public abstract void NotifyMouseDoubleClick(MouseEventArgs e);
        public abstract void NotifyMouseWheel(MouseEventArgs e);

        public abstract void Render();

    }
}
