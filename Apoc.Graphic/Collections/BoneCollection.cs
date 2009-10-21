using System;
using System.Collections.Generic;
using System.Text;
using Apoc.Graphics.Animation;

namespace Apoc.Collections
{
    public class BoneCollection : CollectionBase<Bone>, ICollection<Bone>
    {
        public BoneCollection(Bone[] bones)
        {
            base.Add(bones);
        }

        #region ICollection<Bone> 成员

        public new void Add(Bone item)
        {
            base.Add(item);
        }

        public new void Clear()
        {
            base.Clear();
        }

        public new bool Contains(Bone item)
        {
            return base.Contains(item);
        }

        public bool IsReadOnly
        {
            get { return false; }
        }

        public new bool Remove(Bone item)
        {
            return base.Remove(item);
        }

        #endregion

    }
}
