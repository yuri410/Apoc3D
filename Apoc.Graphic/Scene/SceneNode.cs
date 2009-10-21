using System;
using System.Collections.Generic;
using System.Text;
using Apoc.Collections;

namespace Apoc.Scene
{
    /// <summary>
    ///  场景节点。具有几何意义。
    /// </summary>
    public abstract class SceneNodeBase
    {
        protected SceneNodeBase()
        {
            AttchedObjects = new FastList<SceneObject>(8);
        }

        /// <summary>
        ///  获取一个列表，包含附上的场景物体
        /// </summary>
        public FastList<SceneObject> AttchedObjects
        {
            get;
            protected set;
        }

        #region 方法

        /// <summary>
        ///  给该节点附加场景物体
        /// </summary>
        /// <param name="obj"></param>
        public abstract void AddObject(SceneObject obj);

        /// <summary>
        ///  从该节点移除场景物体
        /// </summary>
        /// <param name="obj"></param>
        public abstract void RemoveObject(SceneObject obj);

        public virtual void Update() { }

        [Obsolete()]
        protected abstract void Eject(SceneObject sceneObj);

        #endregion
    }


    public class SceneNode : SceneNodeBase
    {
        #region 字段

        protected List<SceneNode> children;
        protected SceneNode parent;

        #endregion

        public SceneNode(SceneManager mgr, SceneNode parent)
        {
            children = new List<SceneNode>(8);

            Manager = mgr;
            this.parent = parent;
        }

        #region 属性

        public SceneManager Manager
        {
            get;
            private set;
        }
        public int ChildrenCount
        {
            get { return children.Count; }
        }

        #endregion

        #region 方法

        public override void AddObject(SceneObject obj)
        {
            // the default scene node simply add the object to itself.
            AttchedObjects.Add(obj);
            obj.ParentSceneNode = this;
        }

        public override void RemoveObject(SceneObject obj)
        {
            AttchedObjects.Remove(obj);
            obj.ParentSceneNode = null;
        }

        [Obsolete()]
        protected override void Eject(SceneObject sceneObj)
        {
            throw new NotSupportedException();
        }

        #endregion

    }
}
