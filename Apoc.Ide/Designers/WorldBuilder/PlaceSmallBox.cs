using System;
using System.Collections.Generic;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using SlimDX;
using VirtualBicycle.Ide.Editors.EditableObjects;
using VirtualBicycle.Ide.Properties;
using VirtualBicycle.Ide.Tools;
using VirtualBicycle;
using VirtualBicycle.MathLib;
using VirtualBicycle.Scene;

namespace VirtualBicycle.Ide.Designers.WorldBuilder
{
    [TestOnly()]
    class PlaceSmallBox : WBToolBoxItem
    {
        public WorldDesigner WorldBuilder
        {
            get;
            private set;
        }

        public PlaceSmallBox(WorldDesigner wb)
        {
            WorldBuilder = wb;
        }

        public override void Render()
        {

        }

        public override Image Icon
        {
            get { return Resources.New; }
        }

        public override string Name
        {
            get { return "Small Box"; }
        }

        public override ToolBoxCategory Category
        {
            get { return ToolCategories.Instance.Building; }
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

        public override void NotifyMouseClick(MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                LineSegment ray = WorldBuilder.GetPickRay(e.X, e.Y);

                if (WorldBuilder.CurrentTerrain != null)
                {
                    Vector3 pos;
                    if (WorldBuilder.CurrentTerrain.Intersects(ray, out pos))
                    {
                        EditableCluster cluster = WorldBuilder.CurrentCluster;
                        SceneManagerBase sceMgr = cluster.SceneManager;

                        FakeSmallBox bike = new FakeSmallBox(GraphicsDevice.Instance.Device);
                        bike.Position = pos;
                        bike.Orientation = Quaternion.Identity;
                        bike.UpdateTransform();

                        sceMgr.AddObjectToScene(bike);
                    }
                }
            }
            else if (e.Button == MouseButtons.Right)
            {
                WorldBuilder.CurrentToolBoxItem = null;
            }
        }

        public override void NotifyMouseDoubleClick(MouseEventArgs e)
        {

        }

        public override void NotifyMouseWheel(MouseEventArgs e)
        {

        }

        public override void NotifyActivated()
        {
            base.NotifyActivated();

            WorldBuilder.CurrentToolBoxItem = this;
        }
    }
}
