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
					std::swap(arr[i], arr[j]);

					i++; j--;
				}
			}

			if (left < j) QuickSort(arr, left, j);
			if (i < right) QuickSort(arr, i, right);
		}

		template<typename T, int (* comparer)(const T& a, const T& b)>
		void QuickSort(T* arr, int left, int right)
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
					std::swap(arr[i], arr[j]);

					i++; j--;
				}
			}

			if (left < j) QuickSort<T, comparer>(arr, left, j);
			if (i < right) QuickSort<T, comparer>(arr, i, right);
		}

		template<typename T, typename Func>
		void QuickSort(T* arr, int left, int right, Func comparer)
		{
			int i = left, j = right;
			const T pivot = arr[(left + right) / 2];

			while (i <= j)
			{
				while (comparer(arr[i], pivot) < 0)
					i++;
				while (comparer(arr[j], pivot) > 0)
					j--;

				if (i <= j)
				{
					std::swap(arr[i], arr[j]);

					i++; j--;
				}
			}

			if (left < j) QuickSort(arr, left, j, comparer);
			if (i < right) QuickSort(arr, i, right, comparer);
		}


		template <typename T, typename CT>
		class ListBase
		{
		public:

			void DeleteAndClear()
			{
				for (int32 i = 0; i < m_count; i++)
					delete m_elements[i];
				Clear();
			}

			void Clear() { m_count = 0; }

			int32 IndexOf(const T& item) const
			{
				for (int32 i = 0; i < m_count; i++)
				{
					if (m_elements[i] == item)
						return i;
				}
				return -1;
			}

			bool Contains(const T& item) const { return IndexOf(item) != -1; }

			bool Remove(const T& item)
			{
				int32 index = IndexOf(item);
				if (index != -1)
				{
					RemoveAt(index);
					return true;
				}
				return false;
			}
			void RemoveChecked(const T& item)
			{
				if (!Remove(item))
					throw AP_EXCEPTION(ExceptID::InvalidOperation, L"Removing failed.");
			}
			void RemoveAt(int32 index)
			{
				assert(index >= 0 && index < m_count);

				if (index < m_count - 1)
				{
					for (int32 i = index + 1; i < m_count; i++)
					{
						m_elements[i - 1] = m_elements[i];
					}
				}

				m_count--;
			}
			void RemoveAtSwapping(int32 index)
			{
				assert(index >= 0 && index < m_count);

				m_count--;
				std::swap(m_elements[index], m_elements[m_count]);
			}
			void RemoveRange(int32 start, int32 count)
			{
				if (count > 0)
				{
					assert(start >= 0 && start + count <= m_count);

					m_count -= count;

					if (start < m_count)
					{
						for (int32 i = start; i < m_count; i++)
						{
							m_elements[i] = m_elements[i + count];
						}
					}
				}
			}

			void Reverse()
			{
				if (m_elements == nullptr)
					return;

				for (int i = 0; i < m_count / 2; i++)
				{
					int32 another = m_count - i - 1;
					T temp = m_elements[i];
					m_elements[i] = m_elements[another];
					m_elements[another] = temp;
				}
			}

			T* AllocateArrayCopy() const
			{
				T* result = new T[m_count];
				CopyTo(result, m_elements, m_count);
				return result;
			}

			void Sort() { if (m_count > 0) QuickSort(m_elements, 0, m_count - 1); }
			template <int(*comparer)(const T&, const T&)>
			void Sort() { if (m_count > 0) QuickSort<T, comparer>(m_elements, 0, m_count - 1); }
			template <typename Func>
			void Sort(Func comparer) { if (m_count > 0) QuickSort(m_elements, 0, m_count - 1, comparer); }

		
			int32 getCount() const { return m_count; }

		protected:
			ListBase() { }
			ListBase(CT elem) : m_elements(elem) { }
			ListBase(CT elem, int32 count)
				: m_elements(elem), m_count(count) { }
			
			~ListBase() { }

			static void CopyTo(T* dest, const T* src, int32 count)
			{
				if (std::is_trivially_copyable<T>::value)
				{
					memcpy(dest, src, count*sizeof(T));
				}
				else
				{
					for (int32 i = 0; i < count; i++)
						dest[i] = src[i];
				}
			}

			CT m_elements;
			int m_count = 0;

		};

		template <typename T, int32 MaxSize>
		class FixedList : public ListBase<T, T[MaxSize]>
		{
		public:
			FixedList() { }
			FixedList(std::initializer_list<T> l)
			{
				assert(l.size() < MaxSize);

				AddList(l);
			}

			void Add(const T& val)
			{
				assert(m_count < MaxSize);
				if (m_count < MaxSize)
					m_elements[m_count++] = val;
			}

			void Add(T&& val)
			{
				assert(m_count < MaxSize);
				if (m_count < MaxSize)
					m_elements[m_count++] = std::move(val);
			}

			template <int32 N>
			void AddArray(const T(&val)[N]) { AddArray(val, N); }
			void AddArray(const T* val, int32 count)
			{
				assert(m_count + count <= MaxSize);
				for (int32 i = 0; i < count && m_count < MaxSize; i++)
					m_elements[m_count++] = val[i];
			}

			template<int32 FixedSize>
			void AddList(const FixedList<T,FixedSize>& other) { AddArray(other.m_elements, other.m_count); }
			void AddList(std::initializer_list<T> l)
			{
				assert(m_count + l.size() <= MaxSize);
				for (auto iter = l.begin(); iter != l.end() && m_count < MaxSize; ++iter)
					m_elements[m_count++] = e;
			}

			const T* getElements() const { return m_elements; }
			T* getElements() { return m_elements; }

			T& operator [](int32 i)
			{
				assert(i >= 0 && i < m_count);
				return m_elements[i];
			}
			const T& operator [](int32 i) const
			{
				assert(i >= 0 && i < m_count);
				return m_elements[i];
			}

			const T* begin() const { return m_elements; }
			const T* end() const { return m_elements + m_count; }

			T* begin() { return m_elements; }
			T* end() { return m_elements + m_count; }

		};

		template <typename T>
		class List : public ListBase<T, T*>
		{
		public:
			List() : ListBase(nullptr), m_length(4) { }

			explicit List(int capacity)  
				: ListBase(nullptr), m_length(capacity)  { }


			List(std::initializer_list<T> l) 
				: ListBase(nullptr, l.size()),  m_length(l.size())
			{
				EnsureElements();
				T* dst = m_elements;
				for (const T& e : l)
					*dst++ = e;
			}

			List(const List& another)
				: ListBase(nullptr, another.m_count), m_length(another.m_length)
			{
				if (another.m_elements)
				{
					m_elements = new T[m_length];
				}

				CopyTo(m_elements, another.m_elements, m_count);
			}
			List(List&& other)
				: ListBase(other.m_elements, other.m_count), m_length(other.m_length)
			{
				other.m_elements = nullptr;
			}
			~List()
			{
				delete[] m_elements;
			}

			List& operator=(const List& rhs)
			{
				if (this != &rhs)
				{
					if (m_length != rhs.m_length)
					{
						delete[] m_elements;
						m_elements = nullptr;

						m_length = rhs.m_length;

						if (rhs.m_elements)
							m_elements = new T[m_length];
					}
					else
					{
						if (m_elements == nullptr && rhs.m_elements)
							m_elements = new T[m_length];
					}

					m_count = rhs.m_count;

					CopyTo(m_elements, rhs.m_elements, m_count);
				}
				return *this; 
			}
			List& operator=(List&& rhs)
			{
				if (this != &rhs)
				{
					delete[] m_elements;
					m_elements = rhs.m_elements;
					m_length = rhs.m_length;
					m_count = rhs.m_count;

					rhs.m_elements = nullptr;
				}
				return *this;
			}


			void Add(const T& val)
			{
				EnsureElementIncrSize();
				m_elements[m_count++] = val;
			}

			void Add(T&& val)
			{
				EnsureElementIncrSize();
				m_elements[m_count++] = std::move(val);
			}

			template <int32 N>
			void AddArray(const T (&val)[N]) { AddArray(val, N); }
			void AddArray(const T* val, int32 count)
			{
				EnsureElementIncrSize(count);

				for (int32 i = 0; i < count; i++)
					m_elements[m_count++] = val[i];
			}

			template<typename = void> // helps overload resolution with initializer_list
			void AddList(const List<T>& other) { AddArray(other.m_elements, other.m_count); }
			template<int32 FixedSize>
			void AddList(const FixedList<T,FixedSize>& other) { AddArray(other.getElements(), other.getCount()); }
			void AddList(std::initializer_list<T> l)
			{
				EnsureElementIncrSize((int32)l.size());

				for (const T& e : l)
					m_elements[m_count++] = e;
			}

			void ResizeDiscard(int32 newSize)
			{
				delete[] m_elements;
				m_elements = new T[newSize];
				m_length = newSize;
			}
			void Resize(int32 newSize)
			{
				assert(newSize >= m_count);
				T* newArr = new T[newSize];

				CopyTo(newArr, m_elements, m_count);

				delete[] m_elements;
				m_elements = newArr;
				m_length = newSize;
			}
			void Reserve(int32 newCount)
			{
				if (m_length < newCount)
					Resize(newCount);
				else 
					EnsureElements();
				m_count = newCount;
			}
			void ReserveDiscard(int32 newCount)
			{
				if (m_length < newCount)
					ResizeDiscard(newCount);
				else 
					EnsureElements();
				m_count = newCount;
			}
			void Trim()
			{
				if (m_count == 0)
					return;

				if (m_count < m_length)
				{
					Resize(m_count);
				}
			}

			void Insert(int32 index, const T& item)
			{
				if (m_count == m_length)
				{
					Resize(m_length * 2);
				}
				else
				{
					EnsureElements();
				}

				if (index < m_count)
				{
					for (int32 i = m_count; i > index; i--)
					{
						m_elements[i] = m_elements[i - 1];
					}
				}
				m_elements[index] = item;
				m_count++;
			}

			
			//////////////////////////////////////////////////////////////////////////

			int32 getCapacity() const { return m_length; }
			T* getElements() const { return m_elements; }

			T& operator [](int32 i) const
			{
				assert(i >= 0 && i < m_count);
				return m_elements[i];
			}

			T* begin() const { return m_elements; }
			T* end() const { return m_elements + m_count; }

		private:
			void EnsureElementIncrSize()
			{
				if (m_length <= m_count)
				{
					Resize(m_length == 0 ? 4 : (m_length * 2));
				}
				else
				{
					EnsureElements();
				}
			}

			void EnsureElementIncrSize(int32 count)
			{
				int32 finalCount = m_count + count;
				if (m_length < finalCount)
				{
					int32 nextSize = m_length == 0 ? 4 : (m_length * 2);
					if (nextSize < finalCount)
						nextSize = finalCount;
					Resize(nextSize);
				}
				else
				{
					m_length = count;
					EnsureElements();
				}
			}

			void EnsureElements()
			{
				if (m_elements == nullptr)
				{
					m_elements = new T[m_length];
				}
			}

			int m_length = 0;
		};

	}
}
#endif