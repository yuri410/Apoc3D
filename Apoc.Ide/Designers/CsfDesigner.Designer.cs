namespace VirtualBicycle.Ide.Designers
{
    partial class CsfDesigner
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(CsfDesigner));
            this.plMain = new System.Windows.Forms.Panel();
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.listView1 = new System.Windows.Forms.ListView();
            this.chCsfType = new System.Windows.Forms.ColumnHeader();
            this.chCsfCount = new System.Windows.Forms.ColumnHeader();
            this.splitContainer2 = new System.Windows.Forms.SplitContainer();
            this.listView2 = new System.Windows.Forms.ListView();
            this.chCsfEntry = new System.Windows.Forms.ColumnHeader();
            this.chCsfContent = new System.Windows.Forms.ColumnHeader();
            this.chCsfExtra = new System.Windows.Forms.ColumnHeader();
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.toolStrip1 = new System.Windows.Forms.ToolStrip();
            this.newEntryTool = new System.Windows.Forms.ToolStripButton();
            this.editEntryTool = new System.Windows.Forms.ToolStripButton();
            this.delEntryTool = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.toolStripSplitButton1 = new System.Windows.Forms.ToolStripSplitButton();
            this.toolStripComboBox1 = new System.Windows.Forms.ToolStripComboBox();
            this.toolStripButton1 = new System.Windows.Forms.ToolStripButton();
            this.plMain.SuspendLayout();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.splitContainer2.Panel1.SuspendLayout();
            this.splitContainer2.Panel2.SuspendLayout();
            this.splitContainer2.SuspendLayout();
            this.toolStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // plMain
            // 
            this.plMain.Controls.Add(this.splitContainer1);
            this.plMain.Controls.Add(this.label1);
            this.plMain.Dock = System.Windows.Forms.DockStyle.Fill;
            this.plMain.Location = new System.Drawing.Point(0, 0);
            this.plMain.Name = "plMain";
            this.plMain.Size = new System.Drawing.Size(582, 340);
            this.plMain.TabIndex = 0;
            // 
            // splitContainer1
            // 
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.FixedPanel = System.Windows.Forms.FixedPanel.Panel1;
            this.splitContainer1.Location = new System.Drawing.Point(0, 0);
            this.splitContainer1.Name = "splitContainer1";
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.listView1);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.splitContainer2);
            this.splitContainer1.Size = new System.Drawing.Size(582, 340);
            this.splitContainer1.SplitterDistance = 200;
            this.splitContainer1.TabIndex = 1;
            // 
            // listView1
            // 
            this.listView1.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.chCsfType,
            this.chCsfCount});
            this.listView1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.listView1.FullRowSelect = true;
            this.listView1.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.Nonclickable;
            this.listView1.HideSelection = false;
            this.listView1.Location = new System.Drawing.Point(0, 0);
            this.listView1.Name = "listView1";
            this.listView1.Size = new System.Drawing.Size(200, 340);
            this.listView1.Sorting = System.Windows.Forms.SortOrder.Ascending;
            this.listView1.TabIndex = 0;
            this.listView1.UseCompatibleStateImageBehavior = false;
            this.listView1.View = System.Windows.Forms.View.Details;
            this.listView1.SelectedIndexChanged += new System.EventHandler(this.listView1_SelectedIndexChanged);
            // 
            // chCsfType
            // 
            this.chCsfType.Text = "GUI:CSFCATENAME";
            this.chCsfType.Width = 100;
            // 
            // chCsfCount
            // 
            this.chCsfCount.Text = "GUI:CSFEntryCOUNT";
            this.chCsfCount.Width = 84;
            // 
            // splitContainer2
            // 
            this.splitContainer2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer2.FixedPanel = System.Windows.Forms.FixedPanel.Panel2;
            this.splitContainer2.Location = new System.Drawing.Point(0, 0);
            this.splitContainer2.Name = "splitContainer2";
            this.splitContainer2.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer2.Panel1
            // 
            this.splitContainer2.Panel1.Controls.Add(this.listView2);
            // 
            // splitContainer2.Panel2
            // 
            this.splitContainer2.Panel2.Controls.Add(this.textBox1);
            this.splitContainer2.Size = new System.Drawing.Size(378, 340);
            this.splitContainer2.SplitterDistance = 240;
            this.splitContainer2.TabIndex = 1;
            // 
            // listView2
            // 
            this.listView2.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.chCsfEntry,
            this.chCsfContent,
            this.chCsfExtra});
            this.listView2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.listView2.FullRowSelect = true;
            this.listView2.GridLines = true;
            this.listView2.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.Nonclickable;
            this.listView2.HideSelection = false;
            this.listView2.Location = new System.Drawing.Point(0, 0);
            this.listView2.Name = "listView2";
            this.listView2.Size = new System.Drawing.Size(378, 240);
            this.listView2.Sorting = System.Windows.Forms.SortOrder.Ascending;
            this.listView2.TabIndex = 0;
            this.listView2.UseCompatibleStateImageBehavior = false;
            this.listView2.View = System.Windows.Forms.View.Details;
            this.listView2.SelectedIndexChanged += new System.EventHandler(this.listView2_SelectedIndexChanged);
            this.listView2.DoubleClick += new System.EventHandler(this.listView2_DoubleClick);
            // 
            // chCsfEntry
            // 
            this.chCsfEntry.Text = "GUI:CSFENTRY";
            this.chCsfEntry.Width = 111;
            // 
            // chCsfContent
            // 
            this.chCsfContent.Text = "GUI:CSFCONTENT";
            this.chCsfContent.Width = 197;
            // 
            // chCsfExtra
            // 
            this.chCsfExtra.Text = "GUI:CSFEXTRA";
            this.chCsfExtra.Width = 64;
            // 
            // textBox1
            // 
            this.textBox1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.textBox1.Location = new System.Drawing.Point(0, 0);
            this.textBox1.Multiline = true;
            this.textBox1.Name = "textBox1";
            this.textBox1.ReadOnly = true;
            this.textBox1.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.textBox1.Size = new System.Drawing.Size(378, 96);
            this.textBox1.TabIndex = 0;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(167, 85);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(41, 12);
            this.label1.TabIndex = 0;
            this.label1.Text = "label1";
            // 
            // toolStrip1
            // 
            this.toolStrip1.Dock = System.Windows.Forms.DockStyle.None;
            this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.newEntryTool,
            this.editEntryTool,
            this.delEntryTool,
            this.toolStripSeparator1,
            this.toolStripSplitButton1,
            this.toolStripComboBox1,
            this.toolStripButton1});
            this.toolStrip1.Location = new System.Drawing.Point(0, 0);
            this.toolStrip1.Name = "toolStrip1";
            this.toolStrip1.Size = new System.Drawing.Size(265, 25);
            this.toolStrip1.TabIndex = 1;
            this.toolStrip1.Text = "toolStrip1";
            // 
            // newEntryTool
            // 
            this.newEntryTool.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.newEntryTool.Image = global::VirtualBicycle.Ide.Properties.Resources.List_BulletsHS;
            this.newEntryTool.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.newEntryTool.Name = "newEntryTool";
            this.newEntryTool.Size = new System.Drawing.Size(23, 22);
            this.newEntryTool.Text = "GUI:CSFNewEntry";
            this.newEntryTool.Click += new System.EventHandler(this.newEntryTool_Click);
            // 
            // editEntryTool
            // 
            this.editEntryTool.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.editEntryTool.Enabled = false;
            this.editEntryTool.Image = global::VirtualBicycle.Ide.Properties.Resources.EditTableHS;
            this.editEntryTool.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.editEntryTool.Name = "editEntryTool";
            this.editEntryTool.Size = new System.Drawing.Size(23, 22);
            this.editEntryTool.Text = "GUI:CSFEdit";
            this.editEntryTool.Click += new System.EventHandler(this.editEntryTool_Click);
            // 
            // delEntryTool
            // 
            this.delEntryTool.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.delEntryTool.Enabled = false;
            this.delEntryTool.Image = global::VirtualBicycle.Ide.Properties.Resources.DeleteHS;
            this.delEntryTool.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.delEntryTool.Name = "delEntryTool";
            this.delEntryTool.Size = new System.Drawing.Size(23, 22);
            this.delEntryTool.Text = "GUI:CSFDeleteEntry";
            this.delEntryTool.Click += new System.EventHandler(this.delEntryTool_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(6, 25);
            // 
            // toolStripSplitButton1
            // 
            this.toolStripSplitButton1.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolStripSplitButton1.Image = ((System.Drawing.Image)(resources.GetObject("toolStripSplitButton1.Image")));
            this.toolStripSplitButton1.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripSplitButton1.Name = "toolStripSplitButton1";
            this.toolStripSplitButton1.Size = new System.Drawing.Size(32, 22);
            this.toolStripSplitButton1.Text = "toolStripSplitButton1";
            // 
            // toolStripComboBox1
            // 
            this.toolStripComboBox1.Name = "toolStripComboBox1";
            this.toolStripComboBox1.Size = new System.Drawing.Size(121, 25);
            // 
            // toolStripButton1
            // 
            this.toolStripButton1.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolStripButton1.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton1.Image")));
            this.toolStripButton1.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButton1.Name = "toolStripButton1";
            this.toolStripButton1.Size = new System.Drawing.Size(23, 22);
            this.toolStripButton1.Text = "toolStripButton1";
            // 
            // CsfDocument
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(582, 340);
            this.Controls.Add(this.toolStrip1);
            this.Controls.Add(this.plMain);
            this.DockAreas = WeifenLuo.WinFormsUI.Docking.DockAreas.Document;
            this.Name = "CsfDocument";
            this.ShowInTaskbar = false;
            this.TabText = "CsfEditor";
            this.Text = "CsfEditor";
            this.Load += new System.EventHandler(this.CsfDocument_Load);
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.CsfDocument_FormClosing);
            this.plMain.ResumeLayout(false);
            this.plMain.PerformLayout();
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            this.splitContainer1.ResumeLayout(false);
            this.splitContainer2.Panel1.ResumeLayout(false);
            this.splitContainer2.Panel2.ResumeLayout(false);
            this.splitContainer2.Panel2.PerformLayout();
            this.splitContainer2.ResumeLayout(false);
            this.toolStrip1.ResumeLayout(false);
            this.toolStrip1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.ListView listView1;
        private System.Windows.Forms.SplitContainer splitContainer2;
        private System.Windows.Forms.ListView listView2;
        private System.Windows.Forms.ToolStripButton newEntryTool;
        private System.Windows.Forms.Panel plMain;
        private System.Windows.Forms.ToolStrip toolStrip1;
        private System.Windows.Forms.ColumnHeader chCsfType;
        private System.Windows.Forms.ColumnHeader chCsfCount;
        private System.Windows.Forms.ColumnHeader chCsfEntry;
        private System.Windows.Forms.ColumnHeader chCsfContent;
        private System.Windows.Forms.ColumnHeader chCsfExtra;
        private System.Windows.Forms.ToolStripButton editEntryTool;
        private System.Windows.Forms.ToolStripButton delEntryTool;
        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripSplitButton toolStripSplitButton1;
        private System.Windows.Forms.ToolStripComboBox toolStripComboBox1;
        private System.Windows.Forms.ToolStripButton toolStripButton1;
    }
}