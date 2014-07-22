#pragma once

#ifndef APOC3D_FASTLIST_H
#define APOC3D_FASTLIST_H

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
		void QuickSort(T* arr, int left, int right)
		{
			int i = left, j = right;
			const T pivot = arr[(left + right) / 2];

			while (i<=j)
			{
				while (arr[i] < pivot)
					i++;
				while (arr[j] > pivot)
					j--;

				if (i<=j)
				{
					const T tmp = arr[i];
					arr[i] = arr[j];
					arr[j] = tmp;
					i++; j--;
				}
			}

			if (left < j) QuickSort(arr, left, j);
			if (i < right) QuickSort(arr, i, right);
		}

		template<typename T>
		void QuickSort(T* arr, int left, int right, int (* comparer)(const T& a, const T& b))
		{
			int i = left, j = right;
			const T pivot = arr[(left + right) / 2];

			while (i<=j)
			{
				while (comparer(arr[i], pivot) < 0)
					i++;
				while (comparer(arr[j], pivot) > 0)
					j--;

				if (i<=j)
				{
					const T tmp = arr[i];
					arr[i] = arr[j];
					arr[j] = tmp;
					i++; j--;
				}
			}

			if (left < j) QuickSort(arr, left, j, comparer);
			if (i < right) QuickSort(arr, i, right, comparer);
		}


		template<typename T>
		class List
		{
		private:
			static const bool CanDirectCopy = std::is_trivially_copyable<T>::value;
		public:
			explicit List(int capacity)
				: m_internalPointer(0), m_length(capacity)
			{
				m_elements = new T[capacity];
			}
			List()
				: m_internalPointer(0), m_length(4)
			{
				m_elements = new T[m_length];
			}
			List(const List& another)
				: m_internalPointer(another.m_internalPointer), m_length(another.m_length)
			{
				m_elements = new T[m_length];

				if (CanDirectCopy)
				{
					memcpy(m_elements, another.m_elements, m_internalPointer * sizeof(T));
				}
				else
				{
					for (int i=0;i<m_internalPointer;i++)
						m_elements[i] = another.m_elements[i];
				}
			}
			~List()
			{
				delete[] m_elements;
			}
			List& operator=(const List& rhs)
			{
				if (&rhs == this)
					return *this;

				delete[] m_elements;
				m_internalPointer = rhs.m_internalPointer;
				m_length = rhs.m_length;
				m_elements = new T[m_length];

				if (CanDirectCopy)
				{
					memcpy(m_elements, rhs.m_elements, m_internalPointer * sizeof(T));
				}
				else
				{
					for (int i=0;i<m_internalPointer;i++)
						m_elements[i] = rhs.m_elements[i];
				}

				return *this; 
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
				m_internalPointer = 0;
			}

			void DeleteAndClear()
			{
				for (int32 i=0;i<getCount();i++)
					delete m_elements[i];
				Clear();
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
				assert (index < m_internalPointer);
				assert(index>=0);

				if (index < m_internalPointer-1)
				{
					for (int i=index+1 ;i<m_internalPointer; i++)
					{
						m_elements[i-1] = m_elements[i];
					}
				}

				m_internalPointer--;
			}
			void RemoveRange(int32 start, int32 count)
			{
				if (count>0)
				{
					m_internalPointer -= count;

					if (start < m_internalPointer)
					{
						for (int i=start; i<m_internalPointer; i++)
						{
							m_elements[i] = m_elements[i+count];
						}
					}
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
				if (CanDirectCopy)
				{
					memcpy(newArr, m_elements, m_internalPointer*sizeof(T));
				}
				else
				{
					for (int i=0;i<m_internalPointer;i++)
					{
						newArr[i] = m_elements[i];
					}
				}
				delete[] m_elements;
				m_elements = newArr;
				m_length = newSize;
			}
			void Reserve(int newCount)
			{
				if (m_length < newCount)
					Reserve(newCount);
				m_internalPointer = newCount;
			}
			void ReserveDiscard(int newCount)
			{
				if (m_length < newCount)
					ResizeDiscard(newCount);
				m_internalPointer = newCount;
			}
			void Trim()
			{
				if (m_internalPointer<m_length)
				{
					Resize(m_internalPointer);
				}
			}

			void Insert(int32 index, const T& item)
			{
				if (m_internalPointer == m_length)
				{
					Resize(m_length+1);
				}
				if (index<m_internalPointer)
				{
					for (int i=m_internalPointer;i>index;i--)
					{
						m_elements[i] = m_elements[i-1];
					}
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

			void Reverse()
			{
				for (int i=0;i<m_internalPointer/2;i++)
				{
					int32 another = m_internalPointer-i-1;
					T temp = m_elements[i];
					m_elements[i] = m_elements[another];
					m_elements[another] = temp;
				}
			}

			T* AllocateArrayCopy() const
			{
				T* result = new T[getCount()];
				if (CanDirectCopy)
				{
					memcpy(result, m_elements, sizeof(T)*getCount());
				}
				else
				{
					for (int32 i=0;i<getCount();i++)
						result[i] = m_elements[i];
				}
				return result;
			}

			void Sort() { QuickSort(m_elements, 0, m_internalPointer-1); }
			void Sort(int (* comparer)(const T& a, const T& b)) { QuickSort(m_elements, 0, m_internalPointer-1, comparer); }

			T& operator [](int32 i) const
			{
				assert(i>=0);
				assert(i<m_internalPointer);
				return m_elements[i];
			}

			int32 getCapacity() const { return m_length; }
			int32 getCount() const { return m_internalPointer; }
			T* getInternalPointer() const { return m_elements; }
		private:

			T* m_elements;

			int m_internalPointer;

			int m_length;
		};


	}
}
#endif