using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Apoc
{
    public class LogManager
    {
        static LogManager singleton;

        public static LogManager Instance 
        {
            get 
            {
                return singleton;
            }
        }
    }
}
