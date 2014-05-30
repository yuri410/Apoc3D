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

#include "MaterialData.h"

#include "Streams.h"
#include "TaggedData.h"
#include "BinaryReader.h"
#include "BinaryWriter.h"
#include "apoc3d/ApocException.h"
#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/Core/Logging.h"
#include "apoc3d/Vfs/ResourceLocation.h"
#include "apoc3d/Vfs/PathUtils.h"
#include "apoc3d/Graphics/Material.h"

using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace IO
	{
		/** Used only when storing a material as a file
		*/
		const int MtrlId_V3 = ((byte)'M' << 24) | ((byte)'T' << 16) | ((byte)'R' << 8) | ((byte)'L');

		// =============================================================

		String TAG_3_CustomParamCount = L"CustomParamCount";
		String TAG_3_CustomParam = L"CustomParam";

		String TAG_3_HasTexture = L"HasTexture";
		String TAG_3_Texture = L"Texture";
		String TAG_3_HasEffect = L"HasEffect";
		String TAG_3_Effect = L"Effect";


		String TAG_3_RenderPriority = L"RenderPriority";
		String TAG_3_PassFlags = L"PassFlags";

		String TAG_3_IsBlendTransparent = L"IsBlendTransparent";
		String TAG_3_SourceBlend = L"SourceBlend";
		String TAG_3_DestinationBlend = L"DestinationBlend";
		String TAG_3_BlendFunction = L"BlendFunction";

		String TAG_3_CullMode = L"CullMode";


		String TAG_3_AlphaReference = L"AlphaReference";
		String TAG_3_AlphaTestEnable = L"AlphaTestEnable";

		String TAG_3_DepthTestEnabled = L"DepthTestEnabled";
		String TAG_3_DepthWriteEnabled = L"DepthWriteEnabled";

		String TAG_3_MaterialColorTag = L"MaterialColor";
		String TAG_3_MaterialRefName = L"MaterialRefName";

		String TAG_3_UsePointSprite = L"UsePointSprite";


		void MaterialData::AddCustomParameter(const MaterialCustomParameter& value)
		{
			if (value.Usage.empty())
			{
				throw AP_EXCEPTION(EX_Argument, L"usage can not be empty");
			}
			CustomParametrs.Add(value.Usage, value);
		}

		void MaterialData::LoadV3(TaggedDataReader* data)
		{
			// load custom material parameters
			uint32 cmpCount = 0;
			data->TryGetDataUInt32(TAG_3_CustomParamCount, cmpCount);
			//m_customParametrs.reserve(cmpCount);
			CustomParametrs.Resize(cmpCount);

			for (uint32 i=0;i<cmpCount;i++)
			{
				String tag = StringUtils::UIntToString(i);
				tag = TAG_3_CustomParam + tag;

				BinaryReader* br = data->GetData(tag);

				MaterialCustomParameter mcp;
				mcp.Type = static_cast<CustomEffectParameterType>(br->ReadUInt32());

				br->ReadBytes(reinterpret_cast<char*>(mcp.Value), sizeof(mcp.Value));

				mcp.Usage = br->ReadString();

				br->Close();
				delete br;

				AddCustomParameter(mcp);
			}

			// Load textures
			{
				bool hasTexture[MaxTextures];
				data->GetDataBool(TAG_3_HasTexture, hasTexture, MaxTextures);

				for (int32 i=0;i<MaxTextures;i++)
				{
					if (hasTexture[i])
					{
						String tag = StringUtils::IntToString(i);
						tag = tag + TAG_3_Texture;

						String name;
						data->GetDataString(tag, name);

						if (!TextureName.Contains(i))
							TextureName.Add(i, name);
						else
							TextureName[i] = name;

					}
				}
			}

			// Load effects
			{
				bool hasEffect[MaxScenePass];
				data->GetDataBool(TAG_3_HasEffect, hasEffect, MaxScenePass);

				for (int32 i=0;i<MaxScenePass;i++)
				{
					if (hasEffect[i])
					{
						String tag = StringUtils::IntToString(i);
						tag = tag + TAG_3_Effect;

						String name;
						data->GetDataString(tag, name);

						if (!EffectName.Contains(i))
							EffectName.Add(i, name);
						else
							EffectName[i] = name;

						//data->GetDataString(tag, EffectName[i]);
					}
				}
			}

			Priority = data->GetDataInt32(TAG_3_RenderPriority);
			PassFlags = data->GetDataUInt64(TAG_3_PassFlags);

			IsBlendTransparent = data->GetDataBool(TAG_3_IsBlendTransparent);

			uint32 val = static_cast<uint32>(SourceBlend);
			data->TryGetDataUInt32(TAG_3_SourceBlend, val);
			SourceBlend = static_cast<Blend>(val);

			val = static_cast<uint32>(DestinationBlend);
			data->TryGetDataUInt32(TAG_3_DestinationBlend, val);
			DestinationBlend = static_cast<Blend>(val);

			val = static_cast<uint32>(BlendFunction);
			data->TryGetDataUInt32(TAG_3_BlendFunction, val);
			BlendFunction = static_cast<Apoc3D::Graphics::RenderSystem::BlendFunction>(val);

			Cull = static_cast<CullMode>(data->GetDataUInt32(TAG_3_CullMode));

			AlphaReference = data->GetDataUInt32(TAG_3_AlphaReference);
			AlphaTestEnabled = data->GetDataBool(TAG_3_AlphaTestEnable);

			DepthTestEnabled = data->GetDataBool(TAG_3_DepthTestEnabled);
			DepthWriteEnabled = data->GetDataBool(TAG_3_DepthWriteEnabled);

			UsePointSprite = false;
			data->TryGetDataBool(TAG_3_UsePointSprite, UsePointSprite);

			// load material basic color
			{
				BinaryReader* br = data->GetData(TAG_3_MaterialColorTag);

				br->ReadColor4(Ambient);
				br->ReadColor4(Diffuse);
				br->ReadColor4(Emissive);
				br->ReadColor4(Specular);
				Power = br->ReadSingle();

				br->Close();
				delete br;
			}

			data->TryGetString(TAG_3_MaterialRefName, ExternalRefName);
		}

		void MaterialData::LoadData(TaggedDataReader* data)
		{
			LoadV3(data);
		}
		TaggedDataWriter* MaterialData::SaveData()
		{
			TaggedDataWriter* data = new TaggedDataWriter(true);

			data->AddEntryInt32(TAG_3_CustomParamCount, CustomParametrs.getCount());
			int32 index = 0;
			for (CustomParamTable::Enumerator e = CustomParametrs.GetEnumerator(); e.MoveNext();)
			{				
				const MaterialCustomParameter& mcp = *e.getCurrentValue();

				if (!EffectParameter::IsReference(mcp.Type))
				{
					String tag = StringUtils::IntToString(index++);
					tag = TAG_3_CustomParam + tag;// tag + TAG_3_CustomParam;

					BinaryWriter* bw = data->AddEntry(tag);

					bw->WriteUInt32(static_cast<uint32>(mcp.Type));
					bw->Write(reinterpret_cast<const char*>(mcp.Value), sizeof(mcp.Value));
					bw->WriteString(mcp.Usage);

					bw->Close();
					delete bw;
				}
			}

			// save textures
			{
				bool hasTexture[MaxTextures];
				for (int32 i=0;i<MaxTextures;i++)
				{
					if (TextureName.Contains(i))
					{
						String name = TextureName[i];
						hasTexture[i] = !!name.size();
					}
					else
					{
						hasTexture[i] = false;
					}
				}
				data->AddEntryBool(TAG_3_HasTexture, hasTexture, MaxTextures);

				for (int32 i=0;i<MaxTextures;i++)
				{
					if (hasTexture[i])
					{
						String tag = StringUtils::IntToString(i);
						tag = tag + TAG_3_Texture;

						data->AddEntryString(tag, TextureName[i]);
					}
				}
			}
			// save effects
			{
				bool hasEffects[MaxScenePass];
				for (int32 i=0;i<MaxScenePass;i++)
				{
					if (EffectName.Contains(i))
					{
						String name = EffectName[i];
						hasEffects[i] = !!name.size();
					}
					else
					{
						hasEffects[i] = false;
					}
				}
				data->AddEntryBool(TAG_3_HasEffect, hasEffects, MaxScenePass);

				for (int32 i=0;i<MaxScenePass;i++)
				{
					if (hasEffects[i])
					{
						String tag = StringUtils::IntToString(i);
						tag = tag + TAG_3_Effect;

						data->AddEntryString(tag, EffectName[i]);
					}
				}
			}

			data->AddEntryInt32(TAG_3_RenderPriority, Priority);
			data->AddEntryUInt64(TAG_3_PassFlags, PassFlags);

			data->AddEntryBool(TAG_3_IsBlendTransparent, IsBlendTransparent);
			data->AddEntryUInt32(TAG_3_SourceBlend, static_cast<uint32>(SourceBlend));
			data->AddEntryUInt32(TAG_3_DestinationBlend, static_cast<uint32>(DestinationBlend));
			data->AddEntryUInt32(TAG_3_BlendFunction, static_cast<uint32>(BlendFunction));

			data->AddEntryUInt32(TAG_3_CullMode, static_cast<uint32>(Cull));			

			data->AddEntryUInt32(TAG_3_AlphaReference, AlphaReference);
			data->AddEntryBool(TAG_3_AlphaTestEnable, AlphaTestEnabled);

			data->AddEntryBool(TAG_3_DepthTestEnabled, DepthTestEnabled);
			data->AddEntryBool(TAG_3_DepthWriteEnabled, DepthWriteEnabled);
			data->AddEntryBool(TAG_3_UsePointSprite, UsePointSprite);

			// save material basic color
			{
				BinaryWriter* bw = data->AddEntry(TAG_3_MaterialColorTag);

				bw->WriteColor4(Ambient);
				bw->WriteColor4(Diffuse);
				bw->WriteColor4(Emissive);
				bw->WriteColor4(Specular);
				bw->WriteSingle(Power);


				bw->Close();
				delete bw;
			}

			data->AddEntryString(TAG_3_MaterialRefName, ExternalRefName);

			return data;
		}

		void MaterialData::Load(const ResourceLocation* rl)
		{
#if _DEBUG
			{
				const FileLocation* fl = dynamic_cast<const FileLocation*>(rl);
				if (fl)
					DebugName = PathUtils::GetFileNameNoExt(fl->getPath());
				else
					DebugName = rl->getName();
			}
#endif

			BinaryReader* br = new BinaryReader(rl->GetReadStream());

			int32 id = br->ReadInt32();
			if (id == MtrlId_V3)
			{
				TaggedDataReader* data = br->ReadTaggedDataBlock();

				LoadData(data);

				data->Close();
				delete data;
			}
			else
			{
				LogManager::getSingleton().Write(LOG_Graphics, L"Invalid material file. " + rl->getName(), LOGLVL_Error);
			}

			br->Close();
			delete br;
		}
		void MaterialData::Save(Stream* strm)
		{
			BinaryWriter* bw = new BinaryWriter(strm);

			bw->WriteInt32(MtrlId_V3);

			TaggedDataWriter* mdlData = SaveData();
			bw->WriteTaggedDataBlock(mdlData);
			delete mdlData;

			bw->Close();
			delete bw;
		}

		MaterialData::MaterialData(const MaterialData& other)
			: ExternalRefName(other.ExternalRefName), CustomParametrs(other.CustomParametrs), PassFlags(other.PassFlags),
			Priority(other.Priority), SourceBlend(other.SourceBlend), DestinationBlend(other.DestinationBlend),
			BlendFunction(other.BlendFunction), IsBlendTransparent(other.IsBlendTransparent), Cull(other.Cull), 
			AlphaTestEnabled(other.AlphaTestEnabled), AlphaReference(other.AlphaReference), DepthWriteEnabled(other.DepthWriteEnabled), 
			DepthTestEnabled(other.DepthTestEnabled), Ambient(other.Ambient), Diffuse(other.Diffuse), 
			Emissive(other.Emissive), Specular(other.Specular), Power(other.Power), UsePointSprite(other.UsePointSprite),
			EffectName(other.EffectName), TextureName(other.TextureName)
		{
#if _DEBUG
			DebugName = other.DebugName;
#endif
		}
	}
}
