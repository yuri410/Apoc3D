#pragma once
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