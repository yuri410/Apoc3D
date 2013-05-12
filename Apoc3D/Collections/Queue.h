#pragma once

#ifndef APOC3D_FASTQUEUE_H
#define APOC3D_FASTQUEUE_H

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
#include "apoc3d/Common.h"

namespace Apoc3D
{
	namespace Collections
	{
		template<typename T>
		class Queue
		{
		public:

			Queue()
				: m_head(0), m_tail(0), m_size(0), m_arrLength(4)
			{
				m_array = new T[4];
			}
			explicit Queue(int capacity)
				: m_head(0), m_tail(0), m_size(0), m_arrLength(capacity)
			{
				m_array = new T[capacity];
			}
			~Queue()
			{
				delete[] m_array;
			}


			Queue(const Queue& another)
				: m_head(another.m_head), m_tail(another.m_tail), m_size(another.m_size), m_arrLength(another.m_arrLength)
			{
				m_array = new T[m_arrLength];
				for (int i=0;i<m_size;i++)
					m_array[i] = another.m_array[i];
			}

			Queue& operator=(const Queue& rhs)
			{
				delete[] m_array;

				m_head = rhs.m_head;
				m_tail = rhs.m_tail;
				m_size = rhs.m_size;
				m_arrLength = rhs.m_arrLength;

				m_array = new T[m_arrLength];
				for (int i=0;i<m_size;i++)
					m_array[i] = rhs.m_array[i];

				return *this; 
			}

			void Clear()
			{
				//if (m_head<m_tail)
				//{
				//	memset(m_array+m_head, 0, m_size * sizeof(T));
				//}
				//else
				//{
				//	memset(m_array+m_head, 0, (m_arrLength - m_head)*sizeof(T));
				//	memset(m_array, 0, m_tail * sizeof(T));
				//}
				m_head = 0;
				m_tail = 0;
				m_size = 0;
			}
			bool Replace(const T& item, const T& item2)
			{
				int index = m_head;
				int counter = m_size;

				while (counter-- > 0)
				{
					if (m_array[index] == item)
					{
						m_array[index] = item2;
						return true;
					}

					index = (index + 1) % m_arrLength;
				}
				return false;
			}
			bool Contains(const T& item) const
			{
				int index = m_head;
				int counter = m_size;

				while (counter-- > 0)
				{
					if (m_array[index] == item)
					{
						return true;
					}

					index = (index + 1) % m_arrLength;
				}
				return false;
			}

			void CopyTo(T* dest) const
			{
				int remaining = m_size;

				if (remaining > 0)
				{
					int numHeadToEnd = (m_arrLength - m_head < remaining) ? (m_arrLength - m_head) : remaining;
					for (int i=0;i<numHeadToEnd;i++)
					{
						dest[i] = m_array[i+m_head];
					}
					//memcpy(dest,m_array+m_head,num3*sizeof(T));
					remaining -= numHeadToEnd;
					if (remaining > 0)
					{
						for (int i=0;i<remaining;i++)
						{
							dest[i + m_arrLength-m_head] = m_array[i];
						}
						//memcpy(dest+(m_arrLength-m_head)*sizeof(T), m_array, num2*sizeof(T));
						//Array.Copy(m_array, 0, m_array, (arrayIndex + m_arrLength) - m_head, num2);
					}
				}
			}

			void DequeueOnly()
			{
				//memset(m_array+m_head, 0, sizeof(T));

				m_head = (m_head + 1) % m_arrLength;
				m_size--;
			}
			T Dequeue()
			{
				const T& local = m_array[m_head];
				//memset(m_array+m_head, 0, sizeof(T));

				m_head = (m_head + 1) % m_arrLength;
				m_size--;

				return local;
			}

			void Enqueue(const T& item)
			{
				if (m_size == m_arrLength)
				{
					int newCapacity = m_arrLength * 2;
					if (newCapacity < m_arrLength + 4)
					{
						newCapacity = m_arrLength + 4;
					}
					SetCapacity(newCapacity);
				}
				m_array[m_tail] = item;
				m_tail = (m_tail + 1) % m_arrLength;
				m_size++;
			}

			int getCount() const { return m_size; }

			const T& GetElement(int i) const { return m_array[(m_head + i) % m_arrLength]; }
			void SetElement(int i, const T& value) { m_array[(m_head + i) % m_arrLength] = value; }

			const T& Head() const { return m_array[m_head]; }
			const T& Tail() const { return GetElement(m_size - 1); }
		private:
			void SetCapacity(int capacity)
			{
				T* destinationArray = new T[capacity];
				if (m_size > 0)
				{
					if (m_head < m_tail)
					{
						for (int i=0;i<m_size;i++)
						{
							destinationArray[i] = m_array[i+m_head];
						}
						//memcpy(destinationArray, m_array+m_head, m_size * sizeof(T));
					}
					else
					{
						for (int i=m_head;i<m_arrLength;i++)
						{
							destinationArray[i-m_head] = m_array[i];
						}
						//memcpy(destinationArray, m_array+m_head, (m_arrLength-m_head) * sizeof(T));
						for (int i=0;i<m_tail;i++)
						{
							destinationArray[m_arrLength-m_head+i] = m_array[i];
						}
						//memcpy(destinationArray+(m_arrLength-m_head), m_array, (m_tail) * sizeof(T));
					}
				}
				delete[] m_array;
				m_array = destinationArray;
				m_arrLength = capacity;
				m_head = 0;
				m_tail = (m_size == capacity) ? 0 : m_size;
			}

			T* m_array;
			int32 m_arrLength;

			int32 m_head;
			int32 m_tail;
			int32 m_size;
		};
	}
}
#endif