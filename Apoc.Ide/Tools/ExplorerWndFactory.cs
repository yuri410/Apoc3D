using System;
using System.Collections.Generic;
using System.Text;

namespace VirtualBicycle.Ide.Tools
{
    public class ExplorerWndFactory : IToolAbstractFactory
    {
        static ExplorerWindow singleton;

        #region IToolAbstractFactory 成员

        public ITool CreateInstance()
        {
            if (singleton == null)
            {
                singleton = new ExplorerWindow();
            }
            return singleton;
        }

        public Type CreationType
        {
            get { return typeof(ExplorerWindow); }
        }

        #endregion
    }
}
