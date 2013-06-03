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
#include "ModelData.h"
#include "TaggedData.h"
#include "BinaryReader.h"
#include "BinaryWriter.h"
#include "apoc3d/ApocException.h"
#include "apoc3d/Vfs/ResourceLocation.h"
#include "apoc3d/Graphics/Animation/AnimationData.h"
#include "apoc3d/Core/Logging.h"
#include "apoc3d/Utility/StringUtils.h"

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

		//static const String TAG_3_ParentBoneTag = L"ParentBone";
		static const String TAG_3_BoundingSphereTag = L"BoundingSphere";

		uint32 MeshData::ComputeVertexSize(const FastList<VertexElement>& elements)
		{
			uint32 vertexSize = 0;
			for (int i = 0; i < elements.getCount(); i++)
			{
				vertexSize += elements[i].getSize();
			}
			return vertexSize;
		}
		void MeshData::LoadData(TaggedDataReader* data)
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

						MaterialData* mtrl = new MaterialData();
						mtrl->LoadData(matData);

						if (i==0)
						{
							Materials.Add(mtrl);
						}
						else
						{
							Materials.AddFrame(mtrl, j);
						}

						matData->Close();
						delete matData;
					}
				}
				

				br->Close();
				delete br;
			}

			//ParentBoneID = -1;
			//data->TryGetDataInt32(TAG_3_ParentBoneTag, ParentBoneID);

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
			data->GetDataString(TAG_3_NameTag, Name);

			// read faces
			{
				uint32 faceCount = data->GetDataInt32(TAG_3_FaceCountTag);

				Faces.ResizeDiscard(faceCount);

				BinaryReader* br = data->GetData(TAG_3_FacesTag);

				for (uint i=0;i<faceCount;i++)
				{
					MeshFace face;
					face.IndexA = br->ReadInt32();
					face.IndexB = br->ReadInt32();
					face.IndexC = br->ReadInt32();

					face.MaterialID = br->ReadInt32();

					Faces.Add(face);
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
					throw AP_EXCEPTION(EX_NotSupported, L"The format version of this mesh data is out of date.");
				}
			}
			else
			{
				// read vertex element
				BinaryReader* br = data->GetData(TAG_3_VertexDeclTag);

				int elemConut = br->ReadInt32();
				VertexElements.ResizeDiscard(elemConut);
				for (int i=0;i<elemConut;i++)
				{
					int emOfs = br->ReadInt32();
					VertexElementFormat emFormat = static_cast<VertexElementFormat>(br->ReadUInt32());
					VertexElementUsage emUsage = static_cast<VertexElementUsage>(br->ReadUInt32());
					int emIndex = br->ReadInt32();

					VertexElements.Add(VertexElement(emOfs, emFormat, emUsage, emIndex));
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
				VertexData = new char[VertexCount*VertexSize];
				br->ReadBytes(reinterpret_cast<char*>(VertexData), VertexCount*VertexSize);

				br->Close();
				delete br;
			}

		}

		TaggedDataWriter* MeshData::SaveData()
		{
			TaggedDataWriter* data = new TaggedDataWriter(true);

			uint32 materialCount = Materials.getMaterialCount();
			data->AddEntryUInt32(TAG_3_MaterialCountTag, materialCount);

			// save material
			{
				BinaryWriter* bw = data->AddEntry(TAG_3_MaterialsTag);
				for (uint32 i = 0; i < materialCount; i++)
				{
					int frameCount = Materials.getFrameCount(static_cast<int32>(i));

					bw->WriteInt32(frameCount);
					for (int j = 0; j < frameCount; j++)
					{
						TaggedDataWriter* matData = Materials.getMaterial(i, j)->SaveData();
						bw->WriteTaggedDataBlock(matData);
						delete matData;
					}
				}
				bw->Close();
				delete bw;
			}

			//data->AddEntry(TAG_3_ParentBoneTag, ParentBoneID);

			{
				BinaryWriter* bw = data->AddEntry(TAG_3_BoundingSphereTag);
				bw->WriteBoundingSphere(BoundingSphere);
				bw->Close();
				delete bw;
			}

			// write name
			data->AddEntryString(TAG_3_NameTag, Name);
			
			data->AddEntryUInt32(TAG_3_FaceCountTag, static_cast<uint32>(Faces.getCount()));

			// write faces
			{
				BinaryWriter* bw = data->AddEntry(TAG_3_FacesTag);
				for (int i=0;i<Faces.getCount();i++)
				{
					bw->WriteInt32(Faces[i].IndexA);
					bw->WriteInt32(Faces[i].IndexB);
					bw->WriteInt32(Faces[i].IndexC);
					bw->WriteInt32(Faces[i].MaterialID);
				}
				bw->Close();
				delete bw;
			}

			// write vertex elements
			{
				BinaryWriter* bw = data->AddEntry(TAG_3_VertexDeclTag);

				bw->WriteUInt32(static_cast<uint32>(VertexElements.getCount()));
				for (int i = 0; i < VertexElements.getCount(); i++)
				{
					bw->WriteInt32(VertexElements[i].getOffset());
					bw->WriteUInt32(static_cast<uint32>(VertexElements[i].getType()));
					bw->WriteUInt32(static_cast<uint32>(VertexElements[i].getUsage()));
					bw->WriteInt32(VertexElements[i].getIndex());
				}

				bw->Close();
				delete bw;
			}

			data->AddEntryUInt32(TAG_3_VertexSizeTag, VertexSize);
			data->AddEntryUInt32(TAG_3_VertexCountTag, VertexCount);

			// save vertex data
			{
				BinaryWriter* bw = data->AddEntry(TAG_3_VertexDataTag);
				bw->Write(VertexData, VertexSize*VertexCount);
				bw->Close();
			}

			return data;
		}


		MeshData::MeshData()
			: VertexData(0), VertexSize(0), VertexCount(0)
		{

		}
		MeshData::~MeshData()
		{
			if (VertexData)
				delete[] VertexData;

			for (int32 i=0;i<Materials.getMaterialCount();i++)
			{
				for (int32 j=0;j<Materials.getFrameCount(i);j++)
				{
					MaterialData* md = Materials.getMaterial(i,j);
					delete md;
				}
			}
		}

		/************************************************************************/
		/*                                                                      */
		/************************************************************************/
		//  With the help from VirtualStream and TaggedData, data like this, models
		//  are stored in a hierarchical structure, since a model contains meshes, and meshes
		//  contain materials, materials at last contain effects, and this is not
		//  all of it.


		static const String TAG_3_EntityCountTag = L"EntityCount";
		static const String TAG_3_EntityPrefix = L"Ent";


		static const String TAG_3_AnimationDataTag = L"AnimationData";
		


		ModelData::~ModelData()
		{
			if (Entities.getCount())
			{
				for (int i=0; i<Entities.getCount();i++)
				{
					delete Entities[i];
				}
				Entities.Clear();
			}
			
			//if (AnimationData)
			//{
			//	delete AnimationData;
			//	AnimationData = 0;
			//}
		}

		void ModelData::ReadData(TaggedDataReader* data, int32 id)
		{
			int entCount = data->GetDataInt32(TAG_3_EntityCountTag);
			Entities.ResizeDiscard(entCount);

			for (int i=0;i<entCount;i++)
			{
				String tag = StringUtils::ToString(i);
				tag = TAG_3_EntityPrefix + tag;
				BinaryReader* br = data->GetData(tag);

				TaggedDataReader* meshData = br->ReadTaggedDataBlock();

				MeshData* mesh = new MeshData();
				mesh->LoadData(meshData);
				Entities.Add(mesh);

				meshData->Close();
				delete meshData;

				br->Close();
				delete br;
			}

			




			//if (id == MdlId_V2)
			//{
			//	if (data->Contains(TAG_2_MaterialAnimationTag))
			//	{
			//		this->AnimationData = new Graphics::Animation::AnimationData();
			//		BinaryReader* br = data->GetData(TAG_2_MaterialAnimationTag);
			//		TaggedDataReader* ad = br->ReadTaggedDataBlock();
			//		this->AnimationData->Load(ad);
			//		ad->Close();
			//		delete ad;
			//		br->Close();
			//		delete br;
			//	}
			//	
			//	if (data->Contains(TAG_3_AnimationDataTag))
			//	{
			//		this->AnimationData = new Graphics::Animation::AnimationData();
			//		BinaryReader* br = data->GetData(TAG_3_AnimationDataTag);
			//		TaggedDataReader* ad = br->ReadTaggedDataBlock();
			//		this->AnimationData->Load(ad);
			//		ad->Close();
			//		delete ad;
			//		br->Close();
			//		delete br;
			//	}
			//}
			//else if (id == MdlId_V3)
			//{
			//	if (data->Contains(TAG_3_AnimationDataTag))
			//	{
			//		this->AnimationData = new Graphics::Animation::AnimationData();

			//		BinaryReader* br = data->GetData(TAG_3_AnimationDataTag);
			//		TaggedDataReader* ad = br->ReadTaggedDataBlock();
			//		this->AnimationData->Load(ad);
			//		ad->Close();
			//		delete ad;
			//		br->Close();
			//		delete br;
			//	}
			//	
			//}
			
		}
		TaggedDataWriter* ModelData::WriteData() const
		{
			TaggedDataWriter* data = new TaggedDataWriter(true);

			data->AddEntryInt32(TAG_3_EntityCountTag, static_cast<int32>(Entities.getCount()));

			for (int i=0;i<Entities.getCount();i++)
			{
				String tag = StringUtils::ToString(i);
				tag = TAG_3_EntityPrefix + tag;
				BinaryWriter* bw = data->AddEntry(tag);

				TaggedDataWriter* meshData = Entities[i]->SaveData();
				bw->WriteTaggedDataBlock(meshData);
				delete meshData;

				bw->Close();
				delete bw;
			}

			//if (AnimationData)
			//{
			//	BinaryWriter* bw = data->AddEntry(TAG_3_AnimationDataTag);

			//	TaggedDataWriter* ad = AnimationData->Save();
			//	bw->Write(ad);
			//	delete ad;

			//	bw->Close();
			//	delete bw;
			//}

			return data;
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
			else
			{
				LogManager::getSingleton().Write(LOG_Graphics, L"Invalid model file. " + rl->getName(), LOGLVL_Error);
			}

			br->Close();
			delete br;
		}
		void ModelData::Save(Stream* strm) const
		{
			BinaryWriter* bw = new BinaryWriter(strm);

			bw->WriteInt32(MdlId_V3);

			TaggedDataWriter* mdlData = WriteData();
			bw->WriteTaggedDataBlock(mdlData);
			delete mdlData;

			bw->Close();
			delete bw;
		}
	}
}