#pragma once

#ifndef APOC3D_FASTLIST_H
#define APOC3D_FASTLIST_H

/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2011-2017 Tao Xin
 * 
 * This content of this file is subject to the terms of the Mozilla Public 
 * License v2.0. If a copy of the MPL was not distributed with this file, 
 * you can obtain one at http://mozilla.org/MPL/2.0/.
 * 
 * This program is distributed in the hope that it will be useful, 
 * WITHOUT WARRANTY OF ANY KIND; either express or implied. See the 
 * Mozilla Public License for more details.
 * 
 * ------------------------------------------------------------------------
 */

#include "CollectionsCommon.h"

namespace Apoc3D
{
	namespace Collections
	{
		template <typename T>
		struct Swapper
		{
			void operator()(T& a, T& b)
			{
				T* tmp = new (m_buf)T(std::move(a));
				a = std::move(b);
				b = std::move(*tmp);
				tmp->~T();
			}

		private:
			char m_buf[sizeof(T)];
		};

		template<typename T, int32(*comparer)(const T& a, const T& b)>
		void QuickSort(T* arr, int32 left, int32 right, Swapper<T>& swp = Swapper<T>())
		{
			int32 i = left, j = right;
			const T pivot = arr[(left + right) / 2];

			while (i <= j)
			{
				while (comparer(arr[i], pivot) < 0) i++;
				while (comparer(arr[j], pivot) > 0) j--;

				if (i <= j)
				{
					swp(arr[i], arr[j]);

					i++; j--;
				}
			}

			if (left < j) QuickSort<T, comparer>(arr, left, j, swp);
			if (i < right) QuickSort<T, comparer>(arr, i, right, swp);
		}

		template<typename T, typename Func>
		void QuickSort(T* arr, int32 left, int32 right, Func& comparer, Swapper<T>& swp = Swapper<T>())
		{
			int32 i = left, j = right;
			const T pivot = arr[(left + right) / 2];

			while (i <= j)
			{
				while (comparer(arr[i], pivot) < 0) i++;
				while (comparer(arr[j], pivot) > 0) j--;

				if (i <= j)
				{
					swp(arr[i], arr[j]);

					i++; j--;
				}
			}

			if (left < j) QuickSort(arr, left, j, comparer, swp);
			if (i < right) QuickSort(arr, i, right, comparer, swp);
		}

		template <typename T, typename SorterType, SorterType(*sorterCalc)(const T& a, const T& b)>
		void QuickSortWithSorter(T* arr, int32 left, int32 right, Swapper<T>& swp = Swapper<T>())
		{
			int32 i = left, j = right;
			SorterType pivot = sorterCalc(arr[(left + right) / 2]);

			while (i <= j)
			{
				while (sorterCalc(arr[i]) < pivot) i++;
				while (sorterCalc(arr[j]) > pivot) j--;

				if (i <= j)
				{
					swp(arr[i], arr[j]);

					i++; j--;
				}
			}

			if (left < j) QuickSortWithSorter<T, SorterType, sorterCalc>(arr, left, j, swp);
			if (i < right) QuickSortWithSorter<T, SorterType, sorterCalc>(arr, i, right, swp);
		}

		template <typename T, typename SorterType, typename SorterCalc>
		void QuickSortWithSorter(T* arr, int32 left, int32 right, SorterCalc& sorterCalc, Swapper<T>& swp = Swapper<T>())
		{
			int32 i = left, j = right;
			SorterType pivot = sorterCalc(arr[(left + right) / 2]);

			while (i <= j)
			{
				while (sorterCalc(arr[i]) < pivot) i++;
				while (sorterCalc(arr[j]) > pivot) j--;

				if (i <= j)
				{
					swp(arr[i], arr[j]);

					i++; j--;
				}
			}

			if (left < j) QuickSortWithSorter<T, SorterType>(arr, left, j, sorterCalc, swp);
			if (i < right) QuickSortWithSorter<T, SorterType>(arr, i, right, sorterCalc, swp);
		}

		template <typename T, typename ST>
		class ListBase
		{
		public:

			template <typename = typename std::enable_if<std::is_pointer<T>::value>>
			void DeleteAndClear()
			{
				T* elm = (T*)m_elements;

				for (int32 i = 0; i < m_count; i++)
					delete elm[i];
				Clear();
			}

			void Clear()
			{
				Utils::DoDestory(0, m_count, (T*)m_elements);
				m_count = 0;
			}

			int32 IndexOf(const T& item) const
			{
				T* elm = (T*)m_elements;

				for (int32 i = 0; i < m_count; i++)
				{
					if (elm[i] == item)
						return i;
				}
				return -1;
			}

			template <typename Func>
			int32 IndexOf(Func f) const
			{
				const T* elm = (const T*)m_elements;

				for (int32 i = 0; i < m_count; i++)
				{
					if (f(std::cref(elm[i])))
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
				{
					AP_EXCEPTION(ErrorID::InvalidOperation, L"Removing failed.");
				}
			}
			void RemoveAt(int32 index)
			{
				assert(index >= 0 && index < m_count);
				T* elm = (T*)m_elements;

				for (int32 i = index + 1; i < m_count; i++)
				{
					elm[i - 1] = std::move(elm[i]);
				}
				Utils::DoDestory(m_count - 1, 1, elm);
				m_count--;
			}
			template <typename DLT>
			void RemoveAt(DLT& deadList)
			{
				int32 removeCount = deadList.getCount();
				if (removeCount > 0)
				{
					T* elm = (T*)m_elements;

					int32 sourceCount = m_count;
					int32 dstCount = m_count;

					int32 curDeadIdx = 0;

					int32 writePos = deadList[curDeadIdx];
					assert(writePos >= 0);
					for (int32 i = deadList[curDeadIdx]; i < sourceCount; i++, writePos++)
					{
						// skip deleted
						while (curDeadIdx < removeCount && deadList[curDeadIdx] == i)
						{
							assert(curDeadIdx + 1 >= removeCount || deadList[curDeadIdx] < deadList[curDeadIdx + 1]);

							curDeadIdx++;
							i++;
							dstCount--;
						}

						if (i >= sourceCount)
							break;

						assert(i >= 0 && i < sourceCount);
						assert(writePos >= 0 && writePos < dstCount);
						elm[writePos] = std::move(elm[i]);
					}

					Utils::DoDestory(dstCount, sourceCount - dstCount, elm);
					m_count = dstCount;
				}
			}
			void RemoveAtSwapping(int32 index)
			{
				assert(index >= 0 && index < m_count);

				T* elm = (T*)m_elements;

				m_count--;
				std::swap(elm[index], elm[m_count]);
				Utils::DoDestory(m_count, 1, elm);
			}
			void RemoveRange(int32 start, int32 count)
			{
				if (count > 0)
				{
					assert(start >= 0 && start + count <= m_count);

					T* elm = (T*)m_elements;

					m_count -= count;

					for (int32 i = start; i < m_count; i++)
					{
						elm[i] = std::move(elm[i + count]);
					}
					
					Utils::DoDestory(m_count, count, elm);
				}
			}

			void RemoveEnd(int32 start)
			{
				int32 count = m_count - start;
				if (count > 0)
				{
					assert(start >= 0 );

					m_count -= count;

					Utils::DoDestory(m_count, count, (T*)m_elements);
				}
			}

			void Reverse()
			{
				if (m_elements == nullptr)
					return;

				T* elm = (T*)m_elements;

				for (int32 i = 0; i < m_count / 2; i++)
				{
					int32 another = m_count - i - 1;
					std::swap(elm[i], elm[another]);
				}
			}

			void Swap(int32 i, int32 j)
			{
				T* elm = (T*)m_elements;
				std::swap(elm[i], elm[j]);
			}

			bool GetMinMax(T& minValue, T& maxValue) const
			{
				const int32 count = m_count;
				if (count > 0)
				{
					const T* elm = (const T*)m_elements;

					minValue = elm[0];
					maxValue = elm[0];

					for (int32 i = 0; i < count; i++)
					{
						const T& v = elm[i];
						if (OrderComparer(v, minValue) < 0)
							minValue = v;
						if (OrderComparer(v, maxValue) > 0)
							maxValue = v;
					}
					return true;
				}
				return false;
			}

			template <typename Func>
			const T* Find(Func f) const
			{
				const T* elm = (const T*)m_elements;

				for (int32 i = 0; i < m_count; i++)
				{
					if (f(std::cref(elm[i])))
						return &elm[i];
				}
				return nullptr;
			}

			template <typename Func>
			T* Find(Func f)
			{
				T* elm = (T*)m_elements;

				for (int32 i = 0; i < m_count; i++)
				{
					if (f(std::cref(elm[i])))
						return &elm[i];
				}
				return nullptr;
			}

			T* AllocateArrayCopy() const
			{
				T* result = new T[m_count];
				CopyTo(result, (T*)m_elements, m_count);
				return result;
			}

			template <int32 N>
			void CopyTo(T (&dst)[N]) const
			{
				assert(m_count <= N);
				int32 count = m_count > N ? N : m_count;
				CopyTo(dst, (T*)m_elements, count);
			}

			void Sort() { Sort<OrderComparer>(); }

			template <int32(*comparer)(const T&, const T&)>
			void Sort() { if (m_count > 0) QuickSort<T, comparer>((T*)m_elements, 0, m_count - 1); }

			template <typename Func>
			void Sort(Func& comparer) { if (m_count > 0) QuickSort((T*)m_elements, 0, m_count - 1, comparer); }

			template <typename SorterType, typename SorterCalc>
			void SortWithSorter(SorterCalc& sorter) { if (m_count > 0) QuickSortWithSorter<T, SorterType>((T*)m_elements, 0, m_count - 1, sorter); }

			template <typename SorterType, SorterType(*sorterCalc)(const T& a, const T& b)>
			void SortWithSorter() { if (m_count > 0) QuickSortWithSorter<T, SorterType, sorterCalc>((T*)m_elements, 0, m_count - 1); }


			int32 ClampIndexInRange(int32 idx) const
			{
				if (idx >= m_count) return m_count - 1;
				if (idx < 0) return 0;
				return idx;
			}


			T& LastItem() { assert(m_count > 0); return ((T*)m_elements)[m_count - 1]; }
			const T& LastItem() const { assert(m_count > 0); return ((T*)m_elements)[m_count - 1]; }

			bool isIndexInRange(int32 idx) const { return idx >= 0 && idx < m_count; }
			int32 getCount() const { return m_count; }



		protected:
			ListBase() { }
			ListBase(ST elem) : m_elements(elem) { }
			ListBase(ST elem, int32 count)
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
			
			ST m_elements;
			int32 m_count = 0;
		};


		template <typename T, int32 MaxSize>
		class FixedList : public ListBase<T, char[MaxSize*sizeof(T)]>
		{
		public:
			static const int32 Capacity = MaxSize;

			FixedList() { }
			FixedList(std::initializer_list<T> l)
			{
				assert(l.size() < MaxSize);

				AddList(l);
			}
			FixedList(const FixedList& o)
			{
				m_count = o.m_count;
				Utils::CopyToNew((T*)m_elements, (T*)o.m_elements, m_count);
			}
			FixedList& operator=(const FixedList& o)
			{
				if (this != &o)
				{
					Clear();
					m_count = o.m_count;
					Utils::CopyToNew((T*)m_elements, (T*)o.m_elements, m_count);
				}
				return *this;
			}

			FixedList(FixedList&& o)
			{
				m_count = o.m_count;
				Utils::MoveToNew((T*)m_elements, (T*)o.m_elements, m_count);
				o.Clear();
			}
			FixedList& operator=(FixedList&& o)
			{
				if (this != &o)
				{
					Clear();
					m_count = o.m_count;
					Utils::MoveToNew((T*)m_elements, (T*)o.m_elements, m_count);
					o.Clear();
				}
				return *this;
			}

			~FixedList()
			{
				Clear();
			}

			void Add(const T& val)
			{
				assert(m_count < MaxSize);
				if (m_count < MaxSize)
					Utils::DoPutNew(val, m_count++, (T*)m_elements);
			}

			void Add(T&& val)
			{
				assert(m_count < MaxSize);
				if (m_count < MaxSize)
					Utils::DoPutNew(std::move(val), m_count++, (T*)m_elements);
			}

			template <int32 N>
			void AddArray(const T(&val)[N]) { AddArray(val, N); }
			void AddArray(const T* val, int32 count)
			{
				assert(m_count + count <= MaxSize);
				for (int32 i = 0; i < count && m_count < MaxSize; i++)
					Utils::DoPutNew(val[i], m_count++, (T*)m_elements);
			}

			template <int32 N>
			void AddList(const FixedList<T, N>& other) { AddArray(other.getElements(), other.getCount()); }
			void AddList(std::initializer_list<T> l)
			{
				assert(m_count + l.size() <= MaxSize);
				for (auto iter = l.begin(); iter != l.end() && m_count < MaxSize; ++iter)
					Utils::DoPutNew(*iter, m_count++, (T*)m_elements);
			}

			void Reserve(int32 newCount)
			{
				assert(newCount <= MaxSize); 
				
				for (int32 i = m_count; i < newCount; i++)
					new ((T*)(m_elements) + i)T();

				m_count = newCount; 
			}

			void CopyTo(T(&dst)[MaxSize]) { CopyTo<MaxSize>(dst); }

			int32 getCapacity() const { return MaxSize; }
			bool isFull() const { return m_count >= MaxSize; }

			const T* getElements() const { return (T*)m_elements; }
			T* getElements() { return (T*)m_elements; }

			T& operator [](int32 i)
			{
				assert(i >= 0 && i < m_count);
				return ((T*)m_elements)[i];
			}
			const T& operator [](int32 i) const
			{
				assert(i >= 0 && i < m_count);
				return ((T*)m_elements)[i];
			}

			const T* begin() const { return (T*)m_elements; }
			const T* end() const { return ((T*)m_elements) + m_count; }

			T* begin() { return (T*)m_elements; }
			T* end() { return (T*)(m_elements)+m_count; }

		};

		template <typename T>
		class WrappedList : public ListBase<T, T*>
		{
		public:
			WrappedList(T* externalBuffer, int32 maxCapacity)
				: ListBase(externalBuffer), m_length(maxCapacity) { }

			void Add(const T& val)
			{
				assert(m_count < m_length);
				if (m_count < m_length)
					m_elements[m_count++] = val;
			}
			
			void Add(T&& val)
			{
				assert(m_count < m_length);
				if (m_count < m_length)
					m_elements[m_count++] = std::move(val);
			}

			template <int32 N>
			void AddArray(const T(&val)[N]) { AddArray(val, N); }
			void AddArray(const T* val, int32 count)
			{
				assert(m_count + count <= m_length);
				for (int32 i = 0; i < count && m_count < m_length; i++)
					m_elements[m_count++] = val[i];
			}

			template<typename = void>
			void AddList(const WrappedList<T>& other) { AddArray(other.getElements(), other.getCount()); }
			
			void AddList(std::initializer_list<T> l)
			{
				assert(m_count + l.size() <= m_length);
				for (auto iter = l.begin(); iter != l.end() && m_count < m_length; ++iter)
					m_elements[m_count++] = *iter;
			}

			int32 getCapacity() const { return m_length; }

			T* getElements() { return m_elements; }

			T& operator [](int32 i)
			{
				assert(i >= 0 && i < m_count);
				return m_elements[i];
			}

			T* begin() { return m_elements; }
			T* end() { return m_elements + m_count; }

		private:
			int32 m_length = 0;
		};

		//template <typename T>
		//class ListAllocatedStorage
		//{
		//public:
		//	ListAllocatedStorage(int32 sz);
		//	~ListAllocatedStorage();
		//
		//	void EnsureElements();
		//	void EnsureElementIncrSize();
		//	void EnsureElementIncrSize(int32 count);
		//
		//	T* getElements() const;
		//
		//private:
		//	static T* Allocate(int32 size) { return (T*)malloc(sizeof(T)*size); }
		//	static void Free(T* ptr) { free(ptr); }
		//
		//	T* m_elements;
		//	int32 m_length = 0;
		//
		//};


		template <typename T>
		class List : public ListBase<T, T*>
		{
		public:
			List() : ListBase(nullptr), m_length(4) { }

			explicit List(int32 capacity)
				: ListBase(nullptr), m_length(capacity) { }

			List(std::initializer_list<T> l)
				: ListBase(nullptr, 0), m_length((int32)l.size())
			{
				EnsureElements();
				
				for (const T& e : l)
					Utils::DoPutNew(e, m_count++, m_elements);
			}

			List(const List& another)
				: ListBase(nullptr, another.m_count), m_length(another.m_length)
			{
				if (another.m_elements)
					m_elements = Allocate(m_length);

				Utils::CopyToNew(m_elements, another.m_elements, m_count);
			}
			List(List&& other) noexcept
				: ListBase(other.m_elements, other.m_count), m_length(other.m_length)
			{
				other.m_elements = nullptr;
				other.m_count = 0;
				other.m_length = 0;
			}
			~List()
			{
				Clear();
				Free(m_elements);
			}

			List& operator=(const List& rhs)
			{
				if (this != &rhs)
				{
					Clear();

					if (m_length != rhs.m_length)
					{
						Free(m_elements);

						m_length = rhs.m_length;

						if (rhs.m_elements)
							m_elements = Allocate(m_length);
					}
					else
					{
						if (m_elements == nullptr && rhs.m_elements)
							m_elements = Allocate(m_length);
					}

					m_count = rhs.m_count;
					Utils::CopyToNew(m_elements, rhs.m_elements, m_count);
				}
				return *this;
			}
			List& operator=(List&& rhs) noexcept
			{
				if (this != &rhs)
				{
					Clear();
					Free(m_elements);
					
					m_elements = rhs.m_elements;
					m_count = rhs.m_count;
					m_length = rhs.m_length;

					rhs.m_elements = nullptr;
					rhs.m_count = 0;
					rhs.m_length = 0;
				}
				return *this;
			}


			void Add(const T& val)
			{
				EnsureElementIncrSize();
				Utils::DoPutNew(val, m_count++, m_elements);
			}
			void Add(T&& val)
			{
				EnsureElementIncrSize();
				Utils::DoPutNew(std::move(val), m_count++, m_elements);
			}

			template <int32 N>
			void AddArray(const T(&val)[N]) { AddArray(val, N); }
			void AddArray(const T* val, int32 count)
			{
				EnsureElementIncrSize(count);

				for (int32 i = 0; i < count; i++)
					Utils::DoPutNew(val[i], m_count++, m_elements);
			}

			template <typename = void>
			void AddList(const List<T>& other) { AddArray(other.getElements(), other.getCount()); }
			template <int32 N>
			void AddList(const FixedList<T, N>& other) { AddArray(other.getElements(), other.getCount()); }

			void AddList(std::initializer_list<T> l)
			{
				EnsureElementIncrSize((int32)l.size());

				for (const T& e : l)
					Utils::DoPutNew(e, m_count++, m_elements);
			}

			
			void Resize(int32 newSize)
			{
				assert(newSize >= m_count);
				T* newArr = Allocate(newSize);

				Utils::MoveToNew(newArr, m_elements, m_count);

				Utils::DoDestory(0, m_count, m_elements);
				Free(m_elements);

				m_elements = newArr;
				m_length = newSize;
			}
			void ResizeDiscard(int32 newSize)
			{
				Clear();
				Free(m_elements);

				m_elements = Allocate(newSize);
				m_length = newSize;
			}
			void Reserve(int32 newCount)
			{
				if (m_length < newCount)
					Resize(newCount);
				else
					EnsureElements();
				
				for (int32 i = m_count; i < newCount; i++)
					new (&m_elements[i])T();
				m_count = newCount;
			}
			void ReserveDiscard(int32 newCount)
			{
				if (m_length < newCount)
					ResizeDiscard(newCount);
				else
					EnsureElements();

				Utils::DoDestory(0, m_count, m_elements);
				for (int32 i = 0; i < newCount;i++)
					new (&m_elements[i])T();
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
				assert(index >= 0 && index <= m_count);
				EnsureElementIncrSize();

				if (index < m_count)
				{
					Utils::DoPutNew(std::move(m_elements[m_count - 1]), m_count, m_elements);

					for (int32 i = m_count - 1; i > index; i--)
					{
						m_elements[i] = std::move(m_elements[i - 1]);
					}

					m_elements[index] = item;
				}
				else
				{
					Utils::DoPutNew(item, m_count, m_elements);
				}

				m_count++;
			}


			void InsertArray(int32 index, const T* item, int32 count)
			{
				assert(index >= 0 && index <= m_count);

				EnsureElementIncrSize(count);

				if (index < m_count)
				{
					for (int32 i = m_count + count - 1; i >= index + count; i--)
					{
						if (i >= m_count)
							Utils::DoPutNew(std::move(m_elements[i - count]), i, m_elements);
						else
							m_elements[i] = std::move(m_elements[i - count]);
					}

					for (int32 i = 0; i < count; i++)
					{
						if (index + i >= m_count)
							Utils::DoPutNew(item[i], index + i, m_elements);
						else
							m_elements[index + i] = item[i];
					}
					m_count += count;
				}
				else
				{
					AddArray(item, count);
				}
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
			static T* Allocate(int32 size) { return (T*)malloc(sizeof(T)*size); }
			static void Free(T*& ptr) { free(ptr); ptr = nullptr; }
			
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
					if (m_length == 0)
						m_length = finalCount;
					EnsureElements();
				}
			}

			void EnsureElements()
			{
				if (m_elements == nullptr)
				{
					m_elements = Allocate(m_length);
				}
			}

			int32 m_length = 0;

		};


		template <typename T>
		class List64
		{
		public:

			template <typename = typename std::enable_if<std::is_pointer<T>::value>>
			void DeleteAndClear()
			{
				T* elm = (T*)m_elements;

				for (int64 i = 0; i < m_count; i++)
					delete elm[i];
				Clear();
			}

			void Clear()
			{
				Utils::DoDestory(0, m_count, (T*)m_elements);
				m_count = 0;
			}

			int64 IndexOf(const T& item) const
			{
				T* elm = (T*)m_elements;

				for (int64 i = 0; i < m_count; i++)
				{
					if (elm[i] == item)
						return i;
				}
				return -1;
			}

			template <typename Func>
			int64 IndexOf(Func f) const
			{
				const T* elm = (const T*)m_elements;

				for (int64 i = 0; i < m_count; i++)
				{
					if (f(std::cref(elm[i])))
						return i;
				}
				return -1;
			}

			bool Contains(const T& item) const { return IndexOf(item) != -1; }

			bool Remove(const T& item)
			{
				int64 index = IndexOf(item);
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
				{
					AP_EXCEPTION(ErrorID::InvalidOperation, L"Removing failed.");
				}
			}
			void RemoveAt(int64 index)
			{
				assert(index >= 0 && index < m_count);
				T* elm = (T*)m_elements;

				for (int64 i = index + 1; i < m_count; i++)
				{
					elm[i - 1] = std::move(elm[i]);
				}
				Utils::DoDestory(m_count - 1, 1, elm);
				m_count--;
			}

			void RemoveAtSwapping(int64 index)
			{
				assert(index >= 0 && index < m_count);

				T* elm = (T*)m_elements;

				m_count--;
				std::swap(elm[index], elm[m_count]);
				Utils::DoDestory(m_count, 1, elm);
			}
			void RemoveRange(int64 start, int64 count)
			{
				if (count > 0)
				{
					assert(start >= 0 && start + count <= m_count);

					T* elm = (T*)m_elements;

					m_count -= count;

					for (int64 i = start; i < m_count; i++)
					{
						elm[i] = std::move(elm[i + count]);
					}

					Utils::DoDestory(m_count, count, elm);
				}
			}

			void RemoveEnd(int64 start)
			{
				int64 count = m_count - start;
				if (count > 0)
				{
					assert(start >= 0);

					m_count -= count;

					Utils::DoDestory(m_count, count, (T*)m_elements);
				}
			}

			void Reverse()
			{
				if (m_elements == nullptr)
					return;

				T* elm = (T*)m_elements;

				for (int64 i = 0; i < m_count / 2; i++)
				{
					int64 another = m_count - i - 1;
					std::swap(elm[i], elm[another]);
				}
			}

			void Swap(int64 i, int64 j)
			{
				T* elm = (T*)m_elements;
				std::swap(elm[i], elm[j]);
			}

			template <typename Func>
			const T* Find(Func f) const
			{
				const T* elm = (const T*)m_elements;

				for (int64 i = 0; i < m_count; i++)
				{
					if (f(std::cref(elm[i])))
						return &elm[i];
				}
				return nullptr;
			}

			template <typename Func>
			T* Find(Func f)
			{
				T* elm = (T*)m_elements;

				for (int64 i = 0; i < m_count; i++)
				{
					if (f(std::cref(elm[i])))
						return &elm[i];
				}
				return nullptr;
			}

			int64 ClampIndexInRange(int64 idx) const
			{
				if (idx >= m_count) return m_count - 1;
				if (idx < 0) return 0;
				return idx;
			}


			T& LastItem() { assert(m_count > 0); return ((T*)m_elements)[m_count - 1]; }
			const T& LastItem() const { assert(m_count > 0); return ((T*)m_elements)[m_count - 1]; }

			bool isIndexInRange(int64 idx) const { return idx >= 0 && idx < m_count; }
			int64 getCount() const { return m_count; }

			List64() : m_length(4) { }

			explicit List64(int64 capacity)
				: m_length(capacity) { }

			List64(std::initializer_list<T> l)
				: m_length(l.size())
			{
				EnsureElements();

				for (const T& e : l)
					Utils::DoPutNew(e, m_count++, m_elements);
			}

			List64(const List64& another)
				: m_count(another.m_count), m_length(another.m_length)
			{
				if (another.m_elements)
					m_elements = Allocate(m_length);

				Utils::CopyToNew(m_elements, another.m_elements, m_count);
			}
			List64(List64&& other) noexcept
				: m_elements(other.m_elements)
				, m_count(other.m_count)
				, m_length(other.m_length)
			{
				other.m_elements = nullptr;
				other.m_count = 0;
				other.m_length = 0;
			}
			~List64()
			{
				Clear();
				Free(m_elements);
			}

			List64& operator=(const List64& rhs)
			{
				if (this != &rhs)
				{
					Clear();

					if (m_length != rhs.m_length)
					{
						Free(m_elements);

						m_length = rhs.m_length;

						if (rhs.m_elements)
							m_elements = Allocate(m_length);
					}
					else
					{
						if (m_elements == nullptr && rhs.m_elements)
							m_elements = Allocate(m_length);
					}

					m_count = rhs.m_count;
					Utils::CopyToNew(m_elements, rhs.m_elements, m_count);
				}
				return *this;
			}
			List64& operator=(List64&& rhs) noexcept
			{
				if (this != &rhs)
				{
					Clear();
					Free(m_elements);

					m_elements = rhs.m_elements;
					m_count = rhs.m_count;
					m_length = rhs.m_length;

					rhs.m_elements = nullptr;
					rhs.m_count = 0;
					rhs.m_length = 0;
				}
				return *this;
			}


			void Add(const T& val)
			{
				EnsureElementIncrSize();
				Utils::DoPutNew(val, m_count++, m_elements);
			}
			void Add(T&& val)
			{
				EnsureElementIncrSize();
				Utils::DoPutNew(std::move(val), m_count++, m_elements);
			}

			void AddArray(const T* val, int64 count)
			{
				EnsureElementIncrSize(count);

				for (int64 i = 0; i < count; i++)
					Utils::DoPutNew(val[i], m_count++, m_elements);
			}

			void AddList(std::initializer_list<T> l)
			{
				EnsureElementIncrSize(l.size());

				for (const T& e : l)
					Utils::DoPutNew(e, m_count++, m_elements);
			}


			void Resize(int64 newSize)
			{
				assert(newSize >= m_count);
				T* newArr = Allocate(newSize);

				Utils::MoveToNew(newArr, m_elements, m_count);

				Utils::DoDestory(0, m_count, m_elements);
				Free(m_elements);

				m_elements = newArr;
				m_length = newSize;
			}
			void ResizeDiscard(int64 newSize)
			{
				Clear();
				Free(m_elements);

				m_elements = Allocate(newSize);
				m_length = newSize;
			}
			void Reserve(int64 newCount)
			{
				if (m_length < newCount)
					Resize(newCount);
				else
					EnsureElements();

				for (int64 i = m_count; i < newCount; i++)
					new (&m_elements[i])T();
				m_count = newCount;
			}
			void ReserveDiscard(int64 newCount)
			{
				if (m_length < newCount)
					ResizeDiscard(newCount);
				else
					EnsureElements();

				Utils::DoDestory(0, m_count, m_elements);
				for (int64 i = 0; i < newCount; i++)
					new (&m_elements[i])T();
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

			void Insert(int64 index, const T& item)
			{
				assert(index >= 0 && index <= m_count);
				EnsureElementIncrSize();

				if (index < m_count)
				{
					Utils::DoPutNew(std::move(m_elements[m_count - 1]), m_count, m_elements);

					for (int64 i = m_count - 1; i > index; i--)
					{
						m_elements[i] = std::move(m_elements[i - 1]);
					}

					m_elements[index] = item;
				}
				else
				{
					Utils::DoPutNew(item, m_count, m_elements);
				}

				m_count++;
			}


			void InsertArray(int64 index, const T* item, int64 count)
			{
				assert(index >= 0 && index <= m_count);

				EnsureElementIncrSize(count);

				if (index < m_count)
				{
					for (int64 i = m_count + count - 1; i >= index + count; i--)
					{
						if (i >= m_count)
							Utils::DoPutNew(std::move(m_elements[i - count]), i, m_elements);
						else
							m_elements[i] = std::move(m_elements[i - count]);
					}

					for (int64 i = 0; i < count; i++)
					{
						if (index + i >= m_count)
							Utils::DoPutNew(item[i], index + i, m_elements);
						else
							m_elements[index + i] = item[i];
					}
					m_count += count;
				}
				else
				{
					AddArray(item, count);
				}
			}

			//////////////////////////////////////////////////////////////////////////

			int64 getCapacity() const { return m_length; }
			T* getElements() const { return m_elements; }

			T& operator [](int64 i) const
			{
				assert(i >= 0 && i < m_count);
				return m_elements[i];
			}

			T* begin() const { return m_elements; }
			T* end() const { return m_elements + m_count; }

		private:
			static T* Allocate(int64 size) { return (T*)malloc(sizeof(T) * size); }
			static void Free(T*& ptr) { free(ptr); ptr = nullptr; }

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

			void EnsureElementIncrSize(int64 count)
			{
				int64 finalCount = m_count + count;
				if (m_length < finalCount)
				{
					int64 nextSize = m_length == 0 ? 4 : (m_length * 2);
					if (nextSize < finalCount)
						nextSize = finalCount;
					Resize(nextSize);
				}
				else
				{
					if (m_length == 0)
						m_length = finalCount;
					EnsureElements();
				}
			}

			void EnsureElements()
			{
				if (m_elements == nullptr)
				{
					m_elements = Allocate(m_length);
				}
			}

			T* m_elements = nullptr;
			int64 m_count = 0;
			int64 m_length = 0;
		};



		namespace Utils
		{
			template <typename T, typename RetType>
			RetType defaultRetConv(T& e) { return static_cast<RetType>(e); }

			template <typename T>
			bool defaultChecker(T& e) { return true; }
		}

		template <typename T, typename RetType, bool(*checker)(T&), RetType(*retConv)(T&)>
		class ConditonalInterator
		{
		public:
			ConditonalInterator() { }
			ConditonalInterator(T* begin, T* end) 
				: m_item(begin), m_end(end)
			{
				MoveToNext();
			}

			ConditonalInterator& operator++()
			{
				MoveToNext();
				return *this;
			}
			ConditonalInterator operator++(int) { ConditonalInterator result = *this; ++(*this); return result; }

			bool operator==(const ConditonalInterator& rhs) const { return m_item == rhs.m_item && m_end == rhs.m_end; }
			bool operator!=(const ConditonalInterator& rhs) const { return !this->operator==(rhs); }

			RetType operator*() const
			{
				return retConv(*(m_item - 1));
			}
		private:
			void MoveToNext()
			{
				while (m_item != m_end)
				{
					T& e = *m_item++;

					if (checker(e))
						return;
				}
				m_item = nullptr;
				m_end = nullptr;
			}

			T* m_item = nullptr;
			T* m_end = nullptr;
		};

		template <typename T, typename RetType = T, bool(*checker)(T&) = Utils::defaultChecker<T>, RetType(*retConv)(T&) = Utils::defaultRetConv<T, RetType>>
		class ConditionalAccessor
		{
			typedef ConditonalInterator<T, RetType, checker, retConv> IterType;

		public:
			ConditionalAccessor(T* begin, T* end)
				: m_begin(begin), m_end(end) { }

			ConditionalAccessor(const List<T>& cont)
				: ConditionalAccessor(cont.begin(), cont.end()) { }
			
			IterType begin() { return IterType(m_begin, m_end); }
			IterType end() { return IterType(); }

		private:
			T* m_begin = nullptr;
			T* m_end = nullptr;
		};


		template <typename T, int32 GroupSize>
		class GroupIteratorItem
		{
		public:
			GroupIteratorItem(T* begin, T* end)
				: m_begin(begin), m_end(end) { }

			T* begin() { return m_begin; }
			T* end() { return m_end; }

			bool isFullGroup() const { return m_end - m_begin == GroupSize; }
			
			int32 getCount() const { return (int32)(m_end - m_begin); }

			T& operator [](int32 i) { return m_begin[i]; }
			
			const T& operator [](int32 i) const { return m_begin[i]; }

		private:

			T* m_begin = nullptr;
			T* m_end = nullptr;
		};

		template <typename T, int32 GroupSize>
		class GroupInterator
		{
			typedef GroupIteratorItem<T, GroupSize> ItemType;
		public:
			GroupInterator() { }
			GroupInterator(T* begin, T* end)
				: m_item(begin), m_end(end) { }

			GroupInterator& operator++()
			{
				MoveToNext();
				return *this;
			}
			GroupInterator operator++(int) { GroupInterator result = *this; ++(*this); return result; }

			bool operator==(const GroupInterator& rhs) const { return m_item == rhs.m_item && m_end == rhs.m_end; }
			bool operator!=(const GroupInterator& rhs) const { return !this->operator==(rhs); }

			ItemType operator*() const
			{
				T* groupEnd = m_item + GroupSize;
				return ItemType(m_item, groupEnd > m_end ? m_end : groupEnd);
			}
		private:
			void MoveToNext()
			{
				m_item += GroupSize;

				if (m_item >= m_end)
				{
					m_item = nullptr;
					m_end = nullptr;
				}
			}

			T* m_item = nullptr;
			T* m_end = nullptr;
		};

		template <typename T, int32 GroupSize>
		class GroupAccessor
		{
			typedef GroupInterator<T, GroupSize> IterType;
			static_assert(GroupSize > 0, "GroupSize must be at least 1.");
		public:
			GroupAccessor(T* begin, T* end)
				: m_begin(begin), m_end(end) { }

			GroupAccessor(const List<T>& cont)
				: GroupAccessor(cont.begin(), cont.end()) { }

			IterType begin() { return IterType(m_begin, m_end); }
			IterType end() { return IterType(); }

		private:
			T* m_begin = nullptr;
			T* m_end = nullptr;
		};

	}
}
#endif