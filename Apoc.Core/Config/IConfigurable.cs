using System;
using System.Collections.Generic;
using System.Text;

namespace Apoc.Config
{
    /// <summary>
    ///  定义一种 从ConfigurationSection获取配置信息并应用设置的方式
    /// </summary>
    public interface IConfigurable
    {
        /// <summary>
        ///  从配置块中读取设置
        /// </summary>
        /// <param name="sect"></param>
        void Parse(ConfigurationSection sect);

    }
}
