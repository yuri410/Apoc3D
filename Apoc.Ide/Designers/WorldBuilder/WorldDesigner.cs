using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Text;
using System.Windows.Forms;
using SlimDX;
using SlimDX.Direct3D9;
using VirtualBicycle.Ide.Editors.EditableObjects;
using VirtualBicycle.Ide.Tools;
using VirtualBicycle;
using VirtualBicycle.CollisionModel.Broadphase;
using VirtualBicycle.CollisionModel.Dispatch;
using VirtualBicycle.Graphics;
using VirtualBicycle.IO;
using VirtualBicycle.Logic;
using VirtualBicycle.Logic.Traffic;
using VirtualBicycle.MathLib;
using VirtualBicycle.Physics.Dynamics;
using VirtualBicycle.Scene;
using VirtualBicycle.UI;

namespace VirtualBicycle.Ide.Designers.WorldBuilder
{
    public partial class WorldDesigner : GeneralDocumentBase
    {
        static Texture brushMap;

        static void InitializeBrushMap()
        {
            string path = Path.Combine(VirtualBicycle.IO.Paths.Textures, "brushTex.png");
            FileLocation fl = FileSystem.Instance.Locate(path, FileLocateRules.Default);
            brushMap = TextureLoader.LoadUITexture(GraphicsDevice.Instance.Device, fl);
        }

        public static Texture BrushMap
        {
            get
            {
                if (brushMap == null)
                {
                    InitializeBrushMap();
                }
                return brushMap;
            }
        }

        public const string Extension = ".vmp";

        enum ToolType
        {
            ToolStrip,
            ToolBox
        }

        ViewCamera camera;

        FillMode fillMode = FillMode.Solid;

        #region 工具栏

        SelectTool selectTool;
        RotateTool rotateTool;
        MoveTool moveTool;

        SelectRoadNodeTool selectSplineNodeTool;
        MoveSplineNodeTool moveRoadNodeTool;
        SelectPortTool selectPortTool;
        TerrainShapeTool terrainShapeTool;
        TerrainMaterialTool terrainMaterialTool;
        #endregion


        Point lastPos;
        bool isMovingCamera;

        InGameObjectManager objectTypes;


        public TrafficNet Traffic
        {
            get;
            private set;
        }

        #region 工具箱

        ToolBoxItem[] tbxItems;
        ToolBoxCategory[] tbxCates;
        #endregion

        #region 当前工具
        WBToolBoxItem curToolBoxItem;
        public WBToolBoxItem CurrentToolBoxItem
        {
            get { return curToolBoxItem; }
            set
            {
                curToolBoxItem = value;

                if (value != null)
                {
                    this.CurrentToolType = ToolType.ToolBox;
                }
            }
        }

        WBTool curToolStripItem;
        WBTool CurrentToolStripItem
        {
            get { return curToolStripItem; }
            set
            {
                if (value != curToolStripItem)
                {
                    if (value != null)
                    {
                        value.Activate();
                    }
                    if (curToolStripItem != null)
                    {
                        curToolStripItem.Deactivate();
                    }
                }

                curToolStripItem = value;

                if (value != null)
                {
                    this.CurrentToolType = ToolType.ToolStrip;
                }
            }
        }

        ToolType CurrentToolType
        {
            get;
            set;
        }
        #endregion

        #region 场景
        EditableGameScene scene;
        public EditableGameScene Scene
        {
            get { return scene; }
        }

        EditableCluster cluster;
        public EditableCluster CurrentCluster
        {
            get { return cluster; }
        }

        EditableTerrain currentTerrain;
        public EditableTerrain CurrentTerrain
        {
            get { return currentTerrain; }
        }

        #endregion

        void UpdateMaterialCombo(string[] detailMaps)
        {
            materialCombo.Items.Clear();
            for (int i = 0; i < detailMaps.Length; i++)
            {
                materialCombo.Items.Add(detailMaps[i]);
            }
            if (materialCombo.Items.Count > 0)
            {
                materialCombo.SelectedIndex = 0;
            }
        }

        public void SetCluster(EditableCluster cluster)
        {
            if (cluster != this.cluster)
            {
                this.cluster = cluster;

                //ClusterDescription desc = cluster.Description;
                this.scene.ClusterMarkerTransform = Matrix.Translation(cluster.WorldX, cluster.WorldY, cluster.WorldZ);

                if (this.currentTerrain != null && this.currentTerrain != cluster.Terrain)
                {
                    this.currentTerrain.Unselect();
                }

                this.currentTerrain = cluster.Terrain;
                this.currentTerrain.Select();
                string[] maps = new string[4];

                for (int i = 0; i < 4; i++)
                {
                    maps[i] = currentTerrain.GetDetailMapName(i);
                }

                UpdateMaterialCombo(maps);
            }
        }

        #region 初始化
        InGameObjectManager CreateObjectManager()
        {
            Device device = GraphicsDevice.Instance.Device;

            string path = Path.Combine(VirtualBicycle.IO.Paths.Configs, "objects.ini");
            FileLocation fl = FileSystem.Instance.Locate(path, FileLocateRules.Default);


            InGameObjectManager objectMgr = new InGameObjectManager(fl);

            objectMgr.RegisterObjectType(new BuildingFactory(objectMgr));
            objectMgr.RegisterObjectType(new EditableTerrainFactory(device, objectMgr));
            objectMgr.RegisterObjectType(new TerrainObjectFactory(objectMgr));
            objectMgr.RegisterObjectType(new RoadFactory(device, objectMgr, Traffic));
            objectMgr.RegisterObjectType(new JunctionFactory(objectMgr, Traffic));

            List<VirtualBicycle.Logic.Mod.IPlugin> plugs = IdeLogicModManager.Instance.Plugins;

            for (int i = 0; i < plugs.Count; i++)
            {
                InGameObjectFactory[] facs = plugs[i].GetObjectTypes(GraphicsDevice.Instance.Device, objectMgr);
                for (int j = 0; j < facs.Length; j++)
                {
                    objectMgr.RegisterObjectType(facs[j]);
                }
            }


            objectMgr.LoadGraphics(device, null);

            return objectMgr;
        }

        void LoadToolStrip()
        {
            selectTool = new SelectTool(this, scene);
            rotateTool = new RotateTool(this, scene);
            moveTool = new MoveTool(this, scene);

            
            terrainShapeTool = new TerrainShapeTool(this, scene);
            terrainMaterialTool = new TerrainMaterialTool(this, scene);
            selectSplineNodeTool = new SelectRoadNodeTool(this, scene);
            moveRoadNodeTool = new MoveSplineNodeTool(this, scene);
            selectPortTool = new SelectPortTool(this, scene);

            selectToolBtn.Checked = true;

            string[] names = TextureLibrary.Instance.GetNames();
            for (int i = 0; i < names.Length; i++)
            {
                roadTextureCombo.Items.Add(names[i]);
            }
        }
        void LoadToolBox()
        {
            List<ToolBoxItem> tbxItems = new List<ToolBoxItem>();

            PlaceRoadSpline placeRoadSpline = new PlaceRoadSpline(this);

            tbxItems.Add(placeRoadSpline);

            Dictionary<string, BuildingType>.ValueCollection bldType = objectTypes.BuildingTypes.Values;

            foreach (BuildingType bt in bldType)
            {
                tbxItems.Add(new PlaceBuilding(this, bt));
            }

            Dictionary<string, TerrainObjectType>.ValueCollection treeType = objectTypes.TreeTypes.Values;

            foreach (TerrainObjectType tt in treeType)
            {
                tbxItems.Add(new PlaceTree(this, tt));
            }

            Dictionary<string, TerrainObjectType>.ValueCollection objType = objectTypes.TOTypes.Values;

            foreach (TerrainObjectType tt in objType)
            {
                tbxItems.Add(new PlaceTO(this, tt));
            }

            Dictionary<string, JunctionType>.ValueCollection crsType = objectTypes.CrossingTypes.Values;

            foreach (JunctionType tt in crsType)
            {
                tbxItems.Add(new PlaceCrossing(this, tt));
            }


            tbxItems.Add(new PlaceSmallBox(this));
            tbxItems.Add(new PlaceLogicalArea(this));

            List<ToolBoxCategory> tbxCates = new List<ToolBoxCategory>();
            tbxCates.Add(ToolCategories.Instance.Road);
            tbxCates.Add(ToolCategories.Instance.Building);
            tbxCates.Add(ToolCategories.Instance.Tree);
            tbxCates.Add(ToolCategories.Instance.TO);
            tbxCates.Add(ToolCategories.Instance.Logic);

            this.tbxItems = tbxItems.ToArray();
            this.tbxCates = tbxCates.ToArray();
        }

        public WorldDesigner(DesignerAbstractFactory fac, ResourceLocation rl)
        {
            InitializeComponent();

            Init(fac, rl);

            LanguageParser.ParseLanguage(DevStringTable.Instance, this);
            LanguageParser.ParseLanguage(DevStringTable.Instance, toolStrip1);
            LanguageParser.ParseLanguage(DevStringTable.Instance, toolStrip2);
            LanguageParser.ParseLanguage(DevStringTable.Instance, toolStrip3);
            LanguageParser.ParseLanguage(DevStringTable.Instance, toolStrip4);
            LanguageParser.ParseLanguage(DevStringTable.Instance, toolStrip5);

            camera = new ViewCamera(45f, (float)ClientSize.Width / (float)ClientSize.Height);
            camera.FarPlane = 500;

            Traffic = new TrafficNet();
            objectTypes = CreateObjectManager();


            DevFileLocation sfl = rl as DevFileLocation;

            if (sfl != null)
            {
                EditableSceneData sceData = EditableSceneData.FromFile(sfl, objectTypes);
                scene = new EditableGameScene(this, sceData, camera);

                EditableClusterTable clusterTable = scene.ClusterTable;

                foreach (EditableCluster cl in clusterTable)
                {
                    // 为所有物体加载碰撞形体
                    List<SceneObject> objects = cl.SceneManager.SceneObjects;
                    for (int i = 0; i < objects.Count; i++)
                    {
                        if (objects[i].HasPhysicsModel)
                        {
                            objects[i].BuildPhysicsModel(null);
                        }
                    }
                }
            }
            else
            {
                throw new NotSupportedException();
            }


            Traffic.ParseNodes();

            brushSizeCombo.SelectedIndex = 0;

            LoadToolStrip();


            this.MouseWheel += this.WorldDesigner_MouseWheel;


            ClusterDescription desc = new ClusterDescription(0, 0);
            SetCluster(scene.ClusterTable[desc]);

            LoadToolBox();
        }

        #endregion

        public override ToolStrip[] ToolStrips
        {
            get
            {
                return new ToolStrip[] { toolStrip1, toolStrip2, toolStrip3, toolStrip4, toolStrip5 };
            }
        }

        public ViewCamera Camera
        {
            get { return camera; }
        }

        public void NotifyPropertyChanged(object[] objs)
        {
            base.OnPropertyUpdated(objs);
        }

        public LineSegment GetPickRay(int mouseX, int mouseY)
        {
            Vector3 pNear = new Vector3(mouseX, mouseY, 0);
            Vector3 pFar = new Vector3(mouseX, mouseY, 1);

            Viewport vp = GraphicsDevice.Instance.Device.Viewport;
            Matrix i4 = Matrix.Identity;
            Matrix projection = camera.ProjectionMatrix;
            Matrix view = camera.ViewMatrix;

            Vector3 begin;
            Vector3 end;

            Vector3.Unproject(ref pNear, ref vp, ref projection, ref view, ref i4, out begin);
            Vector3.Unproject(ref pFar, ref vp, ref projection, ref view, ref i4, out end);
            return new LineSegment(begin, end);
        }

        #region 摄像机控制
        private void moveForwardBtn_Click(object sender, EventArgs e)
        {
            camera.MoveForward();
            Draw3D();
        }
        private void moveBackwardBtn_Click(object sender, EventArgs e)
        {
            camera.MoveBack();
            Draw3D();
        }
        private void moveLeftBtn_Click(object sender, EventArgs e)
        {
            camera.MoveLeft();
            Draw3D();
        }
        private void moveRightBtn_Click(object sender, EventArgs e)
        {
            camera.MoveRight();
            Draw3D();
        }
        private void turnLeftBtn_Click(object sender, EventArgs e)
        {
            camera.TurnLeft();
            Draw3D();
        }
        private void turnRightBtn_Click(object sender, EventArgs e)
        {
            camera.TurnRight();
            Draw3D();
        }

        #endregion

        #region 鼠标响应
        private void WorldDesigner_MouseUp(object sender, MouseEventArgs e)
        {
            if (!isMovingCamera)
            {
                if (CurrentToolType == ToolType.ToolBox)
                {
                    if (CurrentToolBoxItem != null)
                    {
                        CurrentToolBoxItem.NotifyMouseUp(e);
                    }
                }
                else if (CurrentToolType == ToolType.ToolStrip)
                {
                    if (CurrentToolStripItem != null)
                    {
                        CurrentToolStripItem.NotifyMouseUp(e);
                    }
                }
            }
            isMovingCamera = false;
        }
        private void WorldDesigner_MouseMove(object sender, MouseEventArgs e)
        {
            bool redraw = false;

            if (!isMovingCamera)
            {
                if (CurrentToolType == ToolType.ToolBox)
                {
                    if (CurrentToolBoxItem != null)
                    {
                        CurrentToolBoxItem.NotifyMouseMove(e);
                    }
                }
                else if (CurrentToolType == ToolType.ToolStrip)
                {
                    if (CurrentToolStripItem != null)
                    {
                        CurrentToolStripItem.NotifyMouseMove(e);
                        if (CurrentToolStripItem.RequiresRedraw)
                        {
                            CurrentToolStripItem.RequiresRedraw = false;
                            redraw = true;
                        }
                    }
                }
            }


            Point loc = e.Location;
            Point offset = new Point();
            offset.X = loc.X - lastPos.X;
            offset.Y = loc.Y - lastPos.Y;

            if ((e.Button & MouseButtons.Middle) == MouseButtons.Middle)
            {
                isMovingCamera = true;

                camera.Move(-offset.X * 0.5f, -offset.Y * 0.5f);

                redraw = true;
                lastPos = loc;
            }
            else if ((e.Button & MouseButtons.Right) == MouseButtons.Right)
            {
                isMovingCamera = true;
                camera.Turn(-offset.X * 0.05f);

                camera.Height -= offset.Y * 0.1f;

                camera.Update(0.025f);

                redraw = true;
                lastPos = loc;
            }


            if (redraw)
            {
                Draw3D();
            }

        }
        private void WorldDesigner_MouseDown(object sender, MouseEventArgs e)
        {
            if (!isMovingCamera)
            {
                if (CurrentToolType == ToolType.ToolBox)
                {
                    if (CurrentToolBoxItem != null)
                    {
                        CurrentToolBoxItem.NotifyMouseDown(e);
                    }
                }
                else if (CurrentToolType == ToolType.ToolStrip)
                {
                    if (CurrentToolStripItem != null)
                    {
                        CurrentToolStripItem.NotifyMouseDown(e);
                    }
                }
            }


            if (e.Button == MouseButtons.Right)
            {
                lastPos = e.Location;
                isMovingCamera = false;
            }
            if (e.Button == MouseButtons.Middle)
            {
                lastPos = e.Location;
                isMovingCamera = false;
            }

        }
        private void WorldDesigner_MouseClick(object sender, MouseEventArgs e)
        {
            if (!isMovingCamera)
            {
                if (CurrentToolType == ToolType.ToolBox)
                {
                    if (CurrentToolBoxItem != null)
                    {
                        CurrentToolBoxItem.NotifyMouseClick(e);
                    }
                }
                else if (CurrentToolType == ToolType.ToolStrip)
                {
                    if (CurrentToolStripItem != null)
                    {
                        CurrentToolStripItem.NotifyMouseClick(e);
                    }
                }
            }
            Draw3D();
        }
        private void WorldDesigner_MouseDoubleClick(object sender, MouseEventArgs e)
        {

            if (!isMovingCamera)
            {
                if (CurrentToolType == ToolType.ToolBox)
                {
                    if (CurrentToolBoxItem != null)
                    {
                        CurrentToolBoxItem.NotifyMouseDoubleClick(e);
                    }
                }
                else if (CurrentToolType == ToolType.ToolStrip)
                {
                    if (CurrentToolStripItem != null)
                    {
                        CurrentToolStripItem.NotifyMouseDoubleClick(e);
                    }
                }
            }
        }
        private void WorldDesigner_MouseWheel(object sender, MouseEventArgs e)
        {
            if (!isMovingCamera)
            {
                if (CurrentToolType == ToolType.ToolBox)
                {
                    if (CurrentToolBoxItem != null)
                    {
                        CurrentToolBoxItem.NotifyMouseWheel(e);
                    }
                }
                else if (CurrentToolType == ToolType.ToolStrip)
                {
                    if (CurrentToolStripItem != null)
                    {
                        CurrentToolStripItem.NotifyMouseWheel(e);
                    }
                }
            }


            isMovingCamera = true;
            camera.HeightOffset -= e.Delta * 0.05f;
            Draw3D();
        }
        #endregion

        #region 工具栏按钮

        void DisableToolButtons(ToolStripButton btn)
        {
            if (selectToolBtn != btn)
                selectToolBtn.Checked = false;

            if (selectNodeToolBtn != btn)
                selectNodeToolBtn.Checked = false;

            if (smoothenToolBtn != btn)
                smoothenToolBtn.Checked = false;

            if (higherTerrainToolBtn != btn)
                higherTerrainToolBtn.Checked = false;

            if (lowerTerrainToolBtn != btn)
                lowerTerrainToolBtn.Checked = false;

            if (paintMaterialBtn != btn)
                paintMaterialBtn.Checked = false;

            if (moveToolBtn != btn)
                moveToolBtn.Checked = false;

            if (rotateToolBtn != btn)
                rotateToolBtn.Checked = false;

            if (portSelectToolBtn != btn)
                portSelectToolBtn.Checked = false;

            if (moveNodeToolBtn != btn)
                moveNodeToolBtn.Checked = false;

            CurrentToolStripItem = null;
        }

        #region 基本编辑
        private void selectToolBtn_CheckedChanged(object sender, EventArgs e)
        {
            if (selectToolBtn.Checked)
            {
                DisableToolButtons(selectToolBtn);

                CurrentToolStripItem = selectTool;
            }
        }

        private void rotateToolBtn_CheckedChanged(object sender, EventArgs e)
        {
            if (rotateToolBtn.Checked)
            {
                DisableToolButtons(rotateToolBtn);

                CurrentToolStripItem = rotateTool;
            }
        }

        private void moveToolBtn_CheckedChanged(object sender, EventArgs e)
        {
            if (moveToolBtn.Checked)
            {
                DisableToolButtons(moveToolBtn);

                CurrentToolStripItem = moveTool;
            }
        }
        private void deleteToolBtn_Click(object sender, EventArgs e)
        {
            ISelectableTool tool = CurrentToolStripItem as ISelectableTool;
            if (tool != null)
            {
                SceneObject obj = tool.SelectedObject;

                if (obj != null)
                {
                    if (obj is Terrain)
                        return;

                    if (obj is RoadSegment)
                    {
                        Road track = ((RoadSegment)obj).Track;
                        Scene.NotifyTrackRemoved(track);
                        Scene.NotifyTrafficConRemoved(track);

                        track.ParentCluster.SceneManager.RemoveObjectFromScene(track);

                        track.Dispose();
                    }
                    else
                    {
                        if (obj is Junction)
                        {
                            Scene.NotifyTrafficConRemoved((ITrafficComponment)obj);
                        }

                        obj.ParentCluster.SceneManager.RemoveObjectFromScene(obj);
                        obj.Dispose();
                    }

                    tool.SelectedObject = null;

                    Draw3D();
                }
            }
        }
        #endregion

        #region 地形编辑

        private void higherTerrainToolBtn_CheckedChanged(object sender, EventArgs e)
        {
            if (higherTerrainToolBtn.Checked)
            {
                terrainShapeTool.Mode = ModifyShapeMode.Higher;
                DisableToolButtons(higherTerrainToolBtn);
                CurrentToolStripItem = terrainShapeTool;
            }
        }

        private void lowerTerrainToolBtn_CheckedChanged(object sender, EventArgs e)
        {
            if (lowerTerrainToolBtn.Checked)
            {
                terrainShapeTool.Mode = ModifyShapeMode.Lower;
                DisableToolButtons(lowerTerrainToolBtn);
                CurrentToolStripItem = terrainShapeTool;
            }
        }

        private void paintMaterialBtn_CheckedChanged(object sender, EventArgs e)
        {
            if (paintMaterialBtn.Checked)
            {

                DisableToolButtons(paintMaterialBtn);
                CurrentToolStripItem = terrainMaterialTool;
            }
        }

        private void smoothenToolBtn_CheckedChanged(object sender, EventArgs e)
        {
            if (smoothenToolBtn.Checked)
            {
                terrainShapeTool.Mode = ModifyShapeMode.Smoothen;
                DisableToolButtons(smoothenToolBtn);
                CurrentToolStripItem = terrainShapeTool;
            }
        }


        private void materialCombo_SelectedIndexChanged(object sender, EventArgs e)
        {
            int index = materialCombo.SelectedIndex;

            //if (index == 4)
            //{
            //    index = 0;
            //}

            terrainMaterialTool.SelectedMaterialIndex = 3 - index;
        }

        private void brushSizeCombo_SelectedIndexChanged(object sender, EventArgs e)
        {
            int size = int.Parse(brushSizeCombo.SelectedItem.ToString());
            if (terrainShapeTool != null)
            {
                terrainShapeTool.BrushSize = size;
            }
            if (terrainMaterialTool != null)
            {
                terrainMaterialTool.BrushSize = size;
            }
        }

        private void changeMaterialsBtn_Click(object sender, EventArgs e)
        {
            string[] maps = new string[4];

            for (int i = 0; i < 4; i++)
            {
                maps[i] = currentTerrain.GetDetailMapName(i);
            }

            if (SelectDetailMapForm.Show(maps) == DialogResult.OK)
            {
                UpdateMaterialCombo(SelectDetailMapForm.SelectedDetailMaps);

                currentTerrain.SetDetailMaps(SelectDetailMapForm.SelectedDetailMaps);
            }
            Draw3D();
        }

        #endregion

        private void scenePropBtn_Click(object sender, EventArgs e)
        {
            SceneProperty prop = new SceneProperty(scene, scene.Data);

            OnPropertyUpdated(new object[] { prop });
        }

        private void wireframeFMItem_Click(object sender, EventArgs e)
        {
            fillMode = FillMode.Wireframe;
        }
        private void solidFMItem_Click(object sender, EventArgs e)
        {
            fillMode = FillMode.Solid;
        }

        
        #region 道路编辑

        private void roadTextureCombo_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (CurrentToolStripItem == selectSplineNodeTool)
            {
                selectSplineNodeTool.SetRoadTexture((string)roadTextureCombo.SelectedItem);
                Draw3D();
            }
        }
        private void moveNodeToolBtn_CheckedChanged(object sender, EventArgs e)
        {
            if (moveNodeToolBtn.Checked)
            {
                DisableToolButtons(moveNodeToolBtn);
                CurrentToolStripItem = moveRoadNodeTool;
            }
        }
        private void straightenRoadTool_Click(object sender, EventArgs e)
        {
            if (CurrentToolStripItem == selectSplineNodeTool)
            {
                selectSplineNodeTool.Streighten();
                Draw3D();
            }
        }

        private unsafe void fitRoadTool_Click(object sender, EventArgs e)
        {
            if (CurrentToolStripItem == selectSplineNodeTool)
            {
                selectSplineNodeTool.TerrainFit();
                Draw3D();
            }
        }
        private void selectNodeToolBtn_CheckedChanged(object sender, EventArgs e)
        {
            if (selectNodeToolBtn.Checked)
            {
                DisableToolButtons(selectNodeToolBtn);
                CurrentToolStripItem = selectSplineNodeTool;
            }
        }
        private void newNodeToolBtn_Click(object sender, EventArgs e)
        {
            if (CurrentToolStripItem == selectSplineNodeTool)
            {
                selectSplineNodeTool.InsertNode();
                Draw3D();
            }
        }

        private void deleteNodeToolBtn_Click(object sender, EventArgs e)
        {
            if (CurrentToolStripItem == selectSplineNodeTool)
            {
                selectSplineNodeTool.DeleteNodes();
                Draw3D();
            }
        }
        private void portSelectToolBtn_CheckedChanged(object sender, EventArgs e)
        {
            if (portSelectToolBtn.Checked)
            {
                DisableToolButtons(portSelectToolBtn);
                CurrentToolStripItem = selectPortTool;
            }
        }
        private void connectPortToolBtn_Click(object sender, EventArgs e)
        {
            if (CurrentToolStripItem == selectPortTool)
            {
                selectPortTool.Connect();
                Draw3D();
            }
        }
        private void disConnectTool_Click(object sender, EventArgs e)
        {

        }

        #endregion

        #endregion

        protected override void active()
        {
            base.active();
            OnTBItemsChanged(tbxItems, tbxCates);
        }
        protected override void deactive()
        {
            base.deactive();
            OnTBItemsChanged(null, null);
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            Draw3D();
        }

        private void Draw3D()
        {
            if (scene != null)
            {
                scene.Update(0.025f);

                GraphicsDevice.Instance.BeginScene(this);

                Device dev = GraphicsDevice.Instance.Device;
                dev.Clear(ClearFlags.Target | ClearFlags.ZBuffer, Color.DarkBlue.ToArgb(), 1f, 0);

                dev.BeginScene();

                camera.Update(0.025f);

                camera.RenderTarget = dev.GetRenderTarget(0);

                dev.SetRenderState<FillMode>(RenderState.FillMode, fillMode);




                scene.RenderScene();

                dev.VertexShader = null;
                dev.PixelShader = null;
                dev.SetRenderState(RenderState.Lighting, true);
                dev.SetRenderState(RenderState.NormalizeNormals, true);
                dev.SetRenderState(RenderState.AlphaBlendEnable, false);
                dev.SetRenderState(RenderState.AlphaTestEnable, false);
                dev.SetRenderState<Cull>(RenderState.CullMode, Cull.None);
                dev.SetTexture(0, null);

                dev.SetTextureStageState(0, TextureStage.ColorOperation, TextureOperation.SelectArg1);
                dev.SetTextureStageState(0, TextureStage.ColorArg1, TextureArgument.Diffuse);

                dev.SetTransform(TransformState.Projection, camera.ProjectionMatrix);
                dev.SetTransform(TransformState.View, camera.ViewMatrix);
                dev.SetTransform(TransformState.World, Matrix.Identity);

                dev.VertexDeclaration = null;

                dev.EnableLight(0, true);

                Light light = new Light();
                light.Ambient = new Color4(0.4f, 0.4f, 0.4f);
                light.Diffuse = new Color4(0.8f, 0.8f, 0.8f);
                light.Specular = new Color4(0, 0, 0, 0);
                light.Type = LightType.Directional;

                Vector3 lightDir = new Vector3(-1, -1, -1);
                lightDir.Normalize();
                light.Direction = lightDir;

                dev.SetLight(0, light);

                if (CurrentToolType == ToolType.ToolBox) 
                {
                    if (CurrentToolBoxItem != null)
                    {
                        CurrentToolBoxItem.Render();
                    }
                }
                else if (CurrentToolType == ToolType.ToolStrip)
                {
                    if (CurrentToolStripItem != null)
                    {
                        CurrentToolStripItem.Render();
                    }
                }

                dev.SetRenderState(RenderState.NormalizeNormals, false);
                dev.SetRenderState(RenderState.Lighting, false);

                Sprite spr = GraphicsDevice.Instance.GetSprite;

                spr.Begin(SpriteFlags.AlphaBlend | SpriteFlags.DoNotSaveState);

                GraphicsDevice.Instance.DrawString(string.Format(DevStringTable.Instance["GUI:BatchCount"], scene.BatchCount.ToString()), 5, 5, true, Color.White);

                GraphicsDevice.Instance.DrawString(string.Format(DevStringTable.Instance["GUI:ClusterCount"], scene.ClusterCount.ToString()), 5, 25, true, Color.White);

                spr.End();

                dev.EndScene();

                GraphicsDevice.Instance.EndScene();
            }
        }

        public override bool LoadRes()
        {
            if (ResourceLocation != null)
            {
                return true;
            }
            return false;
        }

        public override bool SaveRes()
        {
            if (ResourceLocation.IsReadOnly)
                throw new InvalidOperationException();

            scene.Data.Rebuild(scene);
            scene.Data.Save(ResourceLocation.GetStream);

            return true;
        }

        #region 贴图导入
        private void importDispMapTool_Click(object sender, EventArgs e)
        {
            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                Device device = GraphicsDevice.Instance.Device;
                DevFileLocation fl = new DevFileLocation(openFileDialog1.FileName);

                TerrainTexture ttex = TerrainTextureManager.Instance.CreateInstance(device, fl, true);

                currentTerrain.SetDisplacementMap(ttex);
            }
        }

        private void importColorMapTool_Click(object sender, EventArgs e)
        {
            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                Device device = GraphicsDevice.Instance.Device;
                DevFileLocation fl = new DevFileLocation(openFileDialog1.FileName);

                TerrainTexture ttex = TerrainTextureManager.Instance.CreateInstance(device, fl, false);

                currentTerrain.SetColorMap(ttex);
            }
        }

        private void importNormalMapTool_Click(object sender, EventArgs e)
        {
            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                Device device = GraphicsDevice.Instance.Device;
                DevFileLocation fl = new DevFileLocation(openFileDialog1.FileName);

                TerrainTexture ttex = TerrainTextureManager.Instance.CreateInstance(device, fl, false);

                currentTerrain.SetNormalMap(ttex);
            }
        }

        private void importIndexMapTool_Click(object sender, EventArgs e)
        {
            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                Device device = GraphicsDevice.Instance.Device;
                DevFileLocation fl = new DevFileLocation(openFileDialog1.FileName);

                TerrainTexture ttex = TerrainTextureManager.Instance.CreateInstance(device, fl, false);

                currentTerrain.SetIndexMap(ttex);
            }
        }
        #endregion
    }
}
