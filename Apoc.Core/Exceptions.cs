using System;
using System.Collections.Generic;
using System.Text;

namespace Apoc
{
    [Serializable]
    public class InvalidFormatException : Exception
    {
        public InvalidFormatException() { }
        public InvalidFormatException(string message) : base(message) { }
        public InvalidFormatException(string message, Exception inner) : base(message, inner) { }
        protected InvalidFormatException(
          System.Runtime.Serialization.SerializationInfo info,
          System.Runtime.Serialization.StreamingContext context)
            : base(info, context) { }
    }

    [Serializable]
    public class DataFormatException : Exception
    {

        public DataFormatException() { }
        public DataFormatException(string message) : base(message) { }
        public DataFormatException(string message, Exception inner) : base(message, inner) { }
        protected DataFormatException(
          System.Runtime.Serialization.SerializationInfo info,
          System.Runtime.Serialization.StreamingContext context)
            : base(info, context) { }
    }
}
