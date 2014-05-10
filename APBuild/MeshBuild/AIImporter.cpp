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

#include "AIImporter.h"

#include "../BuildConfig.h"

#include "apoc3d/Collections/List.h"
#include "apoc3d/Collections/HashMap.h"
#include "apoc3d/Math/Matrix.h"
#include "apoc3d/Graphics/Animation/AnimationTypes.h"
#include "apoc3d/IOLib/MaterialData.h"
#include "apoc3d/IOLib/ModelData.h"
#include "apoc3d/Graphics/Animation/AnimationData.h"
#include "apoc3d/IOLib/Streams.h"
#include "apoc3d/Utility/StringUtils.h"

#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4819)
#endif

//#include <assimp/assimp.hpp>
#include "assimp/postProcess.h"
#include "assimp/scene.h"
#include "assimp/Importer.hpp"

#ifdef _MSC_VER
#pragma warning (pop)
#endif

using namespace Assimp;
using namespace Apoc3D::Utility;

namespace APBuild
{
	Color4 ConvertPropertyColor4(aiMaterialProperty* prop)
	{
		Color4 result;
		assert(prop->mType == aiPTI_Float);
		if (prop->mDataLength == sizeof(float) * 3)
		{
			result.Alpha = 1;
			result.Red = reinterpret_cast<const float*>(prop->mData)[0];
			result.Green = reinterpret_cast<const float*>(prop->mData)[1];
			result.Blue = reinterpret_cast<const float*>(prop->mData)[2];
		}
		else
		{
			result.Red = reinterpret_cast<const float*>(prop->mData)[0];
			result.Green = reinterpret_cast<const float*>(prop->mData)[1];
			result.Blue = reinterpret_cast<const float*>(prop->mData)[2];
			result.Alpha = reinterpret_cast<const float*>(prop->mData)[3];
		}
		return result;
	}
	float ConvertPropertyFloat(aiMaterialProperty* prop)
	{
		assert(prop->mType == aiPTI_Float);
		return *reinterpret_cast<const float*>(prop->mData);
	}
	ModelData* AIImporter::Import(const MeshBuildConfig& config)
	{
		Importer importer;
		
		const aiScene* scene = importer.ReadFile(StringUtils::toPlatformNarrowString(config.SrcFile), 
			aiProcess_Triangulate | aiProcess_RemoveRedundantMaterials | aiProcess_OptimizeMeshes | aiProcess_FlipUVs);
		
		ModelData* result = new ModelData();

		//FastMap<string, aiNode*> cachedbones;

		for (uint i=0;i<scene->mNumMeshes;i++)
		{
			MeshData* data = new MeshData();

			aiMesh* m = scene->mMeshes[i];

			{
				aiMaterial* amtrl = scene->mMaterials[m->mMaterialIndex];

				MaterialData* mtrl = new MaterialData();
				mtrl->SetDefaults();

				for (uint j=0;j<amtrl->mNumProperties;j++)
				{
					aiMaterialProperty* prop = amtrl->mProperties[j];
					if (std::string(prop->mKey.data) == std::string("$clr.ambient"))
					{
						mtrl->Ambient = ConvertPropertyColor4(prop);
					}
					else if (std::string(prop->mKey.data) == std::string("$clr.diffuse"))
					{
						mtrl->Diffuse = ConvertPropertyColor4(prop);
					}
					else if (std::string(prop->mKey.data) == std::string("$clr.specular"))
					{
						mtrl->Specular = ConvertPropertyColor4(prop);
					}
					else if (std::string(prop->mKey.data) == std::string("$mat.shininess"))
					{
						mtrl->Power = ConvertPropertyFloat(prop);
					}
					else if (std::string(prop->mKey.data) == std::string("$tex.file"))
					{
						String texFile = StringUtils::toPlatformWideString(prop->mData);
						StringUtils::Trim(texFile, L" \t\r\n\07");
						mtrl->TextureName[0] = texFile;
					}
				}


				data->Materials.Add(mtrl);
			}
			data->Name = StringUtils::toPlatformWideString(m->mName.data);

			// faces
			{
				data->Faces.ResizeDiscard(m->mNumFaces);
				for (uint j=0;j<m->mNumFaces;j++)
				{
					const aiFace& f = m->mFaces[j];
					assert(f.mNumIndices == 3);

					MeshFace face(f.mIndices[0], f.mIndices[1], f.mIndices[2], 0);
					data->Faces.Add(face);
				}
			}

			data->VertexCount = m->mNumVertices;
			
			FastList<VertexElement>& vtxelems = data->VertexElements;
			int offset = 0;
			//int blendIndexOffset = -1;
			//int blendWeightOffset = -1;

			if (m->HasPositions())
			{
				vtxelems.Add(VertexElement(offset, VEF_Vector3, VEU_Position, 0));
				offset += VertexElement::GetTypeSize(VEF_Vector3);
			}
			
			if (m->HasNormals())
			{
				vtxelems.Add(VertexElement(offset, VEF_Vector3, VEU_Normal, 0));
				offset += VertexElement::GetTypeSize(VEF_Vector3);
			}

			if (m->HasTangentsAndBitangents())
			{
				vtxelems.Add(VertexElement(offset, VEF_Vector3, VEU_Tangent, 0));
				offset += VertexElement::GetTypeSize(VEF_Vector3);

				vtxelems.Add(VertexElement(offset, VEF_Vector3, VEU_Binormal, 0));
				offset += VertexElement::GetTypeSize(VEF_Vector3);
			}

			for (uint j=0;j<m->GetNumUVChannels();j++)
			{
				switch (m->mNumUVComponents[j])
				{
				case 1:
					vtxelems.Add(VertexElement(offset, VEF_Single, VEU_TextureCoordinate, j));
					offset += VertexElement::GetTypeSize(VEF_Single);
					break;
				case 2:
					vtxelems.Add(VertexElement(offset, VEF_Vector2, VEU_TextureCoordinate, j));
					offset += VertexElement::GetTypeSize(VEF_Vector2);
					break;
				case 3:
					vtxelems.Add(VertexElement(offset, VEF_Vector3, VEU_TextureCoordinate, j));
					offset += VertexElement::GetTypeSize(VEF_Vector3);
					break;
				}
			}
			
			//if (m->HasBones())
			//{
			//	blendIndexOffset = offset;
			//	vtxelems.Add(VertexElement(offset, VEF_Vector4, VEU_BlendIndices, 0));
			//	offset += VertexElement::GetTypeSize(VEF_Vector4);

			//	blendWeightOffset = offset;
			//	vtxelems.Add(VertexElement(offset, VEF_Vector4, VEU_BlendWeight, 0));
			//	offset += VertexElement::GetTypeSize(VEF_Vector4);
			//}
			data->VertexSize = offset;
			
			data->VertexData = new char[data->VertexCount * data->VertexSize];

			data->BoundingSphere.Radius = 0;
			data->BoundingSphere.Center = Vector3::Zero;

			for (uint j=0;j<m->mNumVertices;j++)
			{
				int baseOffset = j * data->VertexSize;
				for (int k=0;k<vtxelems.getCount();k++)
				{
					const VertexElement& e = vtxelems[k];
					float* vertexPtr = (float*)(data->VertexData + baseOffset+e.getOffset());
					switch (e.getUsage())
					{
					case VEU_Position:
						*(vertexPtr) = m->mVertices[j].x;
						*(vertexPtr+1) = m->mVertices[j].y;
						*(vertexPtr+2) = m->mVertices[j].z;

						data->BoundingSphere.Center.X += m->mVertices[j].x;
						data->BoundingSphere.Center.Y += m->mVertices[j].y;
						data->BoundingSphere.Center.Z += m->mVertices[j].z;

						break;
					case VEU_Normal:
						*(vertexPtr) = m->mNormals[j].x;
						*(vertexPtr+1) = m->mNormals[j].y;
						*(vertexPtr+2) = m->mNormals[j].z;
						break;
					case VEU_Tangent:
						*(vertexPtr) = m->mTangents[j].x;
						*(vertexPtr+1) = m->mTangents[j].y;
						*(vertexPtr+2) = m->mTangents[j].z;
						break;
					case VEU_Binormal:
						*(vertexPtr) = m->mBitangents[j].x;
						*(vertexPtr+1) = m->mBitangents[j].y;
						*(vertexPtr+2) = m->mBitangents[j].z;
						break;
					case VEU_TextureCoordinate:
						switch (m->mNumUVComponents[e.getIndex()])
						{						
						case 3:
							*(vertexPtr+2) = m->mTextureCoords[e.getIndex()][j].z;
						case 2:
							*(vertexPtr+1) = m->mTextureCoords[e.getIndex()][j].y;	
						case 1:
							*(vertexPtr) = m->mTextureCoords[e.getIndex()][j].x;
							break;
						}
						break;
					case VEU_BlendIndices:
						break;
					case VEU_BlendWeight:
						break;
					}
				}
			}

			data->BoundingSphere.Center = Vector3::Divide(data->BoundingSphere.Center, static_cast<float>(m->mNumVertices));
			for (uint j=0;j<m->mNumVertices;j++)
			{
				Vector3 pos(m->mVertices[j].x, m->mVertices[j].y, m->mVertices[j].z);
				float rr = Vector3::DistanceSquared(data->BoundingSphere.Center, pos);
				if (rr > data->BoundingSphere.Radius)
				{
					data->BoundingSphere.Radius = rr;
				}
			}
			data->BoundingSphere.Radius = sqrtf(data->BoundingSphere.Radius);

			//data->Materials.Add();

			//int* blendCountMap = new int[m->mNumVertices];
			//memset(blendCountMap, 0, sizeof(sizeof(int)*m->mNumVertices));
			//for (int j=0;j<m->mNumBones;j++)
			//{
			//	aiBone* bone = m->mBones[j];
			//	for (int k=0;k<bone->mNumWeights;k++)
			//	{
			//		int idx = bone->mWeights[k].mVertexId;

			//		float* vertexBIPtr = (float*)(data->VertexData + idx * data->VertexSize + blendIndexOffset);
			//		float* vertexBWPtr = (float*)(data->VertexData + idx * data->VertexSize + blendWeightOffset);

			//		if (!blendCountMap[idx])
			//		{
			//			memset(vertexBIPtr, 0, VertexElement::GetTypeSize(VEF_Vector4));
			//			memset(vertexBWPtr, 0, VertexElement::GetTypeSize(VEF_Vector4));
			//		}

			//		//vertexBIPtr[blendCountMap[idx]] = bone->;
			//		blendCountMap[idx]++;
			//	}
			//}


			result->Entities.Add(data);
		}
		


		//if (scene->HasAnimations())
		//{
		//	AnimationData* animData = new AnimationData();

		//	for (int i=0;i<scene->mNumAnimations;i++)
		//	{
		//		//scene->mAnimations[i].mChannels[0].
		//	}
		//}
		return result;
	}

}