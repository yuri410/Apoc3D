namespace VirtualBicycle.Ide
{
    partial class MainForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.statusStrip1 = new System.Windows.Forms.StatusStrip();
            this.statusLabel = new System.Windows.Forms.ToolStripStatusLabel();
            this.toolStripProgressBar1 = new System.Windows.Forms.ToolStripProgressBar();
            this.menuStripMain = new System.Windows.Forms.MenuStrip();
            this.fileMemu = new System.Windows.Forms.ToolStripMenuItem();
            this.newMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.newProjMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.newFileMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.openMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.openProjMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.openFileMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.insertMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem1 = new System.Windows.Forms.ToolStripSeparator();
            this.saveMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.saveAsMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.saveAllMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem2 = new System.Windows.Forms.ToolStripSeparator();
            this.closeMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.closeAllMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.closeProjMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem3 = new System.Windows.Forms.ToolStripSeparator();
            this.recentFileMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.recentProjMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem4 = new System.Windows.Forms.ToolStripSeparator();
            this.exitMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.editMenu = new System.Windows.Forms.ToolStripMenuItem();
            this.viewMenu = new System.Windows.Forms.ToolStripMenuItem();
            this.propertyWndMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.managerWndMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.errorListMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolBoxMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.projectMenu = new System.Windows.Forms.ToolStripMenuItem();
            this.buildMenu = new System.Windows.Forms.ToolStripMenuItem();
            this.debugMenu = new System.Windows.Forms.ToolStripMenuItem();
            this.toolsMenu = new System.Windows.Forms.ToolStripMenuItem();
            this.converterMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.helpMenu = new System.Windows.Forms.ToolStripMenuItem();
            this.gUIAboutToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMain = new System.Windows.Forms.ToolStrip();
            this.newTool = new System.Windows.Forms.ToolStripDropDownButton();
            this.newProjToolMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.newFileToolMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.insertTool = new System.Windows.Forms.ToolStripDropDownButton();
            this.openTool = new System.Windows.Forms.ToolStripButton();
            this.saveTool = new System.Windows.Forms.ToolStripButton();
            this.saveAllTool = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.runTool = new System.Windows.Forms.ToolStripButton();
            this.debugPauseTool = new System.Windows.Forms.ToolStripButton();
            this.stopDebugTool = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.toolStripContainer1 = new System.Windows.Forms.ToolStripContainer();
            this.dockPanel = new WeifenLuo.WinFormsUI.Docking.DockPanel();
            this.openFileDialog1 = new System.Windows.Forms.OpenFileDialog();
            this.saveFileDialog1 = new System.Windows.Forms.SaveFileDialog();
            this.statusStrip1.SuspendLayout();
            this.menuStripMain.SuspendLayout();
            this.toolStripMain.SuspendLayout();
            this.toolStripContainer1.BottomToolStripPanel.SuspendLayout();
            this.toolStripContainer1.ContentPanel.SuspendLayout();
            this.toolStripContainer1.TopToolStripPanel.SuspendLayout();
            this.toolStripContainer1.SuspendLayout();
            this.SuspendLayout();
            // 
            // statusStrip1
            // 
            this.statusStrip1.Dock = System.Windows.Forms.DockStyle.None;
            this.statusStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.statusLabel,
            this.toolStripProgressBar1});
            this.statusStrip1.Location = new System.Drawing.Point(0, 0);
            this.statusStrip1.Name = "statusStrip1";
            this.statusStrip1.Size = new System.Drawing.Size(795, 22);
            this.statusStrip1.TabIndex = 0;
            // 
            // statusLabel
            // 
            this.statusLabel.Name = "statusLabel";
            this.statusLabel.Size = new System.Drawing.Size(780, 17);
            this.statusLabel.Spring = true;
            this.statusLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // toolStripProgressBar1
            // 
            this.toolStripProgressBar1.Name = "toolStripProgressBar1";
            this.toolStripProgressBar1.Size = new System.Drawing.Size(100, 16);
            this.toolStripProgressBar1.Visible = false;
            // 
            // menuStripMain
            // 
            this.menuStripMain.Dock = System.Windows.Forms.DockStyle.None;
            this.menuStripMain.GripStyle = System.Windows.Forms.ToolStripGripStyle.Visible;
            this.menuStripMain.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileMemu,
            this.editMenu,
            this.viewMenu,
            this.projectMenu,
            this.buildMenu,
            this.debugMenu,
            this.toolsMenu,
            this.helpMenu});
            this.menuStripMain.Location = new System.Drawing.Point(0, 0);
            this.menuStripMain.Name = "menuStripMain";
            this.menuStripMain.Size = new System.Drawing.Size(795, 25);
            this.menuStripMain.TabIndex = 0;
            // 
            // fileMemu
            // 
            this.fileMemu.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.newMenuItem,
            this.openMenuItem,
            this.insertMenuItem,
            this.toolStripMenuItem1,
            this.saveMenuItem,
            this.saveAsMenuItem,
            this.saveAllMenuItem,
            this.toolStripMenuItem2,
            this.closeMenuItem,
            this.closeAllMenuItem,
            this.closeProjMenuItem,
            this.toolStripMenuItem3,
            this.recentFileMenuItem,
            this.recentProjMenuItem,
            this.toolStripMenuItem4,
            this.exitMenuItem});
            this.fileMemu.Name = "fileMemu";
            this.fileMemu.Size = new System.Drawing.Size(68, 21);
            this.fileMemu.Text = "GUI:FILE";
            // 
            // newMenuItem
            // 
            this.newMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.newProjMenuItem,
            this.newFileMenuItem});
            this.newMenuItem.Name = "newMenuItem";
            this.newMenuItem.Size = new System.Drawing.Size(186, 22);
            this.newMenuItem.Text = "GUI:NEW";
            // 
            // newProjMenuItem
            // 
            this.newProjMenuItem.Name = "newProjMenuItem";
            this.newProjMenuItem.Size = new System.Drawing.Size(131, 22);
            this.newProjMenuItem.Text = "GUI:PROJ";
            // 
            // newFileMenuItem
            // 
            this.newFileMenuItem.Name = "newFileMenuItem";
            this.newFileMenuItem.Size = new System.Drawing.Size(131, 22);
            this.newFileMenuItem.Text = "GUI:FILE";
            this.newFileMenuItem.Click += new System.EventHandler(this.newFileMenuItem_Click);
            // 
            // openMenuItem
            // 
            this.openMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.openProjMenuItem,
            this.openFileMenuItem});
            this.openMenuItem.Name = "openMenuItem";
            this.openMenuItem.Size = new System.Drawing.Size(186, 22);
            this.openMenuItem.Text = "GUI:OPEN";
            // 
            // openProjMenuItem
            // 
            this.openProjMenuItem.Name = "openProjMenuItem";
            this.openProjMenuItem.Size = new System.Drawing.Size(131, 22);
            this.openProjMenuItem.Text = "GUI:PROJ";
            // 
            // openFileMenuItem
            // 
            this.openFileMenuItem.Name = "openFileMenuItem";
            this.openFileMenuItem.Size = new System.Drawing.Size(131, 22);
            this.openFileMenuItem.Text = "GUI:FILE";
            this.openFileMenuItem.Click += new System.EventHandler(this.openFileMenuItem_Click);
            // 
            // insertMenuItem
            // 
            this.insertMenuItem.Name = "insertMenuItem";
            this.insertMenuItem.Size = new System.Drawing.Size(186, 22);
            this.insertMenuItem.Text = "GUI:Insert";
            // 
            // toolStripMenuItem1
            // 
            this.toolStripMenuItem1.Name = "toolStripMenuItem1";
            this.toolStripMenuItem1.Size = new System.Drawing.Size(183, 6);
            // 
            // saveMenuItem
            // 
            this.saveMenuItem.Name = "saveMenuItem";
            this.saveMenuItem.Size = new System.Drawing.Size(186, 22);
            this.saveMenuItem.Text = "GUI:SAVE";
            this.saveMenuItem.Click += new System.EventHandler(this.saveMenuItem_Click);
            // 
            // saveAsMenuItem
            // 
            this.saveAsMenuItem.Name = "saveAsMenuItem";
            this.saveAsMenuItem.Size = new System.Drawing.Size(186, 22);
            this.saveAsMenuItem.Text = "GUI:SAVEAS";
            this.saveAsMenuItem.Click += new System.EventHandler(this.saveAsMenuItem_Click);
            // 
            // saveAllMenuItem
            // 
            this.saveAllMenuItem.Name = "saveAllMenuItem";
            this.saveAllMenuItem.Size = new System.Drawing.Size(186, 22);
            this.saveAllMenuItem.Text = "GUI:SAVEALL";
            this.saveAllMenuItem.Click += new System.EventHandler(this.saveAllMenuItem_Click);
            // 
            // toolStripMenuItem2
            // 
            this.toolStripMenuItem2.Name = "toolStripMenuItem2";
            this.toolStripMenuItem2.Size = new System.Drawing.Size(183, 6);
            // 
            // closeMenuItem
            // 
            this.closeMenuItem.Name = "closeMenuItem";
            this.closeMenuItem.Size = new System.Drawing.Size(186, 22);
            this.closeMenuItem.Text = "GUI:CLOSE";
            // 
            // closeAllMenuItem
            // 
            this.closeAllMenuItem.Name = "closeAllMenuItem";
            this.closeAllMenuItem.Size = new System.Drawing.Size(186, 22);
            this.closeAllMenuItem.Text = "GUI:CLOSEALL";
            // 
            // closeProjMenuItem
            // 
            this.closeProjMenuItem.Name = "closeProjMenuItem";
            this.closeProjMenuItem.Size = new System.Drawing.Size(186, 22);
            this.closeProjMenuItem.Text = "GUI:CloseProject";
            // 
            // toolStripMenuItem3
            // 
            this.toolStripMenuItem3.Name = "toolStripMenuItem3";
            this.toolStripMenuItem3.Size = new System.Drawing.Size(183, 6);
            // 
            // recentFileMenuItem
            // 
            this.recentFileMenuItem.Name = "recentFileMenuItem";
            this.recentFileMenuItem.Size = new System.Drawing.Size(186, 22);
            this.recentFileMenuItem.Text = "GUI:RecentFiles";
            // 
            // recentProjMenuItem
            // 
            this.recentProjMenuItem.Name = "recentProjMenuItem";
            this.recentProjMenuItem.Size = new System.Drawing.Size(186, 22);
            this.recentProjMenuItem.Text = "GUI:RecentProjects";
            // 
            // toolStripMenuItem4
            // 
            this.toolStripMenuItem4.Name = "toolStripMenuItem4";
            this.toolStripMenuItem4.Size = new System.Drawing.Size(183, 6);
            // 
            // exitMenuItem
            // 
            this.exitMenuItem.Name = "exitMenuItem";
            this.exitMenuItem.Size = new System.Drawing.Size(186, 22);
            this.exitMenuItem.Text = "GUI:EXIT";
            this.exitMenuItem.Click += new System.EventHandler(this.exitMenuItem_Click);
            // 
            // editMenu
            // 
            this.editMenu.Name = "editMenu";
            this.editMenu.Size = new System.Drawing.Size(72, 21);
            this.editMenu.Text = "GUI:EDIT";
            // 
            // viewMenu
            // 
            this.viewMenu.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.propertyWndMenuItem,
            this.managerWndMenuItem,
            this.errorListMenuItem,
            this.toolBoxMenuItem});
            this.viewMenu.Name = "viewMenu";
            this.viewMenu.Size = new System.Drawing.Size(76, 21);
            this.viewMenu.Text = "GUI:VIEW";
            // 
            // propertyWndMenuItem
            // 
            this.propertyWndMenuItem.Name = "propertyWndMenuItem";
            this.propertyWndMenuItem.Size = new System.Drawing.Size(181, 22);
            this.propertyWndMenuItem.Text = "GUI:PropertyWnd";
            this.propertyWndMenuItem.Click += new System.EventHandler(this.propertyWndMenuItem_Click);
            // 
            // managerWndMenuItem
            // 
            this.managerWndMenuItem.Name = "managerWndMenuItem";
            this.managerWndMenuItem.Size = new System.Drawing.Size(181, 22);
            this.managerWndMenuItem.Text = "GUI:ManagerWnd";
            this.managerWndMenuItem.Click += new System.EventHandler(this.managerWndMenuItem_Click);
            // 
            // errorListMenuItem
            // 
            this.errorListMenuItem.Name = "errorListMenuItem";
            this.errorListMenuItem.Size = new System.Drawing.Size(181, 22);
            this.errorListMenuItem.Text = "GUI:ErrorList";
            // 
            // toolBoxMenuItem
            // 
            this.toolBoxMenuItem.Name = "toolBoxMenuItem";
            this.toolBoxMenuItem.Size = new System.Drawing.Size(181, 22);
            this.toolBoxMenuItem.Text = "GUI:ToolBox";
            this.toolBoxMenuItem.Click += new System.EventHandler(this.toolBoxMenuItem_Click);
            // 
            // projectMenu
            // 
            this.projectMenu.Name = "projectMenu";
            this.projectMenu.Size = new System.Drawing.Size(97, 21);
            this.projectMenu.Text = "GUI:PROJECT";
            // 
            // buildMenu
            // 
            this.buildMenu.Name = "buildMenu";
            this.buildMenu.Size = new System.Drawing.Size(81, 21);
            this.buildMenu.Text = "GUI:BUILD";
            // 
            // debugMenu
            // 
            this.debugMenu.Name = "debugMenu";
            this.debugMenu.Size = new System.Drawing.Size(87, 21);
            this.debugMenu.Text = "GUI:DEBUG";
            // 
            // toolsMenu
            // 
            this.toolsMenu.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.converterMenuItem});
            this.toolsMenu.Name = "toolsMenu";
            this.toolsMenu.Size = new System.Drawing.Size(85, 21);
            this.toolsMenu.Text = "GUI:TOOLS";
            // 
            // converterMenuItem
            // 
            this.converterMenuItem.Name = "converterMenuItem";
            this.converterMenuItem.Size = new System.Drawing.Size(174, 22);
            this.converterMenuItem.Text = "GUI:CONVERTER";
            // 
            // helpMenu
            // 
            this.helpMenu.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.gUIAboutToolStripMenuItem});
            this.helpMenu.Name = "helpMenu";
            this.helpMenu.Size = new System.Drawing.Size(74, 21);
            this.helpMenu.Text = "GUI:HELP";
            // 
            // gUIAboutToolStripMenuItem
            // 
            this.gUIAboutToolStripMenuItem.Name = "gUIAboutToolStripMenuItem";
            this.gUIAboutToolStripMenuItem.Size = new System.Drawing.Size(136, 22);
            this.gUIAboutToolStripMenuItem.Text = "GUI:About";
            // 
            // toolStripMain
            // 
            this.toolStripMain.AllowItemReorder = true;
            this.toolStripMain.Dock = System.Windows.Forms.DockStyle.None;
            this.toolStripMain.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.newTool,
            this.insertTool,
            this.openTool,
            this.saveTool,
            this.saveAllTool,
            this.toolStripSeparator1,
            this.runTool,
            this.debugPauseTool,
            this.stopDebugTool,
            this.toolStripSeparator2});
            this.toolStripMain.Location = new System.Drawing.Point(3, 25);
            this.toolStripMain.Name = "toolStripMain";
            this.toolStripMain.Size = new System.Drawing.Size(220, 25);
            this.toolStripMain.TabIndex = 1;
            // 
            // newTool
            // 
            this.newTool.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.newTool.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.newProjToolMenuItem,
            this.newFileToolMenuItem});
            this.newTool.Image = global::VirtualBicycle.Ide.Properties.Resources.New;
            this.newTool.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.newTool.Name = "newTool";
            this.newTool.Size = new System.Drawing.Size(29, 22);
            this.newTool.Text = "GUI:New";
            // 
            // newProjToolMenuItem
            // 
            this.newProjToolMenuItem.Name = "newProjToolMenuItem";
            this.newProjToolMenuItem.Size = new System.Drawing.Size(153, 22);
            this.newProjToolMenuItem.Text = "GUI:PROJECT";
            // 
            // newFileToolMenuItem
            // 
            this.newFileToolMenuItem.Name = "newFileToolMenuItem";
            this.newFileToolMenuItem.Size = new System.Drawing.Size(153, 22);
            this.newFileToolMenuItem.Text = "GUI:FILE";
            this.newFileToolMenuItem.Click += new System.EventHandler(this.newFileMenuItem_Click);
            // 
            // insertTool
            // 
            this.insertTool.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.insertTool.Enabled = false;
            this.insertTool.Image = global::VirtualBicycle.Ide.Properties.Resources.NewItem;
            this.insertTool.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.insertTool.Name = "insertTool";
            this.insertTool.Size = new System.Drawing.Size(29, 22);
            this.insertTool.Text = "GUI:INSERT";
            // 
            // openTool
            // 
            this.openTool.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.openTool.Image = global::VirtualBicycle.Ide.Properties.Resources.openHS;
            this.openTool.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.openTool.Name = "openTool";
            this.openTool.Size = new System.Drawing.Size(23, 22);
            this.openTool.Text = "GUI:OPEN";
            this.openTool.Click += new System.EventHandler(this.openFileMenuItem_Click);
            // 
            // saveTool
            // 
            this.saveTool.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.saveTool.Enabled = false;
            this.saveTool.Image = global::VirtualBicycle.Ide.Properties.Resources.saveHS;
            this.saveTool.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.saveTool.Name = "saveTool";
            this.saveTool.Size = new System.Drawing.Size(23, 22);
            this.saveTool.Text = "GUI:SAVE";
            this.saveTool.Click += new System.EventHandler(this.saveMenuItem_Click);
            // 
            // saveAllTool
            // 
            this.saveAllTool.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.saveAllTool.Image = global::VirtualBicycle.Ide.Properties.Resources.SaveAllHS;
            this.saveAllTool.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.saveAllTool.Name = "saveAllTool";
            this.saveAllTool.Size = new System.Drawing.Size(23, 22);
            this.saveAllTool.Text = "GUI:SAVEALL";
            this.saveAllTool.Click += new System.EventHandler(this.saveAllMenuItem_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(6, 25);
            // 
            // runTool
            // 
            this.runTool.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.runTool.Enabled = false;
            this.runTool.Image = global::VirtualBicycle.Ide.Properties.Resources.PlayHS;
            this.runTool.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.runTool.Name = "runTool";
            this.runTool.Size = new System.Drawing.Size(23, 22);
            this.runTool.Text = "GUI:RunDebug";
            // 
            // debugPauseTool
            // 
            this.debugPauseTool.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.debugPauseTool.Enabled = false;
            this.debugPauseTool.Image = global::VirtualBicycle.Ide.Properties.Resources.PauseHS;
            this.debugPauseTool.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.debugPauseTool.Name = "debugPauseTool";
            this.debugPauseTool.Size = new System.Drawing.Size(23, 22);
            this.debugPauseTool.Text = "GUI:Pause";
            // 
            // stopDebugTool
            // 
            this.stopDebugTool.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.stopDebugTool.Enabled = false;
            this.stopDebugTool.Image = global::VirtualBicycle.Ide.Properties.Resources.StopHS;
            this.stopDebugTool.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.stopDebugTool.Name = "stopDebugTool";
            this.stopDebugTool.Size = new System.Drawing.Size(23, 22);
            this.stopDebugTool.Text = "GUI:StopDebug";
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(6, 25);
            // 
            // toolStripContainer1
            // 
            // 
            // toolStripContainer1.BottomToolStripPanel
            // 
            this.toolStripContainer1.BottomToolStripPanel.Controls.Add(this.statusStrip1);
            // 
            // toolStripContainer1.ContentPanel
            // 
            this.toolStripContainer1.ContentPanel.BackColor = System.Drawing.SystemColors.Control;
            this.toolStripContainer1.ContentPanel.Controls.Add(this.dockPanel);
            this.toolStripContainer1.ContentPanel.Size = new System.Drawing.Size(795, 393);
            this.toolStripContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.toolStripContainer1.Location = new System.Drawing.Point(0, 0);
            this.toolStripContainer1.Name = "toolStripContainer1";
            this.toolStripContainer1.Size = new System.Drawing.Size(795, 465);
            this.toolStripContainer1.TabIndex = 5;
            this.toolStripContainer1.Text = "toolStripContainer1";
            // 
            // toolStripContainer1.TopToolStripPanel
            // 
            this.toolStripContainer1.TopToolStripPanel.Controls.Add(this.menuStripMain);
            this.toolStripContainer1.TopToolStripPanel.Controls.Add(this.toolStripMain);
            // 
            // dockPanel
            // 
            this.dockPanel.ActiveAutoHideContent = null;
            this.dockPanel.Dock = System.Windows.Forms.DockStyle.Fill;
            this.dockPanel.DocumentStyle = WeifenLuo.WinFormsUI.Docking.DocumentStyle.DockingWindow;
            this.dockPanel.Location = new System.Drawing.Point(0, 0);
            this.dockPanel.Name = "dockPanel";
            this.dockPanel.Size = new System.Drawing.Size(795, 393);
            this.dockPanel.TabIndex = 7;
            this.dockPanel.ActiveDocumentChanged += new System.EventHandler(this.dockPanel_ActiveDocumentChanged);
            // 
            // openFileDialog1
            // 
            this.openFileDialog1.Multiselect = true;
            this.openFileDialog1.ShowReadOnly = true;
            // 
            // saveFileDialog1
            // 
            this.saveFileDialog1.RestoreDirectory = true;
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(795, 465);
            this.Controls.Add(this.toolStripContainer1);
            this.MainMenuStrip = this.menuStripMain;
            this.MinimumSize = new System.Drawing.Size(300, 300);
            this.Name = "MainForm";
            this.Text = "GUI:VBIDE";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.MainForm_FormClosing);
            this.statusStrip1.ResumeLayout(false);
            this.statusStrip1.PerformLayout();
            this.menuStripMain.ResumeLayout(false);
            this.menuStripMain.PerformLayout();
            this.toolStripMain.ResumeLayout(false);
            this.toolStripMain.PerformLayout();
            this.toolStripContainer1.BottomToolStripPanel.ResumeLayout(false);
            this.toolStripContainer1.BottomToolStripPanel.PerformLayout();
            this.toolStripContainer1.ContentPanel.ResumeLayout(false);
            this.toolStripContainer1.TopToolStripPanel.ResumeLayout(false);
            this.toolStripContainer1.TopToolStripPanel.PerformLayout();
            this.toolStripContainer1.ResumeLayout(false);
            this.toolStripContainer1.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.MenuStrip menuStripMain;
        private System.Windows.Forms.ToolStrip toolStripMain;
        private System.Windows.Forms.StatusStrip statusStrip1;
        private System.Windows.Forms.ToolStripMenuItem fileMemu;
        private System.Windows.Forms.ToolStripMenuItem editMenu;
        private System.Windows.Forms.ToolStripMenuItem viewMenu;
        private System.Windows.Forms.ToolStripMenuItem projectMenu;
        private System.Windows.Forms.ToolStripMenuItem buildMenu;
        private System.Windows.Forms.ToolStripMenuItem debugMenu;
        private System.Windows.Forms.ToolStripMenuItem toolsMenu;
        private System.Windows.Forms.ToolStripMenuItem helpMenu;
        private System.Windows.Forms.ToolStripMenuItem newMenuItem;
        private System.Windows.Forms.ToolStripMenuItem openMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripMenuItem1;
        private System.Windows.Forms.ToolStripMenuItem saveMenuItem;
        private System.Windows.Forms.ToolStripMenuItem saveAllMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripMenuItem2;
        private System.Windows.Forms.ToolStripMenuItem closeMenuItem;
        private System.Windows.Forms.ToolStripMenuItem closeAllMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripMenuItem3;
        private System.Windows.Forms.ToolStripMenuItem recentFileMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripMenuItem4;
        private System.Windows.Forms.ToolStripMenuItem exitMenuItem;
        private System.Windows.Forms.ToolStripButton openTool;
        private System.Windows.Forms.ToolStripButton saveTool;
        private System.Windows.Forms.ToolStripButton saveAllTool;
        private System.Windows.Forms.ToolStripMenuItem saveAsMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripButton runTool;
        private System.Windows.Forms.ToolStripButton debugPauseTool;
        private System.Windows.Forms.ToolStripButton stopDebugTool;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.ToolStripMenuItem closeProjMenuItem;
        private System.Windows.Forms.ToolStripContainer toolStripContainer1;
        private WeifenLuo.WinFormsUI.Docking.DockPanel dockPanel;
        private System.Windows.Forms.OpenFileDialog openFileDialog1;
        private System.Windows.Forms.ToolStripMenuItem newProjMenuItem;
        private System.Windows.Forms.ToolStripMenuItem newFileMenuItem;
        private System.Windows.Forms.ToolStripMenuItem openFileMenuItem;
        private System.Windows.Forms.ToolStripMenuItem openProjMenuItem;
        private System.Windows.Forms.ToolStripMenuItem insertMenuItem;
        private System.Windows.Forms.SaveFileDialog saveFileDialog1;
        private System.Windows.Forms.ToolStripMenuItem recentProjMenuItem;
        private System.Windows.Forms.ToolStripMenuItem propertyWndMenuItem;
        private System.Windows.Forms.ToolStripMenuItem managerWndMenuItem;
        private System.Windows.Forms.ToolStripMenuItem errorListMenuItem;
        private System.Windows.Forms.ToolStripStatusLabel statusLabel;
        private System.Windows.Forms.ToolStripMenuItem gUIAboutToolStripMenuItem;
        private System.Windows.Forms.ToolStripDropDownButton newTool;
        private System.Windows.Forms.ToolStripMenuItem newProjToolMenuItem;
        private System.Windows.Forms.ToolStripMenuItem newFileToolMenuItem;
        private System.Windows.Forms.ToolStripDropDownButton insertTool;
        private System.Windows.Forms.ToolStripMenuItem converterMenuItem;
        private System.Windows.Forms.ToolStripMenuItem toolBoxMenuItem;
        private System.Windows.Forms.ToolStripProgressBar toolStripProgressBar1;
    }
}