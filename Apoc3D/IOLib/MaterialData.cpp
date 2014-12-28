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
#include "apoc3d/Exception.h"
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

		const char TAG_3_CustomParamCount[] = "CustomParamCount";
		const char TAG_3_CustomParam[] = "CustomParam";

		const char TAG_3_HasTexture[] = "HasTexture";
		const char TAG_3_Texture[] = "Texture";
		const char TAG_3_HasEffect[] = "HasEffect";
		const char TAG_3_Effect[] = "Effect";


		const char TAG_3_RenderPriority[] = "RenderPriority";
		const char TAG_3_PassFlags[] = "PassFlags";

		const char TAG_3_IsBlendTransparent[] = "IsBlendTransparent";
		const char TAG_3_SourceBlend[] = "SourceBlend";
		const char TAG_3_DestinationBlend[] = "DestinationBlend";
		const char TAG_3_BlendFunction[] = "BlendFunction";

		const char TAG_3_CullMode[] = "CullMode";


		const char TAG_3_AlphaReference[] = "AlphaReference";
		const char TAG_3_AlphaTestEnable[] = "AlphaTestEnable";

		const char TAG_3_DepthTestEnabled[] = "DepthTestEnabled";
		const char TAG_3_DepthWriteEnabled[] = "DepthWriteEnabled";

		const char TAG_3_MaterialColorTag[] = "MaterialColor";
		const char TAG_3_MaterialRefName[] = "MaterialRefName";

		const char TAG_3_UsePointSprite[] = "UsePointSprite";


		void MaterialData::AddCustomParameter(const MaterialCustomParameter& value)
		{
			if (value.Usage.empty())
			{
				throw AP_EXCEPTION(ExceptID::Argument, L"usage can not be empty");
			}
			CustomParametrs.Add(value.Usage, value);
		}

		void MaterialData::LoadV3(TaggedDataReader* data)
		{
			// load custom material parameters
			uint32 cmpCount = 0;
			data->TryGetUInt32(TAG_3_CustomParamCount, cmpCount);
			//m_customParametrs.reserve(cmpCount);
			CustomParametrs.Resize(cmpCount);

			for (uint32 i = 0; i < cmpCount; i++)
			{
				std::string tag = StringUtils::UIntToNarrowString(i);
				tag = TAG_3_CustomParam + tag;

				BinaryReader* br = data->GetData(tag);

				MaterialCustomParameter mcp;
				mcp.Type = static_cast<CustomEffectParameterType>(br->ReadUInt32());

				br->ReadBytes(reinterpret_cast<char*>(mcp.Value), sizeof(mcp.Value));

				mcp.Usage = br->ReadString();

				delete br;

				AddCustomParameter(mcp);
			}

			// Load textures
			{
				bool hasTexture[MaxTextures];
				data->GetBool(TAG_3_HasTexture, hasTexture, MaxTextures);

				for (int32 i = 0; i < MaxTextures; i++)
				{
					if (hasTexture[i])
					{
						std::string tag = StringUtils::IntToNarrowString(i);
						tag = tag + TAG_3_Texture;

						String name;
						data->GetString(tag, name);

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
				data->GetBool(TAG_3_HasEffect, hasEffect, MaxScenePass);

				for (int32 i = 0; i < MaxScenePass; i++)
				{
					if (hasEffect[i])
					{
						std::string tag = StringUtils::IntToNarrowString(i);
						tag = tag + TAG_3_Effect;

						String name;
						data->GetString(tag, name);

						if (!EffectName.Contains(i))
							EffectName.Add(i, name);
						else
							EffectName[i] = name;

						//data->GetDataString(tag, EffectName[i]);
					}
				}
			}

			Priority = data->GetInt32(TAG_3_RenderPriority);
			PassFlags = data->GetUInt64(TAG_3_PassFlags);

			IsBlendTransparent = data->GetBool(TAG_3_IsBlendTransparent);

			uint32 val = static_cast<uint32>(SourceBlend);
			data->TryGetUInt32(TAG_3_SourceBlend, val);
			SourceBlend = static_cast<Blend>(val);

			val = static_cast<uint32>(DestinationBlend);
			data->TryGetUInt32(TAG_3_DestinationBlend, val);
			DestinationBlend = static_cast<Blend>(val);

			val = static_cast<uint32>(BlendFunction);
			data->TryGetUInt32(TAG_3_BlendFunction, val);
			BlendFunction = static_cast<Apoc3D::Graphics::RenderSystem::BlendFunction>(val);

			Cull = static_cast<CullMode>(data->GetUInt32(TAG_3_CullMode));

			AlphaReference = data->GetUInt32(TAG_3_AlphaReference);
			AlphaTestEnabled = data->GetBool(TAG_3_AlphaTestEnable);

			DepthTestEnabled = data->GetBool(TAG_3_DepthTestEnabled);
			DepthWriteEnabled = data->GetBool(TAG_3_DepthWriteEnabled);

			UsePointSprite = false;
			data->TryGetBool(TAG_3_UsePointSprite, UsePointSprite);

			// load material basic color
			{
				data->ProcessData(TAG_3_MaterialColorTag, [this](BinaryReader* br)
				{
					br->ReadColor4(Ambient);
					br->ReadColor4(Diffuse);
					br->ReadColor4(Emissive);
					br->ReadColor4(Specular);
					Power = br->ReadSingle();
				});

				/*BinaryReader* br = data->GetData(TAG_3_MaterialColorTag);

				br->ReadColor4(Ambient);
				br->ReadColor4(Diffuse);
				br->ReadColor4(Emissive);
				br->ReadColor4(Specular);
				Power = br->ReadSingle();

				delete br;*/
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

			data->AddInt32(TAG_3_CustomParamCount, CustomParametrs.getCount());
			int32 index = 0;
			for (const MaterialCustomParameter& mcp : CustomParametrs.getValueAccessor())
			{	
				if (!EffectParameter::IsReference(mcp.Type))
				{
					std::string tag = TAG_3_CustomParam + StringUtils::IntToNarrowString(index++);

					BinaryWriter* bw = data->AddEntry(tag);

					bw->WriteUInt32(static_cast<uint32>(mcp.Type));
					bw->Write(reinterpret_cast<const char*>(mcp.Value), sizeof(mcp.Value));
					bw->WriteString(mcp.Usage);

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
				data->AddBool(TAG_3_HasTexture, hasTexture, MaxTextures);

				for (int32 i=0;i<MaxTextures;i++)
				{
					if (hasTexture[i])
					{
						std::string tag = StringUtils::IntToNarrowString(i);
						tag = tag + TAG_3_Texture;

						data->AddString(tag, TextureName[i]);
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
				data->AddBool(TAG_3_HasEffect, hasEffects, MaxScenePass);

				for (int32 i=0;i<MaxScenePass;i++)
				{
					if (hasEffects[i])
					{
						std::string tag = StringUtils::IntToNarrowString(i);
						tag = tag + TAG_3_Effect;

						data->AddString(tag, EffectName[i]);
					}
				}
			}

			data->AddInt32(TAG_3_RenderPriority, Priority);
			data->AddUInt64(TAG_3_PassFlags, PassFlags);

			data->AddBool(TAG_3_IsBlendTransparent, IsBlendTransparent);
			data->AddUInt32(TAG_3_SourceBlend, static_cast<uint32>(SourceBlend));
			data->AddUInt32(TAG_3_DestinationBlend, static_cast<uint32>(DestinationBlend));
			data->AddUInt32(TAG_3_BlendFunction, static_cast<uint32>(BlendFunction));

			data->AddUInt32(TAG_3_CullMode, static_cast<uint32>(Cull));			

			data->AddUInt32(TAG_3_AlphaReference, AlphaReference);
			data->AddBool(TAG_3_AlphaTestEnable, AlphaTestEnabled);

			data->AddBool(TAG_3_DepthTestEnabled, DepthTestEnabled);
			data->AddBool(TAG_3_DepthWriteEnabled, DepthWriteEnabled);
			data->AddBool(TAG_3_UsePointSprite, UsePointSprite);

			// save material basic color
			{
				BinaryWriter* bw = data->AddEntry(TAG_3_MaterialColorTag);

				bw->WriteColor4(Ambient);
				bw->WriteColor4(Diffuse);
				bw->WriteColor4(Emissive);
				bw->WriteColor4(Specular);
				bw->WriteSingle(Power);

				delete bw;
			}

			data->AddString(TAG_3_MaterialRefName, ExternalRefName);

			return data;
		}

		void MaterialData::Load(const ResourceLocation& rl)
		{
#if _DEBUG
			{
				const FileLocation* fl = up_cast<const FileLocation*>(&rl);
				if (fl)
					DebugName = PathUtils::GetFileNameNoExt(fl->getPath());
				else
					DebugName = rl.getName();
			}
#endif

			BinaryReader br(rl);

			int32 id = br.ReadInt32();
			if (id == MtrlId_V3)
			{
				TaggedDataReader* data = br.ReadTaggedDataBlock();

				LoadData(data);

				data->Close();
				delete data;
			}
			else
			{
				LogManager::getSingleton().Write(LOG_Graphics, L"Invalid material file. " + rl.getName(), LOGLVL_Error);
			}

		}
		void MaterialData::Save(Stream& strm)
		{
			BinaryWriter bw(&strm, false);
			
			bw.WriteInt32(MtrlId_V3);

			TaggedDataWriter* mdlData = SaveData();
			bw.WriteTaggedDataBlock(mdlData);
			delete mdlData;
		}


	}
}
