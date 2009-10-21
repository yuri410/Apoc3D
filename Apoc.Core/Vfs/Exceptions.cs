using System;
using System.Collections.Generic;
using System.Text;

namespace Apoc.Vfs
{

    [Serializable]
    public class InvalidFileFormatException : Exception
    {

        public InvalidFileFormatException(string desc) : base("" + desc) { }
        public InvalidFileFormatException(ResourceLocation rl)
            : this(rl.ToString()) { }
    }
}
