#ifndef LIST_H
#define LIST_H

#include "Common.h"

#pragma once

namespace Apoc
{
	namespace Core
	{
		namespace Collections
		{
			template <class T>
			class List
			{
			private:
				T* elements;
				int length;
				int internalPointer;

			public:
				List(void) 
				{
					elements = new T[4];
					length = 4;
					internalPointer = 0;
				}
				List(int capacity)
				{
					elements = new T[capacity];
					length = capacity;
					internalPointer = 0;
				}

				~List(void)
				{
					delete elements;
				}

				int getCount()
				{
					return internalPointer;
				}
				int getCapacity()
				{
					return length;
				}

				void Clear()
				{
					internalPointer = 0;
				}

				void Add(const T &value)
				{
					if (length<=internalPointer)
					{
						Resize(length == 0 £¿ 4 : (length*2));
					}
					elements[internalPointer++] = Data;
				}
				
				void Remove(const T &value)
				{
					int index = IndexOf(item);
					if (index == -1)
					{
						RemoveAt(index);
					}
				}

				void RemoveAt(int idx)
				{
					if (idx == internalPointer-1)
					{
						internalPoiner--;
						elements[idx] = default(T);
					}
					else
					{
						T* newArr = new T[length-1];
						memcpy(newArr, elements, idx * sizeof(T));

						if (internalPointer - ++idx>0)
						{
							memcpy(newArr + idx -1, elements + idx, internalPointer - idx);
						}
						delete elements;

						elements = newArr;
						length--;
						internalPointer--;
					}
				}

				void Resize(int newSize)
				{
					T* newArray = new T[newSize];
					memcpy(newArray, elements, sizeof(T) * internlPointer);
					delete elements;
					elements = newArray;
					length = newSize;
				}

				int IndexOf(const T &value)
				{
					for (int i=0;i<internalPointer;i++)
					{
						if (elements[i] == value)
						{
							return i;
						}
					}
					return -1;
				} 

				T operator [](int index)
				{
#if DEBUG_MODE
					if (index < 0)
					{

					}
					if (index >= internalPointer)
					{

					}
#endif
					return elements[index];
				}
			};
		}
	}
}
#endif
