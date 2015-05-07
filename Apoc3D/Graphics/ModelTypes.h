#pragma once
#ifndef APOC3D_MODELTYPES_H
#define APOC3D_MODELTYPES_H

/**
 * -----------------------------------------------------------------------------
 * This source file is part of Apoc3D Engine
 * 
 * Copyright (c) 2009+ Tao Xin
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  if not, write to the Free Software Foundation, 
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA, or go to
 * http://www.gnu.org/copyleft/gpl.txt.
 * 
 * -----------------------------------------------------------------------------
 */

#include "apoc3d/Collections/List.h"
#include "apoc3d/Math/Vector.h"

using namespace Apoc3D::Math;
using namespace Apoc3D::Collections;

namespace Apoc3D
{
	namespace Graphics
	{
		/** Defines the triangle of triangle mesh */
		struct APAPI MeshFace 
		{
			int32 IndexA;
			int32 IndexB;
			int32 IndexC;
			int32 MaterialID;
			
			MeshFace() { }
			MeshFace(int32 A, int32 B, int32 C)
				: IndexA(A), IndexB(B), IndexC(C), MaterialID(-1)
			{
			}
			MeshFace(int32 A, int32 B, int32 C, int32 matId)
				: IndexA(A), IndexB(B), IndexC(C), MaterialID(matId)
			{
			}
		};

		class APAPI IMeshTriangleCallBack
		{
		protected:
			IMeshTriangleCallBack() {}
		public:
			virtual ~IMeshTriangleCallBack(){}

			virtual void Process(const Vector3& a, const Vector3& b, const Vector3& c) = 0;
		};

		template<class M>
		class MeshMaterialSet
		{
		public:
			class Iterator
			{
				friend class MeshMaterialSet;
			public:
				M& operator*() const 
				{
					assert(m_set);
					const List< List<M>* >& set = *m_set;
					return set[m_index]->operator[](m_frame);
				}

				M* operator->() const { return &operator*(); }

				bool operator==(const Iterator &other) const
				{
					return other.m_set == m_set && other.m_index == m_index && other.m_frame == m_frame;
				}
				bool operator!=(const Iterator &other) const { return !(*this == other); }

				Iterator& operator++()
				{
					MoveToNext();
					
					return *this;
				}
				Iterator operator++(int)
				{
					Iterator result = *this;
					++(*this);
					return result;
				}

			private:
				Iterator(const List< List<M>* >* set) : m_set(set)
				{
					MoveToNext();
				}
				Iterator(const List< List<M>* >* set, int32 idx, int32 frm) : m_set(set), m_index(idx), m_frame(frm) { }

				void MoveToNext()
				{
					assert(m_set);

					const List< List<M>* >& set = *m_set;

					if (m_index != -1)
					{
						if (m_frame != -1)
						{
							m_frame++;
							if (m_frame >= set[m_index]->getCount())
							{
								m_index++;
								m_frame = 0;

								if (m_index >= set.getCount())
								{
									m_index = -1;
									m_frame = -1;
								}
							}
						}
						else
						{
							// initial. Locate to the first one

							while (set[m_index]->getCount() == 0)
							{
								m_index++;

								if (m_index >= set.getCount())
								{
									m_index = -1;
									m_frame = -1;
									break;
								}
							}

							if (m_index != -1)
								m_frame = 0;
						}
					}
				}


				const List< List<M>* >* m_set;

				int32 m_index = 0;
				int32 m_frame = -1;
			};


			~MeshMaterialSet()
			{
				for (int32 i = 0; i < m_set.getCount(); i++)
				{
					delete m_set[i];
				}
				m_set.Clear();
			}

			void AddFrame(const M& mtrl, int index)
			{
				m_set[index]->Add(mtrl);
			}
			void Add(const M& mtrl)
			{
				m_set.Add(new List<M>());
				int idx = m_set.getCount();
				idx--;
				m_set[idx]->Add(mtrl);
			}
			void RemoveFrame(int index, int frameIndex)
			{
				m_set[index]->RemoveAt(frameIndex);
			}
			M& getMaterial(int index, int frame = 0) { return m_set[index]->operator[](frame); }
			const M& getMaterial(int index, int frame = 0) const { return m_set[index]->operator[](frame); }


			void Reserve(int32 mtrlCount)
			{
				m_set.Resize(mtrlCount);
			}
			void Reserve(int32 index, int32 frameCount)
			{
				m_set[index]->Resize(frameCount);
			}

			int32 getMaterialCount() const { return m_set.getCount(); }
			int32 getFrameCount(int index) const { return m_set[index]->getCount(); }

			Iterator begin() const { return Iterator(&m_set); }
			Iterator end() const { return Iterator(&m_set, -1, -1); }
		private:
			List< List<M>* > m_set;

		};
	}
}

#endif