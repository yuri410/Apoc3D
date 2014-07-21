#pragma once
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
#include "APBCommon.h"

namespace APBuild
{
	namespace Utils
	{
		template <typename VertexType>
		void meshWeldVertices(HashMap<VertexType, int>& vtxHashTable, MeshData* mesh, List<VertexType>& result)
		{
			const VertexType* vertices = reinterpret_cast<const VertexType*>(mesh->VertexData);
			uint totalVertexCount = mesh->VertexCount;

			//HashMap<VertexType, int> vtxHashTable(totalVertexCount, &vec3Comparer);
			List<VertexType>& newVertexList = result;
			newVertexList.ReserveDiscard(totalVertexCount);

			for (int j=0;j<mesh->Faces.getCount();j++)
			{
				MeshFace& face = mesh->Faces[j];

				int32* indices[3] = { &face.IndexA, &face.IndexB, &face.IndexC };

				for (int32 k=0;k<3;k++)
				{
					Vector3 va = vertices[*indices[k]];
					int existingIndex;
					if (!vtxHashTable.TryGetValue(va, existingIndex))
					{
						existingIndex = newVertexList.getCount();
						newVertexList.Add(va);
						vtxHashTable.Add(va, existingIndex);
					}
					*indices[k] = existingIndex;
				}
			}

		}

		void meshGenerateVertexElements(uint32 fvf, List<VertexElement>& elements);
		
	}
}