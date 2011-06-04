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
#include "ExistTable.h"
#include "Apoc3DException.h"

template class Apoc3D::Collections::ExistTable<Apoc3D::Core::Resource*>;

namespace Apoc3D
{
	namespace Collections
	{
		
		const int primes[] = { 
			3,		7,		11,		17,		23,		29,		37,		47,		59,		71,		89,		107,		131,		163, 
			197,	239,	293,	353,	431,	521,	631,	761,	919,	1103,	1327,	1597,		1931,		2333,
			2801,	3371,	4049,	4861,	5839,	7013,	8419,	10103,	12143,	14591,	17519,	21023,		25229,		30293,
			36353,	43627,	52361,	62851,	75431,	90523,	108631,	130363,	156437, 187751, 225307, 270371,		324449,		389357,
			467237, 560689, 672827, 807403, 968897, 1162687,1395263,1674319,2009191,2411033,2893249,3471899,	4166287,	4999559,
			5999471,7199369
		};
		bool IsPrime(int candidate);
		int GetPrime(int min)
		{
			for (int i = 0; i < 72; i++)
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
		bool IsPrime(int candidate)
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


		template<typename T>
		void ExistTable<T>::Initialize(int capacity)
		{
			int prime = GetPrime(capacity);
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

		template<typename T>
		void ExistTable<T>::Resize()
		{
			int prime = GetPrime(m_count * 2);
			int* numArray = new int[prime];
			for (int i = 0; i < prime; i++)
			{
				numArray[i] = -1;
			}
			Entry* destinationArray = new Entry[prime];
			memcpy(destinationArray, m_entries, m_count * sizeof(Entry));
			for (int j = 0; j < m_count; j++)
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

		template<typename T>
		void ExistTable<T>::Insert(const T& item, bool add)
		{
			int freeList;
			if (!m_buckets)
			{
				Initialize(0);
			}
			int num = m_comparer->GetHashCode(item) & 2147483647;
			int index = num % m_bucketsLength;
			for (int i = m_buckets[index]; i >= 0; i = m_entries[i].next)
			{
				if ((m_entries[i].hashCode == num) &&
					m_comparer->Equals(m_entries[i].data, item))
				{
					if (add)
					{
						throw Apoc3DException::createException(EX_Duplicate, L"");
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
					index = num % m_bucketsLength;
				}
				freeList = m_count;
				m_count++;
			}
			m_entries[freeList].hashCode = num;
			m_entries[freeList].next = m_buckets[index];
			m_entries[freeList].data = item;
			m_buckets[index] = freeList;
		}


	}
}