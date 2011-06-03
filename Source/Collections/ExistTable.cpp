
#include "ExistTable.h"
#include "Apoc3DException.h"

namespace Apoc3D
{
	namespace Collections
	{
		template<typename T>
		const int ExistTable<T>::HashHelpers::primes[] = { 
			3, 7, 11, 17, 23, 29, 37, 47, 59, 71, 89, 107, 131, 163, 197, 239, 
			293, 353, 431, 521, 631, 761, 919, 1103, 1327, 1597, 1931, 2333, 2801, 3371, 4049, 4861, 
			5839, 7013, 8419, 10103, 12143, 14591, 17519, 21023, 25229, 30293, 36353, 43627, 52361, 62851, 75431, 90523, 
			108631, 130363, 156437, 187751, 225307, 270371, 324449, 389357, 467237, 560689, 672827, 807403, 968897, 1162687, 1395263, 1674319, 
			2009191, 2411033, 2893249, 3471899, 4166287, 4999559, 5999471, 7199369
		};

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
			m_entries[index] = freeList;
		}
	}
}