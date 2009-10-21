using System;
using System.Collections.Generic;
using System.Text;

namespace Apoc
{
    /// <summary>
    ///  主循环可更新
    /// </summary>
    public interface IUpdatable
    {
        /// <summary>
        ///  主循环更新对象的状态
        /// </summary>
        /// <param name="dt">
        /// 时间间隔（通常较短）
        /// </param>
        void Update(float dt);
    }
}
