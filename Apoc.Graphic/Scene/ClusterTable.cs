using System;
using System.Collections.Generic;
using System.Text;

namespace Apoc.Scene
{
    /// <summary>
    ///  可以快速查询Cluster的哈希表
    /// </summary>
    public class ClusterTable : ClusterTableBase<Cluster>
    {
        public ClusterTable(Cluster[] clusters)
            : base(clusters)
        {
        }
    }
}
