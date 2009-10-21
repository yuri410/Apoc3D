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
    /// 为模型添加实体，导入 等功能
    /// </summary>
    public class ModelEditor : UITypeEditor
    {
        ModelEditControl ui;
        public override UITypeEditorEditStyle GetEditStyle(ITypeDescriptorContext context)
        {
            return UITypeEditorEditStyle.DropDown;
        }
        public override object EditValue(ITypeDescriptorContext context, IServiceProvider provider, object value)
        {
            if (ui == null)
                ui = new ModelEditControl();

            //m_ui.SetStates((DockAreas)value);
            ui.Value = (EditableModel)value;
            IWindowsFormsEditorService edSvc = (IWindowsFormsEditorService)provider.GetService(typeof(IWindowsFormsEditorService));
            ui.Service = edSvc;
            edSvc.DropDownControl(ui);

            ui.Service = null;
            EditableModel result = ui.Value;
            ui.Value = null;
            return result;
            //return m_ui.DockAreas;
        }
    }

    //public class EditableBlockModel
    //{
    //    GameModel sounds;
    //    public EditableBlockModel(GameModel d)
    //    {
    //        if (d == null)
    //        {
    //            throw new ArgumentNullException();
    //        }
    //        sounds = d;
    //    }

    //    [Browsable(false)]
    //    public GameModel Data
    //    {
    //        get { return sounds; }
    //        set { sounds = value; }
    //    }

    //    [TypeConverter(typeof(BlockMeshesTypeConverter))]
    //    [Editor(typeof(BlockModelArrayEditor), typeof(UITypeEditor))]
    //    public GameMesh[] Entities
    //    {
    //        get { return sounds.Entities; }
    //        set { sounds.Entities = value; }
    //    }
    //}

}
