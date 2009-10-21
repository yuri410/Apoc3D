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
using VirtualBicycle.Ide.Converters;
using VirtualBicycle.Ide.Editors.EditableObjects;
using VirtualBicycle.IO;
using VirtualBicycle.MathLib;
using VirtualBicycle.UI;
using WeifenLuo.WinFormsUI.Docking;

namespace VirtualBicycle.Ide.Designers
{
    public partial class EmbeddedMeshDocument : DocumentBase
    {
        EditableMesh content;

        //Light light;

        View3D view3D;

        Point lastPos;
        FillMode fillMode = FillMode.Solid;

        public EmbeddedMeshDocument(EditableMesh data)
        {
            InitializeComponent();


            LanguageParser.ParseLanguage(DevStringTable.Instance, this);
            LanguageParser.ParseLanguage(DevStringTable.Instance, toolStrip1);
            LanguageParser.ParseLanguage(DevStringTable.Instance, toolStrip2);

            content = data;
            view3D = new View3D(
                ModelDesignerConfigs.Instance.EyeAngleX,
                ModelDesignerConfigs.Instance.EyeAngleY,
                ModelDesignerConfigs.Instance.EyeDistance,
                ModelDesignerConfigs.Instance.Fovy);
            view3D.ViewChanged += ViewChanged;

            this.MouseWheel += viewMouseWheel;

            if (content.Materials != null)
            {
                for (int i = 0; i < content.Materials.Length; i++)
                {
                    for (int j = 0; j < content.Materials[i].Length; j++)
                    {
                        content.Materials[i][j].StateChanged = ViewChanged;
                    }
                }
            }
            MeshChanged();
        }

        public override ToolStrip[] ToolStrips
        {
            get
            {
                return new ToolStrip[] { view3D.ToolBar, toolStrip1, toolStrip2 };
            }
        }

        public override DesignerAbstractFactory Factory
        {
            get { return null; }
        }

        public override bool LoadRes() { return true; }

        public override bool SaveRes() { return true; }

        public override ResourceLocation ResourceLocation
        {
            get { return null; }
            set { }
        }

        public override bool IsReadOnly
        {
            get { return false; }
        }

        public override bool Saved
        {
            get { return true; }
            protected set { }
        }



        protected override void active()
        {
            //GraphicsDevice.Instance.Bind(this);
            //GraphicsDevice.Instance.MouseClick += viewMouseClick;
            //GraphicsDevice.Instance.MouseMove += viewMouseMove;
            //GraphicsDevice.Instance.MouseDown += viewMouseDown;
            //GraphicsDevice.Instance.MouseUp += viewMouseUp;

            if (propertyUpdated != null)
            {
                propertyUpdated(content, null);
            }
        }
        protected override void deactive()
        {
            //GraphicsDevice.Instance.MouseClick -= viewMouseClick;
            //GraphicsDevice.Instance.MouseMove -= viewMouseMove;
            //GraphicsDevice.Instance.MouseDown -= viewMouseDown;
            //GraphicsDevice.Instance.MouseUp -= viewMouseUp;
            //GraphicsDevice.Instance.Unbind(this);
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            Draw3D();
        }
        unsafe void Draw3D()
        {
            GraphicsDevice.Instance.BeginScene(this);

            Device dev = GraphicsDevice.Instance.Device;

            dev.Clear(ClearFlags.Target | ClearFlags.ZBuffer, Color.DarkBlue, 1, 0);
            dev.BeginScene();

            dev.SetRenderState<FillMode>(RenderState.FillMode, fillMode);
            dev.SetRenderState(RenderState.AlphaBlendEnable, true);
            dev.SetRenderState<Cull>(RenderState.CullMode, Cull.None);
            dev.SetRenderState(RenderState.SpecularEnable, true);
            dev.SetRenderState<ColorSource>(RenderState.SpecularMaterialSource, ColorSource.Material);
            dev.SetRenderState(RenderState.Lighting, true);

            Light light = new Light();
            light.Type = LightType.Point;
            light.Position = ModelDesignerConfigs.Instance.LightPosition;
            light.Range = float.MaxValue;
            //light.Falloff = 0;
            light.Ambient = ModelDesignerConfigs.Instance.LightAmbient;
            light.Diffuse = ModelDesignerConfigs.Instance.LightDiffuse;
            light.Specular = ModelDesignerConfigs.Instance.LightSpecular;
            light.Attenuation0 = 1.0f;

            dev.EnableLight(0, true);
            dev.SetTransform(TransformState.World, Matrix.Identity);
            dev.SetLight(0, light);

            view3D.SetTransform(dev, this);

            //dev.SetTransform(TransformState.World, Matrix.Translation(light.Position));
            //dev.SetRenderState(RenderState.Lighting, false);
            //for (int i = 0; i < 4; i++)            
            //{
            //    dev.SetTexture(i, null);
            //}
            //GraphicsDevice.Instance.BallMesh.DrawSubset(0);
            //dev.SetRenderState(RenderState.Lighting, true);
            DevUtils.DrawLight(dev, light.Position);

            dev.SetTransform(TransformState.World, Matrix.Identity);

            //DevUtils.DrawEditMesh(dev, content);
            content.Render();

            Sprite spr = GraphicsDevice.Instance.GetSprite;

            spr.Begin(SpriteFlags.AlphaBlend | SpriteFlags.DoNotSaveState);

            Rectangle rect = GraphicsDevice.Instance.DefaultFont.MeasureString(spr, " ", DrawTextFormat.Left | DrawTextFormat.Top | DrawTextFormat.SingleLine);

            GraphicsDevice.Instance.DefaultFont.DrawString(spr, DevStringTable.Instance["GUI:MeshVtxCount"] + content.VertexCount.ToString(), 5, 5, Color.White);

            if (content.Faces != null)
            {
                GraphicsDevice.Instance.DefaultFont.DrawString(spr, DevStringTable.Instance["GUI:MeshFaceCount"] + content.Faces.Length.ToString(), 5, 10 + rect.Height, Color.White);
            }
            else
            {
                GraphicsDevice.Instance.DefaultFont.DrawString(spr, DevStringTable.Instance["GUI:MeshFaceCount"] + 0.ToString(), 5, 5, Color.White);
            }
            if (content.Materials != null)
            {
                GraphicsDevice.Instance.DefaultFont.DrawString(spr, DevStringTable.Instance["GUI:MaterialCount"] + content.Materials.Length.ToString(), 5, 15 + 2 * rect.Height, Color.White);
            }
            else
            {
                GraphicsDevice.Instance.DefaultFont.DrawString(spr, DevStringTable.Instance["GUI:MaterialCount"] + 0.ToString(), 5, 5, Color.White);
            }
            spr.End();

            dev.EndScene();

            GraphicsDevice.Instance.EndScene();


        }

        void viewMouseWheel(object sender, MouseEventArgs e)
        {
            view3D.EyeDistance -= e.Delta * 0.02f;
        }

        void ViewChanged()
        {
            Draw3D();
        }
        void MeshChanged()        
        {
            if (propertyUpdated != null)
            {
                propertyUpdated(content, null);
            }
            ViewChanged();
        }
        private void importTool_Click(object sender, EventArgs e)
        {

            ConverterBase[] convs = ConverterManager.Instance.GetConvertersDest(".mesh");
            string[] subFilters = new string[convs.Length + 1];
            for (int i = 0; i < convs.Length; i++)
            {
                subFilters[i] = DevUtils.GetFilter(convs[i].SourceDesc, convs[i].SourceExt);
            }
            subFilters[convs.Length] = DevUtils.GetFilter(DevStringTable.Instance["DOCS:MeshDesc"], new string[] { ".mesh" });

            openFileDialog1.Filter = DevUtils.GetFilter(subFilters);

            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                EditableModel value;
                if (openFileDialog1.FilterIndex == convs.Length)
                {
                    value = EditableModel.FromFile(new DevFileLocation(openFileDialog1.FileName));
                }
                else
                {
                    ConverterBase con = convs[openFileDialog1.FilterIndex];

                    System.IO.MemoryStream ms = new System.IO.MemoryStream(65536 * 4);
                    con.Convert(new DevFileLocation(openFileDialog1.FileName), new StreamedLocation(new VirtualStream(ms, 0)));
                    ms.Position = 0;

                    value = EditableModel.FromFile(new StreamedLocation(ms));
                }
                content = value.Entities[0];
                MeshChanged();
            }
        }

        private void EmbededMeshDocument_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (content.Materials != null)
            {
                for (int i = 0; i < content.Materials.Length; i++)
                {
                    for (int j = 0; j < content.Materials[i].Length; j++)
                    {
                        content.Materials[i][j].StateChanged = null;
                    }
                }
            }

            ModelDesignerConfigs.Instance.EyeAngleX = view3D.EyeAngleX;
            ModelDesignerConfigs.Instance.EyeAngleY = view3D.EyeAngleY;
            ModelDesignerConfigs.Instance.EyeDistance = view3D.EyeDistance;
            ModelDesignerConfigs.Instance.Fovy = view3D.Fovy;
            GraphicsDevice.Instance.Free(this);
            //ModelDocumentConfigs.Instance.LightAmbient = light.Ambient;
            //ModelDocumentConfigs.Instance.LightDiffuse = light.Diffuse;
            //ModelDocumentConfigs.Instance.LightPosition = 
        }

        private void optimizeTool_Click(object sender, EventArgs e)
        {
            content.Optmize(MeshOptimizeFlags.DeviceIndependent | MeshOptimizeFlags.VertexCache | MeshOptimizeFlags.Compact);
            MeshChanged();
        }

        private void simplifyTool_Click(object sender, EventArgs e)
        {
            MeshSimpDlg prev = new MeshSimpDlg(content);

            prev.ShowDialog();

            if (content != prev.SelectedMesh)
            {
                content.Dispose(false);
                //content = prev.SelectedMesh;

                prev.SelectedMesh.CloneTo(content);

                prev.SelectedMesh.Dispose(false);
            }

            prev.Dispose();
        }


        private void EmbededMeshDocument_MouseClick(object sender, MouseEventArgs e)
        {

        }

        private void EmbededMeshDocument_MouseDown(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right)
                lastPos = e.Location;


        }

        private void EmbededMeshDocument_MouseMove(object sender, MouseEventArgs e)
        {
            Point loc = e.Location;
            Point offset = new Point();
            offset.X = loc.X - lastPos.X;
            offset.Y = loc.Y - lastPos.Y;

            if (e.Button == MouseButtons.Right)
            {
                view3D.EyeAngleX += MathEx.PIf * (float)offset.X / 180f;
                view3D.EyeAngleY += MathEx.PIf * (float)offset.Y / 180f;

                lastPos = loc;
            }
        }

        private void EmbededMeshDocument_MouseUp(object sender, MouseEventArgs e)
        {

        }

        private void wireframeMenuItem_Click(object sender, EventArgs e)
        {
            fillMode = FillMode.Wireframe;
            fillModeTool.Text = wireframeMenuItem.Text;
            ViewChanged();
        }

        private void solidMenuItem_Click(object sender, EventArgs e)
        {
            fillMode = FillMode.Solid;
            fillModeTool.Text = solidMenuItem.Text;
            ViewChanged();
        }

        #region 法线调整
        private void normalTool_Click(object sender, EventArgs e)
        {
            content.ComputeNormals();
            MeshChanged();
        }

        private void flatNormalTool_ButtonClick(object sender, EventArgs e)
        {
            content.ComputeFlatNormal();
            MeshChanged();
        }

        private void invNormalTool_Click(object sender, EventArgs e)
        {
            content.InverseNormals();
        }

        private void specialNormalTool_Click(object sender, EventArgs e)
        {
            content.InverseNormalX();
        }

        private void invNormalYTool_Click(object sender, EventArgs e)
        {
            content.InverseNormalY();
        }

        private void invNormalZTool_Click(object sender, EventArgs e)
        {
            content.InverseNormalZ();
        }
        #endregion

        private void exportTool_Click(object sender, EventArgs e)
        {
            if (saveFileDialog1.ShowDialog() == DialogResult.OK)
            {
                content.ExportAsObj(saveFileDialog1.FileName);
            }
        }

        private void subDevisionTool_Click(object sender, EventArgs e)
        {
            InputNumberDlg dlg = new InputNumberDlg();

            dlg.Value = 1;

            if (dlg.ShowDialog() == DialogResult.OK)
            {
                content.Subdevide((float)dlg.Value);
            }
        }

        private void weldingTool_Click(object sender, EventArgs e)
        {
            VertexElementDlg dlg = new VertexElementDlg(content.VertexElements);

            if (dlg.ShowDialog() == DialogResult.OK)
            {
                content.WeldVertices(dlg.Elements);
            }
            dlg.Dispose();

        }


    }
}

