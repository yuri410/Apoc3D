namespace VirtualBicycle.Ide.Tools
{
    partial class ExplorerWindow
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ExplorerWindow));
            this.toolStrip1 = new System.Windows.Forms.ToolStrip();
            this.refreshTool = new System.Windows.Forms.ToolStripButton();
            this.treeView1 = new System.Windows.Forms.TreeView();
            this.toolStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // toolStrip1
            // 
            this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.refreshTool});
            this.toolStrip1.Location = new System.Drawing.Point(0, 0);
            this.toolStrip1.Name = "toolStrip1";
            this.toolStrip1.Size = new System.Drawing.Size(292, 25);
            this.toolStrip1.TabIndex = 0;
            this.toolStrip1.Text = "toolStrip1";
            // 
            // refreshTool
            // 
            this.refreshTool.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.refreshTool.Image = ((System.Drawing.Image)(resources.GetObject("refreshTool.Image")));
            this.refreshTool.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.refreshTool.Name = "refreshTool";
            this.refreshTool.Size = new System.Drawing.Size(23, 22);
            this.refreshTool.Text = "GUI:Refresh";
            this.refreshTool.Click += new System.EventHandler(this.refreshTool_Click);
            // 
            // treeView1
            // 
            this.treeView1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.treeView1.Location = new System.Drawing.Point(0, 25);
            this.treeView1.Name = "treeView1";
            this.treeView1.Size = new System.Drawing.Size(292, 221);
            this.treeView1.TabIndex = 1;
            this.treeView1.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.treeView1_AfterSelect);
            // 
            // ExplorerWindow
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(292, 246);
            this.Controls.Add(this.treeView1);
            this.Controls.Add(this.toolStrip1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.SizableToolWindow;
            this.Name = "ExplorerWindow";
            this.Text = "GUI:ExplorerWnd";
            this.Load += new System.EventHandler(this.ExplorerWindow_Load);
            this.TextChanged += new System.EventHandler(this.ExplorerWindow_TextChanged);
            this.toolStrip1.ResumeLayout(false);
            this.toolStrip1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ToolStrip toolStrip1;
        private System.Windows.Forms.TreeView treeView1;
        private System.Windows.Forms.ToolStripButton refreshTool;
    }
}