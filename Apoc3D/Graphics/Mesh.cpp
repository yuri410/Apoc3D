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

#include "Mesh.h"
#include "RenderSystem/Buffer/HardwareBuffer.h"
#include "RenderSystem/RenderDevice.h"
#include "RenderSystem/VertexDeclaration.h"
#include "RenderSystem/ObjectFactory.h"
#include "IOLib/ModelData.h"
#include "Material.h"
#include "GeometryData.h"

namespace Apoc3D
{
	namespace Graphics
	{
		Mesh::Mesh(RenderDevice* device, const MeshData* data)
			: m_name(data->Name), m_renderDevice(device), m_opBufferBuilt(false)
		{
			m_factory = device->getObjectFactory();

			// Add the materials from MeshData to the Mesh's material table,
			// which is used for material animations. See getMaterials for details.
			const MeshMaterialSet<MaterialData>& mtrls = data->Materials;

			for (uint i=0;i<mtrls.getMaterialCount();i++)
			{
				for (uint j=0;j<mtrls.getFrameCount(i);j++)
				{
					const MaterialData& md = mtrls.getMaterial(i, j);
					
					Material* m = new Material(m_renderDevice);

					m->Load(md);

					if (j)
					{
						m_materials.AddFrame(m, i);
					}
					else
					{
						m_materials.Add(m);
					}
				}
			}


			int matCount = data->Materials.getMaterialCount();
			int vertexCount = data->VertexCount;
			int faceCount = data->Faces.getCount();

			m_primitiveCount = faceCount;
			m_vertexCount = vertexCount;

			m_vertexSize = data->VertexSize;

			m_vertexElements = data->VertexElements;
			//m_parentBoneID = data->ParentBoneID;
			m_boundingSphere = data->BoundingSphere;

			m_vtxDecl = m_factory->CreateVertexDeclaration(data->VertexElements);
			
			// vertex data
			m_vertexBuffer = m_factory->CreateVertexBuffer(vertexCount, m_vtxDecl, BU_Static);
			void* vdst = m_vertexBuffer->Lock(0,0, LOCK_None);

			memcpy(vdst, data->VertexData, m_vertexSize * vertexCount);

			m_vertexBuffer->Unlock();
			
			// index data
			bool useIndex16 = vertexCount < MaxUInt16;
			FastList<uint>* indices = new FastList<uint>[matCount];

			m_partPrimitiveCount = new int[matCount];
			m_partVertexCount = new int[matCount];
			memset(m_partPrimitiveCount, 0, sizeof(int) * matCount);
			memset(m_partVertexCount, 0, sizeof(int) * matCount);

			// Based on the face data, generate indices, grouping them to each sub part.
			// Counting some statistics at the same time.
			// 
			// The mesh only supports triangle list as primitives.
			const FastList<MeshFace>& faces = data->Faces;
			for (int i=0;i<faces.getCount();i++)
			{
				const MeshFace& face = faces[i];

				const int matID = face.MaterialID;
				assert(matID<matCount);
				indices[matID].Add(face.IndexA);
				indices[matID].Add(face.IndexB);
				indices[matID].Add(face.IndexC);
				m_partPrimitiveCount[matID]++;
			}
			
			// Now put the group indices to the index buffers.
			// At the same time, find out how many vertex that each group of
			// indices is pointing to.
			bool* passed = new bool[vertexCount];
			if (useIndex16)
			{
				for (int i=0;i<matCount;i++)
				{
					memset(passed, 0, vertexCount * sizeof(bool));

					const FastList<uint>& idx = indices[i];
					IndexBuffer* indexBuffer = m_factory->CreateIndexBuffer(IBT_Bit16, idx.getCount(), BU_Static);

					ushort* ib = (ushort*)indexBuffer->Lock(0,0, LOCK_None);
					// put the indices in to the buffer
					// while marking the vertices used
					for (int j=0;j<idx.getCount();j++)
					{
						ib[j] = (ushort)idx[j];
						passed[idx[j]] = true;
					}
					indexBuffer->Unlock();

					// find out how many vertex that this index buffer
					// is pointing to.
					int vtxCount = 0;
					for (int j=0;j<vertexCount;j++)
					{
						if (passed[j])
							vtxCount++;
					}
					m_partVertexCount[i] = vtxCount;

					m_indexBuffers.Add(indexBuffer);
				}
			}
			else
			{
				// 32 bit is the way same as 16 bit
				for (int i=0;i<matCount;i++)
				{
					memset(passed, 0, vertexCount * sizeof(bool));

					const FastList<uint>& idx = indices[i];
					IndexBuffer* indexBuffer = m_factory->CreateIndexBuffer(IBT_Bit32, idx.getCount(), BU_Static);

					uint* ib = (uint*)indexBuffer->Lock(0,0, LOCK_None);
					for (int j=0;j<idx.getCount();j++)
					{
						ib[j] = idx[j];
						passed[idx[j]] = true;
					}
					indexBuffer->Unlock();

					int vtxCount = 0;
					for (int j=0;j<vertexCount;j++)
					{
						if (passed[j])
							vtxCount++;
					}
					m_partVertexCount[i] = vtxCount;

					m_indexBuffers.Add(indexBuffer);
				}
			}
			delete[] passed;
			delete[] indices;
		}


		Mesh::~Mesh(void)
		{
			delete m_vertexBuffer;
			for (int i=0;i<m_indexBuffers.getCount();i++)
			{
				delete m_indexBuffers[i];
			}
			delete m_vtxDecl;

			for (uint i=0;i<m_materials.getMaterialCount();i++)
			{
				for (uint j=0;j<m_materials.getFrameCount(i);j++)
				{
					delete m_materials.getMaterial(i,j);
				}
			}

			for (int i=0;i<m_bufferedOp.getCount();i++)
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
			data->VertexElements = m_vertexElements;
			//data->ParentBoneID = m_parentBoneID;

			//data.Materials;
			for (uint i=0;i<m_materials.getMaterialCount();i++)
			{
				for (uint j=0;j<m_materials.getFrameCount(i);j++)
				{
					MaterialData mdata;
					Material* mtrl = m_materials.getMaterial(i,j);
					mtrl->Save(mdata);

					if (j)
					{
						data->Materials.AddFrame(mdata, i);
					}
					else
					{						
						data->Materials.Add(mdata);
					}
				}
			}

			//data.VertexData;
			data->VertexData = new char[m_vertexSize * m_vertexCount];

			void* src = m_vertexBuffer->Lock(0,0,LOCK_ReadOnly);
			memcpy(data->VertexData, src, m_vertexSize * m_vertexCount);
			m_vertexBuffer->Unlock();

			//data.Faces
			data->Faces.ResizeDiscard(m_primitiveCount);
			
			for (int i=0;i<m_indexBuffers.getCount();i++)
			{
				if (m_indexBuffers[i]->getIndexType() == IBT_Bit16)
				{
					ushort* isrc = (ushort*)m_indexBuffers[i]->Lock(0,0, LOCK_ReadOnly);

					for (int j=0;j<m_partPrimitiveCount[i];j++)
					{
						int iPos = j*3;
						MeshFace face(isrc[iPos], isrc[iPos+1], isrc[iPos+2], i);
						data->Faces.Add(face);
					}

					m_indexBuffers[i]->Unlock();
				}
				else
				{
					uint* isrc = (uint*)m_indexBuffers[i]->Lock(0,0, LOCK_ReadOnly);

					for (int j=0;j<m_partPrimitiveCount[i];j++)
					{
						int iPos = j*3;
						MeshFace face(isrc[iPos], isrc[iPos+1], isrc[iPos+2], i);
						data->Faces.Add(face);
					}

					m_indexBuffers[i]->Unlock();
				}
			}
		}

		void Mesh::GetIndices(uint* dest) const
		{
			int index = 0;
			for (int i=0;i<m_indexBuffers.getCount();i++)
			{
				if (m_indexBuffers[i]->getIndexType() == IBT_Bit16)
				{
					const ushort* isrc = (ushort*)m_indexBuffers[i]->Lock(0,0, LOCK_ReadOnly);

					for (int j=0;j<m_partPrimitiveCount[i];j++)
					{
						int iPos = j * 3;

						dest[index++] = isrc[iPos];
						dest[index++] = isrc[iPos+1];
						dest[index++] = isrc[iPos+2];
					}

					m_indexBuffers[i]->Unlock();
				}
				else
				{
					const uint* isrc = (uint*)m_indexBuffers[i]->Lock(0,0, LOCK_ReadOnly);

					for (int j=0;j<m_partPrimitiveCount[i];j++)
					{
						int iPos = j * 3;

						dest[index++] = isrc[iPos];
						dest[index++] = isrc[iPos+1];
						dest[index++] = isrc[iPos+2];
					}

					m_indexBuffers[i]->Unlock();
				}
			}
		}

		void Mesh::ProcessAllTriangles(IMeshTriangleCallBack* callback) const
		{
			const int indiceCount = getIndexCount();
			const int vertexStride = m_vertexSize;
			uint* indices = new uint[indiceCount];

			const byte* src = (byte*)m_vertexBuffer->Lock(0,0,LOCK_ReadOnly);

			for (int i=0;i<indiceCount;i++)
			{
				callback->Process(*((Vector3*)(src + vertexStride * indices[i])), 
					*((Vector3*)(src + vertexStride * indices[i + 1])), 
					*((Vector3*)(src + vertexStride * indices[i + 2])));
			}
			m_vertexBuffer->Unlock();
		}
		int Mesh::CalculateSizeInBytes() const
		{
			int size = m_vertexBuffer->getSize();

			for (int i = 0; i < m_indexBuffers.getCount(); i++)
			{
				size += m_indexBuffers[i]->getSize();
			}

			return size + sizeof(Mesh);
		}

		RenderOperationBuffer* Mesh::GetRenderOperation(int level)
		{
			if (!m_opBufferBuilt)
			{
				m_bufferedOp.ReserveDiscard(getPartCount());

				for (int i=0;i<getPartCount();i++)
				{
					GeometryData* md = new GeometryData();
					md->IndexBuffer = m_indexBuffers[i];
					md->PrimitiveCount = m_partPrimitiveCount[i];
					md->PrimitiveType = PT_TriangleList;
					md->VertexBuffer = m_vertexBuffer;
					md->VertexCount = m_partVertexCount[i];
					md->VertexDecl = m_vtxDecl;
					md->VertexSize = m_vertexSize;
					md->UserData = this;

					RenderOperation op;
					op.GeometryData = md;
					op.Material = m_materials.getMaterial(i,0);
					m_bufferedOp.Add(op);
				}

				m_opBufferBuilt = true;
			}
			return &m_bufferedOp;
		}
	}
}