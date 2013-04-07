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
		private:
			T* m_array;
			int m_size;
			int m_length;
		private:
			Stack& operator=(const Stack &rhs)
			{
				return *this; 
			}
			Stack(const Stack& another){}
		public:
			
			explicit Stack(int capacity = 8)
				: m_length(capacity), m_size(0)
			{
				m_array = new T[m_length];
			}

			int getCount() const { return m_size; }

			void Clear()
			{
				memset(m_array, 0, sizeof(T) * m_size)
			}

			bool Contains(const T& item)
			{
				int index = m_size;
				while (index-->0)
				{
					if (item == m_array[index])
					{
						return true;
					}
				}
				return false;
			}

			T& Peek()
			{
				return m_array[m_size - 1];
			}

			T Pop()
			{
				T local = m_array[--m_size];
				memset(&m_array[m_size], 0, sizeof(T));
				return local;
			}
			void FastPop()
			{
				m_size--;
			}

			void Push(const T& item)
			{
				if (m_size == m_length)
				{
					if (!m_length)
						m_length = 1;
					else
						m_length *= 2;
					T* destinationArray = new T[m_length * 2];
					memcpy(destinationArray, m_array, m_size*sizeof(T));
					delete[] m_array;
					m_array = destinationArray;
				}
				m_array[m_size++] = item;
			}
		};
	}
}

#endif