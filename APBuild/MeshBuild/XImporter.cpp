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
#include "XImporter.h"

#include "Collections/FastList.h"
#include "Collections/FastMap.h"
#include "Math/Matrix.h"
#include "BuildConfig.h"
#include "Graphics/Animation/AnimationTypes.h"
#include "IOLib/MaterialData.h"
#include "IOLib/ModelData.h"
#include "Graphics/Animation/AnimationData.h"
#include "IOLib/Streams.h"
#include "Utility/StringUtils.h"

#include "D3DHelper.h"

#include <d3dx9.h>

using namespace Apoc3D::Utility;

namespace APBuild
{
	Color4 ToColor4(const D3DCOLORVALUE& v)
	{
		return Color4(v.r, v.g, v.b, v.a);
	}
	VertexElementFormat ConvertVEFormat(BYTE s)
	{
		switch(s)
		{
		case D3DDECLTYPE_FLOAT1:
			return VEF_Single;
		case D3DDECLTYPE_FLOAT2:
			return VEF_Vector2;
		case D3DDECLTYPE_FLOAT3:
			return VEF_Vector3;
		case D3DDECLTYPE_FLOAT4:
			return VEF_Vector4;
		case D3DDECLTYPE_D3DCOLOR:
			return VEF_Color;
		case D3DDECLTYPE_UBYTE4:
			return VEF_Byte4;
		case D3DDECLTYPE_SHORT2:
			return VEF_Short2;
		case D3DDECLTYPE_SHORT4:
			return VEF_Short4;
		case D3DDECLTYPE_UBYTE4N:
			return VEF_NormalizedByte4;
		case D3DDECLTYPE_SHORT2N:
			return VEF_NormalizedShort2;
		case D3DDECLTYPE_SHORT4N:
			return VEF_NormalizedShort4;
		case D3DDECLTYPE_UDEC3:
			return VEF_Normalized101010;
		case D3DDECLTYPE_DEC3N:
			return VEF_UInt101010;
		case D3DDECLTYPE_FLOAT16_2:
			return VEF_HalfVector2;
		case D3DDECLTYPE_FLOAT16_4:
			return VEF_HalfVector4;
		}
		throw Apoc3DException::createException(EX_NotSupported, L"");
			//D3DDECLTYPE_USHORT2N    = 11,
			//D3DDECLTYPE_USHORT4N    = 12,
	}
	VertexElementUsage ConvertVEUsage(BYTE s)
	{
		switch (s)
		{
		case D3DDECLUSAGE_POSITION:
			return VEU_Position;
		case D3DDECLUSAGE_BLENDWEIGHT:
			return VEU_BlendWeight;
		case D3DDECLUSAGE_BLENDINDICES:
			return VEU_BlendIndices;
		case D3DDECLUSAGE_NORMAL:
			return VEU_Normal;
		case D3DDECLUSAGE_PSIZE:
			return VEU_PointSize;
		case D3DDECLUSAGE_TEXCOORD:
			return VEU_TextureCoordinate;
		case D3DDECLUSAGE_TANGENT:
			return VEU_Tangent;
		case D3DDECLUSAGE_BINORMAL:
			return VEU_Binormal;
		case D3DDECLUSAGE_TESSFACTOR:
			return VEU_TessellateFactor;
		case D3DDECLUSAGE_POSITIONT:
			return VEU_PositionTransformed;
		case D3DDECLUSAGE_COLOR:
			return VEU_Color;
		case D3DDECLUSAGE_FOG:
			return VEU_Fog;
		case D3DDECLUSAGE_DEPTH:
			return VEU_Depth;
		case D3DDECLUSAGE_SAMPLE:
			return VEU_Sample;
		}
		throw Apoc3DException::createException(EX_NotSupported, L"");
	}
	ModelData* XImporter::Import(const MeshBuildConfig& config)
	{
		ID3DXMesh* mesh;
		ID3DXBuffer* adjacent;
		ID3DXBuffer* materials;
		uint32 partCount;
		HRESULT hr = D3DXLoadMeshFromX(config.SrcFile.c_str(), D3DXMESH_32BIT | D3DXMESH_MANAGED, D3DHelper::getDevice(), &adjacent, &materials, NULL, 
			reinterpret_cast<DWORD*>(&partCount), &mesh);
		assert(SUCCEEDED(hr));
		hr = mesh->OptimizeInplace(D3DXMESHOPT_ATTRSORT|D3DXMESHOPT_VERTEXCACHE, 
			static_cast<DWORD*>(adjacent->GetBufferPointer()), NULL,NULL,NULL);
		assert(SUCCEEDED(hr));
		adjacent->Release();

		ModelData* result = new ModelData();

		D3DVERTEXELEMENT9 d3delements[MAX_FVF_DECL_SIZE];
		D3DXDeclaratorFromFVF(mesh->GetFVF(), d3delements);

		FastList<VertexElement> elements;
		for (int i=0;i<MAX_FVF_DECL_SIZE;i++)
		{
			const D3DVERTEXELEMENT9& ver = d3delements[i];
			D3DVERTEXELEMENT9 end = D3DDECL_END();
			if (memcmp(&ver,&end,sizeof(D3DVERTEXELEMENT9)))
			{
				VertexElement e(ver.Offset, ConvertVEFormat(ver.Type), ConvertVEUsage(ver.Usage), ver.UsageIndex );
				elements.Add(e);
			}
			else break;
		}

		// parse mesh data, each mesh part from x --> mesh
		uint totalVertex = mesh->GetNumVertices();
		uint totalFaces = mesh->GetNumFaces();
		DWORD vertexSize = mesh->GetNumBytesPerVertex();

		char* vtxData;
		int* idxData;
		DWORD* attData;
		mesh->LockVertexBuffer(0, (void**)&vtxData);
		mesh->LockIndexBuffer(0,(void**)&idxData);
		mesh->LockAttributeBuffer(0, &attData);
		for (uint i=0;i<partCount;i++)
		{
			FastMap<int,int> vertexTable((int)totalVertex, Apoc3D::Collections::IBuiltInEqualityComparer<int>::Default);
			FastList<char*> vertices(totalVertex);
			FastList<MeshFace> faces(totalFaces);

			for (uint j=0;j<totalFaces;j+=3)
			{
				if (attData[j] == i)
				{
					int idxA = idxData[j*3];
					int realIdx;
					if (!vertexTable.TryGetValue(idxA, realIdx))
					{
						realIdx = vertices.getCount();
						vertexTable.Add(realIdx, realIdx);
						vertices.Add(vtxData+idxA*vertexSize);
					}
					idxA = realIdx;

					int idxB = idxData[j*3+1];
					if (!vertexTable.TryGetValue(idxB, realIdx))
					{
						realIdx = vertices.getCount();
						vertexTable.Add(realIdx, realIdx);
						vertices.Add(vtxData+idxB*vertexSize);
					}
					idxB = realIdx;


					int idxC = idxData[j*3+2];
					if (!vertexTable.TryGetValue(idxC, realIdx))
					{
						realIdx = vertices.getCount();
						vertexTable.Add(realIdx, realIdx);
						vertices.Add(vtxData+idxC*vertexSize);
					}
					idxC = realIdx;

					faces.Add(MeshFace(idxA,idxB,idxC, (int)i));
				}
			}

			if (!vertices.getCount())
				continue;

			D3DXMATERIAL d3dmtrl = *(((D3DXMATERIAL*)materials->GetBufferPointer()) + i);
			MaterialData mtrl;
			mtrl.SetDefaults();
			mtrl.Ambient = ToColor4( d3dmtrl.MatD3D.Ambient);
			mtrl.Diffuse = ToColor4( d3dmtrl.MatD3D.Diffuse);
			mtrl.Specular = ToColor4( d3dmtrl.MatD3D.Specular);
			mtrl.Emissive = ToColor4( d3dmtrl.MatD3D.Emissive);
			mtrl.Power = d3dmtrl.MatD3D.Power;
			if (d3dmtrl.pTextureFilename)
				mtrl.TextureName[0] = StringUtils::toWString(d3dmtrl.pTextureFilename);

			MeshData* data = new MeshData();
			data->Name = L"Part" + StringUtils::ToString(i,2,'0');
			data->Materials.Add(mtrl);
			data->Faces = faces;
			data->VertexCount = vertices.getCount();
			data->VertexSize = vertexSize;
			
			data->BoundingSphere.Radius = 0;
			data->BoundingSphere.Center = Vector3Utils::Zero;

			data->VertexElements = elements;
			
			const VertexElement* posElem = VertexElement::FindElementBySemantic(elements, VEU_Position);

			data->VertexData = new char[vertexSize*data->VertexCount];
			for (uint j=0;j<data->VertexCount;j++)
			{
				if (posElem)
				{
					const float* vtx = reinterpret_cast<const float*>(data->VertexData + j*vertexSize + posElem->getOffset());
					Vector3 p = Vector3Utils::LDVectorPtr(vtx);
				}
				memcpy(data->VertexData+j*vertexSize, vertices[j], vertexSize);
			}
			data->BoundingSphere.Center = Vector3Utils::Divide(data->BoundingSphere.Center, (float)data->VertexCount);

			result->Entities.Add(data);
		}
		mesh->UnlockIndexBuffer();
		mesh->UnlockVertexBuffer();
		mesh->UnlockAttributeBuffer();


		return result;
	}
}