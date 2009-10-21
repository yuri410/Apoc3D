using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing.Design;
using System.Text;
using System.Windows.Forms.Design;
using R3D.Media;

namespace Ra2Develop.Editors
{
    public class ImageEditor : UITypeEditor
    {
        ImageEditControl ui;
        public override UITypeEditorEditStyle GetEditStyle(ITypeDescriptorContext context)
        {
            return UITypeEditorEditStyle.DropDown;
        }
        public override object EditValue(ITypeDescriptorContext context, IServiceProvider provider, object value)
        {
            if (ui == null)
                ui = new ImageEditControl();

            //m_ui.SetStates((DockAreas)value);
            ui.Value = (ImageBase)value;
            IWindowsFormsEditorService edSvc = (IWindowsFormsEditorService)provider.GetService(typeof(IWindowsFormsEditorService));
            ui.Service = edSvc;
            edSvc.DropDownControl(ui);

            ui.Service = null;
            ImageBase result = ui.Value;
            ui.Value = null;
            return result;
            //return m_ui.DockAreas;
        }
    }
}
