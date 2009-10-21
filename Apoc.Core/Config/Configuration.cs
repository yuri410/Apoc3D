using System;
using System.Collections.Generic;
using System.Text;

namespace Apoc.Config
{
    /// <summary>
    ///  提供配置的抽象接口
    /// </summary>
    public abstract class Configuration : Dictionary<string, ConfigurationSection>
    {
        string name;

        protected Configuration(string name, IEqualityComparer<string> comparer)
            : base(comparer)
        {
            this.name = name;
        }


        protected Configuration(string name, int cap, IEqualityComparer<string> comparer)
            : base(cap, comparer)
        {
            this.name = name;
        }

        public string Name
        {
            get { return name; }
        }

        /// <summary>
        ///  复制该配置，创建令一个相同的。其中的Section也会被复制，不会引用。
        /// </summary>
        /// <returns></returns>
        public abstract Configuration Clone();

        /// <summary>
        ///  将该配置与另一个配置合并
        /// </summary>
        /// <param name="config">另一个配置</param>
        public abstract void Merge(Configuration config);
    }
}
