using System;
using System.Collections.Generic;
using System.Text;
using Apoc.Graphics.Animation;

namespace Apoc.Collections
{
    public class EntityNodeCollection : CollectionBase<EntityNode>, ICollection<EntityNode>
    {
        public EntityNodeCollection(EntityNode[] items)
        {
            base.Add(items);
        }

        #region ICollection <EntityNode> 成员

        public new void Add(EntityNode item)
        {
            base.Add(item);
        }

        public new void Clear()
        {
            base.Clear();
        }

        public new bool Contains(EntityNode item)
        {
            return base.Contains(item);
        }

        public bool IsReadOnly
        {
            get { return false; }
        }

        public new bool Remove(EntityNode item)
        {
            return base.Remove(item);
        }

        #endregion

    }
}
