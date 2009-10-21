using System;
using System.Collections.Generic;
using System.Text;

namespace Apoc.RenderSystem
{
    /// <summary>
    /// 	This class declares the usage of a single vertex buffer as a component
    /// 	of a complete <see cref="VertexDeclaration"/>. 
    /// </summary>
    public struct VertexElement
    {
        #region Fields

        ///// <summary>
        /////     The source vertex buffer, as bound to an index using <see cref="VertexBufferBinding"/>.
        ///// </summary>
        //protected short source;
        /// <summary>
        ///     The offset in the buffer that this element starts at.
        /// </summary>
        int offset;
        /// <summary>
        ///     The type of element.
        /// </summary>
        VertexElementFormat type;
        /// <summary>
        ///     The meaning of the element.
        /// </summary>
        VertexElementUsage semantic;
        /// <summary>
        ///     Index of the item, only applicable for some elements like texture coords.
        /// </summary>
        int index;

        #endregion Fields

        #region Constructors

        /// <summary>
        ///     Constructor.
        /// </summary>
        ///// <param name="source">The source vertex buffer, as bound to an index using <see cref="VertexBufferBinding"/>.</param>
        /// <param name="offset">The offset in the buffer that this element starts at.</param>
        /// <param name="type">The type of element.</param>
        /// <param name="semantic">The meaning of the element.</param>
        public VertexElement(int offset, VertexElementFormat type, VertexElementUsage semantic)
            : this(offset, type, semantic, 0)
        {
        }

        /// <summary>
        ///     Constructor.
        /// </summary>
        ///// <param name="source">The source vertex buffer, as bound to an index using <see cref="VertexBufferBinding"/>.</param>
        /// <param name="offset">The offset in the buffer that this element starts at.</param>
        /// <param name="type">The type of element.</param>
        /// <param name="semantic">The meaning of the element.</param>
        /// <param name="index">Index of the item, only applicable for some elements like texture coords.</param>
        public VertexElement(int offset, VertexElementFormat type, VertexElementUsage semantic, int index)
        {
            //this.source = source;
            this.offset = offset;
            this.type = type;
            this.semantic = semantic;
            this.index = index;
        }

        #endregion

        #region Methods

        /// <summary>
        ///     Utility method for helping to calculate offsets.
        /// </summary>
        public static int GetTypeSize(VertexElementFormat type)
        {

            switch (type)
            {
                case VertexElementFormat.Rg32:
                case VertexElementFormat.Rgba32:
                case VertexElementFormat.Color:
                    return sizeof(int);

                case VertexElementFormat.Single:
                    return sizeof(float);

                case VertexElementFormat.Vector2:
                    return sizeof(float) * 2;

                case VertexElementFormat.Vector3:
                    return sizeof(float) * 3;

                case VertexElementFormat.Vector4:
                    return sizeof(float) * 4;

                case VertexElementFormat.NormalizedShort2:
                case VertexElementFormat.Short2:
                    return sizeof(short) * 2;

                case VertexElementFormat.NormalizedShort4:
                case VertexElementFormat.Short4:
                    return sizeof(short) * 4;

                case VertexElementFormat.Byte4:
                    return sizeof(byte) * 4;
            } // end switch

            // keep the compiler happy
            return 0;
        }

        /// <summary>
        ///     Utility method which returns the count of values in a given type.
        /// </summary>
        public static int GetTypeCount(VertexElementFormat type)
        {
            switch (type)
            {
                case VertexElementFormat.Rg32:
                case VertexElementFormat.Rgba32:
                case VertexElementFormat.Color:
                    return 1;

                case VertexElementFormat.Single:
                    return 1;

                case VertexElementFormat.Vector2:
                    return 2;

                case VertexElementFormat.Vector3:
                    return 3;

                case VertexElementFormat.Vector4:
                    return 4;

                case VertexElementFormat.NormalizedShort2:
                case VertexElementFormat.Short2:
                    return 2;

                case VertexElementFormat.NormalizedShort4:
                case VertexElementFormat.Short4:
                    return 4;

                case VertexElementFormat.Byte4:
                    return 4;
            } // end switch			

            // keep the compiler happy
            return 0;
        }

        /// <summary>
        ///		Returns proper enum for a base type multiplied by a value.  This is helpful
        ///		when working with tex coords especially since you might not know the number
        ///		of texture dimensions at runtime, and when creating the VertexBuffer you will
        ///		have to get a VertexElementType based on that amount to creating the VertexElement.
        /// </summary>
        /// <param name="type">Data type.</param>
        /// <param name="count">Multiplier.</param>
        /// <returns>
        ///     A <see cref="VertexElementType"/> that represents the requested type and count.
        /// </returns>
        /// <example>
        ///     MultiplyTypeCount(VertexElementType.Float1, 3) returns VertexElementType.Float3.
        /// </example>
        public static VertexElementFormat MultiplyTypeCount(VertexElementFormat type, int count)
        {
            switch (type)
            {
                case VertexElementFormat.Single:
                    switch (count)
                    {
                        case 1:
                            return VertexElementFormat.Single;
                        case 2:
                            return VertexElementFormat.Vector2;
                        case 3:
                            return VertexElementFormat.Vector3;
                        case 4:
                            return VertexElementFormat.Vector4;
                    }
                    break;

                //case VertexElementFormat.Short1:
                //    switch (count)
                //    {
                //        case 1:
                //            return VertexElementType.Short1;
                //        case 2:
                //            return VertexElementType.Short2;
                //        case 3:
                //            return VertexElementType.Short3;
                //        case 4:
                //            return VertexElementType.Short4;
                //    }
                //    break;
            }

            throw new Exception("Cannot multiply base vertex element type: " + type.ToString());
        }

        #endregion

        #region Properties

        ///// <summary>
        ///// 
        ///// </summary>
        //public short Source
        //{
        //    get
        //    {
        //        return source;
        //    }
        //}

        /// <summary>
        ///     Gets the offset into the buffer where this element starts.
        /// </summary>
        public int Offset
        {
            get { return offset; }
        }

        /// <summary>
        ///     Gets the data format of this element.
        /// </summary>
        public VertexElementFormat Type
        {
            get { return type; }
        }

        /// <summary>
        ///     Gets the meaning of this element.
        /// </summary>
        public VertexElementUsage Semantic
        {
            get { return semantic; }
        }

        /// <summary>
        ///     Gets the index of this element, only applicable for repeating elements (like texcoords).
        /// </summary>
        public int Index
        {
            get { return index; }
        }

        /// <summary>
        ///     Gets the size of this element in bytes.
        /// </summary>
        public int Size
        {
            get { return GetTypeSize(type); }
        }

        #endregion

        public override bool Equals(object obj)
        {
            if (obj is VertexElement)
            {
                return (VertexElement)obj == this;
            }
            return false;
        }
         

        public static bool operator ==(VertexElement left, VertexElement right)
        {
            return left.index == right.index &&
                left.offset == right.offset &&
                left.semantic == right.semantic &&
                //left.source == right.source &&
                left.type == right.type;
        }

        public static bool operator !=(VertexElement left, VertexElement right)
        {
            return left.index != right.index ||
                left.offset != right.offset ||
                left.semantic != right.semantic ||
                //left.source != right.source ||
                left.type != right.type;
        }

        public override int GetHashCode()
        {
            return index.GetHashCode() + offset.GetHashCode() + semantic.GetHashCode() + type.GetHashCode();
        }
    }
}
