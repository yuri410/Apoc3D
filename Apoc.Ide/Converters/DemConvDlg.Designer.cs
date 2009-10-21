namespace VirtualBicycle.Ide.Converters
{
    partial class DemConvDlg
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
            this.tableLayoutPanel1 = new System.Windows.Forms.TableLayoutPanel();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.progressBar1 = new System.Windows.Forms.ProgressBar();
            this.prgLabel = new System.Windows.Forms.Label();
            this.statLabel = new System.Windows.Forms.Label();
            this.fileListView = new System.Windows.Forms.ListView();
            this.columnHeader1 = new System.Windows.Forms.ColumnHeader();
            this.columnHeader2 = new System.Windows.Forms.ColumnHeader();
            this.okButton = new System.Windows.Forms.Button();
            this.cancelButton = new System.Windows.Forms.Button();
            this.addButton = new System.Windows.Forms.Button();
            this.removeButton = new System.Windows.Forms.Button();
            this.propertyGrid1 = new System.Windows.Forms.PropertyGrid();
            this.openFileDialog1 = new System.Windows.Forms.OpenFileDialog();
            this.tableLayoutPanel1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            this.SuspendLayout();
            // 
            // tableLayoutPanel1
            // 
            this.tableLayoutPanel1.AutoSize = true;
            this.tableLayoutPanel1.ColumnCount = 5;
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 10F));
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 25F));
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 25F));
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 10F));
            this.tableLayoutPanel1.Controls.Add(this.pictureBox1, 1, 1);
            this.tableLayoutPanel1.Controls.Add(this.progressBar1, 1, 3);
            this.tableLayoutPanel1.Controls.Add(this.prgLabel, 1, 2);
            this.tableLayoutPanel1.Controls.Add(this.statLabel, 2, 2);
            this.tableLayoutPanel1.Controls.Add(this.fileListView, 1, 4);
            this.tableLayoutPanel1.Controls.Add(this.okButton, 2, 9);
            this.tableLayoutPanel1.Controls.Add(this.cancelButton, 3, 9);
            this.tableLayoutPanel1.Controls.Add(this.addButton, 2, 8);
            this.tableLayoutPanel1.Controls.Add(this.removeButton, 3, 8);
            this.tableLayoutPanel1.Controls.Add(this.propertyGrid1, 2, 4);
            this.tableLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tableLayoutPanel1.Location = new System.Drawing.Point(0, 0);
            this.tableLayoutPanel1.Name = "tableLayoutPanel1";
            this.tableLayoutPanel1.RowCount = 11;
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 10F));
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 283F));
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 30F));
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 30F));
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 25F));
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 25F));
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 25F));
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 34F));
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 34F));
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 10F));
            this.tableLayoutPanel1.Size = new System.Drawing.Size(870, 574);
            this.tableLayoutPanel1.TabIndex = 0;
            // 
            // pictureBox1
            // 
            this.pictureBox1.BackgroundImageLayout = System.Windows.Forms.ImageLayout.None;
            this.tableLayoutPanel1.SetColumnSpan(this.pictureBox1, 3);
            this.pictureBox1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.pictureBox1.Image = global::VirtualBicycle.Ide.Properties.Resources.worldMap;
            this.pictureBox1.Location = new System.Drawing.Point(13, 13);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(843, 277);
            this.pictureBox1.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.pictureBox1.TabIndex = 0;
            this.pictureBox1.TabStop = false;
            // 
            // progressBar1
            // 
            this.tableLayoutPanel1.SetColumnSpan(this.progressBar1, 3);
            this.progressBar1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.progressBar1.Location = new System.Drawing.Point(13, 326);
            this.progressBar1.Name = "progressBar1";
            this.progressBar1.Size = new System.Drawing.Size(843, 24);
            this.progressBar1.TabIndex = 1;
            // 
            // prgLabel
            // 
            this.prgLabel.Dock = System.Windows.Forms.DockStyle.Fill;
            this.prgLabel.Location = new System.Drawing.Point(13, 293);
            this.prgLabel.Name = "prgLabel";
            this.prgLabel.Size = new System.Drawing.Size(419, 30);
            this.prgLabel.TabIndex = 2;
            this.prgLabel.Text = "label1";
            this.prgLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // statLabel
            // 
            this.tableLayoutPanel1.SetColumnSpan(this.statLabel, 2);
            this.statLabel.Dock = System.Windows.Forms.DockStyle.Fill;
            this.statLabel.Location = new System.Drawing.Point(438, 293);
            this.statLabel.Name = "statLabel";
            this.statLabel.Size = new System.Drawing.Size(418, 30);
            this.statLabel.TabIndex = 3;
            this.statLabel.Text = "label2";
            this.statLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // fileListView
            // 
            this.fileListView.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader1,
            this.columnHeader2});
            this.fileListView.Dock = System.Windows.Forms.DockStyle.Fill;
            this.fileListView.FullRowSelect = true;
            this.fileListView.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.Nonclickable;
            this.fileListView.Location = new System.Drawing.Point(13, 356);
            this.fileListView.MultiSelect = false;
            this.fileListView.Name = "fileListView";
            this.tableLayoutPanel1.SetRowSpan(this.fileListView, 6);
            this.fileListView.Size = new System.Drawing.Size(419, 205);
            this.fileListView.TabIndex = 4;
            this.fileListView.UseCompatibleStateImageBehavior = false;
            this.fileListView.View = System.Windows.Forms.View.Details;
            // 
            // columnHeader1
            // 
            this.columnHeader1.Text = "GUI:DEMTileName";
            this.columnHeader1.Width = 138;
            // 
            // columnHeader2
            // 
            this.columnHeader2.Text = "GUI:DEMTileCoord";
            this.columnHeader2.Width = 173;
            // 
            // okButton
            // 
            this.okButton.Dock = System.Windows.Forms.DockStyle.Fill;
            this.okButton.Location = new System.Drawing.Point(438, 533);
            this.okButton.Name = "okButton";
            this.okButton.Size = new System.Drawing.Size(206, 28);
            this.okButton.TabIndex = 10;
            this.okButton.Text = "GUI:OK";
            this.okButton.UseVisualStyleBackColor = true;
            this.okButton.Click += new System.EventHandler(this.okButton_Click);
            // 
            // cancelButton
            // 
            this.cancelButton.Dock = System.Windows.Forms.DockStyle.Fill;
            this.cancelButton.Location = new System.Drawing.Point(650, 533);
            this.cancelButton.Name = "cancelButton";
            this.cancelButton.Size = new System.Drawing.Size(206, 28);
            this.cancelButton.TabIndex = 11;
            this.cancelButton.Text = "GUI:Cancel";
            this.cancelButton.UseVisualStyleBackColor = true;
            this.cancelButton.Click += new System.EventHandler(this.cancelButton_Click);
            // 
            // addButton
            // 
            this.addButton.Dock = System.Windows.Forms.DockStyle.Fill;
            this.addButton.Location = new System.Drawing.Point(438, 499);
            this.addButton.Name = "addButton";
            this.addButton.Size = new System.Drawing.Size(206, 28);
            this.addButton.TabIndex = 12;
            this.addButton.Text = "GUI:Add";
            this.addButton.UseVisualStyleBackColor = true;
            this.addButton.Click += new System.EventHandler(this.addButton_Click);
            // 
            // removeButton
            // 
            this.removeButton.Dock = System.Windows.Forms.DockStyle.Fill;
            this.removeButton.Location = new System.Drawing.Point(650, 499);
            this.removeButton.Name = "removeButton";
            this.removeButton.Size = new System.Drawing.Size(206, 28);
            this.removeButton.TabIndex = 13;
            this.removeButton.Text = "GUI:Remove";
            this.removeButton.UseVisualStyleBackColor = true;
            // 
            // propertyGrid1
            // 
            this.tableLayoutPanel1.SetColumnSpan(this.propertyGrid1, 2);
            this.propertyGrid1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.propertyGrid1.Location = new System.Drawing.Point(438, 356);
            this.propertyGrid1.Name = "propertyGrid1";
            this.tableLayoutPanel1.SetRowSpan(this.propertyGrid1, 4);
            this.propertyGrid1.Size = new System.Drawing.Size(418, 137);
            this.propertyGrid1.TabIndex = 14;
            // 
            // openFileDialog1
            // 
            this.openFileDialog1.FileName = "openFileDialog1";
            // 
            // DemConvDlg
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(870, 574);
            this.Controls.Add(this.tableLayoutPanel1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "DemConvDlg";
            this.Text = "DemConvDlg";
            this.tableLayoutPanel1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel1;
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.ProgressBar progressBar1;
        private System.Windows.Forms.Label prgLabel;
        private System.Windows.Forms.Label statLabel;
        private System.Windows.Forms.ListView fileListView;
        private System.Windows.Forms.ColumnHeader columnHeader1;
        private System.Windows.Forms.ColumnHeader columnHeader2;
        private System.Windows.Forms.Button okButton;
        private System.Windows.Forms.Button cancelButton;
        private System.Windows.Forms.Button addButton;
        private System.Windows.Forms.Button removeButton;
        private System.Windows.Forms.PropertyGrid propertyGrid1;
        private System.Windows.Forms.OpenFileDialog openFileDialog1;
    }
}