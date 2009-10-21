namespace VirtualBicycle.Ide.Designers.WorldBuilder
{
    partial class WorldDesigner
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(WorldDesigner));
            this.toolStrip1 = new System.Windows.Forms.ToolStrip();
            this.moveForwardBtn = new System.Windows.Forms.ToolStripButton();
            this.moveBackwardBtn = new System.Windows.Forms.ToolStripButton();
            this.moveLeftBtn = new System.Windows.Forms.ToolStripButton();
            this.moveRightBtn = new System.Windows.Forms.ToolStripButton();
            this.turnLeftBtn = new System.Windows.Forms.ToolStripButton();
            this.turnRightBtn = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator4 = new System.Windows.Forms.ToolStripSeparator();
            this.fillModeTool = new System.Windows.Forms.ToolStripDropDownButton();
            this.wireframeFMItem = new System.Windows.Forms.ToolStripMenuItem();
            this.solidFMItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStrip2 = new System.Windows.Forms.ToolStrip();
            this.selectToolBtn = new System.Windows.Forms.ToolStripButton();
            this.rotateToolBtn = new System.Windows.Forms.ToolStripButton();
            this.moveToolBtn = new System.Windows.Forms.ToolStripButton();
            this.deleteToolBtn = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator5 = new System.Windows.Forms.ToolStripSeparator();
            this.higherTerrainToolBtn = new System.Windows.Forms.ToolStripButton();
            this.lowerTerrainToolBtn = new System.Windows.Forms.ToolStripButton();
            this.paintMaterialBtn = new System.Windows.Forms.ToolStripButton();
            this.smoothenToolBtn = new System.Windows.Forms.ToolStripButton();
            this.materialCombo = new System.Windows.Forms.ToolStripComboBox();
            this.changeMaterialsBtn = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.toolStripLabel1 = new System.Windows.Forms.ToolStripLabel();
            this.brushSizeCombo = new System.Windows.Forms.ToolStripComboBox();
            this.toolStripSeparator3 = new System.Windows.Forms.ToolStripSeparator();
            this.toolStrip3 = new System.Windows.Forms.ToolStrip();
            this.scenePropBtn = new System.Windows.Forms.ToolStripButton();
            this.toolStrip4 = new System.Windows.Forms.ToolStrip();
            this.importDispMapToolBtn = new System.Windows.Forms.ToolStripButton();
            this.importColorMapToolBtn = new System.Windows.Forms.ToolStripButton();
            this.importNormalMapToolBtn = new System.Windows.Forms.ToolStripButton();
            this.importIndexMapToolBtn = new System.Windows.Forms.ToolStripButton();
            this.openFileDialog1 = new System.Windows.Forms.OpenFileDialog();
            this.toolStrip5 = new System.Windows.Forms.ToolStrip();
            this.selectNodeToolBtn = new System.Windows.Forms.ToolStripButton();
            this.moveNodeToolBtn = new System.Windows.Forms.ToolStripButton();
            this.newNodeToolBtn = new System.Windows.Forms.ToolStripButton();
            this.deleteNodeToolBtn = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator6 = new System.Windows.Forms.ToolStripSeparator();
            this.fitRoadToolBtn = new System.Windows.Forms.ToolStripButton();
            this.straightenRoadToolBtn = new System.Windows.Forms.ToolStripButton();
            this.roadTextureCombo = new System.Windows.Forms.ToolStripComboBox();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.portSelectToolBtn = new System.Windows.Forms.ToolStripButton();
            this.connectPortToolBtn = new System.Windows.Forms.ToolStripButton();
            this.disConnectTool = new System.Windows.Forms.ToolStripButton();
            this.toolStrip1.SuspendLayout();
            this.toolStrip2.SuspendLayout();
            this.toolStrip3.SuspendLayout();
            this.toolStrip4.SuspendLayout();
            this.toolStrip5.SuspendLayout();
            this.SuspendLayout();
            // 
            // toolStrip1
            // 
            this.toolStrip1.Dock = System.Windows.Forms.DockStyle.None;
            this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.moveForwardBtn,
            this.moveBackwardBtn,
            this.moveLeftBtn,
            this.moveRightBtn,
            this.turnLeftBtn,
            this.turnRightBtn,
            this.toolStripSeparator4,
            this.fillModeTool});
            this.toolStrip1.Location = new System.Drawing.Point(9, 9);
            this.toolStrip1.Name = "toolStrip1";
            this.toolStrip1.Size = new System.Drawing.Size(185, 25);
            this.toolStrip1.TabIndex = 0;
            this.toolStrip1.Text = "toolStrip1";
            // 
            // moveForwardBtn
            // 
            this.moveForwardBtn.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.moveForwardBtn.Image = global::VirtualBicycle.Ide.Properties.Resources.GoToPreviousMessage;
            this.moveForwardBtn.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.moveForwardBtn.Name = "moveForwardBtn";
            this.moveForwardBtn.Size = new System.Drawing.Size(23, 22);
            this.moveForwardBtn.Text = "GUI:WBMoveForward";
            this.moveForwardBtn.Click += new System.EventHandler(this.moveForwardBtn_Click);
            // 
            // moveBackwardBtn
            // 
            this.moveBackwardBtn.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.moveBackwardBtn.Image = global::VirtualBicycle.Ide.Properties.Resources.GoToNextMessage;
            this.moveBackwardBtn.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.moveBackwardBtn.Name = "moveBackwardBtn";
            this.moveBackwardBtn.Size = new System.Drawing.Size(23, 22);
            this.moveBackwardBtn.Text = "GUI:WBMoveBackward";
            this.moveBackwardBtn.Click += new System.EventHandler(this.moveBackwardBtn_Click);
            // 
            // moveLeftBtn
            // 
            this.moveLeftBtn.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.moveLeftBtn.Image = global::VirtualBicycle.Ide.Properties.Resources.GoToPrevious;
            this.moveLeftBtn.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.moveLeftBtn.Name = "moveLeftBtn";
            this.moveLeftBtn.Size = new System.Drawing.Size(23, 22);
            this.moveLeftBtn.Text = "GUI:WBMoveLeft";
            this.moveLeftBtn.Click += new System.EventHandler(this.moveLeftBtn_Click);
            // 
            // moveRightBtn
            // 
            this.moveRightBtn.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.moveRightBtn.Image = global::VirtualBicycle.Ide.Properties.Resources.GoToNextHS;
            this.moveRightBtn.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.moveRightBtn.Name = "moveRightBtn";
            this.moveRightBtn.Size = new System.Drawing.Size(23, 22);
            this.moveRightBtn.Text = "GUI:WBMoveRight";
            this.moveRightBtn.Click += new System.EventHandler(this.moveRightBtn_Click);
            // 
            // turnLeftBtn
            // 
            this.turnLeftBtn.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.turnLeftBtn.Image = global::VirtualBicycle.Ide.Properties.Resources.Edit_UndoHS;
            this.turnLeftBtn.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.turnLeftBtn.Name = "turnLeftBtn";
            this.turnLeftBtn.Size = new System.Drawing.Size(23, 22);
            this.turnLeftBtn.Text = "GUI:WBTrunLeft";
            this.turnLeftBtn.Click += new System.EventHandler(this.turnLeftBtn_Click);
            // 
            // turnRightBtn
            // 
            this.turnRightBtn.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.turnRightBtn.Image = global::VirtualBicycle.Ide.Properties.Resources.Edit_RedoHS;
            this.turnRightBtn.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.turnRightBtn.Name = "turnRightBtn";
            this.turnRightBtn.Size = new System.Drawing.Size(23, 22);
            this.turnRightBtn.Text = "GUI:WBTurnRight";
            this.turnRightBtn.Click += new System.EventHandler(this.turnRightBtn_Click);
            // 
            // toolStripSeparator4
            // 
            this.toolStripSeparator4.Name = "toolStripSeparator4";
            this.toolStripSeparator4.Size = new System.Drawing.Size(6, 25);
            // 
            // fillModeTool
            // 
            this.fillModeTool.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.fillModeTool.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.wireframeFMItem,
            this.solidFMItem});
            this.fillModeTool.Image = ((System.Drawing.Image)(resources.GetObject("fillModeTool.Image")));
            this.fillModeTool.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.fillModeTool.Name = "fillModeTool";
            this.fillModeTool.Size = new System.Drawing.Size(29, 22);
            this.fillModeTool.Text = "toolStripDropDownButton1";
            // 
            // wireframeFMItem
            // 
            this.wireframeFMItem.Name = "wireframeFMItem";
            this.wireframeFMItem.Size = new System.Drawing.Size(197, 22);
            this.wireframeFMItem.Text = "GUI:WireframeMode";
            this.wireframeFMItem.Click += new System.EventHandler(this.wireframeFMItem_Click);
            // 
            // solidFMItem
            // 
            this.solidFMItem.Name = "solidFMItem";
            this.solidFMItem.Size = new System.Drawing.Size(197, 22);
            this.solidFMItem.Text = "GUI:SolidMode";
            this.solidFMItem.Click += new System.EventHandler(this.solidFMItem_Click);
            // 
            // toolStrip2
            // 
            this.toolStrip2.Dock = System.Windows.Forms.DockStyle.None;
            this.toolStrip2.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.selectToolBtn,
            this.rotateToolBtn,
            this.moveToolBtn,
            this.deleteToolBtn,
            this.toolStripSeparator5,
            this.higherTerrainToolBtn,
            this.lowerTerrainToolBtn,
            this.paintMaterialBtn,
            this.smoothenToolBtn,
            this.materialCombo,
            this.changeMaterialsBtn,
            this.toolStripSeparator1,
            this.toolStripLabel1,
            this.brushSizeCombo,
            this.toolStripSeparator3});
            this.toolStrip2.Location = new System.Drawing.Point(9, 34);
            this.toolStrip2.Name = "toolStrip2";
            this.toolStrip2.Size = new System.Drawing.Size(546, 25);
            this.toolStrip2.TabIndex = 1;
            this.toolStrip2.Text = "toolStrip2";
            // 
            // selectToolBtn
            // 
            this.selectToolBtn.CheckOnClick = true;
            this.selectToolBtn.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.selectToolBtn.Image = global::VirtualBicycle.Ide.Properties.Resources.PointerHS;
            this.selectToolBtn.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.selectToolBtn.Name = "selectToolBtn";
            this.selectToolBtn.Size = new System.Drawing.Size(23, 22);
            this.selectToolBtn.Text = "GUI:WBSelection";
            this.selectToolBtn.CheckedChanged += new System.EventHandler(this.selectToolBtn_CheckedChanged);
            // 
            // rotateToolBtn
            // 
            this.rotateToolBtn.CheckOnClick = true;
            this.rotateToolBtn.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.rotateToolBtn.Image = global::VirtualBicycle.Ide.Properties.Resources.rotateTool;
            this.rotateToolBtn.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.rotateToolBtn.Name = "rotateToolBtn";
            this.rotateToolBtn.Size = new System.Drawing.Size(23, 22);
            this.rotateToolBtn.Text = "GUI:WBMoveTool";
            this.rotateToolBtn.CheckedChanged += new System.EventHandler(this.rotateToolBtn_CheckedChanged);
            // 
            // moveToolBtn
            // 
            this.moveToolBtn.CheckOnClick = true;
            this.moveToolBtn.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.moveToolBtn.Image = global::VirtualBicycle.Ide.Properties.Resources.moveTool;
            this.moveToolBtn.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.moveToolBtn.Name = "moveToolBtn";
            this.moveToolBtn.Size = new System.Drawing.Size(23, 22);
            this.moveToolBtn.Text = "GUI:WBRotateTool";
            this.moveToolBtn.CheckedChanged += new System.EventHandler(this.moveToolBtn_CheckedChanged);
            // 
            // deleteToolBtn
            // 
            this.deleteToolBtn.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.deleteToolBtn.Image = global::VirtualBicycle.Ide.Properties.Resources.DeleteHS1;
            this.deleteToolBtn.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.deleteToolBtn.Name = "deleteToolBtn";
            this.deleteToolBtn.Size = new System.Drawing.Size(23, 22);
            this.deleteToolBtn.Text = "GUI:WBDeleteObjectTool";
            this.deleteToolBtn.Click += new System.EventHandler(this.deleteToolBtn_Click);
            // 
            // toolStripSeparator5
            // 
            this.toolStripSeparator5.Name = "toolStripSeparator5";
            this.toolStripSeparator5.Size = new System.Drawing.Size(6, 25);
            // 
            // higherTerrainToolBtn
            // 
            this.higherTerrainToolBtn.CheckOnClick = true;
            this.higherTerrainToolBtn.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.higherTerrainToolBtn.Image = global::VirtualBicycle.Ide.Properties.Resources.raiseTerrain;
            this.higherTerrainToolBtn.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.higherTerrainToolBtn.Name = "higherTerrainToolBtn";
            this.higherTerrainToolBtn.Size = new System.Drawing.Size(23, 22);
            this.higherTerrainToolBtn.Text = "GUI:WBRaiseTerrain";
            this.higherTerrainToolBtn.CheckedChanged += new System.EventHandler(this.higherTerrainToolBtn_CheckedChanged);
            // 
            // lowerTerrainToolBtn
            // 
            this.lowerTerrainToolBtn.CheckOnClick = true;
            this.lowerTerrainToolBtn.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.lowerTerrainToolBtn.Image = global::VirtualBicycle.Ide.Properties.Resources.lowerTerrain;
            this.lowerTerrainToolBtn.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.lowerTerrainToolBtn.Name = "lowerTerrainToolBtn";
            this.lowerTerrainToolBtn.Size = new System.Drawing.Size(23, 22);
            this.lowerTerrainToolBtn.Text = "GUI:WBLowerTerrain";
            this.lowerTerrainToolBtn.CheckedChanged += new System.EventHandler(this.lowerTerrainToolBtn_CheckedChanged);
            // 
            // paintMaterialBtn
            // 
            this.paintMaterialBtn.CheckOnClick = true;
            this.paintMaterialBtn.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.paintMaterialBtn.Image = global::VirtualBicycle.Ide.Properties.Resources.LineColorHS;
            this.paintMaterialBtn.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.paintMaterialBtn.Name = "paintMaterialBtn";
            this.paintMaterialBtn.Size = new System.Drawing.Size(23, 22);
            this.paintMaterialBtn.Text = "GUI:WBPaintMaterial";
            this.paintMaterialBtn.CheckedChanged += new System.EventHandler(this.paintMaterialBtn_CheckedChanged);
            // 
            // smoothenToolBtn
            // 
            this.smoothenToolBtn.CheckOnClick = true;
            this.smoothenToolBtn.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.smoothenToolBtn.Image = global::VirtualBicycle.Ide.Properties.Resources.smoothTerrain;
            this.smoothenToolBtn.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.smoothenToolBtn.Name = "smoothenToolBtn";
            this.smoothenToolBtn.Size = new System.Drawing.Size(23, 22);
            this.smoothenToolBtn.Text = "GUI:WBSmoothenTool";
            this.smoothenToolBtn.CheckedChanged += new System.EventHandler(this.smoothenToolBtn_CheckedChanged);
            // 
            // materialCombo
            // 
            this.materialCombo.Name = "materialCombo";
            this.materialCombo.Size = new System.Drawing.Size(121, 25);
            this.materialCombo.SelectedIndexChanged += new System.EventHandler(this.materialCombo_SelectedIndexChanged);
            // 
            // changeMaterialsBtn
            // 
            this.changeMaterialsBtn.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.changeMaterialsBtn.Image = global::VirtualBicycle.Ide.Properties.Resources.DisplayInColorHS;
            this.changeMaterialsBtn.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.changeMaterialsBtn.Name = "changeMaterialsBtn";
            this.changeMaterialsBtn.Size = new System.Drawing.Size(23, 22);
            this.changeMaterialsBtn.Text = "GUI:WBChangeMaterials";
            this.changeMaterialsBtn.Click += new System.EventHandler(this.changeMaterialsBtn_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(6, 25);
            // 
            // toolStripLabel1
            // 
            this.toolStripLabel1.Name = "toolStripLabel1";
            this.toolStripLabel1.Size = new System.Drawing.Size(109, 22);
            this.toolStripLabel1.Text = "GUI:WBBrushSize";
            // 
            // brushSizeCombo
            // 
            this.brushSizeCombo.DropDownWidth = 75;
            this.brushSizeCombo.Items.AddRange(new object[] {
            "1",
            "5",
            "10",
            "15",
            "20"});
            this.brushSizeCombo.Name = "brushSizeCombo";
            this.brushSizeCombo.Size = new System.Drawing.Size(75, 25);
            this.brushSizeCombo.SelectedIndexChanged += new System.EventHandler(this.brushSizeCombo_SelectedIndexChanged);
            // 
            // toolStripSeparator3
            // 
            this.toolStripSeparator3.Name = "toolStripSeparator3";
            this.toolStripSeparator3.Size = new System.Drawing.Size(6, 25);
            // 
            // toolStrip3
            // 
            this.toolStrip3.Dock = System.Windows.Forms.DockStyle.None;
            this.toolStrip3.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.scenePropBtn});
            this.toolStrip3.Location = new System.Drawing.Point(239, 9);
            this.toolStrip3.Name = "toolStrip3";
            this.toolStrip3.Size = new System.Drawing.Size(35, 25);
            this.toolStrip3.TabIndex = 2;
            this.toolStrip3.Text = "toolStrip3";
            // 
            // scenePropBtn
            // 
            this.scenePropBtn.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.scenePropBtn.Image = global::VirtualBicycle.Ide.Properties.Resources.PropertiesHS;
            this.scenePropBtn.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.scenePropBtn.Name = "scenePropBtn";
            this.scenePropBtn.Size = new System.Drawing.Size(23, 22);
            this.scenePropBtn.Text = "GUI:WBSceneProperties";
            this.scenePropBtn.Click += new System.EventHandler(this.scenePropBtn_Click);
            // 
            // toolStrip4
            // 
            this.toolStrip4.Dock = System.Windows.Forms.DockStyle.None;
            this.toolStrip4.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.importDispMapToolBtn,
            this.importColorMapToolBtn,
            this.importNormalMapToolBtn,
            this.importIndexMapToolBtn});
            this.toolStrip4.Location = new System.Drawing.Point(9, 59);
            this.toolStrip4.Name = "toolStrip4";
            this.toolStrip4.Size = new System.Drawing.Size(104, 25);
            this.toolStrip4.TabIndex = 3;
            this.toolStrip4.Text = "toolStrip4";
            // 
            // importDispMapToolBtn
            // 
            this.importDispMapToolBtn.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.importDispMapToolBtn.Image = ((System.Drawing.Image)(resources.GetObject("importDispMapToolBtn.Image")));
            this.importDispMapToolBtn.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.importDispMapToolBtn.Name = "importDispMapToolBtn";
            this.importDispMapToolBtn.Size = new System.Drawing.Size(23, 22);
            this.importDispMapToolBtn.Text = "GUI:WBImportDispMap";
            this.importDispMapToolBtn.Click += new System.EventHandler(this.importDispMapTool_Click);
            // 
            // importColorMapToolBtn
            // 
            this.importColorMapToolBtn.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.importColorMapToolBtn.Image = global::VirtualBicycle.Ide.Properties.Resources.importColorMap;
            this.importColorMapToolBtn.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.importColorMapToolBtn.Name = "importColorMapToolBtn";
            this.importColorMapToolBtn.Size = new System.Drawing.Size(23, 22);
            this.importColorMapToolBtn.Text = "GUI:WBImportColorMap";
            this.importColorMapToolBtn.Click += new System.EventHandler(this.importColorMapTool_Click);
            // 
            // importNormalMapToolBtn
            // 
            this.importNormalMapToolBtn.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.importNormalMapToolBtn.Image = global::VirtualBicycle.Ide.Properties.Resources.importNrmMap;
            this.importNormalMapToolBtn.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.importNormalMapToolBtn.Name = "importNormalMapToolBtn";
            this.importNormalMapToolBtn.Size = new System.Drawing.Size(23, 22);
            this.importNormalMapToolBtn.Text = "GUI:WBImportNormalMap";
            this.importNormalMapToolBtn.Click += new System.EventHandler(this.importNormalMapTool_Click);
            // 
            // importIndexMapToolBtn
            // 
            this.importIndexMapToolBtn.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.importIndexMapToolBtn.Image = global::VirtualBicycle.Ide.Properties.Resources.importIndexMap;
            this.importIndexMapToolBtn.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.importIndexMapToolBtn.Name = "importIndexMapToolBtn";
            this.importIndexMapToolBtn.Size = new System.Drawing.Size(23, 22);
            this.importIndexMapToolBtn.Text = "GUI:WBImportIndexMap";
            this.importIndexMapToolBtn.Click += new System.EventHandler(this.importIndexMapTool_Click);
            // 
            // openFileDialog1
            // 
            this.openFileDialog1.FileName = "openFileDialog1";
            // 
            // toolStrip5
            // 
            this.toolStrip5.Dock = System.Windows.Forms.DockStyle.None;
            this.toolStrip5.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.selectNodeToolBtn,
            this.moveNodeToolBtn,
            this.newNodeToolBtn,
            this.deleteNodeToolBtn,
            this.toolStripSeparator6,
            this.fitRoadToolBtn,
            this.straightenRoadToolBtn,
            this.roadTextureCombo,
            this.toolStripSeparator2,
            this.portSelectToolBtn,
            this.connectPortToolBtn,
            this.disConnectTool});
            this.toolStrip5.Location = new System.Drawing.Point(184, 59);
            this.toolStrip5.Name = "toolStrip5";
            this.toolStrip5.Size = new System.Drawing.Size(385, 25);
            this.toolStrip5.TabIndex = 4;
            this.toolStrip5.Text = "toolStrip5";
            // 
            // selectNodeToolBtn
            // 
            this.selectNodeToolBtn.CheckOnClick = true;
            this.selectNodeToolBtn.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.selectNodeToolBtn.Image = global::VirtualBicycle.Ide.Properties.Resources.SelectRoadNode;
            this.selectNodeToolBtn.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.selectNodeToolBtn.Name = "selectNodeToolBtn";
            this.selectNodeToolBtn.Size = new System.Drawing.Size(23, 22);
            this.selectNodeToolBtn.Text = "GUI:WBSelectRoadNodeTool";
            this.selectNodeToolBtn.CheckedChanged += new System.EventHandler(this.selectNodeToolBtn_CheckedChanged);
            // 
            // moveNodeToolBtn
            // 
            this.moveNodeToolBtn.CheckOnClick = true;
            this.moveNodeToolBtn.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.moveNodeToolBtn.Image = global::VirtualBicycle.Ide.Properties.Resources.moveNodeTool;
            this.moveNodeToolBtn.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.moveNodeToolBtn.Name = "moveNodeToolBtn";
            this.moveNodeToolBtn.Size = new System.Drawing.Size(23, 22);
            this.moveNodeToolBtn.Text = "GUI:WBMoveNodeTool";
            this.moveNodeToolBtn.CheckedChanged += new System.EventHandler(this.moveNodeToolBtn_CheckedChanged);
            // 
            // newNodeToolBtn
            // 
            this.newNodeToolBtn.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.newNodeToolBtn.Image = global::VirtualBicycle.Ide.Properties.Resources.newNode;
            this.newNodeToolBtn.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.newNodeToolBtn.Name = "newNodeToolBtn";
            this.newNodeToolBtn.Size = new System.Drawing.Size(23, 22);
            this.newNodeToolBtn.Text = "toolStripButton1";
            this.newNodeToolBtn.Click += new System.EventHandler(this.newNodeToolBtn_Click);
            // 
            // deleteNodeToolBtn
            // 
            this.deleteNodeToolBtn.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.deleteNodeToolBtn.Image = global::VirtualBicycle.Ide.Properties.Resources.DeleteNodeTool;
            this.deleteNodeToolBtn.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.deleteNodeToolBtn.Name = "deleteNodeToolBtn";
            this.deleteNodeToolBtn.Size = new System.Drawing.Size(23, 22);
            this.deleteNodeToolBtn.Text = "GUI:WBDeleteNodeTool";
            this.deleteNodeToolBtn.Click += new System.EventHandler(this.deleteNodeToolBtn_Click);
            // 
            // toolStripSeparator6
            // 
            this.toolStripSeparator6.Name = "toolStripSeparator6";
            this.toolStripSeparator6.Size = new System.Drawing.Size(6, 25);
            // 
            // fitRoadToolBtn
            // 
            this.fitRoadToolBtn.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.fitRoadToolBtn.Image = global::VirtualBicycle.Ide.Properties.Resources.fitRoad;
            this.fitRoadToolBtn.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.fitRoadToolBtn.Name = "fitRoadToolBtn";
            this.fitRoadToolBtn.Size = new System.Drawing.Size(23, 22);
            this.fitRoadToolBtn.Text = "GUI:WBFitRoadTool";
            this.fitRoadToolBtn.Click += new System.EventHandler(this.fitRoadTool_Click);
            // 
            // straightenRoadToolBtn
            // 
            this.straightenRoadToolBtn.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.straightenRoadToolBtn.Image = global::VirtualBicycle.Ide.Properties.Resources.streightenTool;
            this.straightenRoadToolBtn.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.straightenRoadToolBtn.Name = "straightenRoadToolBtn";
            this.straightenRoadToolBtn.Size = new System.Drawing.Size(23, 22);
            this.straightenRoadToolBtn.Text = "GUI:WBStreightenRoadTool";
            this.straightenRoadToolBtn.Click += new System.EventHandler(this.straightenRoadTool_Click);
            // 
            // roadTextureCombo
            // 
            this.roadTextureCombo.Name = "roadTextureCombo";
            this.roadTextureCombo.Size = new System.Drawing.Size(121, 25);
            this.roadTextureCombo.SelectedIndexChanged += new System.EventHandler(this.roadTextureCombo_SelectedIndexChanged);
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(6, 25);
            // 
            // portSelectToolBtn
            // 
            this.portSelectToolBtn.CheckOnClick = true;
            this.portSelectToolBtn.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.portSelectToolBtn.Image = global::VirtualBicycle.Ide.Properties.Resources.selectPort;
            this.portSelectToolBtn.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.portSelectToolBtn.Name = "portSelectToolBtn";
            this.portSelectToolBtn.Size = new System.Drawing.Size(23, 22);
            this.portSelectToolBtn.Text = "GUI:WBSelectPortTool";
            this.portSelectToolBtn.CheckedChanged += new System.EventHandler(this.portSelectToolBtn_CheckedChanged);
            // 
            // connectPortToolBtn
            // 
            this.connectPortToolBtn.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.connectPortToolBtn.Image = global::VirtualBicycle.Ide.Properties.Resources.ConflictHS;
            this.connectPortToolBtn.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.connectPortToolBtn.Name = "connectPortToolBtn";
            this.connectPortToolBtn.Size = new System.Drawing.Size(23, 22);
            this.connectPortToolBtn.Text = "GUI:WBConnectPortTool";
            this.connectPortToolBtn.Click += new System.EventHandler(this.connectPortToolBtn_Click);
            // 
            // disConnectTool
            // 
            this.disConnectTool.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.disConnectTool.Image = global::VirtualBicycle.Ide.Properties.Resources.ConflictHS1;
            this.disConnectTool.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.disConnectTool.Name = "disConnectTool";
            this.disConnectTool.Size = new System.Drawing.Size(23, 22);
            this.disConnectTool.Text = "GUI:WBDisconnectPortTool";
            this.disConnectTool.Click += new System.EventHandler(this.disConnectTool_Click);
            // 
            // WorldDesigner
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(662, 397);
            this.Controls.Add(this.toolStrip5);
            this.Controls.Add(this.toolStrip4);
            this.Controls.Add(this.toolStrip3);
            this.Controls.Add(this.toolStrip2);
            this.Controls.Add(this.toolStrip1);
            this.Name = "WorldDesigner";
            this.TabText = "WorldDesigner";
            this.Text = "WorldDesigner";
            this.MouseDown += new System.Windows.Forms.MouseEventHandler(this.WorldDesigner_MouseDown);
            this.MouseUp += new System.Windows.Forms.MouseEventHandler(this.WorldDesigner_MouseUp);
            this.MouseMove += new System.Windows.Forms.MouseEventHandler(this.WorldDesigner_MouseMove);
            this.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.WorldDesigner_MouseDoubleClick);
            this.MouseClick += new System.Windows.Forms.MouseEventHandler(this.WorldDesigner_MouseClick);
            this.toolStrip1.ResumeLayout(false);
            this.toolStrip1.PerformLayout();
            this.toolStrip2.ResumeLayout(false);
            this.toolStrip2.PerformLayout();
            this.toolStrip3.ResumeLayout(false);
            this.toolStrip3.PerformLayout();
            this.toolStrip4.ResumeLayout(false);
            this.toolStrip4.PerformLayout();
            this.toolStrip5.ResumeLayout(false);
            this.toolStrip5.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ToolStrip toolStrip1;
        private System.Windows.Forms.ToolStripButton moveForwardBtn;
        private System.Windows.Forms.ToolStripButton moveBackwardBtn;
        private System.Windows.Forms.ToolStripButton moveLeftBtn;
        private System.Windows.Forms.ToolStripButton moveRightBtn;
        private System.Windows.Forms.ToolStripButton turnLeftBtn;
        private System.Windows.Forms.ToolStripButton turnRightBtn;
        private System.Windows.Forms.ToolStrip toolStrip2;
        private System.Windows.Forms.ToolStripButton selectToolBtn;
        private System.Windows.Forms.ToolStripButton higherTerrainToolBtn;
        private System.Windows.Forms.ToolStripButton lowerTerrainToolBtn;
        private System.Windows.Forms.ToolStripButton paintMaterialBtn;
        private System.Windows.Forms.ToolStripComboBox materialCombo;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripLabel toolStripLabel1;
        private System.Windows.Forms.ToolStripComboBox brushSizeCombo;
        private System.Windows.Forms.ToolStrip toolStrip3;
        private System.Windows.Forms.ToolStripButton scenePropBtn;
        private System.Windows.Forms.ToolStripButton smoothenToolBtn;
        private System.Windows.Forms.ToolStrip toolStrip4;
        private System.Windows.Forms.ToolStripButton importDispMapToolBtn;
        private System.Windows.Forms.ToolStripButton importColorMapToolBtn;
        private System.Windows.Forms.ToolStripButton importNormalMapToolBtn;
        private System.Windows.Forms.ToolStripButton importIndexMapToolBtn;
        private System.Windows.Forms.OpenFileDialog openFileDialog1;
        private System.Windows.Forms.ToolStripButton changeMaterialsBtn;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator3;
        private System.Windows.Forms.ToolStripButton rotateToolBtn;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator4;
        private System.Windows.Forms.ToolStripDropDownButton fillModeTool;
        private System.Windows.Forms.ToolStripMenuItem wireframeFMItem;
        private System.Windows.Forms.ToolStripMenuItem solidFMItem;
        private System.Windows.Forms.ToolStrip toolStrip5;
        private System.Windows.Forms.ToolStripButton fitRoadToolBtn;
        private System.Windows.Forms.ToolStripButton straightenRoadToolBtn;
        private System.Windows.Forms.ToolStripButton connectPortToolBtn;
        private System.Windows.Forms.ToolStripButton moveToolBtn;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator5;
        private System.Windows.Forms.ToolStripButton deleteToolBtn;
        private System.Windows.Forms.ToolStripButton selectNodeToolBtn;
        private System.Windows.Forms.ToolStripButton moveNodeToolBtn;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator6;
        private System.Windows.Forms.ToolStripButton deleteNodeToolBtn;
        private System.Windows.Forms.ToolStripButton disConnectTool;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.ToolStripButton portSelectToolBtn;
        private System.Windows.Forms.ToolStripButton newNodeToolBtn;
        private System.Windows.Forms.ToolStripComboBox roadTextureCombo;
    }
}