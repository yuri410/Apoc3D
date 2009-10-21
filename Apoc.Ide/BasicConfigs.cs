using System;
using System.Collections.Generic;
using System.Text;

namespace VirtualBicycle.Ide
{
    [Serializable()]
    public class BasicConfigs
    {
        string gamePath = string.Empty;
        //string ra2yrPath;
        public string GamePath
        {
            get { return gamePath; }
            set { gamePath = value; }
        }
        //public string RA2YRPath
        //{
        //    get { return ra2yrPath; }
        //    set { ra2yrPath = value; }
        //}
    }
}
