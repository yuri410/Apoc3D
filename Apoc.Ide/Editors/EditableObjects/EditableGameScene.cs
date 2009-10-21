using System;
using System.Collections.Generic;
using System.Drawing;
using System.Text;
using SlimDX;
using SlimDX.Direct3D9;
using VirtualBicycle.Ide.Designers;
using VirtualBicycle.Ide.Designers.WorldBuilder;
using VirtualBicycle;
using VirtualBicycle.Collections;
using VirtualBicycle.Graphics;
using VirtualBicycle.Graphics.Effects;
using VirtualBicycle.Logic.Traffic;
using VirtualBicycle.MathLib;
using VirtualBicycle.Scene;

namespace VirtualBicycle.Ide.Editors.EditableObjects
{
    public unsafe class EditableGameScene : GameSceneBase<EditableCluster, EditableSceneData>
    {
        class NoPost : IPostSceneRenderer
        {

            #region IPostSceneRenderer 成员

            public void Render(ISceneRenderer renderer, Surface screenTarget)
            {
                renderer.RenderScenePost(screenTarget);
            }

            #endregion

            #region IDisposable 成员

            public void Dispose()
            {
                
            }

            #endregion
        }


        EditableSceneData edData;
        EditableClusterTable clusterTable;

        List<EditableTrack> tracks = new List<EditableTrack>();
        MeshMaterial spNodeMat;

        RenderOperation clusterMarker;
        WorldDesigner worldBuilder;

        public EditableClusterTable ClusterTable
        {
            get { return clusterTable; }
        }


        void BuildClusterMarker()
        {
            Device device = GraphicsDevice.Instance.Device;

            int vtxCount = 8;
            VertexBuffer vb = new VertexBuffer(device, sizeof(VertexPC) * vtxCount, Usage.None, VertexPC.Format, Pool.Managed);
            VertexPC* vtxPtr = (VertexPC*)vb.Lock(0, 0, LockFlags.None).DataPointer.ToPointer();

            int color = Color.Red.ToArgb();

            vtxPtr[0].diffuse = color;
            vtxPtr[0].pos = new Vector3(0, -500, 0);
            vtxPtr[1].diffuse = color;
            vtxPtr[1].pos = new Vector3(Terrain.TerrainSize, -500, 0);
            vtxPtr[2].diffuse = color;
            vtxPtr[2].pos = new Vector3(Terrain.TerrainSize, -500, Terrain.TerrainSize);
            vtxPtr[3].diffuse = color;
            vtxPtr[3].pos = new Vector3(0, -500, Terrain.TerrainSize);

            vtxPtr[4].diffuse = color;
            vtxPtr[4].pos = new Vector3(0, 500, 0);
            vtxPtr[5].diffuse = color;
            vtxPtr[5].pos = new Vector3(Terrain.TerrainSize, 500, 0);
            vtxPtr[6].diffuse = color;
            vtxPtr[6].pos = new Vector3(Terrain.TerrainSize, 500, Terrain.TerrainSize);
            vtxPtr[7].diffuse = color;
            vtxPtr[7].pos = new Vector3(0, 500, Terrain.TerrainSize);

            vb.Unlock();

            IndexBuffer ib = new IndexBuffer(device, sizeof(ushort) * 24, Usage.None, Pool.Managed, true);

            short* idxPtr = (short*)ib.Lock(0, 0, LockFlags.None).DataPointer.ToPointer();

            idxPtr[0] = 0;
            idxPtr[1] = 1;

            idxPtr[2] = 1;
            idxPtr[3] = 2;

            idxPtr[4] = 2;
            idxPtr[5] = 3;

            idxPtr[6] = 3;
            idxPtr[7] = 0;



            idxPtr[8] = 4;
            idxPtr[9] = 5;

            idxPtr[10] = 5;
            idxPtr[11] = 6;

            idxPtr[12] = 6;
            idxPtr[13] = 7;

            idxPtr[14] = 7;
            idxPtr[15] = 4;



            idxPtr[16] = 0;
            idxPtr[17] = 4;

            idxPtr[18] = 1;
            idxPtr[19] = 5;

            idxPtr[20] = 2;
            idxPtr[21] = 6;

            idxPtr[22] = 3;
            idxPtr[23] = 7;


            ib.Unlock();

            clusterMarker.Geomentry = new GeomentryData(null);
            clusterMarker.Geomentry.Format = VertexPC.Format;
            clusterMarker.Geomentry.IndexBuffer = ib;
            clusterMarker.Material = MeshMaterial.DefaultMaterial;
            clusterMarker.Geomentry.PrimCount = 24;
            clusterMarker.Geomentry.PrimitiveType = PrimitiveType.LineList;
            clusterMarker.Transformation = Matrix.Identity;
            clusterMarker.Geomentry.VertexBuffer = vb;
            clusterMarker.Geomentry.VertexCount = vtxCount;
            clusterMarker.Geomentry.VertexDeclaration = new VertexDeclaration(device, D3DX.DeclaratorFromFVF(VertexPC.Format));
            clusterMarker.Geomentry.VertexSize = sizeof(VertexPC);

        }

        void AddClusterMarkerOperation()
        {
            ModelEffect effect;
            if (!effects.TryGetValue(string.Empty, out effect))
            {
                effects.Add(string.Empty, null);
            }

            FastList<RenderOperation> opList;
            if (!batchTable.TryGetValue(string.Empty, out opList))
            {
                opList = new FastList<RenderOperation>();
                batchTable.Add(string.Empty, opList);
            }
            opList.Add(clusterMarker);

        }
        void AddSpNodeMarkerOperation(RenderOperation op)
        {
            ModelEffect effect;
            if (!effects.TryGetValue(string.Empty, out effect))
            {
                effects.Add(string.Empty, null);
            }

            FastList<RenderOperation> opList;
            if (!batchTable.TryGetValue(string.Empty, out opList))
            {
                opList = new FastList<RenderOperation>();
                batchTable.Add(string.Empty, opList);
            }
            opList.Add(op);

        }

        public Matrix ClusterMarkerTransform
        {
            get { return clusterMarker.Transformation; }
            set { clusterMarker.Transformation = value; }
        }

        public unsafe EditableGameScene(WorldDesigner wb, EditableSceneData data, Camera cam)
            : base(GraphicsDevice.Instance.Device, data)
        {
            this.worldBuilder = wb;
            this.TrafficConponments = new List<ITrafficComponment>();

            this.edData = data;
            int size = (data.SceneSize - 1) / Cluster.ClusterLength;
            EditableCluster[] clusters = new EditableCluster[size * size];

            for (int x = 0; x < size; x++)
            {
                for (int y = 0; y < size; y++)
                {
                    clusters[x * size + y] = new EditableCluster(null, x, y, data.CellUnit);
                }
            }

            clusterTable = new EditableClusterTable(clusters);



            float invCellUnit = 1.0f / CellUnit;

            SceneObject[] objs = data.SceneObjects;
            for (int i = 0; i < objs.Length; i++)
            {
                int cx = (int)(objs[i].OffsetX * invCellUnit);
                int cy = (int)(objs[i].OffsetZ * invCellUnit);

                ClusterDescription desc = new ClusterDescription(cx, cy);

                EditableCluster cluster = clusterTable.TryGetCluster(desc);

                if (cluster != null)
                {
                    cluster.SceneManager.AddObjectToScene(objs[i]);
                    if (objs[i] is Road)
                    {
                        EditableTrack trackLine = new EditableTrack(wb, this, cluster, (Road)objs[i]);
                        NotifyTrackAdded(trackLine);
                    }
                    if (objs[i] is ITrafficComponment)
                    {
                        NotifyTrafficConAdded((ITrafficComponment)objs[i]);
                    }
                }
            }


            spNodeMat = new MeshMaterial(GraphicsDevice.Instance.Device);
            spNodeMat.Ambient = new Color4(1, 0, 0);


            BuildClusterMarker();

            RegisterCamera(cam);

            PostRenderer = new NoPost();
        }

        protected override void PreRender()
        {
            base.PreRender();

            AddClusterMarkerOperation();
        }

        protected override void PrepareVisibleClusters(ICamera cam)
        {
            visibleClusters.FastClear();

            Frustum frus = cam.Frustum;
            Vector3 camPos = cam.Position;

            float invCellUnit = 1.0f / CellUnit;

            int esx = (int)((camPos.X - cam.FarPlane) * invCellUnit);
            int esy = (int)((camPos.Z - cam.FarPlane) * invCellUnit);

            // 截断到Cluster的第一顶点处
            if (esx < Terrain.TerrainSize)
            {
                esx = 0;
            }
            else
            {
                esx = Cluster.ClusterLength * ((esx - 1) / Cluster.ClusterLength) + 1;
            }

            if (esy < Terrain.TerrainSize)
            {
                esy = 0;
            }
            else
            {
                esy = Cluster.ClusterLength * ((esy - 1) / Cluster.ClusterLength) + 1;
            }


            //float a = 1.5f * cam.FarPlane / (float)Cluster.ClusterLength;
            float enumLength = cam.FarPlane * invCellUnit * 2 + Cluster.ClusterLength;
            //int enumLength = (int)a;
            //float decs = enumLength - a;

            //if (decs < -float.Epsilon)
            //{
            //    enumLength++;
            //}

            for (int x = esx; x < esx + enumLength; x += Terrain.TerrainSize)
            {
                for (int y = esy; y < esy + enumLength; y += Terrain.TerrainSize)
                {
                    ClusterDescription desc = new ClusterDescription(x, y);

                    EditableCluster cluster = clusterTable.TryGetCluster(desc);

                    if (cluster != null)
                    {
                        if (frus.IntersectsSphere(cluster.BoundingVolume))
                        {
                            visibleClusters.Add(cluster);
                        }
                    }
                }
            }
        }

        public override SceneObject FindObject(LineSegment ray)
        {
            return FindObject(ray, null);
        }
        public SceneObject FindObject(LineSegment ray, IObjectFilter callback)
        {
            Vector3 direction = ray.End - ray.Start;
            direction.Normalize();

            float cos = Vector3.Dot(Vector3.UnitY, direction);

            Vector3 insectPt = ray.Start + direction * (ray.Start.Y / cos);

            float dist = Vector3.Distance(direction, insectPt);

            float invCellUnit = 1.0f / CellUnit;
            int esx = (int)((ray.Start.X - dist) * invCellUnit);
            int esy = (int)((ray.Start.Z - dist) * invCellUnit);


            // 截断到Cluster的第一顶点处
            if (esx < Terrain.TerrainSize)
            {
                esx = 0;
            }
            else
            {
                esx = Cluster.ClusterLength * ((esx - 1) / Cluster.ClusterLength) + 1;
            }

            if (esy < Terrain.TerrainSize)
            {
                esy = 0;
            }
            else
            {
                esy = Cluster.ClusterLength * ((esy - 1) / Cluster.ClusterLength) + 1;
            }
            float enumLength = dist * invCellUnit * 2 + Cluster.ClusterLength;

            float minDist = float.MaxValue;
            SceneObject result = null;

            Ray ra = new Ray(ray.Start, direction);

            for (int x = esx; x < esx + enumLength; x += Terrain.TerrainSize)
            {
                for (int y = esy; y < esy + enumLength; y += Terrain.TerrainSize)
                {
                    ClusterDescription desc = new ClusterDescription(x, y);

                    EditableCluster cluster = clusterTable.TryGetCluster(desc);

                    if (cluster != null)
                    {
                        SceneObject sceObj;

                        if (callback != null)
                        {
                            sceObj = cluster.SceneManager.FindObject(ra, callback);
                        }
                        else
                        {
                            sceObj = cluster.SceneManager.FindObject(ra);
                        }
                        
                        if (sceObj != null && !(sceObj is Road))
                        {
                            Vector3 pos = sceObj.BoundingSphere.Center;
                            pos.X += sceObj.OffsetX;
                            pos.Y += sceObj.OffsetY;
                            pos.Z += sceObj.OffsetZ;

                            float curDist = Vector3.Distance(pos, ray.Start);

                            if (curDist < minDist)
                            {
                                minDist = curDist;
                                result = sceObj;
                            }
                        }
                    }
                }
            }
            return result;
        }


        public List<EditableTrack> Tracks
        {
            get { return tracks; }
        }

        public List<ITrafficComponment> TrafficConponments
        {
            get;
            private set;
        }

        public void NotifyTrackAdded(EditableTrack track)
        {
            tracks.Add(track);
        }
        public void NotifyTrackRemoved(Road track)
        {
            for (int i = 0; i < tracks.Count; i++)
            {
                if (tracks[i].Track == track)
                {
                    tracks.RemoveAt(i);
                    return;
                }
            }
        }

        public void NotifyTrafficConAdded(ITrafficComponment con)
        {
            TrafficConponments.Add(con);
        }
        public void NotifyTrafficConRemoved(ITrafficComponment con)
        {
            TrafficConponments.Remove(con);
        }

    }
}
