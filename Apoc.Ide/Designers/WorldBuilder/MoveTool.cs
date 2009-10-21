using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
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
    class MoveTool : WBTool, ISelectableTool
    {
        EditableModel marker;

        Point lastPos;

        bool isMoving;
        bool isMouseLeftDown;

        enum ToolAxis
        {
            XY, YZ, XZ
        }
        ToolAxis axis;

        IPositionedObject currentEntity;

        public MoveTool(WorldDesigner wb, EditableGameScene scene)
            : base(wb, scene)
        {
            byte[] data = Resources.moveMarker;

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

                    currentEntity = sceObj as IPositionedObject;

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
                isMoving = false;
                isMouseLeftDown = false;
            }
        }

        public override void NotifyMouseMove(MouseEventArgs e)
        {
            Point loc = e.Location;

            if (!isMoving)
            {
                if ((e.Button & MouseButtons.Left) == MouseButtons.Left)
                {
                    float dist = (float)Math.Sqrt(MathEx.Sqr(e.X - lastPos.X) + MathEx.Sqr(e.Y - lastPos.Y));

                    if (dist > 3)
                    {
                        isMoving = true;
                    }
                }
            }
            else
            {
                if (currentEntity != null)
                {
                    LineSegment ray = WorldBuilder.GetPickRay(lastPos.X, lastPos.Y);
                    Vector3 dir = ray.End - ray.Start;
                    dir.Normalize();

                    Vector3 n = Vector3.UnitY;
                    float dist = 0;

                    switch (axis)
                    {
                        case ToolAxis.XY:
                            n = Vector3.UnitZ;
                            dist = ray.Start.Z - currentEntity.Position.Z;
                            break;
                        case ToolAxis.XZ:
                            n = Vector3.UnitY;
                            dist = ray.Start.Y - currentEntity.Position.Y;
                            break;
                        case ToolAxis.YZ:
                            n = Vector3.UnitX;
                            dist = ray.Start.X - currentEntity.Position.X;
                            break;
                    }

                    float cos = Vector3.Dot(n, dir);

                    if (cos < 100)
                    {
                        currentEntity.Position = ray.Start + dir * Math.Abs(dist / cos);
                        currentEntity.UpdateTransform();

                        RequiresRedraw = true;
                    }

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
                        case ToolAxis.XY:
                            axis = ToolAxis.XZ;
                            break;
                        case ToolAxis.XZ:
                            axis = ToolAxis.YZ;
                            break;
                        case ToolAxis.YZ:
                            axis = ToolAxis.XY;
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
                    case ToolAxis.XZ:
                        t = Matrix.RotationY(MathEx.PIf);
                        break;
                    case ToolAxis.XY:
                        t = Matrix.RotationX(MathEx.PiOver2) * Matrix.RotationZ(-MathEx.PiOver2);
                        break;
                    case ToolAxis.YZ:
                        t = Matrix.RotationZ(MathEx.PiOver2) * Matrix.RotationX(MathEx.PIf);
                        break;
                }

                SceneObject sobj = (SceneObject)currentEntity;
                float r = sobj.BoundingSphere.Radius;
                float scale = r / 10f;
                t *= Matrix.Scaling(scale, scale, scale);

                t *= Matrix.Translation(sobj.BoundingSphere.Center);

                
                marker.Render(t);
            }
        }
        #region ISelectableTool 成员

        public SceneObject SelectedObject
        {
            get { return (SceneObject)currentEntity; }
            set { currentEntity = (IPositionedObject)value; }
        }
        #endregion
    }
}
