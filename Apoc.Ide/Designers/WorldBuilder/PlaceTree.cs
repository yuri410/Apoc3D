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
    class PlaceTree : WBToolBoxItem
    {
        TerrainObjectType objType;

        public WorldDesigner WorldBuilder
        {
            get;
            private set;
        }
        public PlaceTree(WorldDesigner des, TerrainObjectType bldType)
        {
            this.objType = bldType;
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
            get { return objType.Name; }
        }

        public override ToolBoxCategory Category
        {
            get { return ToolCategories.Instance.Tree; }
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

                        TerrainObject tree = objType.CreateInstance();

                        tree.Position = pos;
                        tree.Orientation = Quaternion.RotationAxis(Vector3.UnitY, VirtualBicycle.Randomizer.GetRandomSingle() * MathEx.PIf);
                        tree.HeightScale = 1 + (VirtualBicycle.Randomizer.GetRandomSingle() - 1) * 0.3f;
                        tree.UpdateTransform();
                        tree.BuildPhysicsModel(null);

                        sceMgr.AddObjectToScene(tree);
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
