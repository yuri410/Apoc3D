using System;
using System.Collections.Generic;
using System.Text;
using SlimDX;
using SlimDX.Direct3D9;
using VirtualBicycle.Ide.Designers;
using VirtualBicycle.Ide.Designers.WorldBuilder;
using VirtualBicycle.Graphics;
using VirtualBicycle.Logic.Traffic;
using VirtualBicycle.MathLib;
using VirtualBicycle.Scene;

namespace VirtualBicycle.Ide.Editors.EditableObjects
{
    public class EditableTrack
    {
        struct FitRoadDMEntry
        {
            Texture dispMap;
            TerrainTexture terrain;

            public FitRoadDMEntry(Texture dispMap, TerrainTexture terrain)
            {
                this.terrain = terrain;
                this.dispMap = dispMap;
            }

            public Texture DisplacementMap
            {
                get { return dispMap; }
            }

            public TerrainTexture TerrainTexture
            {
                get { return terrain; }
            }
        }

        class TriangleCallBack : IMeshTriangleCallBack
        {
            RoadBuilder trackBuilder;

            public TriangleCallBack(RoadBuilder trackBuilder)
            {
                this.trackBuilder = trackBuilder;
            }

            #region IMeshTriangleCallBack 成员

            public void Process(Vector3 a, Vector3 b, Vector3 c)
            {
                trackBuilder.SmoothTerrain(a, b, c);
            }

            #endregion
        }

        int currentNode;

        Device device;

        EditableCluster cluster;
        Road track;
        RoadLine trackLine;

        EditableGameScene scene;
        WorldDesigner worldBuilder;

        string textureName = Road.DefaultTextureName;

        public string TextureName
        {
            get { return textureName; }
            set
            {
                textureName = value;
                if (track != null)
                {
                    track.TextureName = value;
                }
            }
        }

        public EditableTrack(WorldDesigner wb, EditableGameScene scene, EditableCluster cluster, Road track)
        {
            this.worldBuilder = wb;
            this.device = GraphicsDevice.Instance.Device;
            this.cluster = cluster;
            this.scene = scene;
            this.trackLine = track.TrackLine;
            this.track = track;
            this.track.Tag = this;
        }
        public EditableTrack(WorldDesigner wb, EditableGameScene scene, EditableCluster cluster)
        {
            this.worldBuilder = wb;
            this.device = GraphicsDevice.Instance.Device;
            this.cluster = cluster;
            this.scene = scene;
            this.trackLine = new RoadLine();
        }
        public EditableTrack(WorldDesigner wb, EditableGameScene scene, EditableCluster cluster, RoadLine trackLine)
        {
            this.worldBuilder = wb;
            this.device = GraphicsDevice.Instance.Device;
            this.cluster = cluster;
            this.scene = scene;
            this.trackLine = trackLine;
        }

        public Road Track
        {
            get { return track; }
        }
        public RoadLine TrackLine
        {
            get { return trackLine; }
        }

        public unsafe void TerrainFit()
        {
            if (track != null)
            {
                HeightMaps hmps = new HeightMaps();

                ClusterDescription desc = cluster.Description;

                List<FitRoadDMEntry> lockedTextures = new List<FitRoadDMEntry>();

                TerrainTexture dispMap = cluster.Terrain.DisplacementMap;

                Texture texture = dispMap.GetTexture;
                hmps.LeftUp = (float*)texture.LockRectangle(0, LockFlags.None).Data.DataPointer.ToPointer();
                lockedTextures.Add(new FitRoadDMEntry(texture, dispMap));

                desc.X++;

                EditableCluster curCluster = scene.ClusterTable.TryGetCluster(desc);
                if (curCluster != null)
                {
                    dispMap = curCluster.Terrain.DisplacementMap;
                    texture = dispMap.GetTexture;
                    lockedTextures.Add(new FitRoadDMEntry(texture, dispMap));

                    hmps.RightUp = (float*)texture.LockRectangle(0, LockFlags.None).Data.DataPointer.ToPointer();
                }

                desc.Y++;
                curCluster = scene.ClusterTable.TryGetCluster(desc);
                if (curCluster != null)
                {
                    dispMap = curCluster.Terrain.DisplacementMap;
                    texture = dispMap.GetTexture;
                    lockedTextures.Add(new FitRoadDMEntry(texture, dispMap));

                    hmps.LeftDown = (float*)texture.LockRectangle(0, LockFlags.None).Data.DataPointer.ToPointer();
                }

                desc.X--;
                curCluster = scene.ClusterTable.TryGetCluster(desc);
                if (curCluster != null)
                {
                    dispMap = curCluster.Terrain.DisplacementMap;
                    texture = dispMap.GetTexture;
                    lockedTextures.Add(new FitRoadDMEntry(texture, dispMap));

                    hmps.RightDown = (float*)texture.LockRectangle(0, LockFlags.None).Data.DataPointer.ToPointer();
                }

                RoadBuilder trackBuilder = new RoadBuilder();
                trackBuilder.CellUnit = cluster.CellUnit;
                trackBuilder.ClusterOffset = new Vector3(cluster.WorldX, cluster.WorldY, cluster.WorldZ);
                trackBuilder.ClusterBlocks = hmps;
                trackBuilder.HeightScale = cluster.Terrain.HeightScale;

                TriangleCallBack callback = new TriangleCallBack(trackBuilder);

                Model[] models = track.GetModels();

                for (int i = 0; i < models.Length; i++)
                {
                    GameMesh[] meshes = models[i].Entities;
                    for (int j = 0; j < meshes.Length; j++)
                    {
                        meshes[j].ProcessAllTriangles(callback);
                    }
                }

                for (int i = 0; i < lockedTextures.Count; i++)
                {
                    FitRoadDMEntry entry = lockedTextures[i];
                    entry.DisplacementMap.UnlockRectangle(0);
                    entry.TerrainTexture.NotifyChanged();
                }
            }
        }

        public void BuildModel(RoadLine trackLine)
        {
            BuildModel(trackLine, null, null);
        }

        public unsafe void BuildModel(RoadLine trackLine, TCPort? headConnector, TCPort? tailConnector)
        {
            this.trackLine = trackLine;

            HeightMaps hmps = new HeightMaps();

            ClusterDescription desc = cluster.Description;

            List<FitRoadDMEntry> lockedTextures = new List<FitRoadDMEntry>();

            TerrainTexture dispMap = cluster.Terrain.DisplacementMap;

            Texture texture = dispMap.GetTexture;
            hmps.LeftUp = (float*)texture.LockRectangle(0, LockFlags.None).Data.DataPointer.ToPointer();
            lockedTextures.Add(new FitRoadDMEntry(texture, dispMap));

            desc.X++;

            EditableCluster curCluster = scene.ClusterTable.TryGetCluster(desc);
            if (curCluster != null)
            {
                dispMap = curCluster.Terrain.DisplacementMap;
                texture = dispMap.GetTexture;
                lockedTextures.Add(new FitRoadDMEntry(texture, dispMap));

                hmps.RightUp = (float*)texture.LockRectangle(0, LockFlags.None).Data.DataPointer.ToPointer();
            }

            desc.Y++;
            curCluster = scene.ClusterTable.TryGetCluster(desc);
            if (curCluster != null)
            {
                dispMap = curCluster.Terrain.DisplacementMap;
                texture = dispMap.GetTexture;
                lockedTextures.Add(new FitRoadDMEntry(texture, dispMap));

                hmps.LeftDown = (float*)texture.LockRectangle(0, LockFlags.None).Data.DataPointer.ToPointer();
            }

            desc.X--;
            curCluster = scene.ClusterTable.TryGetCluster(desc);
            if (curCluster != null)
            {
                dispMap = curCluster.Terrain.DisplacementMap;
                texture = dispMap.GetTexture;
                lockedTextures.Add(new FitRoadDMEntry(texture, dispMap));

                hmps.RightDown = (float*)texture.LockRectangle(0, LockFlags.None).Data.DataPointer.ToPointer();
            }

            RoadBuilder trackBuilder = new RoadBuilder();
            trackBuilder.CellUnit = cluster.CellUnit;
            trackBuilder.ClusterOffset = new Vector3(cluster.WorldX, cluster.WorldY, cluster.WorldZ);
            trackBuilder.HeightScale = cluster.Terrain.HeightScale;
            trackBuilder.ClusterBlocks = hmps;
            trackBuilder.HeadConnector = headConnector;
            trackBuilder.TailConnector = tailConnector;

            bool hasOldData = track != null;
            int oldId = 0;
            List<TCConnectionInfo> connects = null;

            if (hasOldData)
            {
                oldId = track.Id;
                connects = track.GetConnections();
            }


            track = new Road(GraphicsDevice.Instance.Device, trackLine, trackBuilder, worldBuilder.Traffic);
            track.Tag = this;
            track.BuildPhysicsModel(null);
            track.TextureName = TextureName;

            if (hasOldData)
            {
                track.SetId(oldId);
                track.SetConnections(connects);
            }


            cluster.SceneManager.AddObjectToScene(track);

            for (int i = 0; i < lockedTextures.Count; i++)
            {
                FitRoadDMEntry entry = lockedTextures[i];
                entry.DisplacementMap.UnlockRectangle(0);
                entry.TerrainTexture.NotifyChanged();
            }
        }

        public void UpdateModel()
        {
            UpdateModel(null, null);
        }

        public void UpdateModel(TCPort? headConnector, TCPort? tailConnector)
        {
            cluster.SceneManager.RemoveObjectFromScene(track);
            track.Dispose();

            BuildModel(trackLine, headConnector, tailConnector);
        }

        public bool SelectNode(LineSegment ray)
        {
            RoadNode[] vertices = trackLine.InputPoints;

            for (int i = 0; i < vertices.Length; i++)
            {
                BoundingSphere bs;
                bs.Center = vertices[i].Position;
                bs.Radius = 1;

                if (MathEx.BoundingSphereIntersects(ref bs, ref ray.Start, ref ray.End))
                {
                    currentNode = i;
                    return true;
                }
            }

            return false;
        }

        public void ConnectWith(ITrafficComponment other, int selfPort, int targetPort)
        {
            //RoadConnector port1 = track.GetRoadConnectors()[selfPort];
            TCPort port2 = other.GetPorts()[targetPort];

            bool roadHead = selfPort == 0;

            RoadNode[] newNodes = new RoadNode[trackLine.InputPoints.Length + 2];


            if (roadHead)
            {
                Array.Copy(trackLine.InputPoints, 0, newNodes, 2, trackLine.InputPoints.Length);

                newNodes[0].Position = port2.Position;
                newNodes[1].Position = port2.Position + port2.Direction * 5f;
                newNodes[2].Position = 0.5f * (newNodes[1].Position + newNodes[3].Position);

                newNodes[0].Width = port2.Width;
                newNodes[0].Twist = port2.Twist;

                newNodes[1].Width = 0.5f * (newNodes[2].Width + newNodes[0].Width);
                newNodes[1].Twist = 0.5f * (newNodes[2].Twist + newNodes[0].Twist);


                trackLine.InputPoints = newNodes;

                this.UpdateModel(port2, null);
            }
            else
            {
                Array.Copy(trackLine.InputPoints, newNodes, trackLine.InputPoints.Length);

                int i = newNodes.Length - 1;
                newNodes[i].Position = port2.Position;
                newNodes[i - 1].Position = port2.Position + port2.Direction * 5f;
                newNodes[i - 2].Position = 0.5f * (newNodes[i - 1].Position + newNodes[i - 3].Position);

                newNodes[i].Width = port2.Width;
                newNodes[i].Twist = port2.Twist;

                newNodes[i - 1].Width = 0.5f * (newNodes[i - 2].Width + newNodes[i].Width);
                newNodes[i - 1].Twist = 0.5f * (newNodes[i - 2].Twist + newNodes[i].Twist);

                trackLine.InputPoints = newNodes;

                this.UpdateModel(null, port2);
            }

            this.track.ConnectWith(other, selfPort, targetPort);
        }

        public void ConnectWith(EditableTrack other, int selfPort, int targetPort)
        {
            TCPort port1 = track.GetPorts()[selfPort];
            TCPort port2 = other.track.GetPorts()[targetPort];

            TCPort newCon;

            newCon.Position = 0.5f * (port1.Position + port2.Position);
            newCon.Width = Math.Min(port1.Width, port2.Width);
            newCon.Twist = 0.5f * (port1.Twist + port2.Twist);
            newCon.Direction = port1.Position - port2.Position;
            newCon.Direction.Normalize();

            bool road1Head = selfPort == 0;
            bool road2Head = targetPort == 0;

            if (road1Head)
            {
                trackLine.InputPoints[0].Position = newCon.Position;
                this.UpdateModel(newCon, null);
            }
            else
            {
                int i = trackLine.InputPoints.Length - 1;
                trackLine.InputPoints[i].Position = newCon.Position;
                this.UpdateModel(null, newCon);
            }

            if (road2Head)
            {
                other.trackLine.InputPoints[0].Position = newCon.Position;
                other.UpdateModel(newCon, null);
            }
            else
            {
                int i = other.trackLine.InputPoints.Length - 1;
                other.trackLine.InputPoints[i].Position = newCon.Position; 
                other.UpdateModel(null, newCon);
            }

            this.track.ConnectWith(other.track, selfPort, targetPort);
        }
    }
}
