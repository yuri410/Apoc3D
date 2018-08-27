#pragma once
#ifndef APOC3D_MODELTYPES_H
#define APOC3D_MODELTYPES_H

/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2009-2018 Tao Xin
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

#include "apoc3d/Collections/List.h"
#include "apoc3d/Collections/CollectionsCommon.h"
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
			static M& GetItem(const List< List<M>* >* s, int32 i, int32 j)
			{
				const List< List<M>* >& set = *s;
				return set[i]->operator[](j);
			}
			static int32 GetCountI(const List< List<M>* >* s) { return s->getCount(); }
			static int32 GetCountJ(const List< List<M>* >* s, int32 i) { return s->operator[](i)->getCount(); }

		public:
			using Iterator = Apoc3D::Collections::Iterator2D < M, List< List<M>* >, GetItem, GetCountI, GetCountJ>;
			

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


			void Resize(int32 mtrlCount)
			{
				m_set.Resize(mtrlCount);
			}
			void Resize(int32 index, int32 frameCount)
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