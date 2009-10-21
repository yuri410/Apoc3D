using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing.Design;
using System.Text;
using System.Windows.Forms.Design;
using VirtualBicycle.Ide.Editors.EditableObjects;

namespace VirtualBicycle.Ide.Editors
{
    /// <summary>
    /// 编辑entities列表用
    /// </summary>
    public class MeshArrayEditor : UITypeEditor
    {
        MeshArrayEditControl ui;
        public override UITypeEditorEditStyle GetEditStyle(ITypeDescriptorContext context)
        {
            return UITypeEditorEditStyle.DropDown;
        }
        public override object EditValue(ITypeDescriptorContext context, IServiceProvider provider, object value)
        {
            if (ui == null)
                ui = new MeshArrayEditControl();

            //m_ui.SetStates((DockAreas)value);
            ui.Value = (EditableMesh[])value;
            IWindowsFormsEditorService edSvc = (IWindowsFormsEditorService)provider.GetService(typeof(IWindowsFormsEditorService));
            ui.Service = edSvc;
            edSvc.DropDownControl(ui);

            ui.Service = null;
            EditableMesh[] result = ui.Value;
            ui.Value = null;
            return result;
            //return m_ui.DockAreas;
        }
    }
}
