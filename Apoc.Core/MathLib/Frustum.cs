using System;
using System.Collections.Generic;
using System.Text;

namespace Apoc.MathLib
{
    /// <summary>
    ///  定义视见体裁减平面
    /// </summary>
    public enum FrustumPlane : int
    {
        /// <summary>
        ///  右裁减平面
        /// </summary>
        Right = 0,
        /// <summary>
        ///  左裁减平面
        /// </summary>
        Left = 1,
        /// <summary>
        ///  下裁减平面
        /// </summary>
        Bottom = 2,
        /// <summary>
        ///  上裁减平面
        /// </summary>
        Top = 3,
        /// <summary>
        ///  远裁减平面
        /// </summary>
        Far = 4,
        /// <summary>
        ///  近裁减平面
        /// </summary>
        Near = 5
    }

    /// <summary>
    ///  定义视见体，视见体由六个平面围成。实现提供相交检测。
    /// </summary>
    public class Frustum
    {
        Plane[] planes = new Plane[6];
        //float[] p = new float[16], mv = new float[16];

        internal Matrix proj;
        internal Matrix view;

        /// <summary>
        ///  获取或设置相机投影矩阵
        /// </summary>
        public Matrix Projection
        {
            get { return proj; }
            set { proj = value; }
        }

        /// <summary>
        ///  获取或设置相机查看矩阵
        /// </summary>
        public Matrix View
        {
            get { return view; }
            set { view = value; }
        }

        /// <summary>
        ///  获得视见体的裁减平面
        /// </summary>
        /// <param name="fp"></param>
        /// <param name="pl"></param>
        public void GetPlane(FrustumPlane fp, out Plane pl)
        {
            pl = this.planes[(int)fp];
        }

        /// <summary>
        ///  更新视见体的六个平面
        /// </summary>
        public void Update()
        {
            Matrix mvp = view * proj;

            // 右裁减平面
            planes[0].Normal = new Vector3(mvp.M14 - mvp.M11, mvp.M24 - mvp.M21, mvp.M34 - mvp.M31);
            planes[0].D = mvp.M44 - mvp.M41;
            planes[0].Normalize();

            // 左裁减平面
            planes[1].Normal = new Vector3(mvp.M14 + mvp.M11, mvp.M24 + mvp.M21, mvp.M34 + mvp.M31);
            planes[1].D = mvp.M44 + mvp.M41;
            planes[1].Normalize();

            // 下裁减平面
            planes[2].Normal = new Vector3(mvp.M14 + mvp.M12, mvp.M24 + mvp.M22, mvp.M34 + mvp.M32);
            planes[2].D = mvp.M44 + mvp.M42;
            planes[2].Normalize();

            // 上裁减平面
            planes[3].Normal = new Vector3(mvp.M14 - mvp.M12, mvp.M24 - mvp.M22, mvp.M34 - mvp.M32);
            planes[3].D = mvp.M44 - mvp.M42;
            planes[3].Normalize();

            // 远裁减平面
            planes[4].Normal = new Vector3(mvp.M14 - mvp.M13, mvp.M24 - mvp.M23, mvp.M34 - mvp.M33);
            planes[4].D = mvp.M44 - mvp.M43;
            planes[4].Normalize();

            // 近裁减平面
            planes[5].Normal = new Vector3(mvp.M14 + mvp.M13, mvp.M24 + mvp.M23, mvp.M34 + mvp.M33);
            planes[5].D = mvp.M44 + mvp.M43;
            planes[5].Normalize();
        }

        /// <summary>
        ///  判断球是否和视见体相交
        /// </summary>
        /// <param name="bs"></param>
        /// <returns></returns>
        public bool IntersectsSphere(BoundingSphere bs)
        {
            for (int i = 0; i < 6; i++)
            {
                //已经normalize，不用sqrt
                if (planes[i].Normal.X * bs.Center.X + planes[i].Normal.Y * bs.Center.Y + planes[i].Normal.Z * bs.Center.Z + planes[i].D <= -bs.Radius)
                    return false;
            }
            return true;
        }

        public bool IntersectsSphere(ref Vector2 c, float r)
        {
            if (planes[0].Normal.X * c.X + planes[0].Normal.Z * c.Y + planes[0].D <= -r)
                return false;
            if (planes[1].Normal.X * c.X + planes[1].Normal.Z * c.Y + planes[1].D <= -r)
                return false;
            if (planes[4].Normal.X * c.X + planes[4].Normal.Z * c.Y + planes[4].D <= -r)
                return false;
            if (planes[5].Normal.X * c.X + planes[5].Normal.Z * c.Y + planes[5].D <= -r)
                return false;

            return true;
        }

        /// <summary>
        ///  判断球是否和视见体相交
        /// </summary>
        /// <param name="c"></param>
        /// <param name="r"></param>
        /// <returns></returns>
        public bool IntersectsSphere(Vector3 c, float r)
        {
            for (int i = 0; i < 6; i++)
            {
                //已经normalize，不用sqrt
                if (planes[i].Normal.X * c.X + planes[i].Normal.Y * c.Y + planes[i].Normal.Z * c.Z + planes[i].D <= -r)
                    return false;
            }
            return true;
        }

        /// <summary>
        ///  判断球是否和视见体相交
        /// </summary>
        /// <param name="c"></param>
        /// <param name="r"></param>
        /// <returns></returns>
        public bool IntersectsSphere(ref Vector3 c, float r)
        {
            for (int i = 0; i < 6; i++)
            {
                //已经normalize，不用sqrt
                if (planes[i].Normal.X * c.X + planes[i].Normal.Y * c.Y + planes[i].Normal.Z * c.Z + planes[i].D <= -r)
                    return false;
            }
            return true;
        }

        /// <summary>
        ///  判断点是否在视见体内
        /// </summary>
        /// <param name="c"></param>
        /// <returns></returns>
        public bool IsPointIn(Vector3 c)
        {
            for (int i = 0; i < 6; i++)
            {
                //已经normalize，不用sqrt
                if (planes[i].Normal.X * c.X + planes[i].Normal.Y * c.Y + planes[i].Normal.Z * c.Z + planes[i].D < 0)
                    return false;
            }
            return true;
        }
    }
}
