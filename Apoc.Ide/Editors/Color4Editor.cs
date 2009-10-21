using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing.Design;
using System.Text;
using System.Windows.Forms.Design;
using SlimDX;

namespace VirtualBicycle.Ide.Editors
{
    public class Color4Editor : UITypeEditor
    {
        Color4EditControl ui;
        public override UITypeEditorEditStyle GetEditStyle(ITypeDescriptorContext context)
        {
            return UITypeEditorEditStyle.DropDown;
        }
        public override object EditValue(ITypeDescriptorContext context, IServiceProvider provider, object value)
        {
            if (ui == null)
                ui = new Color4EditControl();

            //m_ui.SetStates((DockAreas)value);
            ui.Value = (Color4)value;
            IWindowsFormsEditorService edSvc = (IWindowsFormsEditorService)provider.GetService(typeof(IWindowsFormsEditorService));
            ui.Service = edSvc;
            edSvc.DropDownControl(ui);
            ui.Service = null;
            return (object)ui.Value;
            //return m_ui.DockAreas;
        }
    }
}
