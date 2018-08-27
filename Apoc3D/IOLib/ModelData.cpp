/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2009-2018 Tao Xin
 * 
 * This content of this file is subject to the terms of the Mozilla Public 
 * License v2.0. If a copy of the MPL was not distributed with this file, 
 * you can obtain one at http://mozilla.org/MPL/2.0/.
 * 
 * This program is distributed in the hope that it will be useful, 
 * WITHOUT WARRANTY OF ANY KIND; either express or implied. See the 
 * Mozilla Public License for more details.
 * 
 * ------------------------------------------------------------------------
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
		const uint32 MdlId_V2 = 0;

		const uint32 MdlId_V3 = FourCC("MESH");
		const uint32 MdlLiteID = FourCC("LMDL");

		constexpr TaggedDataKey TAG_3_MaterialCountTag = "MaterialCount";
		constexpr TaggedDataKey TAG_3_MaterialsTag = "Materials";

		//const String MaterialAnimationTag = L"MaterialAnimation";
		constexpr TaggedDataKey TAG_3_FaceCountTag = "FaceCount";
		constexpr TaggedDataKey TAG_3_FacesTag = "Faces";
		constexpr TaggedDataKey TAG_3_VertexDeclTag = "VertexDeclaration";
		constexpr TaggedDataKey TAG_3_VertexCountTag = "VertexCount";
		constexpr TaggedDataKey TAG_3_VertexSizeTag = "VertexSize";

		constexpr TaggedDataKey TAG_2_MaterialAnimationTag = "MaterialAnimation2.0";
		constexpr TaggedDataKey TAG_3_VertexDataTag = "VertexData";

		constexpr TaggedDataKey TAG_3_NameTag = "Name";

		//const String TAG_3_ParentBoneTag = L"ParentBone";
		constexpr TaggedDataKey TAG_3_BoundingSphereTag = "BoundingSphere";

		uint32 MeshData::ComputeVertexSize(const List<VertexElement>& elements)
		{
			uint32 vertexSize = 0;
			for (const VertexElement& ve : elements)
				vertexSize += ve.getSize();
			return vertexSize;
		}
		void MeshData::LoadData(TaggedDataReader* data)
		{
			uint32 materialCount = data->GetUInt32(TAG_3_MaterialCountTag);
			Materials.Resize(materialCount);

			// load material set
			data->ProcessData(TAG_3_MaterialsTag, [&](BinaryReader* br)
			{
				for (uint32 j = 0; j < materialCount; j++)
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
			});

			//ParentBoneID = -1;
			//data->TryGetDataInt32(TAG_3_ParentBoneTag, ParentBoneID);

			BoundingSphere.Center = Vector3::Zero;
			BoundingSphere.Radius = 0;

			data->TryProcessData(TAG_3_BoundingSphereTag, [this](BinaryReader* br)
			{
				br->ReadBoundingSphere(BoundingSphere);
			});

			// read name
			data->GetString(TAG_3_NameTag, Name);

			// read faces
			{
				uint32 faceCount = data->GetInt32(TAG_3_FaceCountTag);
				Faces.ReserveDiscard(faceCount);

				data->ProcessData(TAG_3_FacesTag, [this](BinaryReader* br)
				{
					for (MeshFace & face : Faces)
					{
						face.IndexA = br->ReadInt32();
						face.IndexB = br->ReadInt32();
						face.IndexC = br->ReadInt32();
						face.MaterialID = br->ReadInt32();
					}
				});
			}

			// vertex element
			data->ProcessData(TAG_3_VertexDeclTag, [this](BinaryReader* br)
			{
				int32 elemConut = br->ReadInt32();
				VertexElements.ResizeDiscard(elemConut);

				for (int32 i = 0; i < elemConut; i++)
				{
					int32 emOfs = br->ReadInt32();
					VertexElementFormat emFormat = static_cast<VertexElementFormat>(br->ReadUInt32());
					VertexElementUsage emUsage = static_cast<VertexElementUsage>(br->ReadUInt32());
					int32 emIndex = br->ReadInt32();

					VertexElements.Add(VertexElement(emOfs, emFormat, emUsage, emIndex));
				}
			});
			
			if (!data->TryGetUInt32(TAG_3_VertexSizeTag, VertexSize))
			{
				VertexSize = ComputeVertexSize(VertexElements);
			}
			VertexCount = data->GetUInt32(TAG_3_VertexCountTag);

			// vertex data
			data->ProcessData(TAG_3_VertexDataTag, [this](BinaryReader* br)
			{
				VertexData = new char[VertexCount*VertexSize];
				br->ReadBytes(reinterpret_cast<char*>(VertexData), VertexCount*VertexSize);
			});
		}

		void MeshData::SaveData(TaggedDataWriter* data) const
		{
			uint32 materialCount = Materials.getMaterialCount();
			data->AddUInt32(TAG_3_MaterialCountTag, materialCount);

			// save material
			data->AddEntry(TAG_3_MaterialsTag, [&](BinaryWriter* bw) 
			{
				for (uint32 i = 0; i < materialCount; i++)
				{
					int32 frameCount = Materials.getFrameCount(static_cast<int32>(i));

					bw->WriteInt32(frameCount);
					for (int32 j = 0; j < frameCount; j++)
					{
						TaggedDataWriter* matData = Materials.getMaterial(i, j)->SaveData();
						bw->WriteTaggedDataBlock(matData);
						delete matData;
					}
				}
			});

			//data->AddEntry(TAG_3_ParentBoneTag, ParentBoneID);

			data->AddEntry(TAG_3_BoundingSphereTag, [this](BinaryWriter* bw) 
			{
				bw->WriteBoundingSphere(BoundingSphere);
			});

			// write name
			data->AddString(TAG_3_NameTag, Name);
			
			data->AddUInt32(TAG_3_FaceCountTag, static_cast<uint32>(Faces.getCount()));

			// write faces
			data->AddEntry(TAG_3_FacesTag, [this](BinaryWriter* bw)
			{
				for (const MeshFace& mf : Faces)
				{
					bw->WriteInt32(mf.IndexA);
					bw->WriteInt32(mf.IndexB);
					bw->WriteInt32(mf.IndexC);
					bw->WriteInt32(mf.MaterialID);
				}
			});

			// write vertex elements
			data->AddEntry(TAG_3_VertexDeclTag, [this](BinaryWriter* bw)
			{
				bw->WriteUInt32(static_cast<uint32>(VertexElements.getCount()));
				for (const VertexElement& ve : VertexElements)
				{
					bw->WriteInt32(ve.getOffset());
					bw->WriteUInt32(static_cast<uint32>(ve.getType()));
					bw->WriteUInt32(static_cast<uint32>(ve.getUsage()));
					bw->WriteInt32(ve.getIndex());
				}
			});

			data->AddUInt32(TAG_3_VertexSizeTag, VertexSize);
			data->AddUInt32(TAG_3_VertexCountTag, VertexCount);

			// save vertex data
			data->AddEntry(TAG_3_VertexDataTag, [this](BinaryWriter* bw)
			{
				bw->WriteBytes(VertexData, VertexSize*VertexCount);
			});
		}

		void MeshData::SaveLite(BinaryWriter& bw) const
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
				if (md->TextureNames.Contains(0))
					bw.WriteString(PathUtils::GetFileNameNoExt(md->TextureNames[0]));
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
			bw.WriteBytes(VertexData, VertexSize*VertexCount);
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
		//  contain materials, materials at least contain effects, and this is not
		//  all of it.

		constexpr TaggedDataKey TAG_3_EntityCountTag = "EntityCount";
		constexpr TaggedDataKey TAG_3_EntityPrefix = "Ent";

		constexpr TaggedDataKey TAG_3_AnimationDataTag = "AnimationData";
		

		ModelData::~ModelData()
		{
			Entities.DeleteAndClear();
		}

		void ModelData::ReadData(TaggedDataReader* data)
		{
			int32 entCount = data->GetInt32(TAG_3_EntityCountTag);
			Entities.ResizeDiscard(entCount);

			for (int32 i = 0; i < entCount; i++)
			{
				TaggedDataKey tag = TAG_3_EntityPrefix + (uint32)i;

				data->ProcessDataSection(tag, [this](TaggedDataReader* meshData) 
				{
					MeshData* mesh = new MeshData();
					mesh->LoadData(meshData);

					Entities.Add(mesh);
				});
			}
		}
		void ModelData::WriteData(TaggedDataWriter* data) const
		{
			data->AddInt32(TAG_3_EntityCountTag, static_cast<int32>(Entities.getCount()));

			for (int32 i = 0; i < Entities.getCount(); i++)
			{
				MeshData* mesh = Entities[i];
				TaggedDataKey tag = TAG_3_EntityPrefix + (uint32)i;

				data->AddEntryDataSection(tag, [mesh](TaggedDataWriter* meshData)
				{
					mesh->SaveData(meshData);
				});
			}
		}
		void ModelData::Load(const ResourceLocation& rl)
		{
			BinaryReader br(rl);

			uint32 id = br.ReadUInt32();
			if (id == MdlId_V2 || id == MdlId_V3)
			{
				br.ReadTaggedDataBlock([this](TaggedDataReader* data)
				{
					ReadData(data);
				});

#if _DEBUG
				for (MeshData* md : Entities)
				{
					for (MaterialData* mtrlData : md->Materials)
					{
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
#endif
			}
			else
			{
				LogManager::getSingleton().Write(LOG_Graphics, L"Invalid model file. " + rl.getName(), LOGLVL_Error);
			}

		}
		void ModelData::Save(Stream& strm) const
		{
			BinaryWriter bw(&strm, false);

			bw.WriteUInt32(MdlId_V3);
			bw.WriteTaggedDataBlock([this](TaggedDataWriter* mdlData)
			{
				WriteData(mdlData);
			});
		}
		void ModelData::SaveLite(Stream& strm) const
		{
			BinaryWriter bw(&strm, false);

			bw.WriteInt32(MdlLiteID);
			bw.WriteInt32(Entities.getCount());
			for (const MeshData* meshData : Entities)
			{
				meshData->SaveLite(bw);
			}
		}
	}
}