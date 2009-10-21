using System;
using System.Collections.Generic;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using SlimDX;
using SlimDX.Direct3D9;
using VirtualBicycle.Ide.Editors.EditableObjects;
using VirtualBicycle.Ide.Properties;
using VirtualBicycle.Ide.Tools;
using VirtualBicycle.Graphics;
using VirtualBicycle.Logic.Traffic;
using VirtualBicycle.MathLib;
using VirtualBicycle.Scene;

namespace VirtualBicycle.Ide.Designers.WorldBuilder
{
    public class PlaceRoadSpline : WBToolBoxItem
    {
        List<Vector3> points;

        public WorldDesigner WorldBuilder
        {
            get;
            private set;
        }

        public PlaceRoadSpline(WorldDesigner wb)
        {
            this.WorldBuilder = wb;
            this.points = new List<Vector3>();
        }

        public override Image Icon
        {
            get { return Resources.PointerHS; }
        }

        public override string Name
        {
            get { return DevStringTable.Instance["Tool:PlaceRoadSpline"]; }
        }

        public override ToolBoxCategory Category
        {
            get { return ToolCategories.Instance.Road; }
        }

        public override void NotifyMouseDown(MouseEventArgs e)
        {

        }

        public override void NotifyMouseUp(MouseEventArgs e)
        {

        }

        public override void NotifyMouseMove(MouseEventArgs e)
        {

        }

        struct FitRoadDMEntry
        {
            Texture dispMap;
            ClusterDescription desc;

            public FitRoadDMEntry(Texture dispMap, ClusterDescription desc)
            {
                this.desc = desc;
                this.dispMap = dispMap;
            }

            public Texture DisplacementMap
            {
                get { return dispMap; }
            }

            public ClusterDescription ClusterDesc
            {
                get { return desc; }
            }
        }
        public unsafe override void NotifyMouseClick(MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                LineSegment ray = WorldBuilder.GetPickRay(e.X, e.Y);

                if (WorldBuilder.CurrentTerrain != null)
                {
                    Vector3 pos;
                    if (WorldBuilder.CurrentTerrain.Intersects(ray, out pos))
                    {
                        points.Add(pos);
                    }
                }
            }
            else if (e.Button == MouseButtons.Right)
            {
                if (MessageBox.Show(DevStringTable.Instance["MSG:CreateTrackSpline"], DevStringTable.Instance[""], MessageBoxButtons.YesNo) == DialogResult.Yes)
                {
                    EditableCluster cluster = WorldBuilder.CurrentCluster;
                    EditableTrack editTrack = new EditableTrack(WorldBuilder, WorldBuilder.Scene, cluster);
                    EditableGameScene scene = WorldBuilder.Scene;

                    RoadNode[] vertices = new RoadNode[points.Count];

                    for (int i = 0; i < vertices.Length; i++)
                    {
                        vertices[i].Position = points[i];
                        vertices[i].Width = 4;
                        vertices[i].Twist = 0;
                    }

                    RoadLine trackLine = new RoadLine(vertices);
                    editTrack.BuildModel(trackLine);
                    


                    WorldBuilder.Scene.NotifyTrackAdded(editTrack);
                    WorldBuilder.Scene.NotifyTrafficConAdded(editTrack.Track);

                }
                WorldBuilder.CurrentToolBoxItem = null;
                points.Clear();
            }

        }
        public override void NotifyActivated()
        {
            base.NotifyActivated();

            WorldBuilder.CurrentToolBoxItem = this;

        }
        public override void NotifyMouseDoubleClick(MouseEventArgs e)
        {

        }

        public override void NotifyMouseWheel(MouseEventArgs e)
        {

        }

        public override void Render()
        {
            Device device = GraphicsDevice.Instance.Device;

            device.VertexShader = null;
            device.PixelShader = null;

            device.SetRenderState<Cull>(RenderState.CullMode, Cull.None);
            device.Material = MeshMaterial.DefaultMatColor;
            device.SetTexture(0, null);

            for (int i = 0; i < points.Count; i++)
            {
                DevUtils.DrawLight(device, points[i]);
            }
        }
    }
}
