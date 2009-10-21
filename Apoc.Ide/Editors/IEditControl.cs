using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms.Design;

namespace VBIDE.Editors
{
    public interface IEditControl<T>
    {
        T Value { get; set; }
        IWindowsFormsEditorService Service { get; set; }
    }
}
