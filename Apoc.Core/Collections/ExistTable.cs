using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using System.Collections;

namespace Apoc.Collections
{
    public class ExistTable<T> : IEnumerable<T>
    {
        [Serializable, StructLayout(LayoutKind.Sequential)]
        public class Enumerator : IEnumerator<T>
        {
            internal const int DictEntry = 1;
            internal const int KeyValuePair = 2;
            private ExistTable<T> dictionary;
            private int version;
            private int index;
            private T current;

            internal Enumerator(ExistTable<T> dictionary)
            {
                this.dictionary = dictionary;
                this.index = 0;
                this.current = default(T);
            }

            public bool MoveNext()
            {
                while (this.index < this.dictionary.count)
                {
                    if (this.dictionary.entries[this.index].hashCode >= 0)
                    {
                        this.current = this.dictionary.entries[this.index].data;
                        this.index++;
                        return true;
                    }
                    this.index++;
                }
                this.index = this.dictionary.count + 1;
                this.current = default(T);
                return false;
            }

            public T Current
            {
                get
                {
                    return this.current;
                }
            }
            public void Dispose()
            {
                dictionary = null;
            }

            object IEnumerator.Current
            {
                get
                {
                    return current;
                }
            }
            void IEnumerator.Reset()
            {
                this.index = 0;
                this.current = default(T);
            }

        }

        [StructLayout(LayoutKind.Sequential)]
        private struct Entry
        {
            public int hashCode;
            public int next;
            public T data;
        }


        private int[] buckets;
        private IEqualityComparer<T> comparer;
        private int count;

        private Entry[] entries;
        private int freeCount;

        private int freeList;

        public ExistTable()
            : this(4, null)
        {
        }
        public ExistTable(int capacity)
            : this(capacity, null)
        {
        }
        public ExistTable(int capacity, IEqualityComparer<T> comparer)
        {
            if (capacity > 0)
            {
                this.Initialize(capacity);
            }
            if (comparer == null)
            {
                comparer = EqualityComparer<T>.Default;
            }
            this.comparer = comparer;
        }

        public void Add(T item)
        {
            Insert(item, true);
        }
        public void Clear()
        {
            if (this.count > 0)
            {
                for (int i = 0; i < this.buckets.Length; i++)
                {
                    this.buckets[i] = -1;
                }
                Array.Clear(this.entries, 0, this.count);
                this.freeList = -1;
                this.count = 0;
                this.freeCount = 0;
            }
        }
        public bool Remove(T item)
        {
            if (this.buckets != null)
            {
                int num = this.comparer.GetHashCode(item) & 2147483647;
                int index = num % this.buckets.Length;
                int num3 = -1;
                for (int i = this.buckets[index]; i >= 0; i = this.entries[i].next)
                {
                    if ((this.entries[i].hashCode == num) && this.comparer.Equals(this.entries[i].data, item))
                    {
                        if (num3 < 0)
                        {
                            this.buckets[index] = this.entries[i].next;
                        }
                        else
                        {
                            this.entries[num3].next = this.entries[i].next;
                        }
                        this.entries[i].hashCode = -1;
                        this.entries[i].next = this.freeList;
                        this.entries[i].data = default(T);
                        this.freeList = i;
                        this.freeCount++;
                        return true;
                    }
                    num3 = i;
                }
            }
            return false;
        }
        public int Count
        {
            get
            {
                return (this.count - this.freeCount);
            }
        }

        public bool Exists(T item)
        {
            if (this.buckets != null)
            {
                int num = this.comparer.GetHashCode(item) & 2147483647;
                for (int i = this.buckets[num % this.buckets.Length]; i >= 0; i = this.entries[i].next)
                {
                    if ((this.entries[i].hashCode == num) && this.comparer.Equals(this.entries[i].data, item))
                    {
                        return true;
                    }
                }
            }
            return false;
        }

        private void Initialize(int capacity)
        {
            int prime = HashHelpers.GetPrime(capacity);
            this.buckets = new int[prime];
            for (int i = 0; i < this.buckets.Length; i++)
            {
                this.buckets[i] = -1;
            }
            this.entries = new Entry[prime];
            this.freeList = -1;
        }

        private void Resize()
        {
            int prime = HashHelpers.GetPrime(this.count * 2);
            int[] numArray = new int[prime];
            for (int i = 0; i < numArray.Length; i++)
            {
                numArray[i] = -1;
            }
            Entry[] destinationArray = new Entry[prime];
            Array.Copy(this.entries, 0, destinationArray, 0, this.count);
            for (int j = 0; j < this.count; j++)
            {
                int index = destinationArray[j].hashCode % prime;
                destinationArray[j].next = numArray[index];
                numArray[index] = j;
            }
            this.buckets = numArray;
            this.entries = destinationArray;
        }

        private void Insert(T item, bool add)
        {
            int freeList;
            if (this.buckets == null)
            {
                this.Initialize(0);
            }
            int num = this.comparer.GetHashCode(item) & 2147483647;
            int index = num % this.buckets.Length;
            for (int i = this.buckets[index]; i >= 0; i = this.entries[i].next)
            {
                if ((this.entries[i].hashCode == num) &&
                    this.comparer.Equals(this.entries[i].data, item))
                {
                    if (add)
                    {
                        throw new ArgumentException("AddingDuplicate");
                    }
                    this.entries[i].data = item;
                    return;
                }
            }
            if (this.freeCount > 0)
            {
                freeList = this.freeList;
                this.freeList = this.entries[freeList].next;
                this.freeCount--;
            }
            else
            {
                if (this.count == this.entries.Length)
                {
                    this.Resize();
                    index = num % this.buckets.Length;
                }
                freeList = this.count;
                this.count++;
            }
            this.entries[freeList].hashCode = num;
            this.entries[freeList].next = this.buckets[index];
            this.entries[freeList].data = item;
            this.buckets[index] = freeList;
        }

        private int FindEntry(T item)
        {
            if (this.buckets != null)
            {
                int num = this.comparer.GetHashCode(item) & 2147483647;
                for (int i = this.buckets[num % this.buckets.Length]; i >= 0; i = this.entries[i].next)
                {
                    if ((this.entries[i].hashCode == num) && this.comparer.Equals(this.entries[i].data, item))
                    {
                        return i;
                    }
                }
            }
            return -1;
        }

        #region IEnumerable<T> 成员

        public IEnumerator<T> GetEnumerator()
        {
            return new Enumerator(this);
        }

        #endregion

        #region IEnumerable 成员

        IEnumerator IEnumerable.GetEnumerator()
        {
            return new Enumerator(this);
        }

        #endregion
    }
}

