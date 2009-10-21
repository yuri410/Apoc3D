using System;
using System.Collections.Generic;
using System.Text;

namespace Apoc.MathLib
{
    /// <summary>
    ///  定义线段
    /// </summary>
    public struct LineSegment
    {
        public LineSegment(Vector3 start, Vector3 end)
        {
            this.Start = start;
            this.End = end;
        }

        /// <summary>
        ///  获取或设置线段起点
        /// </summary>
        public Vector3 Start;
        /// <summary>
        ///  获取或设置线段终点
        /// </summary>
        public Vector3 End;

        /// <summary>
        ///  获得线段的System.String表达形式
        /// </summary>
        /// <returns>线段的System.String表达形式</returns>
        public override string ToString()
        {
            return "Start:" + Start.ToString() + " End:" + End.ToString();
        }
    }
}
