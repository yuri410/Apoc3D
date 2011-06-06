/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Games

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  if not, write to the Free Software Foundation, 
Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/gpl.txt.

-----------------------------------------------------------------------------
*/
#ifndef FASTMAP_H
#define FASTMAP_H

#include "Common.h"
#include "CollectionsCommon.h"

namespace Apoc3D
{
	namespace Collections
	{
		template<typename T, typename S>
		class APAPI FastMap
		{
		private:
			class HashHelpers
			{
			public:
				static const int primes[];

				static int GetPrime(int min)
				{
					for (int i = 0; i < sizeof(primes)/sizeof(int); i++)
					{
						int num2 = primes[i];
						if (num2 >= min)
						{
							return num2;
						}
					}
					for (int j = min | 1; j < 2147483647; j += 2)
					{
						if (IsPrime(j))
						{
							return j;
						}
					}
					return min;
				}

				static bool IsPrime(int candidate)
				{
					if ((candidate & 1) == 0)
					{
						return (candidate == 2);
					}
					int num = (int)sqrtf((float)candidate);
					for (int i = 3; i <= num; i += 2)
					{
						if ((candidate % i) == 0)
						{
							return false;
						}
					}
					return true;
				}
			};
			struct Entry
			{
				int hashCode;
				int next;
				T data;
				S value;
			};

			int* m_buckets;
			int m_bucketsLength;
			int m_count;

			Entry* m_entries;
			int m_entryLength;

			int m_freeCount;
			int m_freeList;
			const IEqualityComparer<T>* m_comparer;

			void Initialize(int capacity)
			{
				int prime = HashHelpers::GetPrime(capacity);
				m_bucketsLength = prime;
				m_buckets = new int[prime];
				for (int i = 0; i < m_bucketsLength; i++)
				{
					m_buckets[i] = -1;
				}
				m_entries = new Entry[prime];
				m_entryLength = prime;
				m_freeList = -1;
			}
			void Resize()
			{
				int prime = HashHelpers::GetPrime(this.count * 2);
				int[] numArray = new int[prime];
				for (int i = 0; i < numArray.Length; i++)
				{
					numArray[i] = -1;
				}
				Entry[] destinationArray = new Entry[prime];
				memcpy(destinationArray, m_entries, m_count * sizeof(Entry));
				for (int j = 0; j < this.count; j++)
				{
					int index = destinationArray[j].hashCode % prime;
					destinationArray[j].next = numArray[index];
					numArray[index] = j;
				}
				delete[] m_buckets;
				m_buckets = numArray;
				m_bucketsLength = prime;
				delete[] m_entries;
				m_entries = destinationArray;
				m_entryLength = prime;
			}
			void Insert(const T& item, const S& value, bool add);
			int FindEntry(const T& item) const
			{
				if (m_buckets)
				{
					int num = m_comparer->GetHashCode(item) & 2147483647;
					for (int i = m_buckets[num % m_bucketsLength]; i >= 0; i = m_entries[i].next)
					{
						if ((m_entries[i].hashCode == num) && m_comparer->Equals(m_entries[i].data, item))
						{
							return i;
						}
					}
				}
				return -1;
			}
		public:
			int32 getCount() const { return m_count - m_freeCount; }

			FastMap(int capacity, const IEqualityComparer<T>* comparer)
				: m_comparer(comparer), m_buckets(0), m_bucketsLength(0), m_count(0), 
				m_entries(0), m_entryLength(0), m_freeCount(0), m_freeList(0)
			{
				if (capacity > 0)
				{
					Initialize(capacity);
				}
			}
			~FastMap()
			{
				delete[] m_entries;
				delete[] m_buckets;
			}
			void Add(const T& item, const S& value)
			{
				Insert(item, value, true);
			}

			void Clear()
			{
				if (m_count > 0)
				{
					for (int i = 0; i < m_bucketsLength; i++)
					{
						m_buckets[i] = -1;
					}
					memset(m_entries, 0, m_count*sizeof(Entry));
					m_freeList = -1;
					m_count = 0;
					m_freeCount = 0;
				}
			}

			bool Remove(const T& item)
			{
				if (m_buckets)
				{
					int num = m_comparer->GetHashCode(item) & 2147483647;
					int index = num % m_bucketsLength;
					int num3 = -1;
					for (int i = m_buckets[index]; i >= 0; i = m_entries[i].next)
					{
						if ((m_entries[i].hashCode == num) && m_comparer->Equals(m_entries[i].data, item))
						{
							if (num3 < 0)
							{
								m_buckets[index] = m_entries[i].next;
							}
							else
							{
								m_entries[num3].next = m_entries[i].next;
							}
							m_entries[i].hashCode = -1;
							m_entries[i].next = m_freeList;
							memset(m_entries[i].data,0,sizeof(T));
							m_freeList = i;
							m_freeCount++;
							return true;
						}
						num3 = i;
					}
				}
				return false;
			}
			bool Contains(const T& item) const
			{
				if (m_buckets)
				{
					int num = m_comparer->GetHashCode(item) & 2147483647;
					for (int i = m_buckets[num % m_bucketsLength]; i >= 0; i = m_entries[i].next)
					{
						if ((m_entries[i].hashCode == num) && m_comparer->Equals(m_entries[i].data, item))
						{
							return true;
						}
					}
				}
				return false;
			}

			inline S& operator [](const T& key) const;

			bool TryGetValue(const T& key, S& value) const;
		};
	}
}

#endif