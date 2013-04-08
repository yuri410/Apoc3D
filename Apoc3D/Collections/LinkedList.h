#pragma once
#ifndef APOC3D_LINKEDLIST_H
#define APOC3D_LINKEDLIST_H

#include "apoc3d/Common.h"

namespace Apoc3D
{
	namespace Collections
	{
		/**
		 * Implements a singly linked list.
		 */
		template <typename T>
		class LinkedList
		{
		private:
			struct Node;

		public:
			/**
			 * 
			 */
			class Iterator
			{
			public:
				Iterator() : m_owner(nullptr), m_referenceNode(nullptr) { }

				T& operator*() const { assert(m_referenceNode); return m_referenceNode->Data; }

				T* operator->() const { assert(m_referenceNode); return &m_referenceNode->Data; }

				bool operator==(const Iterator &other) const
				{
					return other.m_owner == m_owner && other.m_referenceNode == m_referenceNode; 
				}
				bool operator!=(const Iterator &other) const { return !(*this == other); }

				Iterator& operator++()
				{
					assert(m_referenceNode);

					m_referenceNode = m_referenceNode->Next;

					return *this;
				}
				Iterator operator++(int)
				{
					LinkedList<T>::Iterator result = *this; 
					++(*this);
					return result;
				}

			private:
				Iterator(const LinkedList* owner, Node* ref) : m_owner(owner), m_referenceNode(ref) { }

				const LinkedList* m_owner;

				/**
				 *  The end iterator has nullptr for this
				 */
				Node* m_referenceNode;

				friend class LinkedList;
			};
	
			/**
			 * Initializes the list to be empty.
			 */
			LinkedList()
				: m_firstNode(nullptr), m_lastNode(nullptr), m_size(0)
			{ }

			/**
			 * Do a deep-copy of the given list.
			 * @params anther The SList object to copy from.
			 */
			LinkedList(const LinkedList& another)
				: m_size(another.m_size), m_firstNode(nullptr)
			{
				CopyNodesFrom(another.m_firstNode);
			}

			/**
			 * Do a deep-copy assignment from the given list to this list.
			 * @param rhs The SList object to be assigned from.
			 */
			LinkedList& operator=(const LinkedList& rhs)
			{
				if (&rhs == this)
					return *this;

				Clear();

				m_size = rhs.m_size;
				CopyNodesFrom(rhs.m_firstNode);
				return *this;
			}

			/** 
			 * Clear up the memory the list uses.
			 */
			~LinkedList() { Clear(); }

			/**
			 * Insert the given item before the front of the list.
			 */
			void PushFront(const T& item)
			{
				Node* newNode = new Node(item, m_firstNode);

				if (!m_lastNode)
				{
					assert(!m_firstNode);
					m_lastNode = newNode;
				}
				m_firstNode = newNode;

				m_size++;
			}

			/**
			 * Append the given item after the end of the list.
			 */
			void PushBack(const T& item) { PushBack_Iter(item); }

			/**
			 *  Append the given item after the end of the list and
			 *  return an iterator pointing to it.
			 */
			Iterator PushBack_Iter(const T& item)
			{
				Node* newNode = new Node(item);

				if (m_lastNode)
				{
					m_lastNode->Next = newNode;
				}
				else
				{
					assert(!m_firstNode);
					m_firstNode = newNode;
				}
				m_lastNode = newNode;

				m_size++;

				return Iterator(this, newNode);
			}

			/**
			 * Remove the first item at the front of the list.
			 * @see Front() 
			 */
			void PopFront()
			{
				assert(!IsEmpty());

				Node* oldNode = m_firstNode;

				m_firstNode = m_firstNode->Next;

				delete oldNode;

				m_size--;
				if (!m_firstNode)
				{
					m_lastNode = nullptr;
				}
			}

			/**
			 * Removes all the items in the list.
			 */
			void Clear()
			{
				m_size = 0;

				Node* curNode = m_firstNode;
				while (curNode)
				{
					Node* nextNode = curNode->Next;

					delete curNode;

					curNode = nextNode;
				}
				m_firstNode = m_lastNode = nullptr;
			}

			/**
			 * Returns the first item in the list.
			 */
			T& Front() { assert(!IsEmpty()); return m_firstNode->Data; }
			/**
			 * Returns the last item in the list.
			 */
			T& Back() { assert(!IsEmpty()); return m_lastNode->Data; }

			/**
			 * Returns an iterator pointing to the head of the list
			 */
			Iterator Begin() const { return Iterator(this, m_firstNode); }
			/**
			 * Returns an iterator pointing to the past the end of the list
			 */
			Iterator End() const { return Iterator(this, nullptr); }

			/**
			 * Append the given item after the item the iterator points to.
			 */
			void InsertAfter(const Iterator& iter, const T& item)
			{
				assert(iter.m_owner == this);

				Node* node = iter.m_referenceNode;

				if (node)
				{
					Node* newNodeAfter = new Node(item, node->Next);
					node->Next = newNodeAfter;

					if (node == m_lastNode)
					{
						m_lastNode = newNodeAfter;
					}
					m_size++;
				}
				else
				{
					PushBack(item);
				}
			}

			/**
			 * Find the item equals to the given item.
			 * @return the iterator to access the item. If not found, return End().
			 */
			Iterator Find(const T& item) const
			{
				for (Iterator iter = Begin(); iter != End(); ++iter)
				{
					if ((*iter) == item)
					{
						return iter;
					}
				}
				return End();
			}

			/**
			 * Find and remove the given item from the list.
			 * @return true if the item is found and removed. false if not found and nothing affected.
			 */
			bool Remove(const T& item)
			{
				Iterator prev = End();
				Iterator cur = Begin();

				while (cur != End())
				{
					if ((*cur) == item)
					{
						Node* curNode = cur.m_referenceNode;
						Node* prevNode = prev.m_referenceNode;

						if (prevNode)
						{
							prevNode->Next = curNode->Next;
						}
						if (curNode == m_firstNode)
						{
							m_firstNode = curNode->Next;
						}
						if (curNode == m_lastNode)
						{
							m_lastNode = prevNode;
						}

						delete curNode;

						assert(m_size>0);
						m_size--;
						return true;
					}

					prev = cur;
					++cur;
				}

				return false;
			}

			void RemoveAt(const Iterator& iter)
			{
				assert(iter.m_owner == this);
				assert(iter != End());

				Iterator prev = End();
				Iterator cur = Begin();

				while (cur != End())
				{
					if (cur == iter)
					{
						Node* curNode = cur.m_referenceNode;
						Node* prevNode = prev.m_referenceNode;

						if (prevNode)
						{
							prevNode->Next = curNode->Next;
						}
						if (curNode == m_firstNode)
						{
							m_firstNode = curNode->Next;
						}
						if (curNode == m_lastNode)
						{
							m_lastNode = prevNode;
						}

						delete curNode;

						assert(m_size>0);
						m_size--;
						break;
					}

					prev = cur;
					++cur;
				}
			}

			/**
			 * Returns a bool value indicating whether the list contains any items.
			 */
			bool IsEmpty() const { return !m_size; }
			/**
			 * Returns the number of items in the list as an uint32.
			 */
			uint32 Size() const { return m_size; }
		private:
			/** 
			 * Represents a node in the linked list containing the item data;
			 */
			struct Node
			{
				/**
				 * The item data.
				 */
				T Data;
				/**
				 * The pointer to the next node. nullptr means no next node.
				 */
				Node* Next;

				/**
				 * Initializes a new node with the given data and a empty next node.
				 */
				Node(const T& data, Node* next = nullptr) : Data(data), Next(next) { }
			};

			/** 
			 * [Internal] Copy the list from lists beginning with the given node.
			 */
			void CopyNodesFrom(const Node* otherNode)
			{
				const Node* curNode = otherNode;

				Node* prevNode = nullptr;
				while (curNode)
				{
					Node* curNodeClone = new Node(*curNode);

					if (!m_firstNode)
					{
						m_firstNode = curNodeClone;
					}

					if (prevNode)
					{
						prevNode->Next = curNodeClone;
					}

					curNode = curNode->Next;
					prevNode = curNodeClone;
				}

				m_lastNode = prevNode;
			}

			/**
			 * The node containing the first item in the list.
			 */
			Node* m_firstNode;
			/**
			 * The node containing the last item in the list.
			 */
			Node* m_lastNode;
			/** @see Size()
			 */
			uint32 m_size;
		};
	}

}


#endif