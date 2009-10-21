using System;
using System.Collections.Generic;
using System.Drawing;
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
    class RotateTool : WBTool, ISelectableTool
    {
        EditableModel marker;

        Point lastPos;

        bool isRotating;
        bool isMouseLeftDown; 

        enum ToolAxis
        {
            X, Y, Z
        }

        ToolAxis axis;
        IRoatableObject currentEntity;

        public RotateTool(WorldDesigner wb, EditableGameScene scene)
            : base(wb, scene)
        {
            byte[] data = Resources.rotationMark;

            System.IO.MemoryStream ms = new System.IO.MemoryStream(data);
            StreamedLocation sl = new StreamedLocation(ms);

            marker = EditableModel.FromFile(sl);
        }

        public override void NotifyMouseDown(MouseEventArgs e)
        {
            if ((e.Button & MouseButtons.Left) == MouseButtons.Left)
            {
                lastPos = e.Location;
                isMouseLeftDown = true;

                LineSegment ray = WorldBuilder.GetPickRay(e.X, e.Y);

                SceneObject sceObj = Scene.FindObject(ray);

                if (sceObj != null)
                {
                    Cluster clusterInfo = sceObj.ParentCluster;

                    EditableCluster cluster = Scene.ClusterTable[clusterInfo.Description];

                    if (cluster != null)
                    {
                        WorldBuilder.SetCluster(cluster);
                    }
                    WorldBuilder.NotifyPropertyChanged(new object[] { sceObj });

                    currentEntity = sceObj as IRoatableObject;

                    if (currentEntity != null)
                    {
                        if (!currentEntity.EditorMovable)
                        {
                            currentEntity = null;
                        }
                    }
                }
                else 
                {
                    currentEntity = null;
                }
            }
        }

        public override void NotifyMouseUp(MouseEventArgs e)
        {
            if ((e.Button & MouseButtons.Left) == MouseButtons.Left)
            {
                isRotating = false;
                isMouseLeftDown = false;
            }
        }

        public override void NotifyMouseMove(MouseEventArgs e)
        {
            Point loc = e.Location;

            if (!isRotating)
            {
                if ((e.Button & MouseButtons.Left) == MouseButtons.Left)
                {
                    float dist = (float)Math.Sqrt(MathEx.Sqr(e.X - lastPos.X) + MathEx.Sqr(e.Y - lastPos.Y));

                    if (dist > 3)
                    {
                        isRotating = true;
                    }
                }
            }
            else
            {
                if (currentEntity != null)
                {
                    int delta = (loc.Y - lastPos.Y);

                    switch (axis)
                    {
                        case ToolAxis.X:
                            currentEntity.Orientation *= Quaternion.RotationAxis(Vector3.UnitX, 0.05f * delta);
                            currentEntity.UpdateTransform();
                            break;
                        case ToolAxis.Y:
                            currentEntity.Orientation *= Quaternion.RotationAxis(Vector3.UnitY, 0.05f * delta);
                            currentEntity.UpdateTransform();
                            break;
                        case ToolAxis.Z:
                            currentEntity.Orientation *= Quaternion.RotationAxis(Vector3.UnitZ, 0.05f * delta);
                            currentEntity.UpdateTransform();
                            break;
                    }

                    RequiresRedraw = true;

                    lastPos = loc;
                }
            }
        }

        public override void NotifyMouseClick(MouseEventArgs e)
        {
            if ((e.Button & MouseButtons.Right) == MouseButtons.Right)
            {
                if (isMouseLeftDown)
                {
                    switch (axis)
                    {
                        case ToolAxis.X:
                            axis = ToolAxis.Y;
                            break;
                        case ToolAxis.Y:
                            axis = ToolAxis.Z;
                            break;
                        case ToolAxis.Z:
                            axis = ToolAxis.X;
                            break;
                    }
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
            if (currentEntity != null)
            {
                Matrix t = Matrix.Identity;
                switch (axis)
                {
                    case ToolAxis.X:
                        t = Matrix.RotationZ(MathEx.PiOver2);
                        break;
                    case ToolAxis.Z:
                        t = Matrix.RotationX(MathEx.PiOver2);
                        break;
                }

                SceneObject sobj = (SceneObject)currentEntity;
                float r = sobj.BoundingSphere.Radius;
                float scale = sobj.BoundingSphere.Radius / 10f;
                t *= Matrix.Scaling(scale, scale, scale);

                t *= Matrix.Translation(sobj.BoundingSphere.Center);


                marker.Render(t);
            }
        }

        #region ISelectableTool 成员

        public SceneObject SelectedObject
        {
            get { return (SceneObject)currentEntity; }
            set { currentEntity = (IRoatableObject)value; }
        }
        #endregion
    }
}
