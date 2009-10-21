using System;
using System.Collections.Generic;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using SlimDX;
using VirtualBicycle.Ide.Editors.EditableObjects;
using VirtualBicycle.Ide.Properties;
using VirtualBicycle.Ide.Tools;
using VirtualBicycle.Logic.Traffic;
using VirtualBicycle.MathLib;
using VirtualBicycle.Scene;

namespace VirtualBicycle.Ide.Designers.WorldBuilder
{
    class PlaceCrossing : WBToolBoxItem
    {
        JunctionType crsType;

        public WorldDesigner WorldBuilder
        {
            get;
            private set;
        }
        public PlaceCrossing(WorldDesigner des, JunctionType crsType)
        {
            this.crsType = crsType;
            this.WorldBuilder = des;
        }

        public override void Render()
        {

        }

        public override Image Icon
        {
            get { return Resources.PointerHS; }
        }

        public override string Name
        {
            get { return crsType.Name; }
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

                        Junction crs = crsType.CreateInstance(WorldBuilder.Traffic);

                        crs.Position = pos;
                        //bld.Orientation = Quaternion.RotationAxis(Vector3.UnitY, VirtualBike.Randomizer.GetRandomSingle() * MathEx.PIf);
                        crs.UpdateTransform();
                        crs.BuildPhysicsModel(null);
                        sceMgr.AddObjectToScene(crs);
                       

                        WorldBuilder.Scene.NotifyTrafficConAdded(crs);
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
