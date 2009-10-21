using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing.Design;
using System.Text;
using System.Windows.Forms.Design;
using SlimDX.Direct3D9;

namespace VirtualBicycle.Ide.Editors
{
    public class TextureEditor : UITypeEditor
    {
        TextureEditControl ui;
        public override UITypeEditorEditStyle GetEditStyle(ITypeDescriptorContext context)
        {
            return UITypeEditorEditStyle.DropDown;
        }
        public override object EditValue(ITypeDescriptorContext context, IServiceProvider provider, object value)
        {
            if (ui == null)
                ui = new TextureEditControl();

            //m_ui.SetStates((DockAreas)value);
            ui.Value = (Texture)value;
            IWindowsFormsEditorService edSvc = (IWindowsFormsEditorService)provider.GetService(typeof(IWindowsFormsEditorService));
            ui.Service = edSvc;
            edSvc.DropDownControl(ui);

            ui.Service = null;
            Texture result = ui.Value;
            ui.Value = null;
            return result;
            //return m_ui.DockAreas;
        }

        public static string GetFilter()
        {
            //ImageFileFormat;
            string fmts = "*.bmp;*.jpg;*.tga;*.png;*.dds;*.ppm;*.dib;*.pfm";
            return "贴图文件" + "(" + fmts + ")|" + fmts;
        }
    }
}
