using System;
using System.Collections.Generic;
using System.Text;
using System.Collections;

namespace Apoc.Collections
{
    public abstract class CollectionBase<T> : IEnumerable<T>
    {
        public class Enumerator : IEnumerator<T>
        {
            CollectionBase<T> collection;
            int currentIndex;

            public Enumerator(CollectionBase<T> collection)
            {
                this.collection = collection;
            }

            #region IEnumerator<T2> 成员

            public T Current
            {
                get { return collection[currentIndex]; }
            }

            #endregion

            #region IDisposable 成员

            public void Dispose()
            {
                collection = null;
            }

            #endregion

            #region IEnumerator 成员

            object IEnumerator.Current
            {
                get { return (object)collection[currentIndex]; }
            }

            public bool MoveNext()
            {
                return ++currentIndex < collection.Count;
            }

            public void Reset()
            {
                currentIndex = 0;
            }

            #endregion
        }

        public T[] Elements;

        int internalPointer;

        int length;

        protected CollectionBase()
        {
            Elements = new T[4];
            length = 4;
        }

        protected CollectionBase(int elementsCount)
        {
            Elements = new T[elementsCount];
            length = elementsCount;
        }

        public T this[int i]
        {
            get { return Elements[i]; }
        }

        protected void Add(T Data)
        {
            if (length <= internalPointer)
            {
                this.Resize(length == 0 ? 4 : (length * 2));
            }
            Elements[internalPointer++] = Data;
        }
        //[CLSCompliant(false)]
        protected void Add(ref T Data)
        {
            if (length <= internalPointer)
            {
                this.Resize(length == 0 ? 4 : (length * 2));
            }
            Elements[internalPointer++] = Data;
        }

        protected void Add(T[] data)
        {
            int addL = internalPointer + data.Length;

            if (length <= addL)
            {
                int twoL = length * 2;

                this.Resize(twoL > addL ? twoL : addL);
            }
            int len = data.Length;
            Array.Copy(data, 0, Elements, internalPointer, len);
            internalPointer += len;
        }
        protected void Add(CollectionBase<T> data)
        {
            int addL = internalPointer + data.Count;
            if (length <= addL)
            {
                int twoL = length * 2;
                this.Resize(twoL > addL ? twoL : addL);
            }
            int len = data.Count;
            Array.Copy(data.Elements, 0, Elements, internalPointer, len);
            internalPointer += len;
        }

        protected void FastClear()
        {
            internalPointer = 0;
        }
        protected void Clear()
        {
            Array.Clear(Elements, 0, internalPointer);
            internalPointer = 0;
        }
        protected void Resize(int newSize)
        {
            T[] destinationArray = new T[newSize];
            Array.Copy(Elements, destinationArray, internalPointer);
            Elements = destinationArray;
            length = newSize;
        }

        protected bool Remove(T item)
        {
            int index = IndexOf(item);
            if (index != -1)
            {
                Remove(index);
                return true;
            }
            return false;
        }

        protected void Remove(int idx)
        {
            T[] destinationArray = new T[length - 1];
            Array.Copy(Elements, 0, destinationArray, 0, idx);

            if (Count - ++idx > 0)
            {
                Array.Copy(Elements, idx, destinationArray, idx - 1, Count - idx);
            }

            Elements = destinationArray;

            length--;
            internalPointer--;
        }

        public int Count
        {
            get
            {
                return internalPointer;
            }
        }

        public override string ToString()
        {
            return "Count: " + Count.ToString();
        }

        protected int IndexOf(T item)
        {
            if (item == null)
            {
                for (int i = 0; i < Count; i++)
                {
                    if (Elements[i] == null)
                    {
                        return i;
                    }
                }
                return -1;
            }
            else
            {
                EqualityComparer<T> comparer = EqualityComparer<T>.Default;

                for (int i = 0; i < Count; i++)
                {
                    if (comparer.Equals(Elements[i], item))
                    {
                        return i;
                    }
                }
                return -1;
            }
        }

        protected bool Contains(T item)
        {
            if (item == null)
            {
                for (int i = 0; i < Count; i++)
                {
                    if (Elements[i] == null)
                    {
                        return true;
                    }
                }
                return false;
            }
            else
            {
                EqualityComparer<T> comparer = EqualityComparer<T>.Default;

                for (int i = 0; i < Count; i++)
                {
                    if (comparer.Equals(Elements[i], item))
                    {
                        return true;
                    }
                }
                return false;
            }
        }

        public void CopyTo(T[] array, int arrayIndex)
        {
            if (array == null)
            {
                throw new ArgumentNullException("array");
            }

            if (arrayIndex > array.Length || arrayIndex < 0)
            {
                throw new ArgumentOutOfRangeException("arrayInedx");
            }

            if (array.Length - arrayIndex < Count)
            {
                throw new ArgumentException("array");
            }

            Array.Copy(Elements, 0, array, arrayIndex, Count);
        }


        public IEnumerator<T> GetEnumerator()
        {
            return new Enumerator(this);
        }


        #region IEnumerable 成员

        IEnumerator IEnumerable.GetEnumerator()
        {
            return new Enumerator(this);
        }

        #endregion
    }
}
