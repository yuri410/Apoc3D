using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using SlimDX;
using SlimDX.Direct3D9;
using VirtualBicycle.Ide.Editors.EditableObjects;
using VirtualBicycle.Collections;
using VirtualBicycle.Graphics;
using VirtualBicycle.Logic.Traffic;
using VirtualBicycle.MathLib;

namespace VirtualBicycle.Ide.Designers.WorldBuilder
{
    class SelectRoadNodeTool : WBTool
    {
        struct SelectedEntry
        {
            public EditableTrack Track;

            public int Index;
        }

        class EditableEntry
        {
            RoadNode value;
            EditableTrack track;

            SelectRoadNodeTool tool;

            int index;

            public EditableEntry(RoadNode val, SelectRoadNodeTool tool, EditableTrack track, int index)
            {
                this.tool = tool;
                this.value = val;
                this.track = track;
                this.index = index;
            }

            [LocalizedDescription("PROP:SpNodePosition")]
            public Vector3 Position
            {
                get { return value.Position; }
                set
                {
                    if (value != this.value.Position)
                    {
                        this.value.Position = value;
                        if (!tool.updateTable.Exists(track))
                        {
                            tool.updateTable.Add(track);
                        }
                    }
                }
            }

            [LocalizedDescription("PROP:SpNodeWidth")]
            public float Width
            {
                get { return value.Width; }
                set
                {
                    if (value != this.value.Width)
                    {
                        this.value.Width = value;
                        if (!tool.updateTable.Exists(track))
                        {
                            tool.updateTable.Add(track);
                        }
                    }
                }
            }

            [LocalizedDescription("PROP:SpNodeTwist")]
            public float Twist
            {
                get { return MathEx.Radian2Degree(value.Twist); }
                set
                {
                    value = MathEx.Degree2Radian(value);
                    if (value != this.value.Twist)
                    {
                        this.value.Twist = value;
                        if (!tool.updateTable.Exists(track))
                        {
                            tool.updateTable.Add(track);
                        }
                    }
                }
            }

            [Browsable(false)]
            public int Index
            {
                get { return index; }
            }

            [Browsable(false)]
            public RoadNode Value
            {
                get { return value; }
                set { this.value = value; }
            }
            [Browsable(false)]
            public EditableTrack Track
            {
                get { return track; }
            }
        }

        List<SelectedEntry> selectedPoints = new List<SelectedEntry>();

        ExistTable<EditableTrack> updateTable = new ExistTable<EditableTrack>();

        EditableEntry[] editEnts;

        //bool isSelecting;
        Rectangle selectionRect;

        public SelectRoadNodeTool(WorldDesigner wb, EditableGameScene scene)
            : base(wb, scene)
        {
        }

        public override void NotifyMouseDown(MouseEventArgs e)
        {
            //isSelecting = true;
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

            //isSelecting = false;
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

            List<EditableTrack> tracks = Scene.Tracks;

            Matrix view = WorldBuilder.Camera.ViewMatrix;
            Matrix proj = WorldBuilder.Camera.ProjectionMatrix;
            Matrix world = Matrix.Identity;
            Viewport viewport = GraphicsDevice.Instance.Device.Viewport;

            for (int i = 0; i < tracks.Count; i++)
            {
                for (int j = 0; j < tracks[i].TrackLine.InputPoints.Length; j++)
                {
                    Vector3 projectedPos = Vector3.Project(tracks[i].TrackLine.InputPoints[j].Position, viewport, proj, view, world);

                    if (selectionRect.Contains((int)projectedPos.X, (int)projectedPos.Y))
                    {
                        SelectedEntry se;
                        se.Index = j;
                        se.Track = tracks[i];

                        selectedPoints.Add(se);
                    }
                }
            }

            EditableEntry[] editObjs = new EditableEntry[selectedPoints.Count];
            for (int i = 0; i < selectedPoints.Count; i++)
            {
                int idx = selectedPoints[i].Index;
                editObjs[i] = new EditableEntry(selectedPoints[i].Track.TrackLine.InputPoints[idx], this, selectedPoints[i].Track, idx);
            }
            this.editEnts = editObjs;
            WorldBuilder.NotifyPropertyChanged(editObjs);
        }

        public override void NotifyMouseDoubleClick(MouseEventArgs e)
        {

        }

        public override void NotifyMouseWheel(MouseEventArgs e)
        {

        }

        public override void Render()
        {
            if (updateTable.Count > 0)
            {
                for (int i = 0; i < editEnts.Length; i++)
                {
                    int idx = editEnts[i].Index;
                    editEnts[i].Track.TrackLine.InputPoints[idx] = editEnts[i].Value;
                }


                foreach (EditableTrack t in updateTable)
                {
                    t.UpdateModel();
                }

                updateTable.Clear();
            }

            Device device = GraphicsDevice.Instance.Device;

            device.VertexShader = null;
            device.PixelShader = null;

            device.SetRenderState<Cull>(RenderState.CullMode, Cull.None);
            device.Material = MeshMaterial.DefaultMatColor;
            device.SetTexture(0, null);
            device.SetTexture(1, null);

            for (int i = 0; i < selectedPoints.Count; i++)
            {
                int idx = selectedPoints[i].Index;

                DevUtils.DrawLight(GraphicsDevice.Instance.Device, selectedPoints[i].Track.TrackLine.InputPoints[idx].Position);
            }
        }


        Dictionary<EditableTrack, List<int>> GetIndexPerTrackTable()
        {
            Dictionary<EditableTrack, List<int>> table = new Dictionary<EditableTrack, List<int>>();

            for (int i = 0; i < editEnts.Length; i++)
            {
                EditableTrack key = editEnts[i].Track;

                List<int> verts;

                if (!table.TryGetValue(key, out verts))
                {
                    verts = new List<int>();
                    table.Add(key, verts);
                }

                table[key].Add(editEnts[i].Index);
            }
            return table;
        }
        ExistTable<EditableTrack> GetTrackTable()
        {
            ExistTable<EditableTrack> table = new ExistTable<EditableTrack>();

            for (int i = 0; i < editEnts.Length; i++)
            {
                EditableTrack key = editEnts[i].Track;

                if (!table.Exists(key))
                {
                    table.Add(key);
                }
            }
            return table;
        }


        public void SetRoadTexture(string name)
        {
            ExistTable<EditableTrack> table = GetTrackTable();

            foreach (EditableTrack trk in table)
            {
                trk.TextureName = name;
            }
        }

        public void TerrainFit()
        {
            ExistTable<EditableTrack> table = GetTrackTable();

            foreach (EditableTrack trk in table)
            {
                trk.TerrainFit();
            }
        }

        public void Streighten()
        {
            Dictionary<EditableTrack, List<int>> table = GetIndexPerTrackTable();

            List<EditableTrack> processed = new List<EditableTrack>(table.Count);
            foreach (KeyValuePair<EditableTrack, List<int>> e in table)
            {
                EditableTrack track = e.Key;
                List<int> list = e.Value;

                if (list.Count > 1)
                {
                    int maxIdx = -1;
                    int minIdx = int.MaxValue;

                    for (int i = 0; i < list.Count; i++)
                    {
                        if (list[i] > maxIdx)
                        {
                            maxIdx = list[i];
                        }
                        if (list[i] < minIdx)
                        {
                            minIdx = list[i];
                        }
                    }

                    RoadNode[] inputPoint = track.TrackLine.InputPoints;

                    Vector3 dir = inputPoint[maxIdx].Position - inputPoint[minIdx].Position;
                    dir.Normalize();


                    for (int i = 0; i < inputPoint.Length; i++)
                    {
                        float distance = Vector3.Dot(inputPoint[minIdx].Position - inputPoint[i].Position, dir);

                        Vector3 targetPos = inputPoint[minIdx].Position - dir * distance;

                        float dist = Vector3.Distance(targetPos, inputPoint[i].Position);

                        if (dist < 0.5f)
                        {
                            inputPoint[i].Position = targetPos;
                        }
                        else
                        {
                            inputPoint[i].Position = 0.5f * targetPos + 0.5f * inputPoint[i].Position;
                        }
                    }

                    processed.Add(track);
                }
            }

            for (int i = 0; i < processed.Count; i++)
            {
                processed[i].UpdateModel();
            }

        }

        public void Disconnect()
        {
            if (editEnts.Length == 2)
            {
                int index1 = editEnts[0].Index;
                int index2 = editEnts[1].Index;

                EditableTrack track1 = editEnts[0].Track;
                EditableTrack track2 = editEnts[1].Track;

                if (!object.ReferenceEquals(track1, track2))
                {
                    int count1 = track1.TrackLine.InputPoints.Length;
                    int count2 = track2.TrackLine.InputPoints.Length;


                    if ((index1 == 0 || index1 == count1 - 1) &&
                        (index2 == 0 || index2 == count2 - 1))
                    {
                        track1.UpdateModel();
                        track2.UpdateModel();
                    }
                }
            }
        }

        public void InsertNode()
        {
            Dictionary<EditableTrack, List<int>> table = GetIndexPerTrackTable();
            List<EditableTrack> processed = new List<EditableTrack>(table.Count);
            foreach (KeyValuePair<EditableTrack, List<int>> e in table)
            {
                EditableTrack track = e.Key;
                List<int> list = e.Value;

                RoadNode[] inputPoint = track.TrackLine.InputPoints;
                RoadNode[] newPoints = new RoadNode[inputPoint.Length + 1];

                int index = list[0];

                Array.Copy(inputPoint, 0, newPoints, 0, index + 1);

                Array.Copy(inputPoint, index + 1, newPoints, index + 2, inputPoint.Length - index - 1);

                index++;

                int a = index - 1;
                if (a < 0)
                {
                    a = 0;
                }

                int b = index + 1;
                if (b >= newPoints.Length)
                {
                    b = newPoints.Length - 1;
                }
                
                newPoints[index].Position = 0.5f * (newPoints[a].Position + newPoints[b].Position);
                newPoints[index].Width = 0.5f * (newPoints[a].Width + newPoints[b].Width);
                newPoints[index].Twist = 0.5f * (newPoints[a].Twist + newPoints[b].Twist);


                track.TrackLine.InputPoints = newPoints;
            }
            for (int i = 0; i < processed.Count; i++)
            {
                processed[i].UpdateModel();
            }
        }

        public void DeleteNodes()
        {
            Dictionary<EditableTrack, List<int>> table = GetIndexPerTrackTable();

            List<EditableTrack> processed = new List<EditableTrack>(table.Count);
            foreach (KeyValuePair<EditableTrack, List<int>> e in table)
            {
                EditableTrack track = e.Key;
                List<int> list = e.Value;
                 
                RoadNode[] inputPoint = track.TrackLine.InputPoints;
                bool[] hashTable = new bool[inputPoint.Length];

                for (int i = 0; i < list.Count; i++)
                {
                    hashTable[list[i]] = true;
                }

                List<RoadNode> newList = new List<RoadNode>();

                for (int i = 0; i < inputPoint.Length; i++)
                {
                    if (!hashTable[i])
                    {
                        newList.Add(inputPoint[i]);
                    }
                }

                if (newList.Count > 1)
                {
                    track.TrackLine.InputPoints = newList.ToArray();
                    processed.Add(track);
                }
            }

            for (int i = 0; i < processed.Count; i++)
            {
                processed[i].UpdateModel();
            }
            selectedPoints.Clear();
            this.editEnts = null;
            WorldBuilder.NotifyPropertyChanged(editEnts);            
        }
    }
}
