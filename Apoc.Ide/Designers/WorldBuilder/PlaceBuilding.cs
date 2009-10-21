using System;
using System.Collections.Generic;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using SlimDX;
using VirtualBicycle.Ide.Editors.EditableObjects;
using VirtualBicycle.Ide.Properties;
using VirtualBicycle.Ide.Tools;
using VirtualBicycle.Logic;
using VirtualBicycle.MathLib;
using VirtualBicycle.Scene;

namespace VirtualBicycle.Ide.Designers.WorldBuilder
{
    class PlaceBuilding : WBToolBoxItem
    {
        BuildingType bldType;

        public WorldDesigner WorldBuilder
        {
            get;
            private set;
        }
        public PlaceBuilding(WorldDesigner des, BuildingType bldType)
        {
            this.bldType = bldType;
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
            get { return bldType.Name; }
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

                        Building bld = bldType.CreateInstance();

                        bld.Position = pos;
                        //bld.Orientation = Quaternion.RotationAxis(Vector3.UnitY, VirtualBike.Randomizer.GetRandomSingle() * MathEx.PIf);
                        bld.UpdateTransform();
                        bld.BuildPhysicsModel(null);
                        sceMgr.AddObjectToScene(bld);
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
