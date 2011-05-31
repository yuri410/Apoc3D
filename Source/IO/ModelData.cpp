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
#include "ModelData.h"
#include "TaggedData.h"
#include "BinaryReader.h"
#include "BinaryWriter.h"
#include "Apoc3DException.h"
#include "Vfs/ResourceLocation.h"
#include "Graphics/Animation/AnimationData.h"

#include "Utility/StringUtils.h"

using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace IO
	{
		static const int MdlId_V2 = 0;
		static const int MdlId_V3 = ((byte)'M' << 24) | ((byte)'E' << 16) | ((byte)'S' << 8) | ((byte)'H');
		
		static const String TAG_3_MaterialCountTag = L"MaterialCount";
		static const String TAG_3_MaterialsTag = L"Materials";

		//static const String MaterialAnimationTag = L"MaterialAnimation";
		static const String TAG_3_FaceCountTag = L"FaceCount";
		static const String TAG_3_FacesTag = L"Faces";
		static const String TAG_1_VertexFormatTag = L"VertexFormat";
		static const String TAG_3_VertexDeclTag = L"VertexDeclaration";
		static const String TAG_3_VertexCountTag = L"VertexCount";
		static const String TAG_3_VertexSizeTag = L"VertexSize";

		static const String TAG_2_MaterialAnimationTag = L"MaterialAnimation2.0";
		static const String TAG_3_VertexDataTag = L"VertexData";

		static const String TAG_3_NameTag = L"Name";

		static const String TAG_3_ParentBoneTag = L"ParentBone";
		static const String TAG_3_BoundingSphereTag = L"BoundingSphere";

		uint32 MeshData::ComputeVertexSize(const vector<VertexElement>& elements)
		{
			uint32 vertexSize = 0;
			for (size_t i = 0; i < elements.size(); i++)
			{
				vertexSize += elements[i].getSize();
			}
			return vertexSize;
		}
		void MeshData::Load(TaggedDataReader* data)
		{
			uint32 materialCount = data->GetDataUInt32(TAG_3_MaterialCountTag);
			Materials.Reserve(materialCount);

			// load material set
			
			{
				BinaryReader* br = data->GetData(TAG_3_MaterialsTag);

				for (uint32 j = 0; j<materialCount; j++)
				{
					int32 frameCount = br->ReadInt32();

					for (int32 i=0; i<frameCount; i++)
					{
						TaggedDataReader* matData = br->ReadTaggedDataBlock();

						MaterialData mtrl;
						mtrl.Load(matData);

						if (i==0)
						{
							Materials.Add(mtrl);
						}
						else
						{
							Materials.AddFrame(mtrl, i);
						}

						matData->Close();
						delete matData;
					}
				}
				

				br->Close();
				delete br;
			}

			ParentBoneID = -1;
			data->TryGetDataInt32(TAG_3_ParentBoneTag, ParentBoneID);

			if (data->Contains(TAG_3_BoundingSphereTag))
			{
				BinaryReader* br = data->GetData(TAG_3_BoundingSphereTag);
				br->ReadBoundingSphere(BoundingSphere);
				br->Close();
				delete br;
			}
			else
			{
				BoundingSphere.Center = Vector3Utils::Zero;
				BoundingSphere.Radius = 0;
			}

			// read name
			{
				BinaryReader* br = data->GetData(TAG_3_NameTag);

				Name = br->ReadString();

				br->Close();
				delete br;
			}

			// read faces
			{
				uint32 faceCount = data->GetDataInt32(TAG_3_FaceCountTag);

				Faces.reserve(faceCount);

				BinaryReader* br = data->GetData(TAG_3_FacesTag);

				for (uint i=0;i<faceCount;i++)
				{
					MeshFace face;
					face.IndexA = br->ReadInt32();
					face.IndexB = br->ReadInt32();
					face.IndexC = br->ReadInt32();

					face.MaterialID = br->ReadInt32();

					Faces.push_back(face);
				}

				br->Close();
				delete br;
			}

			// vertex element
			if (!data->Contains(TAG_3_VertexDeclTag))
			{
				// latency support
				// vertex format

				int fvf = data->GetDataInt32(TAG_1_VertexFormatTag);

				if (fvf == (256 | 2 | 16))
				{

				}
				else
				{
					throw Apoc3DException::createException(EX_NotSupported, L"The mesh format version is out of date.");
				}
			}
			else
			{
				// read vertex element
				BinaryReader* br = data->GetData(TAG_3_VertexDataTag);

				int elemConut = br->ReadInt32();
				VertexElements.reserve(elemConut);
				for (int i=0;i<elemConut;i++)
				{
					int emOfs = br->ReadInt32();
					VertexElementFormat emFormat = static_cast<VertexElementFormat>(br->ReadUInt32());
					VertexElementUsage emUsage = static_cast<VertexElementUsage>(br->ReadUInt32());
					int emIndex = br->ReadInt32();

					VertexElements.push_back(VertexElement(emOfs, emFormat, emUsage, emIndex));
				}

				br->Close();
				delete br;
			}
			
			if (data->Contains(TAG_3_VertexSizeTag))
			{
				VertexSize = data->GetDataUInt32(TAG_3_VertexSizeTag);
			}
			else
			{
				VertexSize = ComputeVertexSize(VertexElements);
			}
			VertexCount = data->GetDataUInt32(TAG_3_VertexCountTag);

			// vertex data
			{
				BinaryReader* br = data->GetData(TAG_3_VertexDataTag);

				br->ReadBytes(reinterpret_cast<char*>(VertexData), VertexCount*VertexSize);

				br->Close();
				delete br;
			}

		}

		TaggedDataWriter* MeshData::Save()
		{
			TaggedDataWriter* data = new TaggedDataWriter(true);

			uint32 materialCount = Materials.getMaterialCount();
			data->AddEntry(TAG_3_MaterialCountTag, materialCount);

			// save material
			{
				BinaryWriter* bw = data->AddEntry(TAG_3_MaterialsTag);
				for (uint32 i = 0; i < materialCount; i++)
				{
					int frameCount = Materials.getFrameCount(static_cast<int32>(i));

					bw->Write(frameCount);
					for (int j = 0; j < frameCount; j++)
					{
						TaggedDataWriter* matData = Materials.getMaterial(i, j).Save();
						bw->Write(matData);
						delete matData;
					}
				}
				bw->Close();
				delete bw;
			}

			data->AddEntry(TAG_3_ParentBoneTag, ParentBoneID);

			{
				BinaryWriter* bw = data->AddEntry(TAG_3_BoundingSphereTag);
				bw->Write(BoundingSphere);
				bw->Close();
				delete bw;
			}

			// write name
			{
				BinaryWriter* bw = data->AddEntry(TAG_3_NameTag);
				bw->Write(Name);
				bw->Close();
				delete bw;
			}

			data->AddEntry(TAG_3_FaceCountTag, static_cast<uint32>(Faces.size()));

			// write faces
			{
				BinaryWriter* bw = data->AddEntry(TAG_3_FacesTag);
				for (size_t i=0;i<Faces.size();i++)
				{
					bw->Write(Faces[i].IndexA);
					bw->Write(Faces[i].IndexB);
					bw->Write(Faces[i].IndexC);
					bw->Write(Faces[i].MaterialID);
				}
				bw->Close();
				delete bw;
			}

			// write vertex elements
			{
				BinaryWriter* bw = data->AddEntry(TAG_3_VertexDeclTag);

				bw->Write(static_cast<uint32>(VertexElements.size()));
				for (size_t i = 0; i < VertexElements.size(); i++)
				{
					bw->Write(VertexElements[i].getOffset());
					bw->Write(static_cast<uint32>(VertexElements[i].getType()));
					bw->Write(static_cast<uint32>(VertexElements[i].getUsage()));
					bw->Write(VertexElements[i].getIndex());
				}

				bw->Close();
				delete bw;
			}

			data->AddEntry(TAG_3_VertexSizeTag, VertexSize);
			data->AddEntry(TAG_3_VertexCountTag, VertexCount);

			// save vertex data
			{
				BinaryWriter* bw = data->AddEntry(TAG_3_VertexDeclTag);
				bw->Write(VertexData, VertexSize*VertexCount);
				bw->Close();
			}

			return data;
		}


		static const String TAG_3_EntityCountTag = L"EntityCount";
		static const String TAG_3_EntityPrefix = L"Ent";

		static const String TAG_3_RootBoneTag = L"RootBone";

		static const String TAG_3_BonesTag = L"Bones";
		static const String TAG_3_BoneCountTag = L"BoneCount";

		static const String TAG_3_AnimationDataTag = L"AnimationData";
		
		ModelData::~ModelData()
		{
			if (!Entities.empty())
			{
				for (size_t i=0; i<Entities.size();i++)
				{
					delete Entities[i];
				}
				Entities.clear();
			}
			
			if (AnimationData)
			{
				delete AnimationData;
				AnimationData = 0;
			}
		}

		void ModelData::ReadData(TaggedDataReader* data, int32 id)
		{
			int entCount = data->GetDataInt32(TAG_3_EntityCountTag);
			Entities.reserve(entCount);

			for (int i=0;i<entCount;i++)
			{
				String tag = StringUtils::ToString(id);
				tag = TAG_3_EntityPrefix + tag;
				BinaryReader* br = data->GetData(tag);

				TaggedDataReader* meshData = br->ReadTaggedDataBlock();

				MeshData* mesh = new MeshData();
				mesh->Load(meshData);
				Entities.push_back(mesh);

				meshData->Close();
				delete meshData;

				br->Close();
				delete br;
			}

			if (id == MdlId_V2)
			{
				if (data->Contains(TAG_2_MaterialAnimationTag))
				{
					this->AnimationData = new Graphics::Animation::AnimationData();
					BinaryReader* br = data->GetData(TAG_2_MaterialAnimationTag);
					TaggedDataReader* ad = br->ReadTaggedDataBlock();
					this->AnimationData->Load(ad);
					ad->Close();
					delete ad;
					br->Close();
					delete br;
				}
				
				if (data->Contains(TAG_3_AnimationDataTag))
				{
					this->AnimationData = new Graphics::Animation::AnimationData();
					BinaryReader* br = data->GetData(TAG_3_AnimationDataTag);
					TaggedDataReader* ad = br->ReadTaggedDataBlock();
					this->AnimationData->Load(ad);
					ad->Close();
					delete ad;
					br->Close();
					delete br;
				}
			}
			else if (id == MdlId_V3)
			{
				if (data->Contains(TAG_3_AnimationDataTag))
				{
					this->AnimationData = new Graphics::Animation::AnimationData();

					BinaryReader* br = data->GetData(TAG_3_AnimationDataTag);
					TaggedDataReader* ad = br->ReadTaggedDataBlock();
					this->AnimationData->Load(ad);
					ad->Close();
					delete ad;
					br->Close();
					delete br;
				}
				
			}
			
		}

		void ModelData::Load(const ResourceLocation* rl)
		{
			BinaryReader* br = new BinaryReader(rl->GetReadStream());

			int32 id = br->ReadInt32();
			if (id == MdlId_V2 || id == MdlId_V3)
			{
				TaggedDataReader* data = br->ReadTaggedDataBlock();

				ReadData(data, id);

				data->Close();
				delete data;
			}

			br->Close();
			delete br;
		}
		void ModelData::Save(Stream* strm) const
		{

		}
	}
}