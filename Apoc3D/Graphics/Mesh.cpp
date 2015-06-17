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

#include "Mesh.h"
#include "RenderSystem/HardwareBuffer.h"
#include "RenderSystem/RenderDevice.h"
#include "RenderSystem/VertexDeclaration.h"
#include "Material.h"
#include "GeometryData.h"

#include "apoc3d/IOLib/ModelData.h"

namespace Apoc3D
{
	namespace Graphics
	{
		Mesh::Mesh(RenderDevice* device, const MeshData* data)
			: m_name(data->Name), m_renderDevice(device)
		{
			m_factory = device->getObjectFactory();

			int matCount = data->Materials.getMaterialCount();

			// Add the materials from MeshData to the Mesh's material table,
			// which is used for material animations. See getMaterials for details.
			const MeshMaterialSet<MaterialData*>& mtrls = data->Materials;

			m_subParts.ReserveDiscard(matCount);


			for (int32 i = 0; i < mtrls.getMaterialCount(); i++)
			{
				for (int32 j = 0; j < mtrls.getFrameCount(i); j++)
				{
					const MaterialData* md = mtrls.getMaterial(i, j);

					Material* m = new Material(m_renderDevice);

					m->Load(*md);

					m_subParts[i].MaterialFrames.Add(m);
					//if (j)
					//{
					//	m_materials.AddFrame(m, i);
					//}
					//else
					//{
					//	m_materials.Add(m);
					//}
				}
			}


			int vertexCount = data->VertexCount;
			int faceCount = data->Faces.getCount();

			m_primitiveCount = faceCount;
			m_vertexCount = vertexCount;

			m_vertexSize = data->VertexSize;

			//m_vertexElements = data->VertexElements;
			m_boundingSphere = data->BoundingSphere;

			m_vtxDecl = m_factory->CreateVertexDeclaration(data->VertexElements);
			
			// vertex data
			m_vertexBuffer = m_factory->CreateVertexBuffer(vertexCount, m_vtxDecl, BU_Static);
			void* vdst = m_vertexBuffer->Lock(0,0, LOCK_None);
			memcpy(vdst, data->VertexData, m_vertexSize * vertexCount);
			m_vertexBuffer->Unlock();
			
			// index data
			bool useIndex16 = vertexCount < MaxUInt16;
			List<uint>* partIndices = new List<uint>[matCount];

			// Based on the face data, generate indices, grouping them to each sub part.
			// Counting some statistics at the same time.
			// 
			// The mesh only supports triangle list as primitives.
			for (const MeshFace& face : data->Faces)
			{
				const int matID = face.MaterialID;
				assert(matID < matCount);
				partIndices[matID].Add(face.IndexA);
				partIndices[matID].Add(face.IndexB);
				partIndices[matID].Add(face.IndexC);

				m_subParts[matID].PrimitiveCount++;
			}
			
			// Now put the group indices to the index buffers.
			// At the same time, find out how many vertex that each group of
			// indices is pointing to.
			bool* used = new bool[vertexCount];
			for (int i = 0; i < matCount; i++)
			{
				memset(used, 0, vertexCount * sizeof(bool));

				SubPart& sp = m_subParts[i];

				if (useIndex16)
				{
					sp.SetIndexData<ushort, IBT_Bit16>(m_factory, partIndices[i], m_vertexCount, used);
				}
				else
				{
					sp.SetIndexData<uint32, IBT_Bit32>(m_factory, partIndices[i], m_vertexCount, used);
				}
			}

			delete[] used;
			delete[] partIndices;
		}


		Mesh::~Mesh()
		{
			delete m_vertexBuffer;
			delete m_vtxDecl;

			for (int i = 0; i < m_bufferedOp.getCount(); i++)
			{
				delete m_bufferedOp[i].GeometryData;
			}
		}

		void Mesh::Save(MeshData* data)
		{
			data->BoundingSphere = m_boundingSphere;
			data->Name = m_name;
			data->VertexCount = m_vertexCount;
			data->VertexSize = m_vertexSize;

			m_vtxDecl->CopyTo(data->VertexElements);
			//data->ParentBoneID = m_parentBoneID;

			for (int32 i = 0; i < m_subParts.getCount(); i++)
			{
				const SubPart& sp = m_subParts[i];

				for (Material* mtrl : sp.MaterialFrames)
				{
					MaterialData* mdata = new MaterialData();
					mtrl->Save(*mdata);

					if (mtrl == sp.MaterialFrames[0])
					{
						data->Materials.Add(mdata);
					}
					else
					{
						data->Materials.AddFrame(mdata, i);
					}
				}
			}

			data->VertexData = new char[m_vertexSize * m_vertexCount];
			void* src = m_vertexBuffer->Lock(0,0,LOCK_ReadOnly);
			memcpy(data->VertexData, src, m_vertexSize * m_vertexCount);
			m_vertexBuffer->Unlock();

			data->Faces.ResizeDiscard(m_primitiveCount);
			
			for (int32 i = 0; i < m_subParts.getCount();i++)
			{
				const SubPart& sp = m_subParts[i];

				if (sp.Indices->getIndexType() == IBT_Bit16)
				{
					ushort* isrc = (ushort*)sp.Indices->Lock(0, 0, LOCK_ReadOnly);

					for (int j = 0; j < sp.PrimitiveCount; j++)
					{
						int iPos = j * 3;
						MeshFace face(isrc[iPos], isrc[iPos + 1], isrc[iPos + 2], i);
						data->Faces.Add(face);
					}

					sp.Indices->Unlock();
				}
				else
				{
					uint* isrc = (uint*)sp.Indices->Lock(0, 0, LOCK_ReadOnly);

					for (int j = 0; j < sp.PrimitiveCount; j++)
					{
						int iPos = j * 3;
						MeshFace face(isrc[iPos], isrc[iPos + 1], isrc[iPos + 2], i);
						data->Faces.Add(face);
					}

					sp.Indices->Unlock();
				}
			}
		}

		void Mesh::GetIndices(uint* dest) const
		{
			int index = 0;
			for (const SubPart& sp : m_subParts)
			{
				if (sp.Indices->getIndexType() == IBT_Bit16)
				{
					const ushort* isrc = (ushort*)sp.Indices->Lock(0, 0, LOCK_ReadOnly);

					for (int j = 0; j < sp.PrimitiveCount; j++)
					{
						int iPos = j * 3;

						dest[index++] = isrc[iPos];
						dest[index++] = isrc[iPos + 1];
						dest[index++] = isrc[iPos + 2];
					}

					sp.Indices->Unlock();
				}
				else
				{
					const uint* isrc = (uint*)sp.Indices->Lock(0, 0, LOCK_ReadOnly);

					for (int j = 0; j < sp.PrimitiveCount; j++)
					{
						int iPos = j * 3;

						dest[index++] = isrc[iPos];
						dest[index++] = isrc[iPos + 1];
						dest[index++] = isrc[iPos + 2];
					}

					sp.Indices->Unlock();
				}
			}
		}

		void Mesh::ProcessAllTriangles(IMeshTriangleCallBack* callback) const
		{
			const int indiceCount = getIndexCount();
			const int vertexStride = m_vertexSize;
			int32 vertexPositionOffset = 0;
			
			const VertexElement* posElem = m_vtxDecl->FindElementBySemantic(VEU_Position);


			uint* indices = new uint[indiceCount];
			GetIndices(indices);

			const byte* src = (byte*)m_vertexBuffer->Lock(0,0,LOCK_ReadOnly);
			src += vertexPositionOffset; // offset once for all

			for (int i = 0; i < m_primitiveCount; i++)
			{
				int32 idx = i * 3;
				callback->Process(*((Vector3*)(src + vertexStride * indices[idx])),
					*((Vector3*)(src + vertexStride * indices[idx + 1])),
					*((Vector3*)(src + vertexStride * indices[idx + 2])));
			}
			m_vertexBuffer->Unlock();

			delete[] indices;
		}
		int Mesh::CalculateSizeInBytes() const
		{
			int size = m_vertexBuffer->getSize();

			for (const SubPart& sp : m_subParts)
			{
				size += sp.Indices->getSize();
			}

			return size + sizeof(Mesh);
		}

		RenderOperationBuffer* Mesh::GetRenderOperation(int level)
		{
			if (!m_opBufferBuilt)
			{
				m_opBufferBuilt = true;

				m_bufferedOp.ReserveDiscard(getPartCount());

				for (const SubPart& sp : m_subParts)
				{
					GeometryData* md = new GeometryData();
					md->VertexBuffer = m_vertexBuffer;
					md->IndexBuffer = sp.Indices;
					md->VertexDecl = m_vtxDecl;

					md->PrimitiveCount = sp.PrimitiveCount;
					md->PrimitiveType = PT_TriangleList;

					md->VertexCount = sp.VertexCount;
					md->VertexSize = m_vertexSize;

					md->UsedVertexRangeStart = sp.VertexRangeUsedStart;
					md->UsedVertexRangeCount = sp.VertexRangeUsedCount;

					md->UserData = this;

					RenderOperation op;
					op.GeometryData = md;
					op.Material = sp.MaterialFrames[0];
					m_bufferedOp.Add(op);
				}
			}
			return &m_bufferedOp;
		}


		Mesh::SubPart::~SubPart()
		{
			DELETE_AND_NULL(Indices);
			MaterialFrames.DeleteAndClear();
		}

		Mesh::SubPart::SubPart(SubPart&& o)
			: MaterialFrames(std::move(o.MaterialFrames)), Indices(o.Indices),
			PrimitiveCount(o.PrimitiveCount), VertexCount(o.VertexCount), 
			VertexRangeUsedStart(o.VertexRangeUsedStart), VertexRangeUsedCount(o.VertexRangeUsedCount)
		{
			o.Indices = nullptr;
		}

		Mesh::SubPart& Mesh::SubPart::operator=(SubPart&& o)
		{
			if (this != &o)
			{
				MaterialFrames = std::move(o.MaterialFrames);
				Indices = o.Indices;
				PrimitiveCount = o.PrimitiveCount;
				VertexCount = o.VertexCount;
				VertexRangeUsedStart = o.VertexRangeUsedStart;
				VertexRangeUsedCount = o.VertexRangeUsedCount;

				o.Indices = nullptr;
			}
			return *this;
		}

		template <typename T, IndexBufferType IBT>
		void Mesh::SubPart::SetIndexData(ObjectFactory* fac, const List<uint>& pi, int32 vertexCount, bool* used)
		{
			int32 vertexUsedMin = vertexCount - 1;
			int32 vertexUsedMax = 0;

			IndexBuffer* indexBuffer = fac->CreateIndexBuffer(IBT, pi.getCount(), BU_Static);

			T* ib = (T*)indexBuffer->Lock(0, 0, LOCK_None);
			// put the indices in to the buffer
			// and do some other stats work
			for (int j = 0; j < pi.getCount(); j++)
			{
				uint idx = pi[j];

				ib[j] = (T)idx;
				used[idx] = true;

				if ((int32)idx < vertexUsedMin)
					vertexUsedMin = (int32)idx;
				if ((int32)idx > vertexUsedMax)
					vertexUsedMax = (int32)idx;
			}
			indexBuffer->Unlock();

			// find out how many vertex that this index buffer
			// is pointing to.
			for (int j = 0; j < vertexCount; j++)
			{
				if (used[j])
					VertexCount++;
			}

			VertexRangeUsedStart = vertexUsedMin;
			VertexRangeUsedCount = vertexUsedMax - vertexUsedMin + 1;
			if (VertexRangeUsedCount < 0)
				VertexRangeUsedCount = 0;

			Indices = indexBuffer;
		}
	}
}