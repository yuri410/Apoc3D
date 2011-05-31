/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Games

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

#ifndef MODELTYPES_H
#define MODELTYPES_H

#include "Common.h"
#include "Math/Vector.h"

using namespace Apoc3D::Math;

namespace Apoc3D
{
	namespace Graphics
	{
		/** Defines the triangle of triangle mesh
		*/
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
			IMeshTriangleCallBack();
		public:
			virtual ~IMeshTriangleCallBack();

			virtual void Process(const Vector3& a, const Vector3& b, const Vector3& c) = 0;
		};

		template<class M>
		class APAPI MeshMaterialSet
		{
		private:
			vector<vector<M> > m_set;

		public:
			void AddFrame(M& mtrl, int index)
			{
				m_set[index].push_back(mtrl);
			}
			void Add(M& mtrl)
			{
				m_set.push_back(mtrl);
			}

			M& getMaterial(int index, int frame = 0) const { return m_set[index][frame]; }

			void Reserve(int32 mtrlCount)
			{
				m_set.reserve(mtrlCount);
			}
			void Reserve(int32 index, int32 frameCount)
			{
				m_set[index].reserve(frameCount);
			}
		};
	}
}

#endif