using System;
using System.Collections.Generic;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using SlimDX;
using VirtualBicycle.Ide.Editors.EditableObjects;
using VirtualBicycle.Ide.Properties;
using VirtualBicycle.IO;
using VirtualBicycle.Logic.Traffic;
using VirtualBicycle.MathLib;
using VirtualBicycle.Scene;

namespace VirtualBicycle.Ide.Designers.WorldBuilder
{
    class MoveSplineNodeTool : WBTool
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

        EditableTrack currentTrack;
        RoadLine currentTrackLine;
        int index;

        public MoveSplineNodeTool(WorldDesigner wb, EditableGameScene scene)
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

                List<EditableTrack> tracks = Scene.Tracks;

                Matrix view = WorldBuilder.Camera.ViewMatrix;
                Matrix proj = WorldBuilder.Camera.ProjectionMatrix;
                Matrix world = Matrix.Identity;
                Viewport viewport = GraphicsDevice.Instance.Device.Viewport;

                for (int i = 0; i < tracks.Count; i++)
                {
                    RoadLine line = tracks[i].TrackLine;

                    for (int j = 0; j < line.InputPoints.Length; j++)
                    {
                        Vector3 projectedPos = Vector3.Project(line.InputPoints[j].Position, viewport, proj, view, world);
                        projectedPos.Z = 0;

                        Vector3 mousePos = new Vector3(e.X, e.Y, 0);

                        float dist = Vector3.Distance(mousePos, projectedPos);

                        if (dist < 10)
                        {
                            currentTrackLine = line;
                            currentTrack = tracks[i];
                            index = j;
                            break;
                        }
                    }
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
                if (currentTrackLine != null)
                {
                    Vector3 position = currentTrackLine.InputPoints[index].Position;

                    LineSegment ray = WorldBuilder.GetPickRay(lastPos.X, lastPos.Y);
                    Vector3 dir = ray.End - ray.Start;
                    dir.Normalize();

                    Vector3 n = Vector3.UnitY;
                    float dist = 0;

                    switch (axis)
                    {
                        case ToolAxis.XY:
                            n = Vector3.UnitZ;
                            dist = ray.Start.Z - position.Z;
                            break;
                        case ToolAxis.XZ:
                            n = Vector3.UnitY;
                            dist = ray.Start.Y - position.Y;
                            break;
                        case ToolAxis.YZ:
                            n = Vector3.UnitX;
                            dist = ray.Start.X - position.X;
                            break;
                    }

                    float cos = Vector3.Dot(n, dir);

                    if (cos < 100)
                    {
                        currentTrackLine.InputPoints[index].Position = ray.Start + dir * Math.Abs(dist / cos);

                        currentTrack.UpdateModel();

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
            if (currentTrackLine != null)
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

                t *= Matrix.Translation(currentTrackLine.InputPoints[index].Position);

                marker.Render(t);
            }
        }
    }
}
