namespace VirtualBicycle.Ide.Designers
{
    partial class ModelDesigner
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ModelDesigner));
            this.toolStrip1 = new System.Windows.Forms.ToolStrip();
            this.fillModeTool = new System.Windows.Forms.ToolStripDropDownButton();
            this.wireframeMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.solidMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator4 = new System.Windows.Forms.ToolStripSeparator();
            this.importTool = new System.Windows.Forms.ToolStripButton();
            this.exportTool = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.importAnimTool = new System.Windows.Forms.ToolStripButton();
            this.exportAnimTool = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.openFileDialog1 = new System.Windows.Forms.OpenFileDialog();
            this.toolStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // toolStrip1
            // 
            this.toolStrip1.Dock = System.Windows.Forms.DockStyle.None;
            this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fillModeTool,
            this.toolStripSeparator4,
            this.importTool,
            this.exportTool,
            this.toolStripSeparator1,
            this.importAnimTool,
            this.exportAnimTool,
            this.toolStripSeparator2});
            this.toolStrip1.Location = new System.Drawing.Point(18, 9);
            this.toolStrip1.Name = "toolStrip1";
            this.toolStrip1.Size = new System.Drawing.Size(204, 25);
            this.toolStrip1.TabIndex = 2;
            this.toolStrip1.Text = "toolStrip1";
            // 
            // fillModeTool
            // 
            this.fillModeTool.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.wireframeMenuItem,
            this.solidMenuItem});
            this.fillModeTool.Image = ((System.Drawing.Image)(resources.GetObject("fillModeTool.Image")));
            this.fillModeTool.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.fillModeTool.Name = "fillModeTool";
            this.fillModeTool.Size = new System.Drawing.Size(82, 22);
            this.fillModeTool.Text = "fillMode";
            // 
            // wireframeMenuItem
            // 
            this.wireframeMenuItem.Name = "wireframeMenuItem";
            this.wireframeMenuItem.Size = new System.Drawing.Size(148, 22);
            this.wireframeMenuItem.Text = "GUI:wireFrame";
            this.wireframeMenuItem.Click += new System.EventHandler(this.wireframeMenuItem_Click);
            // 
            // solidMenuItem
            // 
            this.solidMenuItem.Name = "solidMenuItem";
            this.solidMenuItem.Size = new System.Drawing.Size(148, 22);
            this.solidMenuItem.Text = "GUI:Solid";
            this.solidMenuItem.Click += new System.EventHandler(this.solidMenuItem_Click);
            // 
            // toolStripSeparator4
            // 
            this.toolStripSeparator4.Name = "toolStripSeparator4";
            this.toolStripSeparator4.Size = new System.Drawing.Size(6, 25);
            // 
            // importTool
            // 
            this.importTool.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.importTool.Image = ((System.Drawing.Image)(resources.GetObject("importTool.Image")));
            this.importTool.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.importTool.Name = "importTool";
            this.importTool.Size = new System.Drawing.Size(23, 22);
            this.importTool.Text = "GUI:ImportModel";
            this.importTool.Click += new System.EventHandler(this.importTool_Click);
            // 
            // exportTool
            // 
            this.exportTool.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.exportTool.Image = ((System.Drawing.Image)(resources.GetObject("exportTool.Image")));
            this.exportTool.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.exportTool.Name = "exportTool";
            this.exportTool.Size = new System.Drawing.Size(23, 22);
            this.exportTool.Text = "GUI:ExportModel";
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(6, 25);
            // 
            // importAnimTool
            // 
            this.importAnimTool.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.importAnimTool.Image = ((System.Drawing.Image)(resources.GetObject("importAnimTool.Image")));
            this.importAnimTool.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.importAnimTool.Name = "importAnimTool";
            this.importAnimTool.Size = new System.Drawing.Size(23, 22);
            this.importAnimTool.Text = "toolStripButton1";
            // 
            // exportAnimTool
            // 
            this.exportAnimTool.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.exportAnimTool.Image = ((System.Drawing.Image)(resources.GetObject("exportAnimTool.Image")));
            this.exportAnimTool.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.exportAnimTool.Name = "exportAnimTool";
            this.exportAnimTool.Size = new System.Drawing.Size(23, 22);
            this.exportAnimTool.Text = "toolStripButton6";
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(6, 25);
            // 
            // openFileDialog1
            // 
            this.openFileDialog1.FileName = "openFileDialog1";
            // 
            // ModelDocument
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(667, 379);
            this.Controls.Add(this.toolStrip1);
            this.Name = "ModelDocument";
            this.TabText = "MeshDocument";
            this.Text = "MeshDocument";
            this.MouseDown += new System.Windows.Forms.MouseEventHandler(this.ModelDocument_MouseDown);
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.ModelDocument_FormClosing);
            this.MouseMove += new System.Windows.Forms.MouseEventHandler(this.ModelDocument_MouseMove);
            this.toolStrip1.ResumeLayout(false);
            this.toolStrip1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ToolStrip toolStrip1;
        private System.Windows.Forms.ToolStripDropDownButton fillModeTool;
        private System.Windows.Forms.ToolStripMenuItem wireframeMenuItem;
        private System.Windows.Forms.ToolStripMenuItem solidMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator4;
        private System.Windows.Forms.ToolStripButton importTool;
        private System.Windows.Forms.ToolStripButton exportTool;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripButton importAnimTool;
        private System.Windows.Forms.ToolStripButton exportAnimTool;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.OpenFileDialog openFileDialog1;

    }
}