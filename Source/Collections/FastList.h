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
#ifndef FASTLIST_H
#define FASTLIST_H

#include "Common.h"

namespace Apoc3D
{
	namespace Collections
	{
		template<typename T>
		class List
		{
		private:
			T* m_elements;

			int m_internalPointer;

			int m_length;


		public:
			int32 getCount() const { return m_internalPointer; }
			T* getInternalPointer() const { return m_elements; }

			List(const List& another)
				: m_internalPointer(another.m_internalPointer), m_length(another.m_length)
			{
				m_elements = new T[m_length];
				for (int i=0;i<m_length;i++)
					m_elements[i] = another.m_elements[i];
			}
			List(int capacity)
				: m_internalPointer(0), m_length(capacity)
			{
				m_elements = new T[capacity];
			}
			List()
				: m_internalPointer(0), m_length(4)
			{
				m_elements = new T[m_length];
			}
			~List()
			{
				delete[] m_elements;
			}

			void Add(const T& val)
			{
				if (m_length<=m_internalPointer)
				{
					Resize(!m_length ? 4 : (m_length * 2));
				}
				m_elements[m_internalPointer++] = val;
			}
			
			void Clear()
			{
				memset(m_elements, 0, m_internalPointer*sizeof(T));
				m_internalPointer = 0;
			}

			int32 IndexOf(const T& item) const
			{
				for (int i = 0; i < m_internalPointer; i++)
				{
					if (m_elements[i] == item)
					{
						return i;
					}
				}
				return -1;
			}
			bool Remove(const T& item)
			{
				int32 index = IndexOf(item);
				if (index!=-1)
				{
					RemoveAt(index);
					return true;
				}
				return false;
			}
			void RemoveAt(int32 index)
			{
				if (index == m_internalPointer - 1)
				{
					m_internalPointer--;
					memset(m_elements+index,0, sizeof(T));
				}
				else
				{
					T* destinationArray = new T[m_length - 1];
					for (int i=0;i<index;i++)
					{
						destinationArray[i] = m_elements[i];
					}

					if (m_internalPointer - ++index > 0)
					{
						for (int i=index;i<m_internalPointer;i++)
						{
							destinationArray[i-1] = m_elements[i];
						}
						//memcpy(destinationArray+ index-1, m_elements+index, (m_internalPointer-index)*sizeof(T));
					}
					delete[] m_elements;
					m_elements = destinationArray;

					m_length--;
					m_internalPointer--;
				}
			}
			void RemoveRange(int32 start, int32 count)
			{
				if (count > 0)
				{
					m_internalPointer -= count;

					if (start < m_internalPointer)
					{
						for (int i=start;i<m_internalPointer;i++)
						{
							m_elements[i] = m_elements[i+count];
						}
						//memcpy( m_elements+start, m_elements+start + count, (m_internalPointer-start)*sizeof(T));
					}
					for (int i=0;i<count;i++)
					{
						m_elements[i+m_internalPointer] = T();
					}
					//memset(m_elements+m_internalPointer, 0, count*sizeof(T));
				}
			}
			void ResizeDiscard(int newSize)
			{
				T* newArr = new T[newSize];
				delete[] m_elements;
				m_elements = newArr;
				m_length = newSize;
			}
			void Resize(int newSize)
			{
				T* newArr = new T[newSize];
				for (int i=0;i<m_internalPointer;i++)
				{
					newArr[i] = m_elements[i];
				}
				//memcpy(newArr, m_elements, m_internalPointer*sizeof(T));
				delete[] m_elements;
				m_elements = newArr;
				m_length = newSize;
			}

			void Insert(int32 index, const T& item)
			{
				if (m_internalPointer == m_length)
				{
					Resize(m_length+1);
				}
				if (index<m_internalPointer)
				{
					for (int i=index;i<m_internalPointer;i++)
					{
						m_elements[i+1] = m_elements[i];
					}
					//memcpy(m_elements+index+1, m_elements+index, sizeof(T)*(m_internalPointer-index));
				}
				m_elements[index] = item;
				m_internalPointer++;
			}

			bool Contains(const T& item) const
			{
				for (int i = 0; i < m_internalPointer; i++)
				{
					if (m_elements[i] == item)
					{
						return true;
					}
				}
				return false;
			}


			T& operator [](int32 i) const
			{
				return m_elements[i];
			}
		};

		template<typename T>
		class FastList
		{
		private:
			T* m_elements;

			int m_internalPointer;

			int m_length;

			
		public:
			int32 getCount() const { return m_internalPointer; }
			T* getInternalPointer() const { return m_elements; }

			FastList(const FastList& another)
				: m_internalPointer(another.m_internalPointer), m_length(another.m_length)
			{
				m_elements = new T[m_length];
				memcpy(m_elements, another.m_elements, m_length * sizeof(T));
			}
			FastList(int capacity)
				: m_internalPointer(0), m_length(capacity)
			{
				m_elements = new T[capacity];
			}
			FastList()
				: m_internalPointer(0), m_length(4)
			{
				m_elements = new T[m_length];
			}
			~FastList()
			{
				delete[] m_elements;
			}

			void Add(const T& val)
			{
				if (m_length<=m_internalPointer)
				{
					Resize(!m_length ? 4 : (m_length * 2));
				}
				m_elements[m_internalPointer++] = val;
			}
			void Trim()
			{
				if (m_internalPointer<m_length)
				{
					 Resize(m_internalPointer);
				}
			}
			void TrimClear()
			{
				if (m_internalPointer<m_length)
				{
					memset(m_elements + m_internalPointer, 0, (m_length - m_internalPointer)*sizeof(T));
				}
			}
			void FastClear()
			{
				m_internalPointer = 0;
			}
			void Clear()
			{
				memset(m_elements, 0, m_internalPointer*sizeof(T));
				m_internalPointer = 0;
			}

			int32 IndexOf(const T& item) const
			{
				for (int i = 0; i < m_internalPointer; i++)
				{
					if (m_elements[i] == item)
					{
						return i;
					}
				}
				return -1;
			}
			bool Remove(const T& item)
			{
				int32 index = IndexOf(item);
				if (index!=-1)
				{
					RemoveAt(index);
					return true;
				}
				return false;
			}
			void RemoveAt(int32 index)
			{
				if (index == m_internalPointer - 1)
				{
					m_internalPointer--;
					memset(m_elements+index,0, sizeof(T));
				}
				else
				{
					T* destinationArray = new T[m_length - 1];
					memcpy(destinationArray, m_elements, index * sizeof(T));

					if (m_internalPointer - ++index > 0)
					{
						memcpy(destinationArray+ index-1, m_elements+index, (m_internalPointer-index)*sizeof(T));
					}
					delete[] m_elements;
					m_elements = destinationArray;

					m_length--;
					m_internalPointer--;
				}
			}
			void RemoveRange(int32 start, int32 count)
			{
				if (count > 0)
				{
					m_internalPointer -= count;

					if (start < m_internalPointer)
					{
						memcpy( m_elements+start, m_elements+start + count, (m_internalPointer-start)*sizeof(T));
					}

					memset(m_elements+m_internalPointer, 0, count*sizeof(T));
				}
			}
			void ResizeDiscard(int newSize)
			{
				T* newArr = new T[newSize];
				delete[] m_elements;
				m_elements = newArr;
				m_length = newSize;
			}
			void Resize(int newSize)
			{
				T* newArr = new T[newSize];
				memcpy(newArr, m_elements, m_internalPointer*sizeof(T));
				delete[] m_elements;
				m_elements = newArr;
				m_length = newSize;
			}

			void Insert(int32 index, const T& item)
			{
				if (m_internalPointer == m_length)
				{
					Resize(m_length+1);
				}
				if (index<m_internalPointer)
				{
					memcpy(m_elements+index+1, m_elements+index, sizeof(T)*(m_internalPointer-index));
				}
				m_elements[index] = item;
				m_internalPointer++;
			}

			bool Contains(const T& item) const
			{
				for (int i = 0; i < m_internalPointer; i++)
				{
					if (m_elements[i] == item)
					{
						return true;
					}
				}
				return false;
			}


			T& operator [](int32 i) const
			{
				return m_elements[i];
			}
			//const T& operator [](int32 i) const
			//{
			//	return m_elements[i];
			//}

		};
	}
}
#endif