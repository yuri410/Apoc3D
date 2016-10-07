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
#include "BatchModelBuilder.h"
#include "apoc3d/Collections/HashMap.h"
#include "apoc3d/IOLib/ModelData.h"
#include "apoc3d/Vfs/ResourceLocation.h"

using namespace Apoc3D::IO;

namespace Apoc3D
{
	namespace Graphics
	{
		typedef MeshData* PtrMeshData;
		struct LPMeshDataEqualityComparer
		{
			static bool Equals(const PtrMeshData& x, const PtrMeshData& y)
			{
				const void* a = x;
				const void* b = y;
				return a==b;
			}

			static int64 GetHashCode(const PtrMeshData& obj)
			{
				const void* s = obj;
				return reinterpret_cast<int64>(s);
			}
		};

		struct Vertex64B
		{
			char data[64];
		};

		ModelData* BatchModelBuilder::BuildData()
		{
			// mapping from MeshData in Models to the index in the sources list
			HashMap<MeshData*, int, LPMeshDataEqualityComparer> targets(10);
			
			List<ModelData*> sources(m_modelTable.getCount());
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

			ModelData* mdlData = new ModelData();

			if (m_modelTable.getCount()==0)
				return mdlData;

			int vertexSize = sources[0]->Entities[0]->VertexSize;
			// fast way: using fixed vertex size
			if (vertexSize<=sizeof(Vertex64B))
			{
				List<Vertex64B>* entVertexData = new List<Vertex64B>[index];
				MeshData** ents = new MeshData*[index]; // holds the result entities
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
					const Matrix& trans = m_entires[i].Transform;

					for (int j=0;j<src->Entities.getCount();j++)
					{
						MeshData* md = src->Entities[j];
						MeshData* dest = ents[ targets[md]];
						List<Vertex64B>& destVertex = entVertexData[targets[md]];

						const VertexElement* posElem = VertexElement::FindElementBySemantic(md->VertexElements, VEU_Position);
						const VertexElement* nrmElem = VertexElement::FindElementBySemantic(md->VertexElements, VEU_Normal);

						int baseVertex = destVertex.getCount();
						for (uint k=0;k<md->VertexCount;k++)
						{
							const char* srcVtx = md->VertexData + k * md->VertexSize;

							Vertex64B newVtx;// = new char[md->VertexSize];
							memcpy(&newVtx, srcVtx, md->VertexSize);
							char* newVtxPtr = (char*)&newVtx;

							if (posElem)
							{
								Vector3 v = Vector3::Set((float*)(newVtxPtr + posElem->getOffset()));

								v = Vector3::TransformCoordinate(v, trans);
								*((float*)(newVtxPtr + posElem->getOffset())+0) = v.X;
								*((float*)(newVtxPtr + posElem->getOffset())+1) = v.Y;
								*((float*)(newVtxPtr + posElem->getOffset())+2) = v.Z;
							}
							if (nrmElem)
							{
								Vector3 v = Vector3::Set((float*)(newVtxPtr + nrmElem->getOffset()));
								v = Vector3::TransformNormal(v, trans);
								*((float*)(newVtxPtr + nrmElem->getOffset())+0) = v.X;
								*((float*)(newVtxPtr + nrmElem->getOffset())+1) = v.Y;
								*((float*)(newVtxPtr + nrmElem->getOffset())+2) = v.Z;
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

				mdlData->Entities.ResizeDiscard(index);
				for (int i=0;i<index;i++)
				{
					ents[i]->VertexCount = entVertexData[i].getCount();
					ents[i]->VertexData = new char[ents[i]->VertexCount * ents[i]->VertexSize];

					for (uint j=0;j<ents[i]->VertexCount;j++)
					{
						memcpy(ents[i]->VertexData+j*ents[i]->VertexSize, &entVertexData[i][j], ents[i]->VertexSize);
					}
					entVertexData[i].Clear();

					mdlData->Entities.Add(ents[i]);
				}
			}
			else // other cases: use the slow way
			{
				// TODO: this can be optimized if using a buffer for the final mesh vertex
				// instead of allocating room for each vertex

				List<char*>* entVertexData = new List<char*>[index];
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
					const Matrix& trans = m_entires[i].Transform;

					for (int j=0;j<src->Entities.getCount();j++)
					{
						MeshData* md = src->Entities[j];
						MeshData* dest = ents[ targets[md]];
						List<char*>& destVertex = entVertexData[targets[md]];

						const VertexElement* posElem = VertexElement::FindElementBySemantic(md->VertexElements, VEU_Position);
						const VertexElement* nrmElem = VertexElement::FindElementBySemantic(md->VertexElements, VEU_Normal);

						int baseVertex = destVertex.getCount();
						for (uint k=0;k<md->VertexCount;k++)
						{
							const char* srcVtx = md->VertexData + k * md->VertexSize;

							char* newVtx = new char[md->VertexSize];
							memcpy(newVtx, srcVtx, md->VertexSize);

							if (posElem)
							{
								Vector3 v = Vector3::Set((float*)(newVtx + posElem->getOffset()));
								v = Vector3::TransformCoordinate(v, trans);
								*((float*)(newVtx + posElem->getOffset())+0) = v.X;
								*((float*)(newVtx + posElem->getOffset())+1) = v.Y;
								*((float*)(newVtx + posElem->getOffset())+2) = v.Z;
							}
							if (nrmElem)
							{
								Vector3 v = Vector3::Set((float*)(newVtx + nrmElem->getOffset()));
								v = Vector3::TransformNormal(v, trans);
								*((float*)(newVtx + nrmElem->getOffset())+0) = v.X;
								*((float*)(newVtx + nrmElem->getOffset())+1) = v.Y;
								*((float*)(newVtx + nrmElem->getOffset())+2) = v.Z;
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

			}
			
			for (int i=0;i<sources.getCount();i++)
			{
				delete sources[i];
			}
			return mdlData;
		}

		BatchModelBuilder::BatchModelBuilder() 
		{

		}

		BatchModelBuilder::~BatchModelBuilder()
		{

		}

		void BatchModelBuilder::AddEntry(int source, const Matrix& transform)
		{
			Entry ent = { source, transform };
			m_entires.Add(ent);
		}

		void BatchModelBuilder::AddSource(const FileLocation& fl)
		{
			m_modelTable.Add(fl);
		}

	}
}