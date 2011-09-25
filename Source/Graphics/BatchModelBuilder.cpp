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
#include "BatchModelBuilder.h"
#include "Collections/ExistTable.h"
#include "Collections/FastMap.h"
#include "IOLib/ModelData.h"
#include "Vfs/ResourceLocation.h"

using namespace Apoc3D::IO;

namespace Apoc3D
{
	namespace Graphics
	{
		ModelData* BatchModelBuilder::BuildData()
		{
			FastMap<MeshData*, int> targets;
			
			FastList<ModelData*> sources(m_modelTable.getCount());
			int index = 0;
			for (int i=0;i<m_modelTable.getCount();i++)
			{
				ModelData* data = new ModelData();
				data->Load(m_modelTable[i]);
				sources.Add(data);

				for (int j=0;j<data->Entities.getCount();j++)
				{
					targets.Add(data->Entities[j], index);
					index++;
				}
			}

			FastList<char*>* entVertexData = new FastList<char*>[index];
			MeshData** ents = new MeshData*[index];
			index = 0;
			for (int i=0;i<m_modelTable.getCount();i++)
			{
				ModelData* data = sources[i];
				for (int j=0;j<data->Entities.getCount();j++)
				{
					ents[index] = new MeshData();
					ents[index]->Materials = data->Entities[index]->Materials;
					ents[index]->VertexElements = data->Entities[index]->VertexElements;
					ents[index]->VertexSize = data->Entities[index]->VertexSize;
					//ents[index]->VertexCount = data->Entities[index]->VertexCount;
					//ents[index]->VertexData = new char[ents[index]->VertexCount * ents[index]->VertexSize];
					//memcpy(ents[index]->VertexData, data->Entities[index]->VertexData, ents[index]->VertexCount * ents[index]->VertexSize);
					ents[index]->BoundingSphere = data->Entities[index]->BoundingSphere;
					ents[index]->Name = data->Entities[index]->Name;
					
					index++;
				}
			}

			
			for (int i=0;i<m_entires.getCount();i++)
			{
				ModelData* src = sources[m_entires[i].SourceId];

				for (int j=0;j<src->Entities.getCount();j++)
				{
					MeshData* md = src->Entities[j];
					const Matrix& trans = m_entires[i].Transform;
					MeshData* dest = ents[ targets[md]];
					FastList<char*>& destVertex = entVertexData[targets[md]];

					const VertexElement* posElem = VertexElement::FindElementBySemantic(dest->VertexElements, VEU_Position);
					const VertexElement* nrmElem = VertexElement::FindElementBySemantic(dest->VertexElements, VEU_Normal);
					
					int baseVertex = destVertex.getCount();
					for (uint k=0;k<dest->VertexCount;k++)
					{
						const char* srcVtx = md->VertexData + k * dest->VertexSize;

						char* newVtx = new char[dest->VertexSize];
						memcpy(newVtx, srcVtx, dest->VertexSize);

						if (posElem)
						{
							Vector3 v = Vector3Utils::LDVectorPtr( (float*)(newVtx + posElem->getOffset()));
							v = Vector3Utils::TransformCoordinate(v, trans);
							*(float*)(newVtx + posElem->getOffset()) = _V3X(v);
							*(float*)((newVtx + posElem->getOffset())+1) = _V3Y(v);
							*(float*)((newVtx + posElem->getOffset())+2) = _V3Z(v);
						}
						if (nrmElem)
						{
							Vector3 v = Vector3Utils::LDVectorPtr( (float*)(newVtx + nrmElem->getOffset()));
							v = Vector3Utils::TransformNormal(v, trans);
							*(float*)(newVtx + nrmElem->getOffset()) = _V3X(v);
							*(float*)((newVtx + nrmElem->getOffset())+1) = _V3Y(v);
							*(float*)((newVtx + nrmElem->getOffset())+2) = _V3Z(v);
						}

						destVertex.Add(newVtx);
					}
					for (int k=0;k<md->Faces.getCount();k++)
					{
						MeshFace face = md->Faces[k];
						face.IndexA += baseVertex;
						face.IndexB += baseVertex;
						face.IndexC += baseVertex;
						dest->Faces.Add(face);
					}
				}
			}
			
			ModelData* mdlData = new ModelData();
			mdlData->Entities.ResizeDiscard(index);
			for (int i=0;i<index;i++)
			{
				ents[i]->VertexCount = entVertexData[i].getCount();
				ents[i]->VertexData = new char[ents[i]->VertexCount * ents[i]->VertexSize];

				for (uint j=0;j<ents[i]->VertexCount;j++)
				{
					memcpy(ents[i]->VertexData+j*ents[i]->VertexSize, entVertexData[i][j], ents[i]->VertexSize);
					delete[] entVertexData[i][j];
				}
				entVertexData[i].Clear();

				mdlData->Entities.Add(ents[i]);
			}

			for (int i=0;i<sources.getCount();i++)
			{
				delete sources[i];
			}
			return mdlData;
		}

		BatchModelBuilder::~BatchModelBuilder()
		{
			for (int i=0;i<m_modelTable.getCount();i++)
			{
				delete m_modelTable[i];
			}
		}
	}
}