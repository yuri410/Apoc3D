#include "AIImporter.h"

#include <assimp/include/assimp.hpp>
#include <assimp/include/aiPostProcess.h>
#include <assimp/include/aiScene.h>

#include "IOLib/ModelData.h"
#include "Graphics/Animation/AnimationData.h"
#include "IOLib/Streams.h"

namespace APBuild
{
	string toString(const String& str)
	{
		char* buffer = new char[str.length()];
		wcstombs(buffer, str.c_str(), str.length());
		string result = buffer;
		delete[] buffer;
		return result;
	}
	String toWString(const string& str)
	{
		wchar_t* buffer = new wchar_t[str.length()];
		mbstowcs(buffer, str.c_str(), str.length());
		String result = buffer;
		delete[] buffer;
		return result;
	}
	ModelData* AIImporter::Import(const MeshBuildConfig& config)
	{
		Assimp::Importer importer;
		
		const aiScene* scene = importer.ReadFile(toString(config.SrcFile), 
			aiProcess_Triangulate | aiProcess_RemoveRedundantMaterials | aiProcess_OptimizeMeshes | aiProcess_FlipUVs);
		
		ModelData* result = new ModelData();

		//FastMap<string, aiNode*> cachedbones;

		for (int i=0;i<scene->mNumMeshes;i++)
		{
			MeshData* data = new MeshData();

			aiMesh* m = scene->mMeshes[i];

			data->Name = toWString(m->mName.data);

			// faces
			{
				data->Faces.ResizeDiscard(m->mNumFaces);
				for (int j=0;j<m->mNumFaces;j++)
				{
					aiFace f = m->mFaces[i];
					assert(f.mNumIndices == 3);

					MeshFace face(f.mIndices[0], f.mIndices[1], f.mIndices[2], m->mMaterialIndex);
					data->Faces.Add(face);
				}
			}

			data->VertexCount = m->mNumVertices;
			
			FastList<VertexElement>& vtxelems = data->VertexElements;
			int offset = 0;
			int blendIndexOffset = -1;
			int blendWeightOffset = -1;

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

			for (int j=0;j<m->GetNumUVChannels();j++)
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
			for (int j=0;j<m->mNumVertices;j++)
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
							*(vertexPtr+2) = m->mTextureCoords[j][e.getIndex()].z;
						case 2:
							*(vertexPtr+1) = m->mTextureCoords[j][e.getIndex()].y;	
						case 1:
							*(vertexPtr) = m->mTextureCoords[j][e.getIndex()].x;
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
		
		FileOutStream* stream = new FileOutStream(config.DstFile);
		result->Save(stream);
		delete stream;

		//if (scene->HasAnimations())
		//{
		//	AnimationData* animData = new AnimationData();

		//	for (int i=0;i<scene->mNumAnimations;i++)
		//	{
		//		//scene->mAnimations[i].mChannels[0].
		//	}
		//}
		
	}

}