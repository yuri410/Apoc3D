namespace VirtualBicycle.Ide.Converters
{
    partial class MeshSimpDlg
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MeshSimpDlg));
            this.toolStrip1 = new System.Windows.Forms.ToolStrip();
            this.toolStripLabel1 = new System.Windows.Forms.ToolStripLabel();
            this.slvlCombo = new System.Windows.Forms.ToolStripComboBox();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.statusStrip1 = new System.Windows.Forms.StatusStrip();
            this.statusLabel = new System.Windows.Forms.ToolStripStatusLabel();
            this.simProgBar = new System.Windows.Forms.ToolStripProgressBar();
            this.reSimTool = new System.Windows.Forms.ToolStripButton();
            this.fillModeTool = new System.Windows.Forms.ToolStripDropDownButton();
            this.wireframeMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.solidMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStrip1.SuspendLayout();
            this.statusStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // toolStrip1
            // 
            this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripLabel1,
            this.slvlCombo,
            this.reSimTool,
            this.toolStripSeparator1,
            this.fillModeTool});
            this.toolStrip1.Location = new System.Drawing.Point(0, 0);
            this.toolStrip1.Name = "toolStrip1";
            this.toolStrip1.Size = new System.Drawing.Size(632, 25);
            this.toolStrip1.TabIndex = 0;
            this.toolStrip1.Text = "toolStrip1";
            // 
            // toolStripLabel1
            // 
            this.toolStripLabel1.Name = "toolStripLabel1";
            this.toolStripLabel1.Size = new System.Drawing.Size(107, 22);
            this.toolStripLabel1.Text = "GUI:SelectedLevel";
            // 
            // slvlCombo
            // 
            this.slvlCombo.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.slvlCombo.Name = "slvlCombo";
            this.slvlCombo.Size = new System.Drawing.Size(175, 25);
            this.slvlCombo.SelectedIndexChanged += new System.EventHandler(this.slvlCombo_SelectedIndexChanged);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(6, 25);
            // 
            // statusStrip1
            // 
            this.statusStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.statusLabel,
            this.simProgBar});
            this.statusStrip1.Location = new System.Drawing.Point(0, 424);
            this.statusStrip1.Name = "statusStrip1";
            this.statusStrip1.Size = new System.Drawing.Size(632, 22);
            this.statusStrip1.TabIndex = 1;
            this.statusStrip1.Text = "statusStrip1";
            // 
            // statusLabel
            // 
            this.statusLabel.Name = "statusLabel";
            this.statusLabel.Size = new System.Drawing.Size(215, 17);
            this.statusLabel.Spring = true;
            this.statusLabel.Text = "Status:Ready";
            // 
            // simProgBar
            // 
            this.simProgBar.Name = "simProgBar";
            this.simProgBar.Size = new System.Drawing.Size(400, 16);
            // 
            // reSimTool
            // 
            this.reSimTool.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.reSimTool.Image = ((System.Drawing.Image)(resources.GetObject("reSimTool.Image")));
            this.reSimTool.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.reSimTool.Name = "reSimTool";
            this.reSimTool.Size = new System.Drawing.Size(23, 22);
            this.reSimTool.Text = "GUI:ReSimplification";
            this.reSimTool.Click += new System.EventHandler(this.reSimTool_Click);
            // 
            // fillModeTool
            // 
            this.fillModeTool.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.wireframeMenuItem,
            this.solidMenuItem});
            this.fillModeTool.Image = ((System.Drawing.Image)(resources.GetObject("fillModeTool.Image")));
            this.fillModeTool.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.fillModeTool.Name = "fillModeTool";
            this.fillModeTool.Size = new System.Drawing.Size(106, 22);
            this.fillModeTool.Text = "fillModeTool";
            // 
            // wireframeMenuItem
            // 
            this.wireframeMenuItem.Name = "wireframeMenuItem";
            this.wireframeMenuItem.Size = new System.Drawing.Size(148, 22);
            this.wireframeMenuItem.Text = "GUI:wireframe";
            this.wireframeMenuItem.Click += new System.EventHandler(this.wireframeMenuItem_Click);
            // 
            // solidMenuItem
            // 
            this.solidMenuItem.Name = "solidMenuItem";
            this.solidMenuItem.Size = new System.Drawing.Size(148, 22);
            this.solidMenuItem.Text = "GUI:solid";
            this.solidMenuItem.Click += new System.EventHandler(this.solidMenuItem_Click);
            // 
            // MeshSimpDlg
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(632, 446);
            this.Controls.Add(this.statusStrip1);
            this.Controls.Add(this.toolStrip1);
            this.Name = "MeshSimpDlg";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "GUI:MeshSimplification";
            this.Load += new System.EventHandler(this.V2MPrevDlg_Load);
            this.Paint += new System.Windows.Forms.PaintEventHandler(this.V2MPrevDlg_Paint);
            this.MouseDown += new System.Windows.Forms.MouseEventHandler(this.V2MPrevDlg_MouseDown);
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.V2MPrevDlg_FormClosing);
            this.MouseMove += new System.Windows.Forms.MouseEventHandler(this.V2MPrevDlg_MouseMove);
            this.toolStrip1.ResumeLayout(false);
            this.toolStrip1.PerformLayout();
            this.statusStrip1.ResumeLayout(false);
            this.statusStrip1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ToolStrip toolStrip1;
        private System.Windows.Forms.ToolStripComboBox slvlCombo;
        private System.Windows.Forms.ToolStripDropDownButton fillModeTool;
        private System.Windows.Forms.ToolStripMenuItem wireframeMenuItem;
        private System.Windows.Forms.ToolStripMenuItem solidMenuItem;
        private System.Windows.Forms.StatusStrip statusStrip1;
        private System.Windows.Forms.ToolStripProgressBar simProgBar;
        private System.Windows.Forms.ToolStripStatusLabel statusLabel;
        private System.Windows.Forms.ToolStripButton reSimTool;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripLabel toolStripLabel1;
    }
}