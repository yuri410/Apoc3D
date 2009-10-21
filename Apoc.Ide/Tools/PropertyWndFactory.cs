using System;
using System.Collections.Generic;
using System.Text;

namespace VirtualBicycle.Ide.Tools
{
    public class PropertyWndFactory : IToolAbstractFactory
    {
        static PropertyWindow singleton;

        #region IToolAbstractFactory 成员

        public ITool CreateInstance()
        {
            if (singleton == null) 
            {
                singleton = new PropertyWindow();
            }
            return singleton;
        }

        public Type CreationType
        {
            get { return typeof(PropertyWindow); }
        }

        #endregion
    }
}
