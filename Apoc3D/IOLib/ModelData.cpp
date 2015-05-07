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
#include "apoc3d/Vfs/PathUtils.h"

#include "apoc3d/Exception.h"
#include "apoc3d/Vfs/ResourceLocation.h"
#include "apoc3d/Graphics/Animation/AnimationData.h"
#include "apoc3d/Core/Logging.h"
#include "apoc3d/Utility/StringUtils.h"

using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace IO
	{
		const int MdlId_V2 = 0;
		const int MdlId_V3 = ((byte)'M' << 24) | ((byte)'E' << 16) | ((byte)'S' << 8) | ((byte)'H');
		const int MdlLiteID = 'LMDL';

		const char TAG_3_MaterialCountTag[] = "MaterialCount";
		const char TAG_3_MaterialsTag[] = "Materials";

		//const String MaterialAnimationTag = L"MaterialAnimation";
		const char TAG_3_FaceCountTag[] = "FaceCount";
		const char TAG_3_FacesTag[] = "Faces";
		const char TAG_1_VertexFormatTag[] = "VertexFormat";
		const char TAG_3_VertexDeclTag[] = "VertexDeclaration";
		const char TAG_3_VertexCountTag[] = "VertexCount";
		const char TAG_3_VertexSizeTag[] = "VertexSize";

		const char TAG_2_MaterialAnimationTag[] = "MaterialAnimation2.0";
		const char TAG_3_VertexDataTag[] = "VertexData";

		const char TAG_3_NameTag[] = "Name";

		//const String TAG_3_ParentBoneTag = L"ParentBone";
		const char TAG_3_BoundingSphereTag[] = "BoundingSphere";

		uint32 MeshData::ComputeVertexSize(const List<VertexElement>& elements)
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
			uint32 materialCount = data->GetUInt32(TAG_3_MaterialCountTag);
			Materials.Reserve(materialCount);

			// load material set
			
			{
				BinaryReader* br = data->GetData(TAG_3_MaterialsTag);

				for (uint32 j = 0; j<materialCount; j++)
				{
					int32 frameCount = br->ReadInt32();

					for (int32 i = 0; i < frameCount; i++)
					{
						br->ReadTaggedDataBlock([this, i, j](TaggedDataReader* matData)
						{
							MaterialData* mtrl = new MaterialData();
							mtrl->LoadData(matData);

							if (i == 0)
							{
								Materials.Add(mtrl);
							}
							else
							{
								Materials.AddFrame(mtrl, j);
							}
						});
					}
				}

				delete br;
			}

			//ParentBoneID = -1;
			//data->TryGetDataInt32(TAG_3_ParentBoneTag, ParentBoneID);

			if (data->Contains(TAG_3_BoundingSphereTag))
			{
				data->ProcessData(TAG_3_BoundingSphereTag, [this](BinaryReader* br) 
				{
					br->ReadBoundingSphere(BoundingSphere);
				});
			}
			else
			{
				BoundingSphere.Center = Vector3::Zero;
				BoundingSphere.Radius = 0;
			}

			// read name
			data->GetString(TAG_3_NameTag, Name);

			// read faces
			{
				uint32 faceCount = data->GetInt32(TAG_3_FaceCountTag);

				Faces.ResizeDiscard(faceCount);

				BinaryReader* br = data->GetData(TAG_3_FacesTag);

				for (uint i = 0; i < faceCount; i++)
				{
					MeshFace face;
					face.IndexA = br->ReadInt32();
					face.IndexB = br->ReadInt32();
					face.IndexC = br->ReadInt32();

					face.MaterialID = br->ReadInt32();

					Faces.Add(face);
				}

				delete br;
			}

			// vertex element
			if (!data->Contains(TAG_3_VertexDeclTag))
			{
				// latency support
				// vertex format

				int fvf = data->GetInt32(TAG_1_VertexFormatTag);

				if (fvf == (256 | 2 | 16))
				{

				}
				else
				{
					throw AP_EXCEPTION(ExceptID::NotSupported, L"The format version of this mesh data is out of date.");
				}
			}
			else
			{
				// read vertex element
				BinaryReader* br = data->GetData(TAG_3_VertexDeclTag);

				int elemConut = br->ReadInt32();
				VertexElements.ResizeDiscard(elemConut);
				for (int i = 0; i < elemConut; i++)
				{
					int emOfs = br->ReadInt32();
					VertexElementFormat emFormat = static_cast<VertexElementFormat>(br->ReadUInt32());
					VertexElementUsage emUsage = static_cast<VertexElementUsage>(br->ReadUInt32());
					int emIndex = br->ReadInt32();

					VertexElements.Add(VertexElement(emOfs, emFormat, emUsage, emIndex));
				}

				delete br;
			}
			
			if (data->Contains(TAG_3_VertexSizeTag))
			{
				VertexSize = data->GetUInt32(TAG_3_VertexSizeTag);
			}
			else
			{
				VertexSize = ComputeVertexSize(VertexElements);
			}
			VertexCount = data->GetUInt32(TAG_3_VertexCountTag);

			// vertex data
			{
				BinaryReader* br = data->GetData(TAG_3_VertexDataTag);
				VertexData = new char[VertexCount*VertexSize];
				br->ReadBytes(reinterpret_cast<char*>(VertexData), VertexCount*VertexSize);

				delete br;
			}

		}

		TaggedDataWriter* MeshData::SaveData()
		{
			TaggedDataWriter* data = new TaggedDataWriter(true);

			uint32 materialCount = Materials.getMaterialCount();
			data->AddUInt32(TAG_3_MaterialCountTag, materialCount);

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
				delete bw;
			}

			//data->AddEntry(TAG_3_ParentBoneTag, ParentBoneID);

			{
				BinaryWriter* bw = data->AddEntry(TAG_3_BoundingSphereTag);
				bw->WriteBoundingSphere(BoundingSphere);
				delete bw;
			}

			// write name
			data->AddString(TAG_3_NameTag, Name);
			
			data->AddUInt32(TAG_3_FaceCountTag, static_cast<uint32>(Faces.getCount()));

			// write faces
			{
				BinaryWriter* bw = data->AddEntry(TAG_3_FacesTag);
				for (const MeshFace& mf : Faces)
				{
					bw->WriteInt32(mf.IndexA);
					bw->WriteInt32(mf.IndexB);
					bw->WriteInt32(mf.IndexC);
					bw->WriteInt32(mf.MaterialID);
				}
				delete bw;
			}

			// write vertex elements
			{
				BinaryWriter* bw = data->AddEntry(TAG_3_VertexDeclTag);

				bw->WriteUInt32(static_cast<uint32>(VertexElements.getCount()));
				for (const VertexElement& ve : VertexElements)
				{
					bw->WriteInt32(ve.getOffset());
					bw->WriteUInt32(static_cast<uint32>(ve.getType()));
					bw->WriteUInt32(static_cast<uint32>(ve.getUsage()));
					bw->WriteInt32(ve.getIndex());
				}

				delete bw;
			}

			data->AddUInt32(TAG_3_VertexSizeTag, VertexSize);
			data->AddUInt32(TAG_3_VertexCountTag, VertexCount);

			// save vertex data
			{
				BinaryWriter* bw = data->AddEntry(TAG_3_VertexDataTag);
				bw->Write(VertexData, VertexSize*VertexCount);
				delete bw;
			}

			return data;
		}

		void MeshData::SaveLite(BinaryWriter& bw)
		{
			bw.WriteInt32(Materials.getMaterialCount());
			for (int i = 0; i < Materials.getMaterialCount(); i++)
			{
				MaterialData* md = Materials.getMaterial(i, 0);
				bw.WriteColor4(md->Ambient);
				bw.WriteColor4(md->Diffuse);
				bw.WriteColor4(md->Specular);
				bw.WriteColor4(md->Emissive);
				bw.WriteSingle(md->Power);
				if (md->TextureName.Contains(0))
					bw.WriteString(PathUtils::GetFileNameNoExt(md->TextureName[0]));
				else
					bw.WriteString(PathUtils::GetFileNameNoExt(L""));
			}
			bw.WriteInt32(Faces.getCount());
			for (const MeshFace& mf : Faces)
			{
				bw.WriteInt32(mf.IndexA);
				bw.WriteInt32(mf.IndexB);
				bw.WriteInt32(mf.IndexC);
				bw.WriteInt32(mf.MaterialID);
			}
			bw.WriteInt32(VertexSize);
			bw.WriteInt32(VertexCount);
			bw.Write(VertexData, VertexSize*VertexCount);
		}

		MeshData::MeshData()
		{

		}
		MeshData::~MeshData()
		{
			delete[] VertexData;
			VertexData = nullptr;

			for (MaterialData* md : Materials)
			{
				delete md;
			}
		}

		/************************************************************************/
		/*                                                                      */
		/************************************************************************/
		//  With the help from VirtualStream and TaggedData, data like this, models
		//  are stored in a hierarchical structure, since a model contains meshes, and meshes
		//  contain materials, materials at last contain effects, and this is not
		//  all of it.


		const char TAG_3_EntityCountTag[] = "EntityCount";
		const char TAG_3_EntityPrefix[] = "Ent";

		const char TAG_3_AnimationDataTag[] = "AnimationData";
		

		ModelData::~ModelData()
		{
			Entities.DeleteAndClear();
		}

		void ModelData::ReadData(TaggedDataReader* data, int32 id)
		{
			int32 entCount = data->GetInt32(TAG_3_EntityCountTag);
			Entities.ResizeDiscard(entCount);

			for (int32 i=0;i<entCount;i++)
			{
				std::string tag = StringUtils::IntToNarrowString(i);
				tag = TAG_3_EntityPrefix + tag;
				BinaryReader* br = data->GetData(tag);

				TaggedDataReader* meshData = br->ReadTaggedDataBlock();

				MeshData* mesh = new MeshData();
				mesh->LoadData(meshData);
				
				
				Entities.Add(mesh);

				meshData->Close();
				delete meshData;

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

			data->AddInt32(TAG_3_EntityCountTag, static_cast<int32>(Entities.getCount()));

			for (int32 i = 0; i < Entities.getCount(); i++)
			{
				std::string tag = StringUtils::IntToNarrowString(i);
				tag = TAG_3_EntityPrefix + tag;
				BinaryWriter* bw = data->AddEntry(tag);

				TaggedDataWriter* meshData = Entities[i]->SaveData();
				bw->WriteTaggedDataBlock(meshData);
				delete meshData;

				delete bw;
			}

			return data;
		}
		void ModelData::Load(const ResourceLocation& rl)
		{
			BinaryReader _br(rl);
			BinaryReader* br = &_br;

			int32 id = br->ReadInt32();
			if (id == MdlId_V2 || id == MdlId_V3)
			{
				TaggedDataReader* data = br->ReadTaggedDataBlock();

				ReadData(data, id);


#if _DEBUG
				for (int32 k = 0; k < Entities.getCount(); k++)
				{
					MeshData* md = Entities[k];

					for (int32 i = 0; i < md->Materials.getMaterialCount(); i++)
					{
						for (int32 j = 0; j < md->Materials.getFrameCount(i); j++)
						{
							MaterialData* mtrlData = md->Materials.getMaterial(i, j);

							if (mtrlData->DebugName.empty())
							{
								const FileLocation* fl = up_cast<const FileLocation*>(&rl);
								if (fl)
									mtrlData->DebugName = PathUtils::GetFileNameNoExt(fl->getPath()) + L"::" + md->Name;
								else
									mtrlData->DebugName = rl.getName() + L"::" + md->Name;
							}
						}
					}
				}
#endif


				data->Close();
				delete data;
			}
			else
			{
				LogManager::getSingleton().Write(LOG_Graphics, L"Invalid model file. " + rl.getName(), LOGLVL_Error);
			}

		}
		void ModelData::Save(Stream& strm) const
		{
			BinaryWriter bw(&strm, false);

			bw.WriteInt32(MdlId_V3);

			TaggedDataWriter* mdlData = WriteData();
			bw.WriteTaggedDataBlock(mdlData);
			delete mdlData;

		}
		void ModelData::SaveLite(Stream& strm) const
		{
			BinaryWriter bw(&strm, false);

			bw.WriteInt32(MdlLiteID);
			bw.WriteInt32(Entities.getCount());
			for (int i=0;i<Entities.getCount();i++)
			{
				Entities[i]->SaveLite(bw);
			}
		}
	}
}