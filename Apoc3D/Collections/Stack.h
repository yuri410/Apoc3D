#pragma once

#ifndef APOC3D_STACK_H
#define APOC3D_STACK_H

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

namespace Apoc3D
{
	namespace Collections
	{
		template<typename T>
		class Stack
		{
		public:	
			explicit Stack(int capacity = 8)
				: m_length(capacity)
			{
				m_array = new T[m_length];
			}
			~Stack()
			{
				delete[] m_array;
			}


			Stack(const Stack& another)
				: m_size(another.m_size), m_length(another.m_length)
			{
				m_array = new T[m_length];
				for (int i = 0; i < m_length; i++)
					m_array[i] = another.m_array[i];
			}

			Stack& operator=(const Stack& another)
			{
				if (this != &another)
				{
					delete[] m_array;

					m_size = another.m_size;
					m_length = another.m_length;
					m_array = new T[m_length];

					for (int i = 0; i < m_length; i++)
						m_array[i] = another.m_array[i];
				}
				return *this;
			}

			Stack(Stack&& another)
				: m_size(another.m_size), m_length(another.m_length)
			{
				m_array = another.m_array;
				another.m_array = nullptr;
				another.m_size = another.m_length = 0;
			}

			Stack& operator=(Stack&& another)
			{
				if (this != &another)
				{
					delete[] m_array;

					m_size = another.m_size;
					m_length = another.m_length;
					m_array = another.m_array;

					another.m_array = nullptr;
					another.m_size = another.m_length = 0;
				}
				return *this;
			}

			int getCount() const { return m_size; }

			void Clear() { m_size = 0; }

			bool Contains(const T& item)
			{
				for (int32 i = m_size - 1; i >= 0; i--)
					if (m_array[i] == item)
						return true;
				return false;
			}

			const T& Peek() const { assert(m_size > 0); return m_array[m_size - 1]; }
			T& Peek() { assert(m_size > 0); return m_array[m_size - 1]; }

			T Pop() { assert(m_size > 0); return m_array[--m_size]; }
			void FastPop() { assert(m_size > 0); m_size--; }


			void Push(const T& item)
			{
				if (m_size == m_length)
				{
					if (m_length == 0)
						m_length = 1;
					else
						m_length *= 2;

					T* destinationArray = new T[m_length * 2];
					for (int i = 0; i < m_size; i++)
						destinationArray[i] = std::move(m_array[i]);

					delete[] m_array;
					m_array = destinationArray;
				}
				m_array[m_size++] = item;
			}

		private:
			T* m_array;
			int m_size = 0;
			int m_length;
		};

		template<typename T, int32 N>
		class FixedStack
		{
		public:
			FixedStack() { }
			~FixedStack() { }

			int getCount() const { return m_size; }

			void Clear() { m_size = 0; }

			bool Contains(const T& item)
			{
				for (int32 i = m_size - 1; i >= 0; i--)
					if (m_array[i] == item)
						return true;
				return false;
			}

			T& Peek() { assert(m_size > 0); return m_array[m_size - 1]; }
			T Pop() { assert(m_size > 0); return m_array[--m_size]; }

			void FastPop() { assert(m_size > 0); m_size--; }

			void Push(const T& item)
			{
				assert(m_size < m_length);
				m_array[m_size++] = item;
			}

		private:
			int m_size = 0;
			T m_array[N];
		};
	}
}

#endif