using System;
using System.Collections.Generic;
using System.Text;

namespace Apoc.Graphics.Geometry
{
    /// <summary>
    /// A partial disk geometry primitive constructed with PrimitiveMesh
    /// </summary>
    public class PartialDisk : Disk
    {
        /// <summary>
        /// Creates a partial disk on the Y = 0 plane. A partial disk is similar to a 
        /// full disk, except that only the subset of the disk from start through start + sweep is
        /// included (where 0 degrees is along the +Y axis, 90 degrees along the +X axis)
        /// </summary>
        /// <param name="inner">Specifies the inner radius of the partial disk (can be 0).</param>
        /// <param name="outer">Specifies the outer radius of the partial disk. Must be greater
        /// than the 'inner' radius.</param>
        /// <param name="slices">Specifies the number of subdivisions around the Z axis. Must be
        /// greater than 2.</param>
        /// <param name="start">Specifies the starting angle, in radians, of the disk portion.</param>
        /// <param name="sweep">Specifies the sweep angle, in radians, of the disk portion.</param>
        /// /// <param name="twoSided">Specifies whether to render both front and back side</param>
        public PartialDisk(float inner, float outer, int slices, double start, double sweep, bool twoSided)
            : base(inner, outer, slices, start, sweep, twoSided)
        {
        }
    }
}
