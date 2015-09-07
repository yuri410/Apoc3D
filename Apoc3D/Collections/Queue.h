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

#include "List.h"

namespace Apoc3D
{
	namespace Collections
	{
		template<typename T, typename ST>
		class QueueBase
		{
		public:

			void Clear()
			{
				if (!std::is_trivially_copyable<T>::value)
				{
					if (m_head < m_tail)
						DoDestory(m_head, m_tail - m_head);
					else
					{
						DoDestory(m_head, m_arrLength - m_head);
						DoDestory(0, m_tail);
					}
				}

				m_head = m_tail = 0;
				m_count = 0;
			}

			template <typename = typename std::enable_if<std::is_pointer<T>::value && std::is_destructible<typename std::remove_pointer<T>::type>::value>::type>
			void DeleteAndClear()
			{
				for (int32 i = 0; i < getCount(); i++)
					delete Element(i);
				Clear();
			}

			bool Replace(const T& item, const T& item2)
			{
				T* arr = (T*)m_array;

				int index = m_head;
				int counter = m_count;

				while (counter-- > 0)
				{
					if (arr[index] == item)
					{
						arr[index] = item2;
						return true;
					}

					index = (index + 1) % m_arrLength;
				}
				return false;
			}
			bool Contains(const T& item) const
			{
				T* arr = (T*)m_array;

				int index = m_head;
				int counter = m_count;

				while (counter-- > 0)
				{
					if (arr[index] == item)
					{
						return true;
					}

					index = (index + 1) % m_arrLength;
				}
				return false;
			}

			void CopyTo(T* dest) const
			{
				T* arr = (T*)m_array;

				int remaining = m_count;

				if (remaining > 0)
				{
					int numHeadToEnd = (m_arrLength - m_head < remaining) ? (m_arrLength - m_head) : remaining;
					for (int i = 0; i < numHeadToEnd; i++)
					{
						dest[i] = arr[i + m_head];
					}
					remaining -= numHeadToEnd;
					if (remaining > 0)
					{
						for (int i = 0; i < remaining; i++)
						{
							dest[i + m_arrLength - m_head] = arr[i];
						}
					}
				}
			}

			void DequeueOnly()
			{
				assert(m_count > 0);

				DoDestory(m_head, 1);
				m_head = (m_head + 1) % m_arrLength;
				m_count--;
			}
			T Dequeue()
			{
				assert(m_count > 0);
				T* arr = (T*)m_array;

				T temp = std::move(arr[m_head]);
				DequeueOnly();
				return temp;
			}


			void RemoveAt(int32 idx)
			{
				assert(m_count > 0);
				assert(idx >= 0 && idx < m_count);

				T* arr = (T*)m_array;

				int32 head = m_head;
				int32 tail = m_tail;
				if (head < tail)
				{
					for (int32 i = head + idx + 1; i < tail; i++)
					{
						arr[i - 1] = std::move(arr[i]);
					}
				}
				else
				{
					if (head + idx < m_arrLength)
					{
						for (int32 i = head + idx + 1; i < m_arrLength; i++)
						{
							arr[i - 1] = std::move(arr[i]);
						}

						arr[m_arrLength - 1] = std::move(arr[0]);

						for (int32 i = 1; i < m_tail; i++)
						{
							arr[i - 1] = std::move(arr[i]);
						}
					}
					else
					{
						for (int32 i = head + idx - m_arrLength + 1; i < m_tail; i++)
						{
							arr[i - 1] = std::move(arr[i]);
						}
					}
				}

				m_tail--;
				if (m_tail < 0)
					m_tail = m_arrLength - 1;

				DoDestory(m_tail, 1);

				m_count--;
			}

			int getCount() const { return m_count; }

			T& operator[](int i) { assert(i >= 0 && i < m_count); return ((T*)m_array)[(m_head + i) % m_arrLength]; }
			const T& operator[](int i) const { assert(i >= 0 && i < m_count); return  ((T*)m_array)[(m_head + i) % m_arrLength]; }

			T& Element(int i) { return  ((T*)m_array)[(m_head + i) % m_arrLength]; }
			const T& GetElement(int i) const { return  ((T*)m_array)[(m_head + i) % m_arrLength]; }
			void SetElement(int i, const T& value) { ((T*)m_array)[(m_head + i) % m_arrLength] = value; }

			const T& Head() const { return ((T*)m_array)[m_head]; }
			const T& Tail() const { return operator[](m_count - 1); }


		protected:
			void DoPutNew(const T& val, int32 idx)
			{
				assert(idx >= 0);

				T* elm = (T*)m_array;

				if (std::is_trivially_copyable<T>::value)
				{
					elm[idx] = val;
				}
				else
				{
					new (&elm[idx])T(val);
				}
			}
			void DoPutNew(T&& val, int32 idx)
			{
				assert(idx >= 0);

				T* elm = (T*)m_array;
				if (std::is_trivially_copyable<T>::value)
				{
					elm[idx] = std::move(val);
				}
				else
				{
					new (&elm[idx])T(std::move(val));
				}
			}
			void DoDestory(int32 start, int32 count)
			{
				assert(start >= 0);
				if (!std::is_trivially_copyable<T>::value)
				{
					T* elm = (T*)m_array;

					for (int32 i = start; i < start + count; i++)
					{
						elm[i].~T();
					}
				}
			}


			ST m_array;

			int32 m_arrLength;
			int32 m_count = 0;

			int32 m_head = 0;
			int32 m_tail = 0;
		};

		template<typename T>
		class Queue
		{
		public:
			Queue()
				: m_arrLength(4)
			{
				m_array = new T[4];
			}
			explicit Queue(int capacity)
				: m_arrLength(capacity)
			{
				m_array = new T[capacity];
			}
			~Queue()
			{
				delete[] m_array;
			}

			Queue(const Queue& another)
				: m_head(another.m_head), m_tail(another.m_tail), m_count(another.m_count), m_arrLength(another.m_arrLength)
			{
				m_array = new T[m_arrLength];
				for (int i = 0; i < m_arrLength; i++)
					m_array[i] = another.m_array[i];
			}
			Queue& operator=(const Queue& rhs)
			{
				if (this != &rhs)
				{
					delete[] m_array;

					m_head = rhs.m_head;
					m_tail = rhs.m_tail;
					m_count = rhs.m_count;
					m_arrLength = rhs.m_arrLength;

					m_array = new T[m_arrLength];
					for (int i = 0; i < m_arrLength; i++)
						m_array[i] = rhs.m_array[i];
				}
				return *this;
			}

			Queue(Queue&& o)
				: m_array(o.m_array), m_arrLength(o.m_arrLength), m_count(o.m_count), m_head(o.m_head), m_tail(o.m_tail)
			{
				o.m_array = nullptr;
				o.m_count = 0;
				o.m_arrLength = 0;
				o.m_head = 0;
				o.m_tail = 0;
			}
			Queue& operator=(Queue&& o)
			{
				if (this != &o)
				{
					delete[] m_array;
					new (this)Queue(std::move(o));
				}
				return *this;
			}

			void Clear()
			{
				m_head = m_tail = 0;
				m_count = 0;
			}

			template <typename = typename std::enable_if<std::is_pointer<T>::value && std::is_destructible<typename std::remove_pointer<T>::type>::value>::type>
			void DeleteAndClear()
			{
				for (int32 i = 0; i < getCount(); i++)
					delete Element(i);
				Clear();
			}

			bool Replace(const T& item, const T& item2)
			{
				int index = m_head;
				int counter = m_count;

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
				int counter = m_count;

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
				int remaining = m_count;

				if (remaining > 0)
				{
					int numHeadToEnd = (m_arrLength - m_head < remaining) ? (m_arrLength - m_head) : remaining;
					for (int i = 0; i < numHeadToEnd; i++)
					{
						dest[i] = m_array[i + m_head];
					}
					remaining -= numHeadToEnd;
					if (remaining > 0)
					{
						for (int i = 0; i < remaining; i++)
						{
							dest[i + m_arrLength - m_head] = m_array[i];
						}
					}
				}
			}

			void DequeueOnly()
			{
				assert(m_count > 0);

				m_head = (m_head + 1) % m_arrLength;
				m_count--;
			}
			T Dequeue()
			{
				assert(m_count > 0);

				int32 oldHead = m_head;
				DequeueOnly();
				return m_array[oldHead];
			}

			void Enqueue(const T& item)
			{
				EnsureIncrCapacity();

				m_array[m_tail] = item;
				m_tail = (m_tail + 1) % m_arrLength;
				m_count++;
			}

			void Enqueue(T&& item)
			{
				EnsureIncrCapacity();

				m_array[m_tail] = std::move(item);
				m_tail = (m_tail + 1) % m_arrLength;
				m_count++;
			}

			void RemoveAt(int32 idx)
			{
				assert(m_count > 0);
				assert(idx >= 0 && idx < m_count);

				int32 head = m_head;
				int32 tail = m_tail;
				if (head < tail)
				{
					for (int32 i = head + idx + 1; i < tail; i++)
					{
						m_array[i - 1] = std::move(m_array[i]);
					}
				}
				else
				{
					if (head + idx < m_arrLength)
					{
						for (int32 i = head + idx + 1; i < m_arrLength; i++)
						{
							m_array[i - 1] = std::move(m_array[i]);
						}

						m_array[m_arrLength - 1] = std::move(m_array[0]);

						for (int32 i = 1; i < m_tail; i++)
						{
							m_array[i - 1] = std::move(m_array[i]);
						}
					}
					else
					{
						for (int32 i = head + idx - m_arrLength + 1; i < m_tail; i++)
						{
							m_array[i - 1] = std::move(m_array[i]);
						}
					}
				}
				//for (int32 i = idx + 1; i < m_count; i++)
				//{
				//	m_array[(m_head + i - 1) % m_arrLength] = std::move(m_array[(m_head + i) % m_arrLength]);
				//}
				m_tail--;
				if (m_tail < 0)
					m_tail = m_arrLength - 1;
				
				m_count--;
			}

			int getCount() const { return m_count; }

			T& operator[](int i) { assert(i >= 0 && i < m_count); return m_array[(m_head + i) % m_arrLength]; }
			const T& operator[](int i) const { assert(i >= 0 && i < m_count); return m_array[(m_head + i) % m_arrLength]; }

			T& Element(int i) { return m_array[(m_head + i) % m_arrLength]; }
			const T& GetElement(int i) const { return m_array[(m_head + i) % m_arrLength]; }
			void SetElement(int i, const T& value) { m_array[(m_head + i) % m_arrLength] = value; }

			const T& Head() const { return m_array[m_head]; }
			const T& Tail() const { return operator[](m_count - 1); }




			class Iterator
			{
			public:
				Iterator() { }
				Iterator(const Queue& ctn) : m_owner(&ctn) { }
				Iterator(const Queue& ctn, int32 idx) : m_owner(&ctn), m_idx(idx) { }

				T& operator*() const { return  m_owner->m_array[(m_owner->m_head + m_idx) % m_owner->m_arrLength]; }

				bool operator==(const Iterator& o) const { return m_owner == o.m_owner && m_idx != o.m_idx; }
				bool operator!=(const Iterator& o) const { return !(*this == o); }

				Iterator& operator++() { m_idx++; if (m_idx > m_owner->getCount()) m_idx = -1; return *this; }
				Iterator operator++(int)
				{
					Iterator result = *this;
					++(*this);
					return result;
				}

				Iterator& operator=(const Iterator& o)
				{
					m_owner = o.m_owner;
					m_idx = o.m_idx;
					return *this;
				}

			private:
				const Queue* m_owner = nullptr;
				int32 m_idx = -1;
			};

			Iterator begin() const { return Iterator(*this, 0); }
			Iterator end() const { return Iterator(*this); }

		private:
			void SetCapacity(int capacity)
			{
				T* destinationArray = new T[capacity];
				if (m_count > 0)
				{
					if (m_head < m_tail)
					{
						for (int i = 0; i < m_count; i++)
						{
							destinationArray[i] = std::move(m_array[i + m_head]);
						}
					}
					else
					{
						for (int i = m_head; i < m_arrLength; i++)
						{
							destinationArray[i - m_head] = std::move(m_array[i]);
						}

						for (int i = 0; i < m_tail; i++)
						{
							destinationArray[m_arrLength - m_head + i] = std::move(m_array[i]);
						}
					}
				}
				delete[] m_array;
				m_array = destinationArray;
				m_arrLength = capacity;
				m_head = 0;
				m_tail = (m_count == capacity) ? 0 : m_count;
			}

			void EnsureIncrCapacity()
			{
				if (m_count == m_arrLength)
				{
					int newCapacity = m_arrLength * 2;
					if (newCapacity < m_arrLength + 4)
					{
						newCapacity = m_arrLength + 4;
					}
					SetCapacity(newCapacity);
				}
			}



			T* m_array;
			int32 m_arrLength;
			int32 m_count = 0;

			int32 m_head = 0;
			int32 m_tail = 0;
		};

		template<typename T, int32 N>
		class FixedQueue
		{
		public:
			static const int32 Size = N;

			FixedQueue() { }
			~FixedQueue() { }

			void Clear()
			{
				m_head = m_tail = 0;
				m_count = 0;
			}

			template <typename = typename std::enable_if<std::is_pointer<T>::value && std::is_destructible<typename std::remove_pointer<T>::type>::value>::type>
			void DeleteAndClear()
			{
				for (int32 i = 0; i < getCount(); i++)
					delete Element(i);
				Clear();
			}

			bool Replace(const T& item, const T& item2)
			{
				int index = m_head;
				int counter = m_count;

				while (counter-- > 0)
				{
					if (m_array[index] == item)
					{
						m_array[index] = item2;
						return true;
					}

					index = (index + 1) % N;
				}
				return false;
			}
			bool Contains(const T& item) const
			{
				int index = m_head;
				int counter = m_count;

				while (counter-- > 0)
				{
					if (m_array[index] == item)
					{
						return true;
					}

					index = (index + 1) % N;
				}
				return false;
			}

			void CopyTo(T* dest, int32 amount) const
			{
				assert(amount <= m_count);
				int remaining = amount;

				if (remaining > 0)
				{
					int numHeadToEnd = (N - m_head < remaining) ? (N - m_head) : remaining;
					for (int i = 0; i < numHeadToEnd; i++)
					{
						dest[i] = m_array[i + m_head];
					}
					remaining -= numHeadToEnd;
					if (remaining > 0)
					{
						for (int i = 0; i < remaining; i++)
						{
							dest[i + N - m_head] = m_array[i];
						}
					}
				}
			}

			void DequeueOnly(int32 amount)
			{
				assert(m_count >= amount);
				m_head = (m_head + amount) % N;
				m_count -= amount;
			}
			void DequeueOnly()
			{
				assert(m_count > 0);
				m_head = (m_head + 1) % N;
				m_count--;
			}
			T Dequeue()
			{
				assert(m_count > 0);

				const T& local = m_array[m_head];
				DequeueOnly();
				return local;
			}

			void Enqueue(const T& item)
			{
				assert(m_count < N);

				m_array[m_tail] = item;
				m_tail = (m_tail + 1) % N;
				m_count++;
			}
			void Enqueue(const T* items, int32 count)
			{
				assert(m_count + count <= N);

				int32 partCount = Math::Min(count, N - m_tail);

				if (partCount > 0)
				{
					for (int32 i = 0; i < partCount; i++)
					{
						m_array[m_tail++] = items[i];
					}
					m_tail = m_tail % N;
					m_count += partCount;

					items += partCount;
					count -= partCount;
				}
				
				if (count > 0)
				{
					for (int32 i = 0; i < count; i++)
					{
						m_array[m_tail++] = items[i];
					}
					m_tail = m_tail % N;
					m_count += count;
				}
			}

			int getCount() const { return m_count; }
			int32 getCapacity() const { return N; }

			T& operator[](int i) { assert(i >= 0 && i < m_count); return m_array[(m_head + i) % N]; }
			const T& operator[](int i) const { assert(i >= 0 && i < m_count); return m_array[(m_head + i) % N]; }

			const T& Head() const { return m_array[m_head]; }
			const T& Tail() const { return operator[](m_count - 1); }

		private:
			int32 m_count = 0;

			int32 m_head = 0;
			int32 m_tail = 0;

			T m_array[N];
		};


		template<typename T, int32 (*Comparer)(const T& a, const T& b) = &OrderComparer<T>>
		class PriorityQueue
		{
		public:
			PriorityQueue() 
			{ }

			PriorityQueue(int32 capacity) 
				: m_innerList(capacity) { }


			void Enqueue(const T& item)
			{
				int32 p = m_innerList.getCount();
				int32 p2;

				m_innerList.Add(item);

				for (;;)
				{
					if (p == 0) break;
					p2 = (p - 1) / 2;

					if (CompareListItem(p, p2) < 0)
					{
						SwapListItem(p, p2);
						p = p2;
					}
					else break;
				}
			}

			T Dequeue()
			{
				assert(m_innerList.getCount() > 0);

				const T result = std::move(m_innerList[0]);

				int32 p = 0;

				m_innerList[0] = m_innerList[m_innerList.getCount() - 1];
				m_innerList.RemoveAt(m_innerList.getCount() - 1);

				for (;;)
				{
					int32 pn = p;
					int32 p1 = p * 2 + 1;
					int32 p2 = p * 2 + 2;

					if (m_innerList.getCount() > p1 && CompareListItem(p, p1) > 0)
						p = p1;
					if (m_innerList.getCount() > p2 && CompareListItem(p, p2) > 0)
						p = p2;

					if (p == pn) break;

					SwapListItem(p, pn);
				}

				return result;
			}
			T& Top() { return m_innerList[0]; }

			void UpdatePriorityForChange(int32 index)
			{
				int32 p = index;

				for (;;)
				{
					if (p == 0) break;

					int32 p2 = (p - 1) / 2;

					if (CompareListItem(p, p2) < 0)
					{
						SwapListItem(p, p2);
						p = p2;
					}
					else break;
				}

				if (p < index) return;

				for (;;)
				{
					int32 pn = p;
					int32 p1 = p * 2 + 1;
					int32 p2 = p * 2 + 2;

					if (m_innerList.getCount() > p1 && CompareListItem(p, p1) > 0)
						p = p1;
					if (m_innerList.getCount() > p2 && CompareListItem(p, p2) > 0)
						p = p2;

					if (p == pn) break;

					SwapListItem(p, pn);
				}
			}


			void Clear() { m_innerList.Clear(); }

			List<T>& getInternalList() { return m_innerList; }

			int32 getCount() const { return m_innerList.getCount(); }

		private:
			void SwapListItem(int32 a, int32 b) { std::swap(m_innerList[a], m_innerList[b]); }
			int32 CompareListItem(int32 a, int32 b) { return Comparer(m_innerList[a], m_innerList[b]); }
			
			List<T> m_innerList;
		};
	}
}
#endif