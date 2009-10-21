using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Windows.Forms.Design;
using VirtualBicycle.Ide.Converters;
using VirtualBicycle.Ide.Editors.EditableObjects;
using VirtualBicycle.IO;
using VirtualBicycle.Design;

namespace VirtualBicycle.Ide.Editors
{
    public partial class ModelEditControl : UserControl, IEditControl<EditableModel>
    {
        public ModelEditControl()
        {
            InitializeComponent();
        }

        EditableModel value;
        IWindowsFormsEditorService service;
        #region IEditControl<EditableModel> 成员

        public EditableModel Value
        {
            get
            {
                return value;
            }
            set
            {
                this.value = value;
            }
        }

        public IWindowsFormsEditorService Service
        {
            get
            {
                return service;
            }
            set
            {
                service = value;
            }
        }

        #endregion

        private void button1_Click(object sender, EventArgs e)
        {
            ConverterBase[] convs = ConverterManager.Instance.GetConvertersDest(".mesh");
            string[] subFilters = new string[convs.Length + 1];
            for (int i = 0; i < convs.Length; i++)
            {
                subFilters[i] = DevUtils.GetFilter(convs[i].SourceDesc, convs[i].SourceExt);
            }
            subFilters[convs.Length] = DevUtils.GetFilter("网格文件", new string[] { ".mesh" });

            openFileDialog1.Filter = DevUtils.GetFilter(subFilters);

            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                if (openFileDialog1.FilterIndex == subFilters.Length)
                {
                    value = EditableModel.FromFile(new DevFileLocation(openFileDialog1.FileName));
                }
                else
                {
                    ConverterBase con = convs[openFileDialog1.FilterIndex - 1];

                    System.IO.MemoryStream ms = new System.IO.MemoryStream(65536 * 4);
                    con.Convert(new DevFileLocation(openFileDialog1.FileName), new StreamedLocation(new VirtualStream(ms, 0)));
                    ms.Position = 0;

                    value = EditableModel.FromFile(new StreamedLocation(ms));
                }
                //// EditableModel.ImportFromXml(openFileDialog1.FileName);
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            if (value != null)
            {
                if (openFileDialog1.ShowDialog() == DialogResult.OK)
                {
                    value.ImportEntityFromXml(openFileDialog1.FileName);
                }
            }
            else
            {
                button1_Click(sender, e);
            }
        }
    }
}
