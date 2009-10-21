namespace VirtualBicycle.Ide.Designers
{
    partial class EmbeddedMeshDocument
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(EmbeddedMeshDocument));
            this.toolStrip1 = new System.Windows.Forms.ToolStrip();
            this.fillModeTool = new System.Windows.Forms.ToolStripDropDownButton();
            this.wireframeMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.solidMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator4 = new System.Windows.Forms.ToolStripSeparator();
            this.importTool = new System.Windows.Forms.ToolStripButton();
            this.exportTool = new System.Windows.Forms.ToolStripButton();
            this.openFileDialog1 = new System.Windows.Forms.OpenFileDialog();
            this.toolStrip2 = new System.Windows.Forms.ToolStrip();
            this.optimizeTool = new System.Windows.Forms.ToolStripButton();
            this.simplifyTool = new System.Windows.Forms.ToolStripButton();
            this.weldingTool = new System.Windows.Forms.ToolStripButton();
            this.subDevisionTool = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator3 = new System.Windows.Forms.ToolStripSeparator();
            this.softNormalTool = new System.Windows.Forms.ToolStripSplitButton();
            this.flatNormalTool = new System.Windows.Forms.ToolStripSplitButton();
            this.invNormalTool = new System.Windows.Forms.ToolStripButton();
            this.invNormalXTool = new System.Windows.Forms.ToolStripButton();
            this.invNormalYTool = new System.Windows.Forms.ToolStripButton();
            this.invNormalZTool = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator5 = new System.Windows.Forms.ToolStripSeparator();
            this.saveFileDialog1 = new System.Windows.Forms.SaveFileDialog();
            this.toolStrip1.SuspendLayout();
            this.toolStrip2.SuspendLayout();
            this.SuspendLayout();
            // 
            // toolStrip1
            // 
            this.toolStrip1.Dock = System.Windows.Forms.DockStyle.None;
            this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fillModeTool,
            this.toolStripSeparator4,
            this.importTool,
            this.exportTool});
            this.toolStrip1.Location = new System.Drawing.Point(9, 0);
            this.toolStrip1.Name = "toolStrip1";
            this.toolStrip1.Size = new System.Drawing.Size(149, 25);
            this.toolStrip1.TabIndex = 1;
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
            this.fillModeTool.Size = new System.Drawing.Size(85, 22);
            this.fillModeTool.Text = "fillMode";
            // 
            // wireframeMenuItem
            // 
            this.wireframeMenuItem.Name = "wireframeMenuItem";
            this.wireframeMenuItem.Size = new System.Drawing.Size(161, 22);
            this.wireframeMenuItem.Text = "GUI:wireFrame";
            this.wireframeMenuItem.Click += new System.EventHandler(this.wireframeMenuItem_Click);
            // 
            // solidMenuItem
            // 
            this.solidMenuItem.Name = "solidMenuItem";
            this.solidMenuItem.Size = new System.Drawing.Size(161, 22);
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
            this.exportTool.Click += new System.EventHandler(this.exportTool_Click);
            // 
            // openFileDialog1
            // 
            this.openFileDialog1.FileName = "openFileDialog1";
            this.openFileDialog1.RestoreDirectory = true;
            // 
            // toolStrip2
            // 
            this.toolStrip2.Dock = System.Windows.Forms.DockStyle.None;
            this.toolStrip2.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.optimizeTool,
            this.simplifyTool,
            this.weldingTool,
            this.subDevisionTool,
            this.toolStripSeparator3,
            this.softNormalTool,
            this.flatNormalTool,
            this.invNormalTool,
            this.invNormalXTool,
            this.invNormalYTool,
            this.invNormalZTool,
            this.toolStripSeparator5});
            this.toolStrip2.Location = new System.Drawing.Point(250, 0);
            this.toolStrip2.Name = "toolStrip2";
            this.toolStrip2.Size = new System.Drawing.Size(303, 25);
            this.toolStrip2.TabIndex = 2;
            this.toolStrip2.Text = "toolStrip2";
            // 
            // optimizeTool
            // 
            this.optimizeTool.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.optimizeTool.Image = global::VirtualBicycle.Ide.Properties.Resources.CheckGrammarHS;
            this.optimizeTool.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.optimizeTool.Name = "optimizeTool";
            this.optimizeTool.Size = new System.Drawing.Size(23, 22);
            this.optimizeTool.Text = "GUI:OptimizeMesh";
            this.optimizeTool.Click += new System.EventHandler(this.optimizeTool_Click);
            // 
            // simplifyTool
            // 
            this.simplifyTool.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.simplifyTool.Image = ((System.Drawing.Image)(resources.GetObject("simplifyTool.Image")));
            this.simplifyTool.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.simplifyTool.Name = "simplifyTool";
            this.simplifyTool.Size = new System.Drawing.Size(23, 22);
            this.simplifyTool.Text = "GUI:SimplifyMesh";
            this.simplifyTool.Click += new System.EventHandler(this.simplifyTool_Click);
            // 
            // weldingTool
            // 
            this.weldingTool.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.weldingTool.Image = ((System.Drawing.Image)(resources.GetObject("weldingTool.Image")));
            this.weldingTool.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.weldingTool.Name = "weldingTool";
            this.weldingTool.Size = new System.Drawing.Size(23, 22);
            this.weldingTool.Text = "GUI:WeldVertices";
            this.weldingTool.Click += new System.EventHandler(this.weldingTool_Click);
            // 
            // subDevisionTool
            // 
            this.subDevisionTool.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.subDevisionTool.Image = ((System.Drawing.Image)(resources.GetObject("subDevisionTool.Image")));
            this.subDevisionTool.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.subDevisionTool.Name = "subDevisionTool";
            this.subDevisionTool.Size = new System.Drawing.Size(23, 22);
            this.subDevisionTool.Text = "GUI:MeshSubDevision";
            this.subDevisionTool.Click += new System.EventHandler(this.subDevisionTool_Click);
            // 
            // toolStripSeparator3
            // 
            this.toolStripSeparator3.Name = "toolStripSeparator3";
            this.toolStripSeparator3.Size = new System.Drawing.Size(6, 25);
            // 
            // softNormalTool
            // 
            this.softNormalTool.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.softNormalTool.Image = ((System.Drawing.Image)(resources.GetObject("softNormalTool.Image")));
            this.softNormalTool.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.softNormalTool.Name = "softNormalTool";
            this.softNormalTool.Size = new System.Drawing.Size(32, 22);
            this.softNormalTool.Text = "GUI:ComputeNormals";
            this.softNormalTool.Click += new System.EventHandler(this.normalTool_Click);
            // 
            // flatNormalTool
            // 
            this.flatNormalTool.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.flatNormalTool.Image = ((System.Drawing.Image)(resources.GetObject("flatNormalTool.Image")));
            this.flatNormalTool.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.flatNormalTool.Name = "flatNormalTool";
            this.flatNormalTool.Size = new System.Drawing.Size(32, 22);
            this.flatNormalTool.Text = "GUI:FlatenNormals";
            this.flatNormalTool.ButtonClick += new System.EventHandler(this.flatNormalTool_ButtonClick);
            // 
            // invNormalTool
            // 
            this.invNormalTool.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.invNormalTool.Image = ((System.Drawing.Image)(resources.GetObject("invNormalTool.Image")));
            this.invNormalTool.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.invNormalTool.Name = "invNormalTool";
            this.invNormalTool.Size = new System.Drawing.Size(23, 22);
            this.invNormalTool.Text = "GUI:InverseNormal";
            this.invNormalTool.Click += new System.EventHandler(this.invNormalTool_Click);
            // 
            // invNormalXTool
            // 
            this.invNormalXTool.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.invNormalXTool.Image = ((System.Drawing.Image)(resources.GetObject("invNormalXTool.Image")));
            this.invNormalXTool.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.invNormalXTool.Name = "invNormalXTool";
            this.invNormalXTool.Size = new System.Drawing.Size(23, 22);
            this.invNormalXTool.Text = "GUI:InverseNX";
            this.invNormalXTool.Click += new System.EventHandler(this.specialNormalTool_Click);
            // 
            // invNormalYTool
            // 
            this.invNormalYTool.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.invNormalYTool.Image = ((System.Drawing.Image)(resources.GetObject("invNormalYTool.Image")));
            this.invNormalYTool.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.invNormalYTool.Name = "invNormalYTool";
            this.invNormalYTool.Size = new System.Drawing.Size(23, 22);
            this.invNormalYTool.Text = "GUI:InverseNY";
            this.invNormalYTool.Click += new System.EventHandler(this.invNormalYTool_Click);
            // 
            // invNormalZTool
            // 
            this.invNormalZTool.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.invNormalZTool.Image = ((System.Drawing.Image)(resources.GetObject("invNormalZTool.Image")));
            this.invNormalZTool.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.invNormalZTool.Name = "invNormalZTool";
            this.invNormalZTool.Size = new System.Drawing.Size(23, 22);
            this.invNormalZTool.Text = "GUI:InverseNZ";
            this.invNormalZTool.Click += new System.EventHandler(this.invNormalZTool_Click);
            // 
            // toolStripSeparator5
            // 
            this.toolStripSeparator5.Name = "toolStripSeparator5";
            this.toolStripSeparator5.Size = new System.Drawing.Size(6, 25);
            // 
            // saveFileDialog1
            // 
            this.saveFileDialog1.RestoreDirectory = true;
            // 
            // EmbeddedMeshDocument
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(576, 314);
            this.Controls.Add(this.toolStrip2);
            this.Controls.Add(this.toolStrip1);
            this.Name = "EmbeddedMeshDocument";
            this.TabText = "EmbeddedMeshDocument";
            this.Text = "EmbeddedMeshDocument";
            this.MouseDown += new System.Windows.Forms.MouseEventHandler(this.EmbededMeshDocument_MouseDown);
            this.MouseUp += new System.Windows.Forms.MouseEventHandler(this.EmbededMeshDocument_MouseUp);
            this.MouseMove += new System.Windows.Forms.MouseEventHandler(this.EmbededMeshDocument_MouseMove);
            this.MouseClick += new System.Windows.Forms.MouseEventHandler(this.EmbededMeshDocument_MouseClick);
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.EmbededMeshDocument_FormClosing);
            this.toolStrip1.ResumeLayout(false);
            this.toolStrip1.PerformLayout();
            this.toolStrip2.ResumeLayout(false);
            this.toolStrip2.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ToolStrip toolStrip1;
        private System.Windows.Forms.ToolStripButton importTool;
        private System.Windows.Forms.OpenFileDialog openFileDialog1;
        private System.Windows.Forms.ToolStrip toolStrip2;
        private System.Windows.Forms.ToolStripButton optimizeTool;
        private System.Windows.Forms.ToolStripButton simplifyTool;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator3;
        private System.Windows.Forms.ToolStripSplitButton flatNormalTool;
        private System.Windows.Forms.ToolStripDropDownButton fillModeTool;
        private System.Windows.Forms.ToolStripMenuItem wireframeMenuItem;
        private System.Windows.Forms.ToolStripMenuItem solidMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator4;
        private System.Windows.Forms.ToolStripButton invNormalTool;
        private System.Windows.Forms.ToolStripButton invNormalXTool;
        private System.Windows.Forms.ToolStripButton invNormalYTool;
        private System.Windows.Forms.ToolStripButton invNormalZTool;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator5;
        private System.Windows.Forms.ToolStripButton exportTool;
        private System.Windows.Forms.SaveFileDialog saveFileDialog1;
        private System.Windows.Forms.ToolStripButton subDevisionTool;
        private System.Windows.Forms.ToolStripSplitButton softNormalTool;
        private System.Windows.Forms.ToolStripButton weldingTool;
    }
}