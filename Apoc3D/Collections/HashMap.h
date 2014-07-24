#pragma once

#ifndef APOC3D_HASHMAP_H
#define APOC3D_HASHMAP_H

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
		template <typename T, typename S, typename ComparerType = Apoc3D::Collections::EqualityComparer<T>>
		class HashMap
		{
		public:
			class Enumerator
			{
			public:
				Enumerator(const HashMap<T, S, ComparerType>* dict)
					: m_dict(dict), m_index(0), m_current(nullptr), m_currentVal(nullptr)
				{
				}

				bool MoveNext()
				{
					while (m_index<m_dict->m_count)
					{
						if (m_dict->m_entries[m_index].hashCode>=0)
						{
							m_current = &m_dict->m_entries[m_index].data;
							m_currentVal = &m_dict->m_entries[m_index].value;
							m_index++;
							return true;
						}
						m_index++;
					}
					m_index = m_dict->m_count + 1;
					m_current = nullptr;
					m_currentVal = nullptr;
					return false;
				}

				const T& getCurrentKey() const { return *m_current; }
				S& getCurrentValue() const { return *m_currentVal; }
			private:
				const HashMap<T, S, ComparerType>* m_dict;
				int m_index;
				T* m_current;
				S* m_currentVal;
			};

		public:

			HashMap(const HashMap& another)
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
			HashMap()
				: m_buckets(0), m_bucketsLength(0), m_count(0), 
				m_entries(0), m_entryLength(0), m_freeCount(0), m_freeList(0)
			{
				int capacity = 8;
				if (capacity > 0)
				{
					Initialize(capacity);
				}
			}
			explicit HashMap(int capacity)
				: m_buckets(0), m_bucketsLength(0), m_count(0), 
				m_entries(0), m_entryLength(0), m_freeCount(0), m_freeList(0)
			{
				if (capacity > 0)
				{
					Initialize(capacity);
				}
			}
			~HashMap()
			{
				delete[] m_entries;
				delete[] m_buckets;
			}
			HashMap& operator=(const HashMap& another)
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
					m_freeList = -1;
					m_count = 0;
					m_freeCount = 0;
				}
			}

			void DeleteValuesAndClear()
			{
				for (Enumerator e = GetEnumerator(); e.MoveNext();)
				{
					S val = e.getCurrentValue();
					delete val;
				}

				Clear();
			}


			bool Remove(const T& item)
			{
				if (m_buckets)
				{
					int hash = ComparerType::GetHashCode(item) & 2147483647;
					int index = hash % m_bucketsLength;
					int lastI = -1;
					for (int i = m_buckets[index]; i >= 0; i = m_entries[i].next)
					{
						if ((m_entries[i].hashCode == hash) && ComparerType::Equals(m_entries[i].data, item))
						{
							if (lastI < 0)
							{
								m_buckets[index] = m_entries[i].next;
							}
							else
							{
								m_entries[lastI].next = m_entries[i].next;
							}
							m_entries[i].hashCode = -1;
							m_entries[i].next = m_freeList;
							m_freeList = i;
							m_freeCount++;
							return true;
						}
						lastI = i;
					}
				}
				return false;
			}
			bool Contains(const T& item) const
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
			void Resize(int32 newSize)
			{
				int primeCapacity = HashHelpers::GetPrime(newSize);
				int* newBuckets = new int[primeCapacity];
				for (int i = 0; i < primeCapacity; i++)
				{
					newBuckets[i] = -1;
				}
				Entry* newEntries = new Entry[primeCapacity];
				//memcpy(destinationArray, m_entries, m_count * sizeof(Entry));
				for (int j = 0; j < m_count; j++)
				{
					newEntries[j] = m_entries[j];
					int index = newEntries[j].hashCode % primeCapacity;
					newEntries[j].next = newBuckets[index];
					newBuckets[index] = j;
				}
				delete[] m_buckets;
				m_buckets = newBuckets;
				m_bucketsLength = primeCapacity;
				delete[] m_entries;
				m_entries = newEntries;
				m_entryLength = primeCapacity;
			}
			S& operator [](const T& key) const
			{
				int index = FindEntry(key);
				if (index>=0)
				{
					return m_entries[index].value;
				}
				throw AP_EXCEPTION(EX_KeyNotFound, L"");
			}

			bool TryGetValue(const T& key, S& value) const
			{
				int index = FindEntry(key);
				if (index>=0)
				{
					value = m_entries[index].value;
					return true;
				}
				return false;
			}
			S* TryGetValue(const T& key) const
			{
				int index = FindEntry(key);
				if (index>=0)
				{
					return &m_entries[index].value;
				}
				return nullptr;
			}
			Enumerator GetEnumerator() const
			{
				return Enumerator(this);
			}

			void FillKeys(List<T>& list) const
			{
				if (list.getCount() == 0)
					list.ResizeDiscard(getCount());

				for (Enumerator e = GetEnumerator(); e.MoveNext();)
					list.Add(e.getCurrentKey());
			}

			void FillValues(List<S>& list) const
			{
				if (list.getCount() == 0)
					list.ResizeDiscard(getCount());

				for (Enumerator e = GetEnumerator(); e.MoveNext();)
					list.Add(e.getCurrentValue());
			}

			//////////////////////////////////////////////////////////////////////////

			void ForEachAbortable(bool (*proc)(const T& key, T& value))
			{
				for (Enumerator e = GetEnumerator(); e.MoveNext();)
					if (proc(e.getCurrentKey(), e.getCurrentValue()))
						return;
			}
			template <bool (*proc)(const T& key, T& value)>
			void ForEachAbortable() { ForEachAbortable(proc); }
			
			void ForEachAbortableConst(bool (*proc)(const T& key, const T& value)) const
			{
				for (Enumerator e = GetEnumerator(); e.MoveNext();)
					if (proc(e.getCurrentKey(), e.getCurrentValue()))
						return;
			}
			template <bool (*proc)(const T& key, const T& value)>
			void ForEachAbortableConst() const { ForEachAbortableConst(proc); }

			void ForEach(void (*proc)(const T& key, T& value))
			{
				for (Enumerator e = GetEnumerator(); e.MoveNext();)
					proc(e.getCurrentKey(), e.getCurrentValue());
			}
			template <void (*proc)(const T& key, T& value)>
			void ForEach() { ForEach(proc); }

			void ForEachConst(void (*proc)(const T& key, const T& value)) const
			{
				for (Enumerator e = GetEnumerator(); e.MoveNext();)
					proc(e.getCurrentKey(), e.getCurrentValue());
			}
			template <void (*proc)(const T& key, const T& value)>
			void ForEachConst() const { ForEachConst(proc); }

			//////////////////////////////////////////////////////////////////////////

			int32 getPrimeCapacity() const { return m_entryLength; }
			int32 getCount() const { return m_count - m_freeCount; }
		private:
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

			void Initialize(int capacity)
			{
				int primeCapacity = HashHelpers::GetPrime(capacity);
				m_bucketsLength = primeCapacity;
				m_buckets = new int[primeCapacity];
				for (int i = 0; i < m_bucketsLength; i++)
				{
					m_buckets[i] = -1;
				}
				m_entries = new Entry[primeCapacity];
				m_entryLength = primeCapacity;
				m_freeList = -1;
			}

			void Insert(const T& item, const S& value, bool add)
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
						Resize(m_count*2);
						index = hash % m_bucketsLength;
					}
					freeList = m_count;
					m_count++;
				}
				m_entries[freeList].hashCode = hash;
				m_entries[freeList].next = m_buckets[index];
				m_entries[freeList].data = item;
				m_entries[freeList].value = value;
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