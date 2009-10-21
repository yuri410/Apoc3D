using System;
using System.Collections.Generic;
using System.Text;

namespace Apoc.MathLib
{
    public interface IPackedVector
    {
        // Methods
        void PackFromVector4(Vector4 vector);
        Vector4 ToVector4();
    }

    public interface IPackedVector<TPacked> : IPackedVector
    {
        // Properties
        TPacked PackedValue { get; set; }
    }

}
