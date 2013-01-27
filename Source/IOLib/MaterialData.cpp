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

#include "MaterialData.h"

#include "Streams.h"
#include "TaggedData.h"
#include "BinaryReader.h"
#include "BinaryWriter.h"
#include "Apoc3DException.h"
#include "Utility/StringUtils.h"
#include "Core/Logging.h"
#include "Vfs/ResourceLocation.h"
#include "Graphics/Material.h"

using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace IO
	{
		/** Used only when storing a material as a file
		*/
		static const int MtrlId_V3 = ((byte)'M' << 24) | ((byte)'T' << 16) | ((byte)'R' << 8) | ((byte)'L');

		static String TAG_2_IsTransparent = L"IsTransparent";
		static String TAG_2_CullMode = L"CullMode";
		static String TAG_2_ZEnabled = L"ZEnabled";
		static String TAG_2_ZWriteEnabled = L"ZWriteEnabled";
		static String TAG_2_AlphaRef = L"AlphaRef";
		static String TAG_2_IsVegetation = L"IsVegetation";
		static String TAG_2_RenderPriority = L"RenderPriority";

		static String TAG_2_MaterialColorTag = L"MaterialColor";

		static String TAG_2_MaterialFlag = L"Flags";
		static String TAG_2_HasTexture = L"HasTexture";
		static String TAG_2_Texture = L"Texture";
		static String TAG_2_Effect = L"Effect";


		// =============================================================

		static String TAG_3_CustomParamCount = L"CustomParamCount";
		static String TAG_3_CustomParam = L"CustomParam";

		static String TAG_3_HasTexture = L"HasTexture";
		static String TAG_3_Texture = L"Texture";
		static String TAG_3_HasEffect = L"HasEffect";
		static String TAG_3_Effect = L"Effect";


		static String TAG_3_RenderPriority = L"RenderPriority";
		static String TAG_3_PassFlags = L"PassFlags";

		static String TAG_3_IsBlendTransparent = L"IsBlendTransparent";
		static String TAG_3_SourceBlend = L"SourceBlend";
		static String TAG_3_DestinationBlend = L"DestinationBlend";
		static String TAG_3_BlendFunction = L"BlendFunction";

		static String TAG_3_CullMode = L"CullMode";


		static String TAG_3_AlphaReference = L"AlphaReference";
		static String TAG_3_AlphaTestEnable = L"AlphaTestEnable";

		static String TAG_3_DepthTestEnabled = L"DepthTestEnabled";
		static String TAG_3_DepthWriteEnabled = L"DepthWriteEnabled";

		static String TAG_3_MaterialColorTag = L"MaterialColor";
		static String TAG_3_MaterialRefName = L"MaterialRefName";

		static String TAG_3_UsePointSprite = L"UsePointSprite";


		void MaterialData::AddCustomParameter(const MaterialCustomParameter& value)
		{
			if (value.Usage.empty())
			{
				throw Apoc3DException::createException(EX_Argument, L"usage can not be empty");
			}
			CustomParametrs.insert(CustomParamTable::value_type(value.Usage, value));
		}

		void MaterialData::LoadV2(TaggedDataReader* data)
		{
			SetDefaults();
			AlphaReference = static_cast<uint32>(data->GetDataSingle(TAG_2_AlphaRef)*255);
			if (AlphaReference > 255)
				AlphaReference = 255;

			AlphaTestEnabled = !!AlphaReference;


			bool isVegetation = false;
			data->TryGetDataBool(TAG_2_IsVegetation, isVegetation);
			if (isVegetation)
			{
				this->AddCustomParameter(MaterialCustomParameter(isVegetation, L"veg"));
			}


			int32 val = static_cast<int32>(CULL_None);
			data->TryGetDataInt32(TAG_2_CullMode, val);
			Cull = static_cast<CullMode>(val);


			IsBlendTransparent = false;
			data->TryGetDataBool(TAG_2_IsTransparent, IsBlendTransparent);

			DepthTestEnabled = true;
			data->TryGetDataBool(TAG_2_ZEnabled, DepthTestEnabled);


			DepthWriteEnabled = true;
			data->TryGetDataBool(TAG_2_ZWriteEnabled, DepthWriteEnabled);

			Priority = 1;
			data->TryGetDataInt32(TAG_2_RenderPriority, Priority);
			Priority++;

			// backward compatibility
			{
				int32 flags = data->GetDataInt32(TAG_2_MaterialFlag);

				if (flags == 2)
				{
					SourceBlend = BLEND_SourceAlpha;
					DestinationBlend = BLEND_One;
				}
				else if (flags == 3)
				{
					SourceBlend = BLEND_SourceColor;
					DestinationBlend = BLEND_One;
				}
				else
				{
					SourceBlend = BLEND_SourceAlpha;
					DestinationBlend = BLEND_InverseSourceAlpha;
				}
			}

			PassFlags = 1;

			// load material basic color
			{
				BinaryReader* br = data->GetData(TAG_2_MaterialColorTag);

				br->ReadColor4(Ambient);
				br->ReadColor4(Diffuse);
				br->ReadColor4(Emissive);
				br->ReadColor4(Specular);
				Power = br->ReadSingle();

				br->Close();
				delete br;
			}
			// load effect
			data->GetDataString(TAG_2_Effect, EffectName[0]);

			// load textures
			{
				BinaryReader* br = data->GetData(TAG_2_HasTexture);
				bool hasTexture[MaxTextures];
				for (int32 i=0;i<MaxTextures;i++)
				{
					hasTexture[i] = br->ReadBoolean();
				} 
				br->Close();
				delete br;

				for (int32 i=0;i<MaxTextures;i++)
				{
					if (hasTexture[i])
					{
						String tag = StringUtils::ToString(i);
						tag = TAG_2_Texture + tag;

						data->GetDataString(TAG_2_Effect, TextureName[i]);
					}
				}
			}
		}
		void MaterialData::LoadV3(TaggedDataReader* data)
		{
			// load custom material parameters
			uint32 cmpCount = 0;
			data->TryGetDataUInt32(TAG_3_CustomParamCount, cmpCount);
			//m_customParametrs.reserve(cmpCount);
			CustomParametrs.rehash(cmpCount);

			for (uint32 i=0;i<cmpCount;i++)
			{
				String tag = StringUtils::ToString(i);
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
				//BinaryReader* br = data->GetData(TAG_3_HasTexture);
				bool hasTexture[MaxTextures];
				data->GetDataBool(TAG_3_HasTexture, hasTexture, MaxTextures);

				for (int32 i=0;i<MaxTextures;i++)
				{
					if (hasTexture[i])
					{
						String tag = StringUtils::ToString(i);
						tag = tag + TAG_3_Texture;

						data->GetDataString(tag, TextureName[i]);
					}
				}
			}

			// Load effects
			{
				//BinaryReader* br = data->GetData(TAG_3_HasEffect);
				bool hasEffect[MaxScenePass];
				data->GetDataBool(TAG_3_HasEffect, hasEffect, MaxScenePass);

				for (int32 i=0;i<MaxScenePass;i++)
				{
					if (hasEffect[i])
					{
						String tag = StringUtils::ToString(i);
						tag = tag + TAG_3_Effect;

						data->GetDataString(tag, EffectName[i]);
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

			UsePointSprite = data->GetDataBool(TAG_3_UsePointSprite);

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
			int version = 3;
			if (data->Contains(TAG_2_AlphaRef))
			{
				version = 2;
			}

			if (version == 2)
				LoadV2(data);
			else
				LoadV3(data);

		}
		TaggedDataWriter* MaterialData::SaveData()
		{
			TaggedDataWriter* data = new TaggedDataWriter(true);

			data->AddEntry(TAG_3_CustomParamCount, static_cast<uint32>(CustomParametrs.size()));
			int32 index = 0;
			for (CustomParamTable::iterator iter = CustomParametrs.begin(); iter != CustomParametrs.end(); iter++)
			{				
				const MaterialCustomParameter& mcp = iter->second;

				if (!EffectParameter::IsReference(mcp.Type))
				{
					String tag = StringUtils::ToString(index++);
					tag = TAG_3_CustomParam + tag;// tag + TAG_3_CustomParam;

					BinaryWriter* bw = data->AddEntry(tag);

					bw->Write(static_cast<uint32>(mcp.Type));
					bw->Write(reinterpret_cast<const char*>(mcp.Value), sizeof(mcp.Value));
					bw->Write(mcp.Usage);

					bw->Close();
					delete bw;
				}
			}

			// save textures
			{
				bool hasTexture[MaxTextures];
				for (int32 i=0;i<MaxTextures;i++)
				{
					hasTexture[i] = (!TextureName[i].empty());
				}
				data->AddEntryBool(TAG_3_HasTexture, hasTexture, MaxTextures);

				for (int32 i=0;i<MaxTextures;i++)
				{
					if (!TextureName[i].empty())
					{
						String tag = StringUtils::ToString(i);
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
					hasEffects[i] = (EffectName.find(i) != EffectName.end());
				}
				data->AddEntryBool(TAG_3_HasEffect, hasEffects, MaxScenePass);

				for (int32 i=0;i<MaxScenePass;i++)
				{
					if (EffectName.find(i) != EffectName.end())
					{
						String tag = StringUtils::ToString(i);
						tag = tag + TAG_3_Effect;

						data->AddEntryString(tag, EffectName[i]);
					}
				}
			}

			data->AddEntry(TAG_3_RenderPriority, Priority);
			data->AddEntry(TAG_3_PassFlags, PassFlags);

			data->AddEntry(TAG_3_IsBlendTransparent, IsBlendTransparent);
			data->AddEntry(TAG_3_SourceBlend, static_cast<uint32>(SourceBlend));
			data->AddEntry(TAG_3_DestinationBlend, static_cast<uint32>(DestinationBlend));
			data->AddEntry(TAG_3_BlendFunction, static_cast<uint32>(BlendFunction));

			data->AddEntry(TAG_3_CullMode, static_cast<uint32>(Cull));			

			data->AddEntry(TAG_3_AlphaReference, AlphaReference);
			data->AddEntry(TAG_3_AlphaTestEnable, AlphaTestEnabled);

			data->AddEntry(TAG_3_DepthTestEnabled, DepthTestEnabled);
			data->AddEntry(TAG_3_DepthWriteEnabled, DepthWriteEnabled);
			data->AddEntry(TAG_3_UsePointSprite, UsePointSprite);

			// save material basic color
			{
				BinaryWriter* bw = data->AddEntry(TAG_3_MaterialColorTag);

				bw->Write(Ambient);
				bw->Write(Diffuse);
				bw->Write(Emissive);
				bw->Write(Specular);
				bw->Write(Power);


				bw->Close();
				delete bw;
			}

			data->AddEntryString(TAG_3_MaterialRefName, ExternalRefName);

			return data;
		}

		void MaterialData::Load(const ResourceLocation* rl)
		{
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

			bw->Write(MtrlId_V3);

			TaggedDataWriter* mdlData = SaveData();
			bw->Write(mdlData);
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
		}
	}
}
