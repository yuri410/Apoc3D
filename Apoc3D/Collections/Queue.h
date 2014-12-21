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
				if (this == &rhs)
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


			void Clear()
			{
				m_head = m_tail = 0;
				m_count = 0;
			}

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

				const T& local = m_array[m_head];
				DequeueOnly();
				return local;
			}

			void Enqueue(const T& item)
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
				m_array[m_tail] = item;
				m_tail = (m_tail + 1) % m_arrLength;
				m_count++;
			}

			int getCount() const { return m_count; }

			T& operator[](int i) { assert(i >= 0 && i < m_count); return m_array[(m_head + i) % m_arrLength]; }
			const T& operator[](int i) const { assert(i >= 0 && i < m_count); return m_array[(m_head + i) % m_arrLength]; }

			T& Element(int i) { return m_array[(m_head + i) % m_arrLength]; }
			const T& GetElement(int i) const { return m_array[(m_head + i) % m_arrLength]; }
			void SetElement(int i, const T& value) { m_array[(m_head + i) % m_arrLength] = value; }

			const T& Head() const { return m_array[m_head]; }
			const T& Tail() const { return operator[](m_count - 1); }
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