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
#include "apoc3d/Config/ConfigurationSection.h"
#include "apoc3d/Vfs/ResourceLocation.h"
#include "apoc3d/Vfs/PathUtils.h"
#include "apoc3d/Graphics/Material.h"

using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace IO
	{
		namespace
		{
			void InsertName(uint32 index, const String& name, HashMap<int32, String>& entries)
			{
				String* val = entries.TryGetValue(index);
				if (val)
					*val = name;
				else
					entries.Add(index, name);
			}

			void ReadNames(BinaryReader* br, HashMap<int32, String>& entries)
			{
				int32 version = br->ReadInt32();
				(void)version;

				int32 flags = br->ReadInt32();
				(void)flags;

				int32 count = br->ReadInt32();
				entries.Resize(count);

				for (int32 i = 0; i < count; i++)
				{
					int32 index = br->ReadInt32();
					String name = br->ReadString();

					InsertName(index, name, entries);
				}
			}

			void WriteNameEntries(BinaryWriter* bw, const HashMap<int32, String>& entries)
			{
				int32 count = 0;
				for (auto e : entries)
				{
					if (e.Value.size())
						count++;
				}

				bw->WriteInt32(1);
				bw->WriteInt32(0);
				bw->WriteInt32(count);

				for (auto e : entries)
				{
					if (e.Value.size())
					{
						bw->WriteInt32(e.Key);
						bw->WriteString(e.Value);
					}
				}
			}

		}

		/** Used only when storing a material as a file */
		const int MtrlId_V3 = ((byte)'M' << 24) | ((byte)'T' << 16) | ((byte)'R' << 8) | ((byte)'L');

		// =============================================================

		const char TAG_3_CustomParamCount[] = "CustomParamCount";
		const char TAG_3_CustomParam[] = "CustomParam";

		const char TAG_3_HasTexture[] = "HasTexture";
		const char TAG_3_Texture[] = "Texture";
		const char TAG_3_HasEffect[] = "HasEffect";
		const char TAG_3_Effect[] = "Effect";
		
		const char TAG_3_1_Textures[] = "Textures";
		const char TAG_3_1_Effects[] = "Effects";

		const char TAG_3_RenderPriority[] = "RenderPriority";
		const char TAG_3_PassFlags[] = "PassFlags";

		const char TAG_3_ColorWriteMasks[] = "ColorWriteMasks";

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



		MaterialData::MaterialData() { }
		MaterialData::~MaterialData() { }


		void MaterialData::AddCustomParameter(const MaterialCustomParameter& value)
		{
			if (value.Usage.empty())
			{
				throw AP_EXCEPTION(ExceptID::Argument, L"usage can not be empty");
			}
			CustomParametrs.Add(value.Usage, value);
		}

		void MaterialData::LoadData(TaggedDataReader* data)
		{
			// load custom material parameters
			uint32 cmpCount = 0;
			data->TryGetUInt32(TAG_3_CustomParamCount, cmpCount);
			CustomParametrs.Resize(cmpCount);

			for (uint32 i = 0; i < cmpCount; i++)
			{
				std::string tag = TAG_3_CustomParam + StringUtils::UIntToNarrowString(i);

				data->ProcessData(tag, [this](BinaryReader* br) 
				{
					MaterialCustomParameter mcp;
					mcp.Type = static_cast<CustomEffectParameterType>(br->ReadUInt32());

					br->ReadBytes(reinterpret_cast<char*>(mcp.Value), sizeof(mcp.Value));

					mcp.Usage = br->ReadString();

					AddCustomParameter(mcp);
				});
			}

			// Load textures
			if (data->Contains(TAG_3_HasTexture))
			{
				bool hasTexture[MaxTextures];
				data->GetBool(TAG_3_HasTexture, hasTexture);

				for (int32 i = 0; i < MaxTextures; i++)
				{
					if (hasTexture[i])
					{
						std::string tag = StringUtils::IntToNarrowString(i);
						tag = tag + TAG_3_Texture;

						String name;
						data->GetString(tag, name);

						InsertName(i, name, TextureNames);
					}
				}
			}
			else
			{
				data->ProcessData(TAG_3_1_Textures, [this](BinaryReader* br)
				{
					ReadNames(br, TextureNames);
				});
			}

			// Load effects
			if (data->Contains(TAG_3_HasEffect))
			{
				bool hasEffect[MaxScenePass];
				data->GetBool(TAG_3_HasEffect, hasEffect);

				for (int32 i = 0; i < MaxScenePass; i++)
				{
					if (hasEffect[i])
					{
						std::string tag = StringUtils::IntToNarrowString(i);
						tag = tag + TAG_3_Effect;

						String name;
						data->GetString(tag, name);

						InsertName(i, name, EffectNames);
					}
				}
			}
			else
			{
				data->ProcessData(TAG_3_1_Effects, [this](BinaryReader* br)
				{
					ReadNames(br, EffectNames);
				});
			}

			Priority = data->GetInt32(TAG_3_RenderPriority);
			PassFlags = data->GetUInt64(TAG_3_PassFlags);

			ColorWriteMasks = 0xffffffffffffffffull;
			data->TryGetUInt64(TAG_3_ColorWriteMasks, ColorWriteMasks);

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
			data->ProcessData(TAG_3_MaterialColorTag, [this](BinaryReader* br)
			{
				br->ReadColor4(Ambient);
				br->ReadColor4(Diffuse);
				br->ReadColor4(Emissive);
				br->ReadColor4(Specular);
				Power = br->ReadSingle();
			});

			data->TryGetString(TAG_3_MaterialRefName, ExternalRefName);
		}
		TaggedDataWriter* MaterialData::SaveData()
		{
			TaggedDataWriter* data = new TaggedDataWriter(true);

			data->AddInt32(TAG_3_CustomParamCount, CustomParametrs.getCount());
			int32 index = 0;
			for (const MaterialCustomParameter& mcp : CustomParametrs.getValueAccessor())
			{	
				if (!CustomEffectParameterType_IsReference(mcp.Type))
				{
					std::string tag = TAG_3_CustomParam + StringUtils::IntToNarrowString(index++);

					data->AddEntry(tag, [&mcp](BinaryWriter* bw) 
					{
						bw->WriteUInt32(static_cast<uint32>(mcp.Type));
						bw->Write(reinterpret_cast<const char*>(mcp.Value), sizeof(mcp.Value));
						bw->WriteString(mcp.Usage);
					});
				}
			}

			// save textures
			data->AddEntry(TAG_3_1_Textures, [this](BinaryWriter* bw)
			{
				WriteNameEntries(bw, TextureNames);
			});

			// save effects
			data->AddEntry(TAG_3_1_Effects, [this](BinaryWriter* bw) 
			{
				WriteNameEntries(bw, EffectNames);
			});
			
			data->AddInt32(TAG_3_RenderPriority, Priority);
			data->AddUInt64(TAG_3_PassFlags, PassFlags);
			data->AddUInt64(TAG_3_ColorWriteMasks, ColorWriteMasks);

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
			data->AddEntry(TAG_3_MaterialColorTag, [this](BinaryWriter* bw)
			{
				bw->WriteColor4(Ambient);
				bw->WriteColor4(Diffuse);
				bw->WriteColor4(Emissive);
				bw->WriteColor4(Specular);
				bw->WriteSingle(Power);
			});

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

		Color4 DefaultColor4Parser(const String& str)
		{
			List<int32> vals = StringUtils::SplitParseInts(str, L" ,");

			switch (vals.getCount())
			{
				case 3: return Color4(vals[0], vals[1], vals[2]);
				case 4: return Color4(vals[0], vals[1], vals[2], vals[3]);
				default:
					ApocLog(LOG_System, L"[MaterialData] Cannot parse " + str, LOGLVL_Error);
					return Color4();
			}
		}

		void MaterialData::Parse(const ConfigurationSection* sect, const String& baseMtrlName, FunctorReference<Color4(const String&)> colorParser)
		{
			if (colorParser.isNull())
			{
				colorParser = DefaultColor4Parser;
			}

			// parse attributes
			String temp;

			sect->TryGetAttributeBool(L"UsePointSprite", UsePointSprite);
			sect->tryGetAttribute(L"ExternalRefName", ExternalRefName);
			sect->TryGetAttributeUInt(L"Priority", Priority);
			sect->TryGetAttributeBool(L"IsBlendTransparent", IsBlendTransparent);
			sect->TryGetAttributeBool(L"AlphaTestEnabled", AlphaTestEnabled);
			sect->TryGetAttributeUInt(L"AlphaReference", AlphaReference);
			sect->TryGetAttributeBool(L"DepthWriteEnabled", DepthWriteEnabled);
			sect->TryGetAttributeBool(L"DepthTestEnabled", DepthTestEnabled);

			if (sect->tryGetAttribute(L"PassFlag", temp))
				PassFlags = StringUtils::ParseUInt64Bin(temp);
			if (sect->tryGetAttribute(L"SourceBlend", temp))		SourceBlend = BlendConverter.Parse(temp);
			if (sect->tryGetAttribute(L"DestinationBlend", temp))	DestinationBlend = BlendConverter.Parse(temp);
			if (sect->tryGetAttribute(L"BlendFunction", temp))		BlendFunction = BlendFunctionConverter.Parse(temp);
			if (sect->tryGetAttribute(L"Cull", temp))				Cull = CullModeConverter.Parse(temp);

			if (sect->tryGetAttribute(L"Ambient", temp))			Ambient = colorParser(temp);
			if (sect->tryGetAttribute(L"Diffuse", temp))			Diffuse = colorParser(temp);
			if (sect->tryGetAttribute(L"Emissive", temp))			Emissive = colorParser(temp);
			if (sect->tryGetAttribute(L"Specular", temp))			Specular = colorParser(temp);
			sect->TryGetAttributeSingle(L"Power", Power);

			for (int32 i = 0; i < MaxTextures; i++)
			{
				String attrName = L"Texture" + StringUtils::IntToString(i + 1);
				
				if (sect->tryGetAttribute(attrName, temp))
				{
					InsertName(i, temp, TextureNames);
				}
			}
			
			String customString;
			if (sect->tryGetAttribute(L"Custom", customString))
			{
				ParseMaterialCustomParams(customString, colorParser);
			}

			String effString;
			if (sect->tryGetAttribute(L"Effect", effString))
			{
				for (const String& v : StringUtils::Split(effString, ';'))
				{
					List<String> lr = StringUtils::Split(v, ':');

					if (lr.getCount() == 1)
					{
						if (lr[0] == L"RST")
							EffectNames.Clear();
					}
					else
					{
						int ord = StringUtils::ParseInt32(lr[0].substr(1));
						String name = lr[1];

						InsertName(ord - 1, name, EffectNames);
					}

				}
			}



		}


		void MaterialData::ParseMaterialCustomParams(const String& value, FunctorReference<Color4(const String&)> colorParser)
		{
			for (const String& v : StringUtils::Split(value, ';'))
			{
				List<String> vals2 = StringUtils::Split(v, '=');

				String& usageName = vals2[0];
				String& valueStr = vals2[1];
				MaterialCustomParameter mcp;
				mcp.Usage = usageName;

				memset(mcp.Value, 0, sizeof(mcp.Value));

				if (StringUtils::StartsWith(valueStr, L"(") && StringUtils::EndsWith(valueStr, L")"))
				{
					String vec = valueStr.substr(1, valueStr.length() - 2);
					List<String> vals3 = StringUtils::Split(v, ',');

					assert(vals3.getCount() == 2 || vals3.getCount() == 4);

					if (vals3.getCount() == 2)
					{
						mcp.Type = CEPT_Vector2;
						float data[2] = { StringUtils::ParseSingle(vals3[0]), StringUtils::ParseSingle(vals3[1]) };
						memcpy(mcp.Value, data, sizeof(data));
					}
					else
					{
						mcp.Type = CEPT_Vector4;
						float data[4] =
						{
							StringUtils::ParseSingle(vals3[0]), StringUtils::ParseSingle(vals3[1]),
							StringUtils::ParseSingle(vals3[2]), StringUtils::ParseSingle(vals3[3])
						};
						memcpy(mcp.Value, data, sizeof(data));
					}
				}
				else
				{
					StringUtils::Trim(valueStr);
					//StringUtils::ToLowerCase(valueStr);

					if (valueStr == L"true" || valueStr == L"false")
					{
						mcp.Type = CEPT_Boolean;
						mcp.Value[0] = valueStr == L"true" ? 1 : 0;
					}
					else
					{
						String::size_type pos = valueStr.find('.');
						if (pos != String::npos)
						{
							mcp.Type = CEPT_Float;
							float data = StringUtils::ParseSingle(valueStr);
							memcpy(mcp.Value, &data, sizeof(data));
						}
						else
						{
							// check if the value str is numerical 
							bool isNumber = true;
							for (size_t i = 0; i < valueStr.size(); i++)
							{
								if (i == 0 && valueStr[i] == '-')
								{
									continue;
								}
								if (valueStr[i] <'0' || valueStr[i] > '9' || valueStr[i] != ' ')
								{
									isNumber = false;
								}
							}

							if (isNumber)
							{
								mcp.Type = CEPT_Integer;
								int data = StringUtils::ParseInt32(valueStr);
								memcpy(mcp.Value, &data, sizeof(data));
							}
							else
							{
								mcp.Type = CEPT_Vector4;
								Color4 v = colorParser(valueStr);
								memcpy(mcp.Value, &v, sizeof(v));
							}

						}
					}

				}

				CustomParametrs.Add(mcp.Usage, mcp);
			}
		}

		void MaterialData::CopyNonDefaultFieldsFrom(MaterialData* mtrl, bool copyTextureNames)
		{
			const MaterialData defaultMtrlSetup;

			if (mtrl->ExternalRefName != defaultMtrlSetup.ExternalRefName) ExternalRefName = mtrl->ExternalRefName;

			if (mtrl->EffectNames.getCount() > 0)
			{
				EffectNames = mtrl->EffectNames;
			}

			if (mtrl->CustomParametrs.getCount() > 0)
			{
				CustomParametrs = mtrl->CustomParametrs;
			}

			if (copyTextureNames && mtrl->TextureNames.getCount() > 0)
			{
				TextureNames = mtrl->TextureNames;
			}

			if (mtrl->PassFlags != defaultMtrlSetup.PassFlags) PassFlags = mtrl->PassFlags;

			if (mtrl->Priority != defaultMtrlSetup.Priority) Priority = mtrl->Priority;

			if (mtrl->ColorWriteMasks != defaultMtrlSetup.ColorWriteMasks) ColorWriteMasks = mtrl->ColorWriteMasks;


			if (mtrl->UsePointSprite != defaultMtrlSetup.UsePointSprite) UsePointSprite = mtrl->UsePointSprite;

			if (mtrl->SourceBlend != defaultMtrlSetup.SourceBlend) SourceBlend = mtrl->SourceBlend;
			if (mtrl->DestinationBlend != defaultMtrlSetup.DestinationBlend) DestinationBlend = mtrl->DestinationBlend;
			if (mtrl->BlendFunction != defaultMtrlSetup.BlendFunction) BlendFunction = mtrl->BlendFunction;
			if (mtrl->IsBlendTransparent != defaultMtrlSetup.IsBlendTransparent) IsBlendTransparent = mtrl->IsBlendTransparent;

			if (mtrl->Cull != defaultMtrlSetup.Cull) Cull = mtrl->Cull;

			if (mtrl->DepthWriteEnabled != defaultMtrlSetup.DepthWriteEnabled) DepthWriteEnabled = mtrl->DepthWriteEnabled;
			if (mtrl->DepthTestEnabled != defaultMtrlSetup.DepthTestEnabled) DepthTestEnabled = mtrl->DepthTestEnabled;

			if (mtrl->Ambient != defaultMtrlSetup.Ambient) Ambient = mtrl->Ambient;
			if (mtrl->Diffuse != defaultMtrlSetup.Diffuse) Diffuse = mtrl->Diffuse;
			if (mtrl->Emissive != defaultMtrlSetup.Emissive) Emissive = mtrl->Emissive;
			if (mtrl->Specular != defaultMtrlSetup.Specular) Specular = mtrl->Specular;
			if (mtrl->Power != defaultMtrlSetup.Power) Power = mtrl->Power;
		}

		void MaterialData::SetTextureName(int32 index, const String& name)
		{
			if (name.size())
				InsertName(index, name, TextureNames);
			else
				TextureNames.Remove(index);
		}

		void MaterialData::CheckObsoleteProps()
		{
			if (AlphaTestEnabled)
			{
				ApocLog(LOG_System, L"Alpha test is obsolete.", LOGLVL_Warning);
			}
		}

	}
}
