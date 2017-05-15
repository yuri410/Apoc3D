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
#include "XImporter.h"

#include "BuildConfig.h"
#include "Utils/D3DHelper.h"
#include "Utils/MeshProcessing.h"

#include <d3dx9.h>

namespace APBuild
{
	Color4 ToColor4(const D3DCOLORVALUE& v)
	{
		return Color4(v.r, v.g, v.b, v.a);
	}
	
	ModelData* XImporter::Import(const ProjectResModel& config)
	{
		ID3DXMesh* d3dmesh;
		ID3DXBuffer* d3dmaterials;
		uint32 partCount;
		{
			ID3DXBuffer* adjacent;
			HRESULT hr = D3DXLoadMeshFromX(config.SrcFile.c_str(), D3DXMESH_32BIT | D3DXMESH_MANAGED, Utils::D3DHelper::getDevice(), &adjacent, &d3dmaterials, NULL, 
				reinterpret_cast<DWORD*>(&partCount), &d3dmesh);
			assert(SUCCEEDED(hr));
			hr = d3dmesh->OptimizeInplace(D3DXMESHOPT_ATTRSORT|D3DXMESHOPT_VERTEXCACHE, 
				static_cast<DWORD*>(adjacent->GetBufferPointer()), NULL,NULL,NULL);
			assert(SUCCEEDED(hr));
			adjacent->Release();
		}

		ModelData* result = new ModelData();
		List<VertexElement> elements;
		
		Utils::meshGenerateVertexElements(d3dmesh->GetFVF(), elements);

		// parse mesh data, each mesh part from x --> mesh
		uint totalVertex = d3dmesh->GetNumVertices();
		uint totalFaces = d3dmesh->GetNumFaces();
		DWORD vertexSize = d3dmesh->GetNumBytesPerVertex();

		char* d3dVtxData;
		int* d3dIdxData;
		DWORD* d3dAttData;
		d3dmesh->LockVertexBuffer(0, (void**)&d3dVtxData);
		d3dmesh->LockIndexBuffer(0,(void**)&d3dIdxData);
		d3dmesh->LockAttributeBuffer(0, &d3dAttData);
		
		for (uint i=0;i<partCount;i++)
		{
			HashMap<int,int> vertexMapping((int)totalVertex);
			List<const char*> verticePointers(totalVertex);
			List<MeshFace> faces(totalFaces);

			for (uint j=0;j<totalFaces;j++)
			{
				if (d3dAttData[j] == i)
				{
					int32 faceIndices[3] = 
					{
						d3dIdxData[j*3], d3dIdxData[j*3+1], d3dIdxData[j*3+2]
					};

					for (int32 k=0;k<3;k++)
					{
						int d3dIdx = faceIndices[k];
						int realIdx;
						if (!vertexMapping.TryGetValue(d3dIdx, realIdx))
						{
							realIdx = verticePointers.getCount();
							vertexMapping.Add(d3dIdx, realIdx);
							verticePointers.Add(d3dVtxData+d3dIdx*vertexSize);
						}
						faceIndices[k] = realIdx;
					}

					faces.Add(MeshFace(faceIndices[0],faceIndices[1],faceIndices[2], (int)0));
				}
			}

			if (verticePointers.getCount() == 0)
				continue;

			D3DXMATERIAL d3dmtrl = *(((D3DXMATERIAL*)d3dmaterials->GetBufferPointer()) + i);
			MaterialData* mtrl = new MaterialData();
			mtrl->Ambient = ToColor4( d3dmtrl.MatD3D.Ambient);
			mtrl->Diffuse = ToColor4( d3dmtrl.MatD3D.Diffuse);
			mtrl->Specular = ToColor4( d3dmtrl.MatD3D.Specular);
			mtrl->Emissive = ToColor4( d3dmtrl.MatD3D.Emissive);
			mtrl->Power = d3dmtrl.MatD3D.Power;
			if (d3dmtrl.pTextureFilename)
				mtrl->SetTextureName(0, StringUtils::toPlatformWideString(d3dmtrl.pTextureFilename));

			MeshData* data = new MeshData();
			data->Name = L"Part" + StringUtils::UIntToString(i, StrFmt::a<2, '0'>::val);
			data->Materials.Add(mtrl);
			data->Faces = faces;
			data->VertexCount = verticePointers.getCount();
			data->VertexSize = vertexSize;
			
			data->BoundingSphere.Radius = 0;
			data->BoundingSphere.Center = Vector3::Zero;

			data->VertexElements = elements;
			
			const VertexElement* posElem = VertexElement::FindElementBySemantic(elements, VEU_Position);

			data->VertexData = new char[vertexSize*data->VertexCount];
			for (uint j = 0; j < data->VertexCount; j++)
			{
				memcpy(data->VertexData + j*vertexSize, verticePointers[j], vertexSize);

				if (posElem)
				{
					const float* vtx = reinterpret_cast<const float*>(data->VertexData + j*vertexSize + posElem->getOffset());
					data->BoundingSphere.Center += Vector3::Set(vtx);
				}
			}
			data->BoundingSphere.Center /= (float)data->VertexCount;

			if (posElem)
			{
				for (uint j = 0; j < data->VertexCount; j++)
				{
					const float* vtx = reinterpret_cast<const float*>(data->VertexData + j*vertexSize + posElem->getOffset());
					float dist = Vector3::DistanceSquared(Vector3::Set(vtx), data->BoundingSphere.Center);
					if (dist > data->BoundingSphere.Radius)
						data->BoundingSphere.Radius = dist;
				}
				data->BoundingSphere.Radius = sqrtf(data->BoundingSphere.Radius);
			}

			result->Entities.Add(data);
		}
		d3dmesh->UnlockIndexBuffer();
		d3dmesh->UnlockVertexBuffer();
		d3dmesh->UnlockAttributeBuffer();

		d3dmaterials->Release();
		d3dmesh->Release();

		return result;
	}
}