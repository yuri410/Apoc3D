using System;
using System.Collections.Generic;
using System.Text;

namespace Apoc.Scene
{
    /// <summary>
    ///  场景查找物体回调
    /// </summary>
    public interface IObjectFilter
    {
        /// <summary>
        ///  检查物体
        ///  对于节点下的物体，引擎会调用这个函数检查物体是否符合条件。当返回true时表明条件符合。
        /// </summary>
        /// <param name="obj">要检查的物体</param>
        /// <returns>布尔值，true时表明条件符合</returns>
        bool Check(SceneObject obj);

        /// <summary>
        ///  检查节点
        ///  引擎会调用这个函数检查节点是否符合条件。当返回true时表明条件符合。
        /// </summary>
        /// <param name="node">要检查的节点</param>
        /// <returns>布尔值，true时表明条件符合</returns>
        bool Check(OctreeSceneNode node);
    }
}
