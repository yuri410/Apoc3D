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
#ifndef FASTQUEUE_H
#define FASTQUEUE_H

#include "Common.h"

namespace Apoc3D
{
	namespace Collections
	{
		template<typename T>
		class APAPI FastQueue
		{
		private:
			//static const int DefaultCapacity = 4;
			//static const int GrowFactor = 200;

			T* m_array;
			int32 m_arrLength;

			int32 m_head;
			int32 m_tail;
			int32 m_size;

			FastQueue(const FastQueue& another) { }
		public:
			int getCount() const { return m_size; }

			FastQueue()
				: m_head(0), m_tail(0), m_size(0), m_arrLength(4)
			{
				m_array = new T[4];
			}
			FastQueue(int capacity)
				: m_head(0), m_tail(0), m_size(0), m_arrLength(capacity)
			{
				m_array = new T[capacity];
			}
			~FastQueue()
			{
				delete[] m_array;
			}
			void Clear()
			{
				if (m_head<m_tail)
				{
					memset(m_array+m_head, 0, m_size * sizeof(T));
				}
				else
				{
					memset(m_array+m_head, 0, (m_arrLength - m_head)*sizeof(T));
					memset(m_array, 0, m_tail * sizeof(T));
				}
				m_head = 0;
				m_tail = 0;
				m_size = 0;
			}

			bool Contains(const T& item) const
			{
				int index = m_head;
				int num2 = m_size;

				while (num2-- > 0)
				{
					if (m_array[index] == item)
					{
						return true;
					}
					
					index = (index + 1) % m_arrLength;
				}
				return false;
			}

			void CopyTo(const char* dest) const
			{
				int num2 = m_size;

				if (num2 != 0)
				{
					int num3 = ((m_arrLength - m_head) < num2) ? (m_arrLength - m_head) : num2;
					memcpy(dest,m_array+m_head,num3*sizeof(T));
					num2 -= num3;
					if (num2 > 0)
					{
						memcpy(dest+(m_arrLength-m_head)*sizeof(T), m_array, num2*sizeof(T));
						//Array.Copy(m_array, 0, m_array, (arrayIndex + m_arrLength) - m_head, num2);
					}
				}
			}

			T Dequeue()
			{
				T local = m_array[m_head];
				memset(m_array+m_head, 0, sizeof(T));

				m_head = (m_head + 1) % m_arrLength;
				m_size--;

				return local;
			}

			void Enqueue(const T& item)
			{
				if (m_size == m_arrLength)
				{
					int capacity = (int)((m_arrLength * 200L) / 100L);
					if (capacity < (m_arrLength + 4))
					{
						capacity = m_arrLength + 4;
					}
					SetCapacity(capacity);
				}
				m_array[m_tail] = item;
				m_tail = (m_tail + 1) % m_arrLength;
				m_size++;
			}
			void SetElement(int i, const T& value)
			{
				m_array[(m_head + i) % m_arrLength] = value;
			}

			const T& GetElement(int i) const
			{
				return m_array[(m_head + i) % m_arrLength];
			}
			const T& Head() const
			{
				return m_array[m_head];
			}
			const T& Tail() const
			{
				return GetElement(m_size - 1);
			}
			void SetCapacity(int capacity)
			{
				T* destinationArray = new T[capacity];
				if (m_size > 0)
				{
					if (m_head < m_tail)
					{
						memcpy(destinationArray, m_array+m_head, m_size * sizeof(T));
					}
					else
					{
						memcpy(destinationArray, m_array+m_head, (m_arrLength-m_head) * sizeof(T));
						memcpy(destinationArray, m_array+(m_arrLength-m_head), (m_tail) * sizeof(T));
					}
				}
				delete[] m_array;
				m_array = destinationArray;
				m_arrLength = capacity;
				m_head = 0;
				m_tail = (m_size == capacity) ? 0 : m_size;
			}

		};
	}
}
#endif