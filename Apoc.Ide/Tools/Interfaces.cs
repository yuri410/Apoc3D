using System;
using System.Collections.Generic;
using System.Text;
using WeifenLuo.WinFormsUI.Docking;

namespace VirtualBicycle.Ide.Tools
{
    public interface ITool
    {
        DockContent Form { get; }
        bool IsVisibleInMenu { get; }
    }

    public interface IToolAbstractFactory
    {
        ITool CreateInstance();
        Type CreationType { get; }
    }
}
