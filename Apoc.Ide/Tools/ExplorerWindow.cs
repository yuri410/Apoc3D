using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Text;
using System.Windows.Forms;
using VirtualBicycle.Ide.Projects;
using VirtualBicycle.UI;
using WeifenLuo.WinFormsUI.Docking;

namespace VirtualBicycle.Ide.Tools
{
    public partial class ExplorerWindow : DockContent, ITool
    {
        enum FSOType : int
        {
            Archive = 0,
            Folder,
            File,
            ArchiveFile,
            ErrorMsg
        }
        class FileSystemObject
        {
            FSOType type;

            string path;

            public FSOType Type
            {
                get { return type; }
                set { type = value; }
            }
            public string Path
            {
                get { return path; }
                set { path = value; }
            }
        }
        
        ProjectBase currentProject;
        
        public ExplorerWindow()
        {
            InitializeComponent();

            LanguageParser.ParseLanguage(DevStringTable.Instance, this);

        }

        public ProjectBase CurrentProject
        {
            get { return currentProject; }
            set { currentProject = value; }
        }

        public ExplorerState State
        {
            get { return currentProject == null ? ExplorerState.FreeBrowse : ExplorerState.Project; }
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

        private void refreshTool_Click(object sender, EventArgs e)
        {
            if (State == ExplorerState.FreeBrowse)
            {
                treeView1.Nodes.Clear();                                

                DriveInfo[] drives = DriveInfo.GetDrives();
                for (int i = 0; i < drives.Length; i++)
                {
                    AddNode(treeView1.Nodes, FSOType.Folder, drives[i].RootDirectory.FullName, drives[i].Name);
                }
            }
            else
            {

            }
        }

        void AddNode(TreeNodeCollection nodes, FSOType type, string path)
        {
            AddNode(nodes, type, path, Path.GetFileName(path));
        }
        void AddNode(TreeNodeCollection nodes, FSOType type, string path, string name)
        {
            TreeNode node = new TreeNode();
            node.Text = name;
            FileSystemObject data = new FileSystemObject();
            data.Path = path;
            data.Type = type;
            node.Tag = data;

            nodes.Add(node);
        }

        private void treeView1_AfterSelect(object sender, TreeViewEventArgs e)
        {
            if (e.Node != null)
            {
                if (e.Node.Nodes.Count == 0)
                {
                    FileSystemObject data = (FileSystemObject)e.Node.Tag;

                    if (data.Type == FSOType.Folder)
                    {
                        if (Directory.Exists(data.Path))
                        {
                            try
                            {
                                string[] subd = Directory.GetDirectories(data.Path, "*", SearchOption.TopDirectoryOnly);
                                for (int i = 0; i < subd.Length; i++)
                                {
                                    AddNode(e.Node.Nodes, FSOType.Folder, subd[i]);
                                }
                                subd = Directory.GetFiles(data.Path, "*.*", SearchOption.TopDirectoryOnly);
                                for (int i = 0; i < subd.Length; i++)
                                {
                                    AddNode(e.Node.Nodes, FSOType.File, subd[i]);
                                }
                            }
                            catch (Exception ex)
                            {
                                AddNode(e.Node.Nodes, FSOType.ErrorMsg, string.Empty, ex.Message);
                            }

                        }
                        else
                        {
                            AddNode(e.Node.Nodes, FSOType.ErrorMsg, string.Empty, DevStringTable.Instance["ERROR:IVAILD"]);
                        }
                    }
                }
                e.Node.Expand();
            }
        }

        private void ExplorerWindow_Load(object sender, EventArgs e)
        {
            refreshTool_Click(null, null);
        }

        private void ExplorerWindow_TextChanged(object sender, EventArgs e)
        {
            TabText = Text;
        }
    }

    public enum ExplorerState
    {
        FreeBrowse,
        Project
    }
}
