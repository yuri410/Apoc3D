using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
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
    public partial class ModelDesigner : GeneralDocumentBase
    {
        public const string Extension = ".mesh";

        View3D view3D;

        Point lastPos;
        FillMode fillMode = FillMode.Solid;

        EditableModel content;

        void propChangeSaveState()
        {
            Saved = false;
        }

        public ModelDesigner(DesignerAbstractFactory fac, ResourceLocation rl)
        {
            InitializeComponent();

            LanguageParser.ParseLanguage(DevStringTable.Instance, this);
            LanguageParser.ParseLanguage(DevStringTable.Instance, toolStrip1);

            Init(fac, rl);

            view3D = new View3D(
              ModelDesignerConfigs.Instance.EyeAngleX,
              ModelDesignerConfigs.Instance.EyeAngleY,
              ModelDesignerConfigs.Instance.EyeDistance,
              ModelDesignerConfigs.Instance.Fovy);
            view3D.ViewChanged += ViewChanged;

            this.MouseWheel += viewMouseWheel;
            Saved = true;

            content = new EditableModel();
            //content.StateChanged += propChangeSaveState;
        }


        public override ToolStrip[] ToolStrips
        {
            get
            {
                return new ToolStrip[] { toolStrip1, view3D.ToolBar };
            }
        }


        public override bool LoadRes()
        {
            if (ResourceLocation != null)
            {
                content = EditableModel.FromFile(ResourceLocation);

                Saved = true;
                ViewChanged();
            }
            return true;
        }

        public override bool SaveRes()
        {
            if (ResourceLocation.IsReadOnly)
                throw new InvalidOperationException();

            EditableModel.ToFile(content, ResourceLocation);

            Saved = true;
            return true;
        }

        public override bool Saved
        {
            get
            {
                return false;
            }
            protected set
            {
                base.Saved = value;
                ViewChanged();
            }
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

        private void Draw3D()
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

            DevUtils.DrawLight(dev, light.Position);

            dev.SetTransform(TransformState.World, Matrix.Identity);


            if (content != null)
            {
                content.Render();
            }

            Sprite spr = GraphicsDevice.Instance.GetSprite;

            spr.Begin(SpriteFlags.AlphaBlend | SpriteFlags.DoNotSaveState);

            //Rectangle rect = GraphicsDevice.Instance.DefaultFont.MeasureString(spr, " ", DrawTextFormat.Left | DrawTextFormat.Top | DrawTextFormat.SingleLine);
            if (content != null && content.Entities != null)
            {
                GraphicsDevice.Instance.DefaultFont.DrawString(spr, DevStringTable.Instance["GUI:ModelEntCount"] + content.Entities.Length.ToString(), 5, 5, Color.White);
            }
            else
            {
                GraphicsDevice.Instance.DefaultFont.DrawString(spr, DevStringTable.Instance["GUI:ModelEntCount"] + 0.ToString(), 5, 5, Color.White);
            }
            spr.End();

            dev.EndScene();

            GraphicsDevice.Instance.EndScene();
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            Draw3D();
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

        private void ModelDocument_MouseMove(object sender, MouseEventArgs e)
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

        private void ModelDocument_MouseDown(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right)
                lastPos = e.Location;
        }

        void viewMouseWheel(object sender, MouseEventArgs e)
        {
            view3D.EyeDistance -= e.Delta * 0.02f;
        }

        private void ModelDocument_FormClosing(object sender, FormClosingEventArgs e)
        {
            content.Dispose();

            ModelDesignerConfigs.Instance.EyeAngleX = view3D.EyeAngleX;
            ModelDesignerConfigs.Instance.EyeAngleY = view3D.EyeAngleY;
            ModelDesignerConfigs.Instance.EyeDistance = view3D.EyeDistance;
            ModelDesignerConfigs.Instance.Fovy = view3D.Fovy;
            GraphicsDevice.Instance.Free(this);
        }

        protected override void active()
        {
            if (propertyUpdated != null)
            {
                propertyUpdated(content, null);
            }
        }
        protected override void deactive()
        {
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

                if (content != null && content.Entities != null && content.Entities.Length > 0)
                {
                    EditableMesh[] newEnts = new EditableMesh[content.Entities.Length + value.Entities.Length];

                    Array.Copy(content.Entities, newEnts, content.Entities.Length);
                    Array.Copy(value.Entities, 0, newEnts, content.Entities.Length, value.Entities.Length);

                    content.Entities = newEnts;
                }
                else
                {
                    if (content != null)
                    {
                        content.Dispose();
                    }
                    content = value;
                }

                MeshChanged();
            }
        }

    }
}
