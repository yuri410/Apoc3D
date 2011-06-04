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
#ifndef EXISTTABLE_H
#define EXISTTABLE_H

#include "Common.h"
#include "CollectionsCommon.h"

namespace Apoc3D
{
	namespace Collections
	{
		template<typename T>
		class APAPI ExistTable
		{
		public:
			class Enumerator
			{
			private:
				const ExistTable<T>* m_dict;
				int m_index;
				const T* m_current;

			public:
				const T* getCurrent() const { return m_current; }

				Enumerator(const ExistTable<T>* dict)
					: m_dict(dict), m_index(0), m_current(0)
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
			};
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
			const IEqualityComparer<T>* m_comparer;

			void Initialize(int capacity);
			void Resize();
			void Insert(const T& item, bool add);

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

			ExistTable()
				:  m_buckets(0), m_bucketsLength(0), m_count(0), 
				m_entries(0), m_entryLength(0), m_freeCount(0), m_freeList(0)
			{
				m_comparer = IEqualityComparer<T>::Default;
				assert(m_comparer);

				int capacity = 40;
				if (capacity > 0)
				{
					Initialize(capacity);
				}
			}

			ExistTable(int capacity, const IEqualityComparer<T>* comparer)
				: m_comparer(comparer), m_buckets(0), m_bucketsLength(0), m_count(0), 
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
			bool Exists(const T& item) const
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

			Enumerator GetEnumerator() const
			{
				return Enumerator(this);
			}
		};
	}
}

#endif