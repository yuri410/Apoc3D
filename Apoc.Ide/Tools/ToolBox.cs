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
    public partial class ToolBox : DockContent, ITool
    {
        ToolBoxItem[] items;
        ToolBoxCategory[] cates;

        public ToolBox()
        {
            InitializeComponent();
            LanguageParser.ParseLanguage(DevStringTable.Instance, this);
        }

        #region ITool 成员

        public bool IsVisibleInMenu
        {
            get { return true; }
        }

        public DockContent Form
        {
            get { return this; }
        }

        #endregion

        public void SetToolItems(ToolBoxItem[] items, ToolBoxCategory[] cates)
        {
            this.treeView1.Nodes.Clear();
            this.imageList1.Images.Clear();

            this.items = items;
            this.cates = cates;

            if (items != null)
            {
                if (cates == null)
                {
                    cates = new ToolBoxCategory[0];
                }

                Dictionary<ToolBoxCategory, TreeNode> table = new Dictionary<ToolBoxCategory, TreeNode>(cates.Length);

                for (int i = 0; i < cates.Length; i++)
                {
                    this.imageList1.Images.Add(cates[i].Icon);

                    string name = cates[i].Name;
                    int imgIndex = imageList1.Images.Count - 1;

                    TreeNode treeNode = treeView1.Nodes.Add(name, name, imgIndex, imgIndex);

                    table.Add(cates[i], treeNode);
                }

                for (int i = 0; i < items.Length; i++)
                {
                    this.imageList1.Images.Add(items[i].Icon);

                    ToolBoxCategory category = items[i].Category;

                    string name = items[i].Name;
                    int imgIndex = imageList1.Images.Count - 1;

                    if (category != null)
                    {
                        TreeNode parent = table[category];

                        TreeNode node = parent.Nodes.Add(name, name, imgIndex, imgIndex);

                        node.Tag = items[i];
                    }
                    else
                    {
                        TreeNode node = treeView1.Nodes.Add(name, name, imgIndex, imgIndex);

                        node.Tag = items[i];
                    }
                }
            }
        }

        private void treeView1_DoubleClick(object sender, EventArgs e)
        {
            if (treeView1.SelectedNode != null)
            {
                if (treeView1.SelectedNode.Tag != null)
                {
                    ToolBoxItem item = (ToolBoxItem)treeView1.SelectedNode.Tag;

                    item.NotifyActivated();
                }
            }
        }


    }
}
