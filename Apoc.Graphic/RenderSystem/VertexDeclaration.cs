using System;
using System.Collections.Generic;
using System.Text;

namespace Apoc.RenderSystem
{

    public abstract class VertexDeclaration : IDisposable
    {
        protected VertexDeclaration(VertexElement[] elements)
        {
            for (int i = 0; i < elements.Length; i++)
            {
                this.elements.Add(elements[i]);
            }
        }

  

        #region Fields

        /// <summary>
        ///     List of elements that make up this declaration.
        /// </summary>
        protected FastList<VertexElement> elements = new FastList<VertexElement>();

        #endregion Fields

        #region Methods

        ///// <summary>
        /////     Adds a new VertexElement to this declaration.
        ///// </summary>
        ///// <remarks>
        /////     This method adds a single element (positions, normals etc) to the
        /////     vertex declaration. <b>Please read the information in <see cref="VertexDeclaration"/> about
        /////     the importance of ordering and structure for compatibility with older D3D drivers</b>.
        ///// </remarks>
        ///// <param name="source">
        /////     The binding index of HardwareVertexBuffer which will provide the source for this element.
        ///// </param>
        ///// <param name="offset">The offset in bytes where this element is located in the buffer.</param>
        ///// <param name="type">The data format of the element (3 floats, a color etc).</param>
        ///// <param name="semantic">The meaning of the data (position, normal, diffuse color etc).</param>
        //public VertexElement AddElement(int offset, VertexElementFormat type, VertexElementUsage semantic)
        //{
        //    return AddElement(offset, type, semantic, 0);
        //}

        ///// <summary>
        /////     Adds a new VertexElement to this declaration.
        ///// </summary>
        ///// <remarks>
        /////     This method adds a single element (positions, normals etc) to the
        /////     vertex declaration. <b>Please read the information in <see cref="VertexDeclaration"/> about
        /////     the importance of ordering and structure for compatibility with older D3D drivers</b>.
        ///// </remarks>
        ///// <param name="source">
        /////     The binding index of HardwareVertexBuffer which will provide the source for this element.
        ///// </param>
        ///// <param name="offset">The offset in bytes where this element is located in the buffer.</param>
        ///// <param name="type">The data format of the element (3 floats, a color etc).</param>
        ///// <param name="semantic">The meaning of the data (position, normal, diffuse color etc).</param>
        ///// <param name="index">Optional index for multi-input elements like texture coordinates.</param>
        //public virtual VertexElement AddElement(int offset, VertexElementFormat type, VertexElementUsage semantic, int index)
        //{
        //    VertexElement element = new VertexElement(offset, type, semantic, index);
        //    elements.Add(element);
        //    return element;
        //}

        /// <summary>
        ///     Finds a <see cref="VertexElement"/> with the given semantic, and index if there is more than 
        ///     one element with the same semantic. 
        /// </summary>
        /// <param name="semantic">Semantic to search for.</param>
        /// <returns>If the element is not found, this method returns null.</returns>
        public bool FindElementBySemantic(VertexElementUsage semantic, out VertexElement result)
        {
            // call overload with a default of index 0
            return FindElementBySemantic(semantic, 0, out result);
        }

        /// <summary>
        ///     Finds a <see cref="VertexElement"/> with the given semantic, and index if there is more than 
        ///     one element with the same semantic. 
        /// </summary>
        /// <param name="semantic">Semantic to search for.</param>
        /// <param name="index">Index of item to looks for using the supplied semantic (applicable to tex coords and colors).</param>
        /// <returns>If the element is not found, this method returns null.</returns>
        public virtual bool FindElementBySemantic(VertexElementUsage semantic, int index, out VertexElement result)
        {
            for (int i = 0; i < elements.Count; i++)
            {
                VertexElement element = elements[i];

                // do they match?
                if (element.Semantic == semantic && element.Index == index)
                {
                    result = element;
                    return true;
                }
            }

            // not found
            result = default(VertexElement);
            return false;
        }

        ///// <summary>
        /////     Gets a list of elements which use a given source.
        ///// </summary>
        //public virtual FastList<VertexElement> FindElementBySource(short source)
        //{
        //    FastList<VertexElement> rv = new FastList<VertexElement>();

        //    for (int i = 0; i < elements.Count; i++)
        //    {
        //        VertexElement element = elements[i];

        //        //// do they match?
        //        if (element.Source == source)
        //        rv.Add(element);
        //    }

        //    // return the list
        //    return rv;
        //}

        /// <summary>
        ///		Gets the <see cref="VertexElement"/> at the specified index.
        /// </summary>
        /// <param name="index">Index of the element to retrieve.</param>
        /// <returns>Element at the requested index.</returns>
        public VertexElement GetElement(int index)
        {
            //Debug.Assert(index < elements.Count && index >= 0, "Element index out of bounds.");

            return elements[index];
        }

        /// <summary>
        ///     Gets the vertex size defined by this declaration for a given source.
        /// </summary>
        /// <param name="source">The buffer binding index for which to get the vertex size.</param>
        public virtual int GetVertexSize()
        {
            int size = 0;

            for (int i = 0; i < elements.Count; i++)
            {
                //VertexElement element = elements[i];

                //// do they match?
                //if (element.Source == source)
                size += elements[i].Size;
            }

            // return the size
            return size;
        }

        ///// <summary>
        /////		Inserts a new <see cref="VertexElement"/> at a given position in this declaration.
        ///// </summary>
        ///// <remarks>
        /////		This method adds a single element (positions, normals etc) at a given position in this
        /////		vertex declaration. <b>Please read the information in VertexDeclaration about
        /////		the importance of ordering and structure for compatibility with older D3D drivers</b>.
        ///// </remarks>
        ///// <param name="position">Position to insert into.</param>
        ///// <param name="source">The binding index of HardwareVertexBuffer which will provide the source for this element.</param>
        ///// <param name="offset">The offset in bytes where this element is located in the buffer.</param>
        ///// <param name="type">The data format of the element (3 floats, a color, etc).</param>
        ///// <param name="semantic">The meaning of the data (position, normal, diffuse color etc).</param>
        ///// <returns>A reference to the newly created element.</returns>
        //public VertexElement InsertElement(int position, short source, int offset, VertexElementFormat type, VertexElementUsage semantic)
        //{
        //    return InsertElement(position, source, offset, type, semantic, 0);
        //}

        ///// <summary>
        /////		Inserts a new <see cref="VertexElement"/> at a given position in this declaration.
        ///// </summary>
        ///// <remarks>
        /////		This method adds a single element (positions, normals etc) at a given position in this
        /////		vertex declaration. <b>Please read the information in VertexDeclaration about
        /////		the importance of ordering and structure for compatibility with older D3D drivers</b>.
        ///// </remarks>
        ///// <param name="position">Position to insert into.</param>
        ///// <param name="source">The binding index of HardwareVertexBuffer which will provide the source for this element.</param>
        ///// <param name="offset">The offset in bytes where this element is located in the buffer.</param>
        ///// <param name="type">The data format of the element (3 floats, a color, etc).</param>
        ///// <param name="semantic">The meaning of the data (position, normal, diffuse color etc).</param>
        ///// <param name="index">Optional index for multi-input elements like texture coordinates.</param>
        ///// <returns>A reference to the newly created element.</returns>
        //public virtual VertexElement InsertElement(int position, short source, int offset, VertexElementFormat type, VertexElementUsage semantic, int index)
        //{
        //    if (position >= elements.Count)
        //    {
        //        return AddElement(offset, type, semantic, index);
        //    }

        //    VertexElement element = new VertexElement(offset, type, semantic, index);

        //    elements.Insert(position, element);

        //    return element;
        //}

        ///// <summary>
        /////		Gets the <see cref="VertexElement"/> at the specified index.
        ///// </summary>
        ///// <param name="index">Index of the element to retrieve.</param>
        ///// <returns>Element at the requested index.</returns>
        //public virtual void RemoveElement(int index)
        //{
        //    //Debug.Assert(index < elements.Count && index >= 0, "Element index out of bounds.");

        //    elements.RemoveAt(index);
        //}

        ///// <summary>
        /////		Removes all <see cref="VertexElement"/> from the declaration.
        ///// </summary>

        //public virtual void RemoveAllElements()
        //{
        //    elements.Clear();
        //}

        ///// <summary>
        /////		Modifies the definition of a <see cref="VertexElement"/>.
        ///// </summary>
        ///// <param name="elemIndex">Index of the element to modify.</param>
        ///// <param name="source">Source of the element.</param>
        ///// <param name="offset">Offset of the element.</param>
        ///// <param name="type">Type of the element.</param>
        ///// <param name="semantic">Semantic of the element.</param>
        //public void ModifyElement(int elemIndex, short source, int offset, VertexElementFormat type, VertexElementUsage semantic)
        //{
        //    ModifyElement(elemIndex, source, offset, type, semantic, 0);
        //}

        ///// <summary>
        /////		Modifies the definition of a <see cref="VertexElement"/>.
        ///// </summary>
        ///// <param name="elemIndex">Index of the element to modify.</param>
        ///// <param name="source">Source of the element.</param>
        ///// <param name="offset">Offset of the element.</param>
        ///// <param name="type">Type of the element.</param>
        ///// <param name="semantic">Semantic of the element.</param>
        ///// <param name="index">Usage index of the element.</param>
        //public virtual void ModifyElement(int elemIndex, short source, int offset, VertexElementFormat type, VertexElementUsage semantic, int index)
        //{
        //    elements[elemIndex] = new VertexElement(offset, type, semantic, index);
        //}

        ///// <summary>
        /////		Remove the element with the given semantic.
        ///// </summary>
        ///// <remarks>
        /////		For elements that have usage indexes, the default of 0 is used.
        ///// </remarks>
        ///// <param name="semantic">Semantic to remove.</param>
        //public void RemoveElement(VertexElementUsage semantic)
        //{
        //    RemoveElement(semantic, 0);
        //}

        ///// <summary>
        /////		Remove the element with the given semantic and usage index.
        ///// </summary>
        ///// <param name="semantic">Semantic to remove.</param>
        ///// <param name="index">Usage index to remove, typically only applies to tex coords.</param>
        //public virtual void RemoveElement(VertexElementUsage semantic, int index)
        //{
        //    for (int i = elements.Count - 1; i >= 0; i--)
        //    {
        //        VertexElement element = elements[i];

        //        if (element.Semantic == semantic && element.Index == index)
        //        {
        //            // we have a winner!
        //            elements.RemoveAt(i);
        //        }
        //    }
        //}

        /// <summary>
        ///     Tests equality of 2 <see cref="VertexElement"/> objects.
        /// </summary>
        /// <param name="left">A <see cref="VertexElement"/></param>
        /// <param name="right">A <see cref="VertexElement"/></param>
        /// <returns>true if equal, false otherwise.</returns>
        public static bool operator ==(VertexDeclaration left, VertexDeclaration right)
        {
            // if element lists are different sizes, they can't be equal
            if (left.elements.Count != right.elements.Count)
                return false;

            for (int i = 0; i < right.elements.Count; i++)
            {
                VertexElement a = left.elements[i];
                VertexElement b = right.elements[i];

                // if they are not equal, this declaration differs
                if (!(a == b))
                    return false;
            }

            // if we got thise far, they are equal
            return true;
        }

        /// <summary>
        ///     Tests in-equality of 2 <see cref="VertexElement"/> objects.
        /// </summary>
        /// <param name="left">A <see cref="VertexElement"/></param>
        /// <param name="right">A <see cref="VertexElement"/></param>
        /// <returns>true if not equal, false otherwise.</returns>
        public static bool operator !=(VertexDeclaration left, VertexDeclaration right)
        {
            return !(left == right);
        }

        #endregion

        #region Properties

        /// <summary>
        ///     Gets the number of elements in the declaration.
        /// </summary>
        public int ElementCount
        {
            get
            {
                return elements.Count;
            }
        }

        #endregion

        #region Object overloads

        /// <summary>
        ///    Override to determine equality between 2 VertexDeclaration objects,
        /// </summary>
        /// <param name="obj"></param>
        /// <returns></returns>
        public override bool Equals(object obj)
        {
            VertexDeclaration decl = obj as VertexDeclaration;

            return (decl == this);
        }

        /// <summary>
        ///    Override GetHashCode.
        /// </summary>
        /// <remarks>
        ///    Done mainly to quash warnings, no real need for it.
        /// </remarks>
        /// <returns></returns>
        // TODO: Does this need to be implemented, dont think we are stuffing these into hashtables.
        public override int GetHashCode()
        {
            return 0;
        }

        #endregion Object overloads

        //#region ICloneable Members

        ///// <summary>
        /////     Clonses this declaration, including a copy of all <see cref="VertexElement"/> objects this declaration holds.
        ///// </summary>
        ///// <returns></returns>
        //public object Clone()
        //{
        //    VertexDeclaration clone = VertexDeclarationManager.Instance.CreateVertexDeclaration();

        //    for (int i = 0; i < elements.Count; i++)
        //    {
        //        VertexElement element = (VertexElement)elements[i];
        //        clone.AddElement(element.Offset, element.Type, element.Semantic, element.Index);
        //    }

        //    return clone;
        //}

        //#endregion

        #region IDisposable 成员

        public abstract void Dispose(bool disposing);

        public bool Disposed
        {
            get;
            private set;
        }

        public void Dispose()
        {
            if (!Disposed)
            {
                Dispose(true);
                Disposed = true;
            }
            else
            {
                throw new ObjectDisposedException(ToString());
            }
        }

        ~VertexDeclaration()        
        {
            if (!Disposed)
            {
                Dispose(false);
                Disposed = true;
            }
        }

        #endregion
    }

}
