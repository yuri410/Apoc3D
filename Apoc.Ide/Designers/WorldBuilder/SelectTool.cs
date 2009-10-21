using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using SlimDX;
using VirtualBicycle.Ide.Editors.EditableObjects;
using VirtualBicycle.Ide.Properties;
using VirtualBicycle.IO;
using VirtualBicycle.MathLib;
using VirtualBicycle.Scene;

namespace VirtualBicycle.Ide.Designers.WorldBuilder
{
    interface ISelectableTool
    {
        SceneObject SelectedObject
        {
            get;
            set;
        }
    }


    class SelectionCallBack : IObjectFilter
    {

        #region IObjectFilter 成员

        public bool Check(SceneObject obj)
        {
            if (obj is Terrain)
                return false;
            return true;
        }

        public bool Check(OctreeSceneNode node)
        {
            return true;
        }

        #endregion
    }

    class SelectTool : WBTool, ISelectableTool
    {
        EditableModel marker;

        SceneObject selectedObject;        

        public SelectTool(WorldDesigner wb, EditableGameScene scene)
            : base(wb, scene)
        {
            byte[] data = Resources.selectionMarker;

            System.IO.MemoryStream ms = new System.IO.MemoryStream(data);
            StreamedLocation sl = new StreamedLocation(ms);

            marker = EditableModel.FromFile(sl);
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

                SceneObject sceObj = Scene.FindObject(ray, new SelectionCallBack());

                if (sceObj != null)
                {
                    Cluster clusterInfo = sceObj.ParentCluster;

                    EditableCluster cluster = Scene.ClusterTable[clusterInfo.Description];

                    if (cluster != null)
                    {
                        WorldBuilder.SetCluster(cluster);
                    }
                    WorldBuilder.NotifyPropertyChanged(new object[] { sceObj });

                    selectedObject = sceObj;
                }
                else 
                {
                    selectedObject = null;
                }
            }
        }

        public override void NotifyMouseWheel(MouseEventArgs e)
        {

        }
        public override void NotifyMouseDoubleClick(MouseEventArgs e)
        {

        }

        public override void Render()
        {
            if (selectedObject != null)
            {
                float r = selectedObject.BoundingSphere.Radius;
                float scale = selectedObject.BoundingSphere.Radius / 10f;
                Matrix t = Matrix.Scaling(scale, scale, scale);

                t *= Matrix.Translation(selectedObject.BoundingSphere.Center);

                marker.Render(t);
            }
        }


        #region ISelectableTool 成员

        public SceneObject SelectedObject
        {
            get { return selectedObject; }
            set { selectedObject = value; }
        }
        #endregion
    }
}
