using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Text;
using SlimDX;
using SlimDX.Direct3D9;
using Apoc.Collections;
using Apoc.Config;
using Apoc.Graphics;
using Apoc.Graphics.Effects;
using Apoc.Vfs;
using Apoc.MathLib;

namespace Apoc.Scene
{
    /// <summary>
    ///  游戏中的场景
    /// </summary>
    public unsafe class GameScene : GameSceneBase<Cluster, SceneData>
    {
        ClusterTable clusterTable;

        /// <summary>
        ///  获取游戏场景的ClusterTable，包含场景所有Cluster
        /// </summary>
        public ClusterTable ClusterTable
        {
            get { return clusterTable; }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="device">d3d设备</param>
        /// <param name="data">构建场景的数据</param>
        /// <param name="cbk">进度指示回调</param>
        public GameScene(Device device, SceneData data, ProgressCallBack cbk)
            : base(device, data)
        {
            int size = (data.SceneSize - 1) / Cluster.ClusterLength;

            SceneObject[] objs = data.SceneObjects;

            int total = 2 * size * size + objs.Length;
            int current = 0;

            Cluster[] clusters = new Cluster[size * size];

            for (int x = 0; x < size; x++)
            {
                for (int y = 0; y < size; y++)
                {
                    clusters[x * size + y] = new Cluster(this, device, x, y, data.CellUnit);

                    if (cbk != null)
                    {                        
                        cbk(current, total);
                        current += 2;
                    }
                }
            }

            clusterTable = new ClusterTable(clusters);


            float invCellUnit = 1.0f / CellUnit;

            for (int i = 0; i < objs.Length; i++)
            {
                int cx = (int)(objs[i].OffsetX * invCellUnit);
                int cy = (int)(objs[i].OffsetZ * invCellUnit);

                ClusterDescription desc = new ClusterDescription(cx, cy);

                Cluster cluster = clusterTable.TryGetCluster(desc);

                if (cluster != null)
                {
                    cluster.SceneManager.AddObjectToScene(objs[i]);
                }

                if (cbk != null)
                {
                    cbk(current, total);
                    current++;
                }
            }

        }

        /// <summary>
        ///  从世界坐标计算Cluster的坐标（地形坐标单位）
        /// </summary>
        /// <param name="x"></param>
        /// <param name="z"></param>
        /// <param name="cx"></param>
        /// <param name="cy"></param>
        public void GetClusterCoord(float x, float z, out int cx, out int cy)
        {
            float invCellUnit = 1.0f / CellUnit;

            int esx = (int)(x * invCellUnit);
            int esy = (int)(z * invCellUnit);

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
            cx = esx;
            cy = esy;
        }

        /// <summary>
        ///  检测游戏场景中所有可见的Cluster
        /// </summary>
        /// <param name="cam">目标摄像机</param>
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

                    Cluster cluster = clusterTable.TryGetCluster(desc);

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

        /// <summary>
        ///  查找和线段相交的物体
        /// </summary>
        /// <param name="ray"></param>
        /// <returns></returns>
        public override SceneObject FindObject(LineSegment ray)
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

                    Cluster cluster = clusterTable.TryGetCluster(desc);

                    if (cluster != null)
                    {
                        SceneObject sceObj = cluster.SceneManager.FindObject(ra);
                        if (sceObj != null)
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

        /// <summary>
        ///  释放使用的所有资源。重写的方法应调用基类中的方法。
        /// </summary>
        /// <param name="disposing">表示是否需要释放该物体所引用的其他资源，当为真时，调用那些资源的Dispose方法</param>      
        protected override void Dispose(bool disposing)
        {
            base.Dispose(disposing);

            if (disposing) 
            {
                foreach (Cluster c in clusterTable)
                {
                    c.Dispose();
                }
            }
            clusterTable = null;
        }

    }
}
