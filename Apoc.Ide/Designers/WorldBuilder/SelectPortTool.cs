using System;
using System.Collections.Generic;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using SlimDX;
using SlimDX.Direct3D9;
using VirtualBicycle.Ide.Editors.EditableObjects;
using VirtualBicycle.Graphics;
using VirtualBicycle.Logic.Traffic;

namespace VirtualBicycle.Ide.Designers.WorldBuilder
{
    class SelectPortTool : WBTool
    {
        struct SelectedEntry
        {
            public ITrafficComponment Conponment;

            public int Index;

            public Vector3 Position;
        }

        List<SelectedEntry> selectedPoints = new List<SelectedEntry>();

        Rectangle selectionRect;

        public SelectPortTool(WorldDesigner wb, EditableGameScene scene)
            : base(wb, scene)
        {

        }

        public override void NotifyMouseDown(MouseEventArgs e)
        {
            selectionRect.X = e.X;
            selectionRect.Y = e.Y;
        }

        public override void NotifyMouseUp(MouseEventArgs e)
        {

        }

        public override void NotifyMouseMove(MouseEventArgs e)
        {

        }

        public override void NotifyMouseClick(MouseEventArgs e)
        {
            selectedPoints.Clear();
            if (e.X < selectionRect.X)
            {
                selectionRect.Width = selectionRect.X - e.X;
                selectionRect.X = e.X;
            }
            else
            {
                selectionRect.Width = e.X - selectionRect.X;
            }

            if (e.Y < selectionRect.Y)
            {
                selectionRect.Height = selectionRect.Y - e.Y;
                selectionRect.Y = e.Y;
            }
            else
            {
                selectionRect.Height = e.Y - selectionRect.Y;
            }

            List<ITrafficComponment> cons = Scene.TrafficConponments;

            Matrix view = WorldBuilder.Camera.ViewMatrix;
            Matrix proj = WorldBuilder.Camera.ProjectionMatrix;
            Matrix world = Matrix.Identity;
            Viewport viewport = GraphicsDevice.Instance.Device.Viewport;

            for (int i = 0; i < cons.Count; i++)
            {
                TCPort[] cs = cons[i].GetPorts();

                for (int j = 0; j < cs.Length; j++)
                {
                    Vector3 projectedPos = Vector3.Project(cs[j].Position, viewport, proj, view, world);

                    if (selectionRect.Contains((int)projectedPos.X, (int)projectedPos.Y))
                    {
                        SelectedEntry se;
                        se.Index = j;
                        se.Conponment = cons[i];
                        se.Position = cs[j].Position;

                        selectedPoints.Add(se);
                    }
                }
            }

            WorldBuilder.NotifyPropertyChanged(null);
        }

        public override void NotifyMouseWheel(MouseEventArgs e)
        {

        }
        public override void NotifyMouseDoubleClick(MouseEventArgs e)
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

            for (int i = 0; i < selectedPoints.Count; i++)
            {
                int idx = selectedPoints[i].Index;

                DevUtils.DrawLight(GraphicsDevice.Instance.Device, selectedPoints[i].Position);
            }
        }

        public void Connect()
        {
            if (selectedPoints.Count == 2)
            {
                int index1 = selectedPoints[0].Index;
                int index2 = selectedPoints[1].Index;

                ITrafficComponment con1 = selectedPoints[0].Conponment;
                ITrafficComponment con2 = selectedPoints[1].Conponment;

                if (!object.ReferenceEquals(con1, con2))
                {
                    Road track1 = con1 as Road;
                    Road track2 = con2 as Road;

                    if (track1 != null && track2 != null)
                    {
                        ((EditableTrack)track1.Tag).ConnectWith((EditableTrack)track2.Tag, index1, index2);
                    }
                    else if (track1 != null && track2 == null)
                    {
                        ((EditableTrack)track1.Tag).ConnectWith(con2, index1, index2);
                    }
                    else if (track1 == null && track2 != null)
                    {
                        ((EditableTrack)track2.Tag).ConnectWith(con1, index2, index1);
                    }
                }
            }
        }
    }
}
