using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using Apoc.Graphics;
using Apoc.Graphics.Effects;
using Apoc.Vfs;
using Apoc.MathLib;

namespace Apoc.Scene
{
    /// <summary>
    ///  表示Cluster。场景分为若干Cluster
    /// </summary>
    public class Cluster : IDisposable, IUpdatable
    {
        #region 常量

        /// <summary>
        ///  表示Cluster的长度（512）
        /// </summary>
        public const int ClusterLength = Terrain.TerrainSize - 1;

        #endregion

        #region 字段

        /// <summary>
        ///  【见属性】
        /// </summary>
        protected ClusterDescription description;

        /// <summary>
        ///  保存D3D的Device对象引用
        /// </summary>
        protected Device device;

        /// <summary>
        ///  Cluster的包围球
        /// </summary>
        protected BoundingSphere boudingSphere;

        /// <summary>
        ///  这个Cluster的场景管理器
        /// </summary>
        protected OctreeSceneManager sceneMgr;

        #endregion

        #region 构造函数
        /// <summary>
        ///  构造一个新的<see cref="Cluster"/>对象
        /// </summary>
        /// <param name="x">以地形单位为单位</param>
        /// <param name="y">以地形单位为单位</param>
        public Cluster(GameScene scene, Device device, int x, int y, float cellUnit)
        {
            this.description.X = x;
            this.description.Y = y;

            this.device = device;
            this.CellUnit = cellUnit;

            this.boudingSphere.Center = new Vector3(
                (x + ClusterLength * 0.5f) * cellUnit,
                0,
                (y + ClusterLength * 0.5f) * cellUnit);
            this.boudingSphere.Radius = (float)ClusterLength * cellUnit * 0.5f * MathEx.Root2;

            this.WorldX = x == 0 ? 0 : (x - 1) * cellUnit;
            this.WorldZ = y == 0 ? 0 : (y - 1) * cellUnit;

            this.sceneMgr = new OctreeSceneManager(this, device, new OctreeBox(ClusterLength, 0), 10f);
            this.GameScene = scene;
        }

        #endregion

        /// <summary>
        ///  当有一物体离开Cluster时被引擎调用
        /// </summary>
        /// <param name="obj">离开的物体</param>
        /// <returns>如果Cluster有可以接受这个物体的Cluster，并成功加入了这个物体，返回true。否则为false。</returns>
        public bool NotifyObjectLeaved(SceneObject obj)
        {
            float ox = obj.OffsetX + obj.BoundingSphere.Center.X;
            float oy = obj.OffsetY + obj.BoundingSphere.Center.Y;

            int cx;
            int cy;

            GameScene.GetClusterCoord(ox, oy, out cx, out cy);            

            ClusterDescription desc = new ClusterDescription(cx, cy);

            if (desc == this.description)
            {
                return false;
            }

            Cluster neightbour = GameScene.ClusterTable[desc];

            if (neightbour != null)             
            {
                neightbour.NotifyObjectEntered(obj);
                return true;
            }
            return false;
        }

        /// <summary>
        ///  当有一个物体进入Cluster是被引擎调用
        /// </summary>
        /// <param name="obj">进入的物体</param>
        public void NotifyObjectEntered(SceneObject obj)
        {
            this.sceneMgr.AddObjectToScene(obj);
        }

        #region 属性

        /// <summary>
        ///  获取该Cluster所在的Game Scene对象
        /// </summary>
        public GameScene GameScene
        {
            get;
            private set;
        }

        /// <summary>
        ///  获取一个浮点数，表示地形单元格的长度
        /// </summary>
        public float CellUnit
        {
            get;
            protected set;
        }

        /// <summary>
        ///  获取该Cluster的包围球
        /// </summary>
        public BoundingSphere BoundingVolume
        {
            get { return boudingSphere; }
        }

        /// <summary>
        ///  获取该Cluster的ClusterDescription
        /// </summary>
        public ClusterDescription Description
        {
            get { return this.description; }
        }

        /// <summary>
        ///  获取该Cluster中的场景管理器
        /// </summary>
        public SceneManagerBase SceneManager
        {
            get { return sceneMgr; }
        }

        /// <summary>
        ///  获取这个Cluster的在世界坐标系中的X坐标
        /// </summary>
        public float WorldX
        {
            get;
            protected set;
        }

        /// <summary>
        ///  获取这个Cluster的在世界坐标系中的Y坐标
        /// </summary>
        public float WorldY
        {
            get;
            protected set;
        }

        /// <summary>
        ///  获取这个Cluster的在世界坐标系中的Z坐标
        /// </summary>
        public float WorldZ
        {
            get;
            protected set;
        }

        #endregion

        #region IUpdatable 成员

        /// <summary>
        ///  更新Cluster的状态，每一帧如果可见，则被引擎调用
        /// </summary>
        /// <param name="dt">帧时间间隔，以秒为单位</param>
        public void Update(float dt)
        {
            sceneMgr.Update(dt);
        }

        #endregion

        #region IDisposable 成员

        /// <summary>
        ///  释放场景物体所使用的所有资源。应在派生类中重写。重写的方法应调用基类中的方法。
        /// </summary>
        /// <param name="disposing">表示是否需要释放该物体所引用的其他资源，当为真时，调用那些资源的Dispose方法</param>
        protected virtual void Dispose(bool disposing)
        {
            sceneMgr.Dispose();
        }

        /// <summary>
        ///  获取一个布尔值，表示该场景物体是否已经释放了资源。
        /// </summary>
        public bool Disposed
        {
            get;
            private set;
        }

        /// <summary>
        ///  释放场景物体所使用的所有资源。
        /// </summary>
        public void Dispose()
        {
            if (!Disposed)
            {
                Dispose(true);

                Disposed = true;
            }
            else
            {
                throw new ObjectDisposedException(ToString());
            }
        }

        #endregion

        ~Cluster()
        {
            if (!Disposed)
                Dispose(false);
        }
    }

    /// <summary>
    ///  包含一个Cluster的信息，用来在ClusterTable中查询Cluster
    /// </summary>
    public struct ClusterDescription
    {
        #region 字段

        /// <summary>
        /// Cluster的X坐标（地形坐标系）。以地形单位为单位
        /// </summary>
        public int X;

        /// <summary>
        /// 以地形单位为单位
        /// </summary>
        public int Y;

        #endregion

        #region 属性

        /// <summary>
        ///  获取Cluster的宽度。
        /// </summary>
        public int Width
        {
            get { return Cluster.ClusterLength; }
        }

        /// <summary>
        ///  获取Cluster的高度。
        /// </summary>
        public int Height
        {
            get { return Cluster.ClusterLength; }
        }

        #endregion

        /// <summary>
        ///  
        /// </summary>
        /// <param name="x">以地形单位为单位</param>
        /// <param name="y">以地形单位为单位</param>
        public ClusterDescription(int x, int y)
        {
            this.X = x;
            this.Y = y;
        }

        /// <summary>
        ///  
        /// </summary>
        /// <param name="obj"></param>
        /// <returns></returns>
        public override bool Equals(object obj)
        {
            if (obj is ClusterDescription)
            {
                ClusterDescription v = (ClusterDescription)obj;
                return v.X == this.X && v.Y == this.Y;
            }
            return false;
        }


        public static bool operator ==(ClusterDescription a, ClusterDescription b)
        {
            return a.X == b.X && a.Y == b.Y;
        }
        public static bool operator !=(ClusterDescription a, ClusterDescription b)
        {
            return a.X != b.X || a.Y != b.Y;
        }

        public override string ToString()
        {
            return string.Format("X: {0}, Y: {1}", X.ToString(), Y.ToString());
        }

        public override int GetHashCode()
        {
            return X.GetHashCode() + Y.GetHashCode();
        }
    }
}
