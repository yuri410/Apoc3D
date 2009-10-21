using System;
using System.Collections.Generic;
using System.Text;

namespace Apoc.Collections
{
    public class FastList<T>
    {
        sealed class FunctorComparer<U> : IComparer<U>
        {
            // Fields
            private Comparer<U> c;
            private Comparison<U> comparison;

            // Methods
            public FunctorComparer(Comparison<U> comparison)
            {
                this.c = Comparer<U>.Default;
                this.comparison = comparison;
            }

            public int Compare(U x, U y)
            {
                return this.comparison(x, y);
            }
        }

        public T[] Elements;

        int internalPointer;

        int length;

        public FastList()
        {
            Elements = new T[4];
            length = 4;
        }

        public FastList(T[] array)
        {
            this.Elements = array;
            length = array.Length;
            internalPointer = length;
        }

        public FastList(int elementsCount)
        {
            Elements = new T[elementsCount];
            length = elementsCount;
        }

        public T this[int i]
        {
            get { return Elements[i]; }
            set { Elements[i] = value; }
        }

        public void Add(T Data)
        {
            if (length <= internalPointer)
            {
                this.Resize(length == 0 ? 4 : (length * 2));
            }
            Elements[internalPointer++] = Data;
        }
        public void Add(ref T Data)
        {
            if (length <= internalPointer)
            {
                this.Resize(length == 0 ? 4 : (length * 2));
            }
            Elements[internalPointer++] = Data;
        }

        public void Add(T[] data)
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
        public void Add(FastList<T> data)
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

        public void FastClear()
        {
            internalPointer = 0;
        }
        public void Clear()
        {
            Array.Clear(Elements, 0, internalPointer);
            internalPointer = 0;
        }
        public void Resize(int newSize)
        {
            T[] destinationArray = new T[newSize];
            Array.Copy(Elements, destinationArray, internalPointer);
            Elements = destinationArray;
            length = newSize;
        }

        public int IndexOf(T item)
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
        public void Remove(T item)
        {
            int index = IndexOf(item);
            if (index != -1)
            {
                RemoveAt(index);
            }
        }

        public void RemoveAt(int idx)
        {
            if (idx == internalPointer - 1)
            {
                internalPointer--;
                Elements[idx] = default(T);
            }
            else
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
        public void RemoveRange(int index, int count)
        {
            if (count > 0)
            {
                internalPointer -= count;

                if (index < this.Count)
                {
                    Array.Copy(this.Elements, index + count, this.Elements, index, this.Count - index);
                }

                Array.Clear(this.Elements, this.Count, count);

            }
        }
        public void Sort(Comparison<T> comparison)
        {
            if (this.Count > 0)
            {
                IComparer<T> comparer = new FunctorComparer<T>(comparison);
                Array.Sort<T>(this.Elements, 0, this.Count, comparer);
            }
        }
        public void Insert(int index, T item)
        {
            if (this.Count == length)
            {
                this.Resize(this.Count + 1);
            }
            if (index < this.Count)
            {
                Array.Copy(this.Elements, index, this.Elements, index + 1, this.Count - index);
            }
            this.Elements[index] = item;
            this.internalPointer++;
        }
        public bool Contains(T item)
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
    }
}
