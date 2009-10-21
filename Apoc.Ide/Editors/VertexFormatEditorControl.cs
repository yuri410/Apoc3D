using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Windows.Forms.Design;
using SlimDX.Direct3D9;
using VirtualBicycle.Design;

namespace VirtualBicycle.Ide.Editors
{
    public partial class VertexFormatEditorControl : UserControl, IEditControl<VertexFormat>
    {
        public VertexFormatEditorControl()
        {
            InitializeComponent();
            //LanguageParser.ParseLanguage(Program.StringTable, this);
        }


        #region IEditControl<VertexFormat> 成员

        public VertexFormat Value
        {
            get
            {
                VertexFormat res = VertexFormat.Position;
                if (checkBox2.Checked)                
                {
                    res |= VertexFormat.Normal;
                }
                if (radioButton1.Checked)
                {
                    res |= VertexFormat.Texture1;
                }
                else if (radioButton2.Checked)                
                {
                    res |= VertexFormat.Texture2;
                }
                else if (radioButton3.Checked)
                {
                    res |= VertexFormat.Texture3;
                }
                else if (radioButton4.Checked)
                {
                    res |= VertexFormat.Texture4;
                }
                return res;
            }
            set
            {
                if ((value & VertexFormat.Normal) == VertexFormat.Normal)                
                {
                    checkBox2.Checked = true;
                }

                if ((value & VertexFormat.Texture1) == VertexFormat.Texture1)
                {
                    radioButton1.Checked = true;
                }
                else if ((value & VertexFormat.Texture2) == VertexFormat.Texture2)
                {
                    radioButton2.Checked = true;
                }
                else if ((value & VertexFormat.Texture3) == VertexFormat.Texture3)
                {
                    radioButton3.Checked = true;
                }
                else if ((value & VertexFormat.Texture4) == VertexFormat.Texture4)
                {
                    radioButton4.Checked = true;
                }
            }
        }

        public IWindowsFormsEditorService Service
        {
            get;
            set;
        }

        #endregion
    }
}
