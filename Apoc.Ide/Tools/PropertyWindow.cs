using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using VirtualBicycle.UI;
using WeifenLuo.WinFormsUI.Docking;

namespace VirtualBicycle.Ide.Tools
{
    public partial class PropertyWindow : DockContent, ITool
    {
        object[] allObjects;

        PropertyGirdUpdateHandler update;
        public PropertyWindow()
        {
            InitializeComponent();

            LanguageParser.ParseLanguage(DevStringTable.Instance, this);

        }

        private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            propertyGrid1.SelectedObject = comboBox1.SelectedItem;
        }
        
        public event PropertyGirdUpdateHandler PropertyUpdate
        {
            add { update += value; }
            remove { update -= value; }
        }

        public void SetObjects(object obj, object[] objs)
        {
            comboBox1.Items.Clear();
            allObjects = null;


            if (obj == null && objs != null)
            {
                obj = objs[0];
                for (int i = 0; i < allObjects.Length; i++)
                {
                    comboBox1.Items.Add(objs[i]);
                }
                comboBox1.SelectedIndex = 0;
            }
            else if (obj != null && objs == null)
            {
                objs = new object[] { obj };
                comboBox1.Items.Add(obj);
                comboBox1.SelectedIndex = 0;
            }
            else if (obj != null && objs != null)
            {
                propertyGrid1.SelectedObject = obj;

                int idx = -1;
                for (int i = 0; i < objs.Length; i++)
                {
                    comboBox1.Items.Add(objs[i]);
                    if (objs[i] == obj)
                    {
                        idx = i;
                    }
                }
                comboBox1.SelectedIndex = idx;
            }
            
            propertyGrid1.SelectedObjects = objs;
            allObjects = objs;

            //if (obj == null && objs != null)
            //{
            //    allObjects = objs;
            //    propertyGrid1.SelectedObject = objs[0];
            //    for (int i = 0; i < allObjects.Length; i++)                
            //    {
            //        comboBox1.Items.Add(objs[i]);
            //    }
            //    comboBox1.SelectedIndex = 0;
            //}
            //else if (objs == null && obj != null)
            //{
            //    allObjects = new object[] { obj };

            //    propertyGrid1.SelectedObject = obj;
            //    comboBox1.Items.Add(obj);
            //    comboBox1.SelectedIndex = 0;
            //}
            //else if (obj != null && objs != null)
            //{
            //    allObjects = objs;
            //    propertyGrid1.SelectedObject = obj;

            //    int idx = 0;
            //    for (int i = 0; i < allObjects.Length; i++)
            //    {
            //        comboBox1.Items.Add(objs[i]);
            //        if (objs[i] == obj)
            //        {
            //            idx = i;
            //        }
            //    }
            //    comboBox1.SelectedIndex = idx;
            //}
            //else
            //{
            //    allObjects = null;
            //    propertyGrid1.SelectedObject = null;
            //}
        }

        private void PropertyTool_Resize(object sender, EventArgs e)
        {
            splitContainer1.SplitterDistance = comboBox1.Height; 
        }

        private void propertyGrid1_SelectedGridItemChanged(object sender, SelectedGridItemChangedEventArgs e)
        {
            if (update != null)
            {
                update(propertyGrid1.SelectedObject);
            }
        }

        #region ITool 成员

        public DockContent Form
        {
            get { return this; }
        }

        public bool IsVisibleInMenu
        {
            get { return true; }
        }

        #endregion

        private void PropertyWindow_TextChanged(object sender, EventArgs e)
        {
            TabText = Text;
        }

    }

    public delegate void PropertyGirdUpdateHandler(object obj);
}
