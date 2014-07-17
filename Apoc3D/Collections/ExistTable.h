#pragma once

#ifndef APOC3D_EXISTTABLE_H
#define APOC3D_EXISTTABLE_H

/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Xin

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

#include "CollectionsCommon.h"
#include "apoc3d/ApocException.h"

namespace Apoc3D
{
	namespace Collections
	{
		template <typename T, typename ComparerType = EqualityComparer<T>>
		class ExistTable
		{
		public:
			class Enumerator
			{
			public:
				Enumerator(const ExistTable<T, ComparerType>* dict)
					: m_dict(dict), m_index(0), m_current(nullptr)
				{
				}

				bool MoveNext()
				{
					while (m_index<m_dict->m_count)
					{
						if (m_dict->m_entries[m_index].hashCode>=0)
						{
							m_current = &m_dict->m_entries[m_index].data;
							m_index++;
							return true;
						}
						m_index++;
					}
					m_index = m_dict->m_count + 1;
					m_current = 0;
					return false;
				}

				const T* getCurrent() const { return m_current; }

			private:
				const ExistTable<T, ComparerType>* m_dict;
				int m_index;
				const T* m_current;

			};

		public:
			ExistTable(const ExistTable& another)
				: m_bucketsLength(another.m_bucketsLength),
				m_count(another.m_count), m_entryLength(another.m_entryLength), m_freeCount(another.m_freeCount),
				m_freeList(another.m_freeList)
			{
				m_buckets = new int[m_bucketsLength];
				memcpy(m_buckets, another.m_buckets, sizeof(int)*m_bucketsLength);

				m_entries = new Entry[m_entryLength];
				for (int i=0;i<m_entryLength;i++)
					m_entries[i] = another.m_entries[i];
			}

			ExistTable()
				: m_buckets(0), m_bucketsLength(0), m_count(0), 
				m_entries(0), m_entryLength(0), m_freeCount(0), m_freeList(0)
			{
				int capacity = 40;
				if (capacity > 0)
				{
					Initialize(capacity);
				}
			}

			explicit ExistTable(int capacity)
				: m_buckets(0), m_bucketsLength(0), m_count(0), 
				m_entries(0), m_entryLength(0), m_freeCount(0), m_freeList(0)
			{
				if (capacity > 0)
				{
					Initialize(capacity);
				}
			}

			~ExistTable()
			{
				delete[] m_buckets;
				delete[] m_entries;
			}

			ExistTable& operator=(const ExistTable& another)
			{
				if (this == &another)
					return *this;

				delete[] m_entries;
				delete[] m_buckets;

				m_bucketsLength = another.m_bucketsLength;

				m_count = another.m_count;
				m_entryLength = another.m_entryLength;
				m_freeCount = another.m_freeCount;
				m_freeList = another.m_freeList;

				m_buckets = new int[m_bucketsLength];
				memcpy(m_buckets, another.m_buckets, sizeof(int)*m_bucketsLength);

				m_entries = new Entry[m_entryLength];
				for (int i=0;i<m_entryLength;i++)
					m_entries[i] = another.m_entries[i];

				return *this; 
			}

			void Add(const T& item)
			{
				Insert(item, true);
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
					int hash = ComparerType::GetHashCode(item) & 2147483647;
					int index = hash % m_bucketsLength;
					int slot = -1;
					for (int i = m_buckets[index]; i >= 0; i = m_entries[i].next)
					{
						if ((m_entries[i].hashCode == hash) && ComparerType::Equals(m_entries[i].data, item))
						{
							if (slot < 0)
							{
								m_buckets[index] = m_entries[i].next;
							}
							else
							{
								m_entries[slot].next = m_entries[i].next;
							}
							m_entries[i].hashCode = -1;
							m_entries[i].next = m_freeList;
							memset(&m_entries[i].data,0,sizeof(T));
							m_freeList = i;
							m_freeCount++;
							return true;
						}
						slot = i;
					}
				}
				return false;
			}
			bool Exists(const T& item) const
			{
				if (m_buckets)
				{
					int hash = ComparerType::GetHashCode(item) & 2147483647;
					for (int i = m_buckets[hash % m_bucketsLength]; i >= 0; i = m_entries[i].next)
					{
						if ((m_entries[i].hashCode == hash) && ComparerType::Equals(m_entries[i].data, item))
						{
							return true;
						}
					}
				}
				return false;
			}

			Enumerator GetEnumerator() const
			{
				return Enumerator(this);
			}

			int32 getCount() const { return m_count - m_freeCount; }

		private:
			struct Entry
			{
				int hashCode;
				int next;
				T data;
			};

			int* m_buckets;
			int m_bucketsLength;
			int m_count;

			Entry* m_entries;
			int m_entryLength;

			int m_freeCount;
			int m_freeList;

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
				int prime = HashHelpers::GetPrime(m_count * 2);
				int* newBuckets = new int[prime];
				for (int i = 0; i < prime; i++)
				{
					newBuckets[i] = -1;
				}
				Entry* destinationArray = new Entry[prime];
				//memcpy(destinationArray, m_entries, m_count * sizeof(Entry));
				for (int j = 0; j < m_count; j++)
				{
					destinationArray[j] = m_entries[j];
					int index = destinationArray[j].hashCode % prime;
					destinationArray[j].next = newBuckets[index];
					newBuckets[index] = j;
				}
				delete[] m_buckets;
				m_buckets = newBuckets;
				m_bucketsLength = prime;
				delete[] m_entries;
				m_entries = destinationArray;
				m_entryLength = prime;
			}
			void Insert(const T& item, bool add)
			{
				int freeList;
				if (!m_buckets)
				{
					Initialize(0);
				}
				int hash = ComparerType::GetHashCode(item) & 2147483647;
				int index = hash % m_bucketsLength;
				for (int i = m_buckets[index]; i >= 0; i = m_entries[i].next)
				{
					if ((m_entries[i].hashCode == hash) &&
						ComparerType::Equals(m_entries[i].data, item))
					{
						if (add)
						{
							throw AP_EXCEPTION(EX_Duplicate, L"");
						}
						m_entries[i].data = item;
						return;
					}
				}
				if (m_freeCount > 0)
				{
					freeList = m_freeList;
					m_freeList = m_entries[freeList].next;
					m_freeCount--;
				}
				else
				{
					if (m_count == m_entryLength)
					{
						Resize();
						index = hash % m_bucketsLength;
					}
					freeList = m_count;
					m_count++;
				}
				m_entries[freeList].hashCode = hash;
				m_entries[freeList].next = m_buckets[index];
				m_entries[freeList].data = item;
				m_buckets[index] = freeList;
			}

			int FindEntry(const T& item) const
			{
				if (m_buckets)
				{
					int hash = ComparerType::GetHashCode(item) & 2147483647;
					for (int i = m_buckets[hash % m_bucketsLength]; i >= 0; i = m_entries[i].next)
					{
						if ((m_entries[i].hashCode == hash) && ComparerType::Equals(m_entries[i].data, item))
						{
							return i;
						}
					}
				}
				return -1;
			}
		};
	}
}

#endif