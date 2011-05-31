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


#include "Material.h"

#include "RenderSystem/Texture.h"
#include "EffectSystem/Effect.h"
#include "EffectSystem/EffectManager.h"
#include "io/Streams.h"
#include "io/TaggedData.h"
#include "IO/BinaryReader.h"
#include "IO/BinaryWriter.h"
#include "Core/ResourceHandle.h"
#include "Utility/StringUtils.h"

using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace Graphics
	{
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





		Material::Material(RenderDevice* device)
			: m_device(device),
			m_passFlags(0), m_priority(2), 
			BlendFunction(BLFUN_Add), IsBlendTransparent(false), 
			SourceBlend(BLEND_SourceAlpha), DestinationBlend(BLEND_InverseSourceAlpha),
			AlphaTestEnabled(false),
			DepthWriteEnabled(true), DepthTestEnabled(true),
			Ambient(0,0,0,0), Diffuse(1,1,1,1), Specular(0,0,0,0), Emissive(0,0,0,0), Power(0),
			Cull(CULL_None)
		{
			memset(m_tex, 0, sizeof(m_tex));
			memset(m_effects, 0, sizeof(m_effects));
		}


		Material::~Material(void)
		{
			for (int i=0;i<MaxTextures;i++)
			{
				if (m_tex[i])
				{
					delete m_tex[i];
					m_tex[i] = 0;
				}
			}
		}
		const MaterialCustomParameter* Material::getCustomParameter(const String& usage) const
		{
			CustomParamTable::const_iterator iter =  m_customParametrs.find(usage);
			if (iter != m_customParametrs.end())
			{
				return &iter->second;
			}
			return 0;
		}
		void Material::AddCustomParameter(const MaterialCustomParameter& value)
		{
			if (value.Usage.empty())
			{
				throw Apoc3DException::createException(EX_Argument, L"usage can not be empty");
			}
			m_customParametrs.insert(CustomParamTable::value_type(value.Usage, value));
		}

		void Material::LoadEffect(BinaryReader* br, int32 index)
		{
			m_effectName[index] = br->ReadString();
			if (m_effectName[index].empty())
				m_effectName[index] = L"Standard";

			m_effects[index] = EffectManager::getSingleton().getEffect(m_effectName[index]);
		}
		void Material::SaveEffect(BinaryWriter* bw, int32 index)
		{
			bw->Write(m_effectName[index]);
		}

		void Material::LoadTexture(BinaryReader* br, int32 index)
		{
			m_texName[index] = br->ReadString();
			
			// load texture
		}
		void Material::SaveTexture(BinaryWriter* bw, int32 index)
		{
			bw->Write(m_texName[index]);
		}

		void Material::LoadV2(TaggedDataReader* data)
		{
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

			m_priority = 1;
			data->TryGetDataInt32(TAG_2_RenderPriority, m_priority);
			m_priority++;

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
			
			m_passFlags = 1;

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
			{
				BinaryReader* br = data->GetData(TAG_2_Effect);
				LoadEffect(br, 0);
				br->Close();
				delete br;
			}
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
						tag = tag + TAG_2_Texture;
						br = data->GetData(tag);

						LoadTexture(br, i);

						br->Close();
						delete br;
					}
				}
			}
		}
		void Material::LoadV3(TaggedDataReader* data)
		{
			// load custom material parameters
			uint32 cmpCount = 0;
			data->TryGetDataUInt32(TAG_3_CustomParamCount, cmpCount);
			//m_customParametrs.reserve(cmpCount);
			m_customParametrs.rehash(cmpCount);

			for (uint32 i=0;i<cmpCount;i++)
			{
				String tag = StringUtils::ToString(i);
				tag = tag + TAG_3_CustomParam;

				BinaryReader* br = data->GetData(tag);

				MaterialCustomParameter mcp;
				mcp.Type = static_cast<MaterialCustomParameterType>(br->ReadUInt32());

				br->ReadBytes(reinterpret_cast<char*>(mcp.Value), sizeof(mcp.Value));

				mcp.Usage = br->ReadString();

				br->Close();
				delete br;

				AddCustomParameter(mcp);
			}

			// Load textures
			{
				BinaryReader* br = data->GetData(TAG_3_HasTexture);
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
						tag = tag + TAG_3_Texture;
						br = data->GetData(tag);

						LoadTexture(br, i);

						br->Close();
						delete br;
					}
				}
			}

			// Load effects
			{
				BinaryReader* br = data->GetData(TAG_3_HasEffect);
				bool hasEffect[MaxScenePass];
				for (int32 i=0;i<MaxScenePass;i++)
				{
					hasEffect[i] = br->ReadBoolean();
				} 
				br->Close();
				delete br;

				for (int32 i=0;i<MaxScenePass;i++)
				{
					if (hasEffect[i])
					{
						String tag = StringUtils::ToString(i);
						tag = tag + TAG_3_Effect;
						br = data->GetData(tag);

						LoadEffect(br, i);

						br->Close();
						delete br;
					}
				}
			}

			m_priority = data->GetDataInt32(TAG_3_RenderPriority);
			m_passFlags = data->GetDataUInt64(TAG_3_PassFlags);

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
		}

		void Material::Load(TaggedDataReader* data)
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
		TaggedDataWriter* Material::Save()
		{
			TaggedDataWriter* data = new TaggedDataWriter(true);

			data->AddEntry(TAG_3_CustomParamCount, m_customParametrs.size());
			int32 index = 0;
			for (CustomParamTable::iterator iter = m_customParametrs.begin(); iter != m_customParametrs.end(); iter++)
			{
				String tag = StringUtils::ToString(index++);
				tag = tag + TAG_3_CustomParam;

				BinaryWriter* bw = data->AddEntry(tag);

				const MaterialCustomParameter& mcp = iter->second;

				bw->Write(static_cast<uint32>(mcp.Type));
				bw->Write(reinterpret_cast<const char*>(mcp.Value), sizeof(mcp.Value));
				bw->Write(mcp.Usage);

				bw->Close();
				delete bw;
			}

			// save textures
			{
				BinaryWriter* bw = data->AddEntry(TAG_3_HasTexture);
				for (int32 i=0;i<MaxTextures;i++)
				{
					bw->Write(!m_texName[i].empty());
				}
				bw->Close();
				delete bw;

				for (int32 i=0;i<MaxTextures;i++)
				{
					if (!m_texName[i].empty())
					{
						String tag = StringUtils::ToString(i);
						tag = tag + TAG_3_Texture;

						bw = data->AddEntry(tag);
						SaveTexture(bw, i);
						bw->Close();
						delete bw;
					}
				}
			}
			// save effects
			{
				BinaryWriter* bw = data->AddEntry(TAG_3_HasEffect);
				for (int32 i=0;i<MaxScenePass;i++)
				{
					bw->Write(!m_effectName[i].empty());
				}
				bw->Close();
				delete bw;

				for (int32 i=0;i<MaxScenePass;i++)
				{
					if (!m_effectName[i].empty())
					{
						String tag = StringUtils::ToString(i);
						tag = tag + TAG_3_Effect;

						bw = data->AddEntry(tag);
						SaveEffect(bw, i);
						bw->Close();
						delete bw;
					}
				}
			}

			data->AddEntry(TAG_3_RenderPriority, m_priority);
			data->AddEntry(TAG_3_PassFlags, m_passFlags);
			
			data->AddEntry(TAG_3_IsBlendTransparent, IsBlendTransparent);
			data->AddEntry(TAG_3_SourceBlend, static_cast<uint32>(SourceBlend));
			data->AddEntry(TAG_3_DestinationBlend, static_cast<uint32>(DestinationBlend));
			data->AddEntry(TAG_3_BlendFunction, static_cast<uint32>(BlendFunction));
			
			data->AddEntry(TAG_3_CullMode, static_cast<uint32>(Cull));			

			data->AddEntry(TAG_3_AlphaReference, AlphaReference);
			data->AddEntry(TAG_3_AlphaTestEnable, AlphaTestEnabled);

			data->AddEntry(TAG_3_DepthTestEnabled, DepthTestEnabled);
			data->AddEntry(TAG_3_DepthWriteEnabled, DepthWriteEnabled);


			// load material basic color
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
			return data;
		}

	}
};