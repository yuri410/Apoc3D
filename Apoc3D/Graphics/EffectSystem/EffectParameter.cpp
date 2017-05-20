/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2011-2017 Tao Xin
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

#include "EffectParameter.h"
#include "apoc3d/Config/ConfigurationSection.h"
#include "apoc3d/Core/Logging.h"
#include "apoc3d/IOLib/BinaryReader.h"
#include "apoc3d/IOLib/BinaryWriter.h"
#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/Utility/TypeConverter.h"
#include "apoc3d/Math/MathCommon.h"

using namespace Apoc3D::Core;
using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace EffectSystem
		{
			const TypeDualConverter<EffectParamUsage> EffectParameterUsageConverter =
			{
				{ L"unknown", EPUSAGE_Unknown },
				{ L"mc4_ambient", EPUSAGE_MtrlC4_Ambient },
				{ L"mc4_diffuse", EPUSAGE_MtrlC4_Diffuse },
				{ L"mc4_emissive", EPUSAGE_MtrlC4_Emissive },
				{ L"mc4_specular", EPUSAGE_MtrlC4_Specular },
				{ L"mc_power", EPUSAGE_MtrlC_Power },

				{ L"tr_worldviewproj", EPUSAGE_Trans_WorldViewProj },
				{ L"tr_worldvieworiproj", EPUSAGE_Trans_WorldViewOriProj },
				{ L"tr_world", EPUSAGE_Trans_World },
				{ L"tr_worldview", EPUSAGE_Trans_WorldView },
				{ L"tr_view", EPUSAGE_Trans_View },
				{ L"tr_viewproj", EPUSAGE_Trans_ViewProj },
				{ L"tr_proj", EPUSAGE_Trans_Projection },

				{ L"tr_invproj", EPUSAGE_Trans_InvProj },
				{ L"tr_invview", EPUSAGE_Trans_InvView },
				{ L"tr_invviewproj", EPUSAGE_Trans_InvViewProj },

				{ L"tr_instanceworld", EPUSAGE_Trans_InstanceWorlds },

				{ L"m4x3_bonestransform", EPUSAGE_M4X3_BoneTrans },
				{ L"m4x4_bonestransform", EPUSAGE_M4X4_BoneTrans },

				{ L"v3_camerax", EPUSAGE_V3_CameraX },
				{ L"v3_cameray", EPUSAGE_V3_CameraY },
				{ L"v3_cameraz", EPUSAGE_V3_CameraZ },
				{ L"v3_camerapos", EPUSAGE_V3_CameraPos },

				{ L"tex_0", EPUSAGE_Tex0 },
				{ L"tex_1", EPUSAGE_Tex1 },
				{ L"tex_2", EPUSAGE_Tex2 },
				{ L"tex_3", EPUSAGE_Tex3 },
				{ L"tex_4", EPUSAGE_Tex4 },
				{ L"tex_5", EPUSAGE_Tex5 },
				{ L"tex_6", EPUSAGE_Tex6 },
				{ L"tex_7", EPUSAGE_Tex7 },
				{ L"tex_8", EPUSAGE_Tex8 },
				{ L"tex_9", EPUSAGE_Tex9 },
				{ L"tex_10", EPUSAGE_Tex10 },
				{ L"tex_11", EPUSAGE_Tex11 },
				{ L"tex_12", EPUSAGE_Tex12 },
				{ L"tex_13", EPUSAGE_Tex13 },
				{ L"tex_14", EPUSAGE_Tex14 },
				{ L"tex_15", EPUSAGE_Tex15 },
				{ L"tex_default", EPUSAGE_DefaultTexture },

				{ L"lv3_lightdir", EPUSAGE_LV3_LightDir },
				{ L"lv3_lightpos", EPUSAGE_LV3_LightPos },
				{ L"lc4_ambient", EPUSAGE_LC4_Ambient },
				{ L"lc4_diffuse", EPUSAGE_LC4_Diffuse },
				{ L"lc4_specular", EPUSAGE_LC4_Specular },

				{ L"pv3_viewpos", EPUSAGE_PV3_ViewPos },
				{ L"sv2_viewportsize", EPUSAGE_SV2_ViewportSize },
				{ L"sv2_invviewportsize", EPUSAGE_SV2_InvViewportSize },
				{ L"s_unifiedtime", EPUSAGE_S_UnifiedTime },

				{ L"s_farplane", EPUSAGE_S_FarPlane },
				{ L"s_nearplane", EPUSAGE_S_NearPlane },

				{ L"instanceblob", EPUSAGE_InstanceBlob },
				{ L"cmtrlparam", EPUSAGE_CustomMaterialParam },
			};
			
			EffectParameter::EffectParameter(const String& name)
				: Name(name) { }

			EffectParameter::~EffectParameter()
			{
			}


			void EffectParameter::Read(BinaryReader* br)
			{
				int32 version = br->ReadInt32();

				Name = br->ReadString();
				Usage = EffectParameter::ParseParamUsage(br->ReadString());
				CustomMaterialParamName = br->ReadString();
				InstanceBlobIndex = br->ReadInt32();
				ProgramType = static_cast<ShaderType>(br->ReadInt32());

				RegisterIndex = br->ReadInt32();
				SamplerIndex = br->ReadInt32();

				SamplerState.AddressU = static_cast<TextureAddressMode>(br->ReadUInt32());
				SamplerState.AddressV = static_cast<TextureAddressMode>(br->ReadUInt32());
				SamplerState.AddressW = static_cast<TextureAddressMode>(br->ReadUInt32());
				SamplerState.BorderColor = br->ReadUInt32();
				SamplerState.MagFilter = static_cast<TextureFilter>(br->ReadUInt32());
				SamplerState.MaxAnisotropy = br->ReadInt32();
				SamplerState.MaxMipLevel = br->ReadInt32();
				SamplerState.MinFilter = static_cast<TextureFilter>(br->ReadUInt32());
				SamplerState.MipFilter = static_cast<TextureFilter>(br->ReadUInt32());
				SamplerState.MipMapLODBias = br->ReadUInt32();

				if (version >= 2)
					SamplerStateOverridenGroupName = br->ReadString();

				DefaultTextureName = br->ReadString();
			}
			void EffectParameter::Write(BinaryWriter* bw)
			{
				bw->WriteInt32(2);

				bw->WriteString(Name);
				bw->WriteString(EffectParameter::ToString(Usage));
				bw->WriteString(CustomMaterialParamName);
				bw->WriteInt32(InstanceBlobIndex);
				bw->WriteInt32((int32)ProgramType);

				bw->WriteInt32(RegisterIndex);
				bw->WriteInt32(SamplerIndex);

				bw->WriteUInt32((uint32)SamplerState.AddressU);
				bw->WriteUInt32((uint32)SamplerState.AddressV);
				bw->WriteUInt32((uint32)SamplerState.AddressW);
				bw->WriteUInt32((uint32)SamplerState.BorderColor);
				bw->WriteUInt32((uint32)SamplerState.MagFilter);
				bw->WriteUInt32((uint32)SamplerState.MaxAnisotropy);
				bw->WriteUInt32((uint32)SamplerState.MaxMipLevel);
				bw->WriteUInt32((uint32)SamplerState.MinFilter);
				bw->WriteUInt32((uint32)SamplerState.MipFilter);
				bw->WriteUInt32((uint32)SamplerState.MipMapLODBias);

				bw->WriteString(SamplerStateOverridenGroupName);

				bw->WriteString(DefaultTextureName);
			}


			EffectParamUsage EffectParameter::ParseParamUsage(const String& val)
			{
				EffectParamUsage usage;
				if (EffectParameterUsageConverter.TryParse(val, usage))
					return usage;
				
				if (val.size())
				{
					LogManager::getSingleton().Write(LOG_Graphics, L"Unknown effect parameter usage: " + val, LOGLVL_Warning );
				}
				return EPUSAGE_Unknown;
			}
			bool EffectParameter::SupportsParamUsage(const String& val)
			{
				return EffectParameterUsageConverter.SupportsName(val);
			}

			String EffectParameter::ToString(EffectParamUsage usage)
			{
				String result;
				if (EffectParameterUsageConverter.TryToString(usage, result))
					return result;
				return L"unknown";
			}

			void EffectParameter::FillParameterUsageNames(List<String>& results)
			{
				EffectParameterUsageConverter.DumpNames(results);
			}

			/************************************************************************/
			/*                                                                      */
			/************************************************************************/

			Camera* RendererEffectParams::CurrentCamera = 0;
			Vector3 RendererEffectParams::LightDirection(0.707f, 0.707f,0);
			Vector3 RendererEffectParams::LightPosition(0, 0, 0);

			Color4 RendererEffectParams::LightAmbient(0.5f,0.5f,0.5f);
			Color4 RendererEffectParams::LightDiffuse(1.0f,1.0f,1.0f);
			Color4 RendererEffectParams::LightSpecular(1.0f,1.0f,1.0f);

			/************************************************************************/
			/*                                                                      */
			/************************************************************************/


			static int32 CustomEffectParameterType_GetSize(CustomEffectParameterType t)
			{
				if (CustomEffectParameterType_IsReference(t))
					return 0;

				if (t == CEPT_Float)
					return sizeof(float);
				if (t == CEPT_Vector2)
					return sizeof(float) * 2;
				if (t == CEPT_Vector4)
					return sizeof(float) * 4;
				if (t == CEPT_Matrix)
					return sizeof(float) * 16;
				if (t == CEPT_Boolean)
					return sizeof(bool);
				if (t == CEPT_Integer)
					return sizeof(int32);

				assert(0);
				return 0;
			}

			InstanceInfoBlob2::InstanceInfoBlob2(std::initializer_list<InstanceInfoBlobValue> layout)
				: m_count(layout.size())
			{
				int32 totalSize = 0;
				int32 localStorageCount = 0;
				int32 localStorageSize = 0;

				for (const InstanceInfoBlobValue& v : layout)
				{
					int32 itmSize = CustomEffectParameterType_GetSize(v.Type);
					totalSize += itmSize;

					if (totalSize < LocalSizeLimit && localStorageCount < LocalCountLimit)
					{
						localStorageCount++;
						localStorageSize += itmSize;
					}
				}
				
				m_locallyStoredCount = localStorageCount;

				int32 allocatedStorageCount = m_count - localStorageCount;
				int32 allocatedStorageSize = totalSize - localStorageSize;

				m_allocatedStorage = new char[allocatedStorageSize];
				m_allocatedItemInfo = new ValueProxy[allocatedStorageCount];

				int32 idx = 0;
				char* localDst = m_localStorage;
				char* allocDst = m_allocatedStorage;

				for (const InstanceInfoBlobValue& v : layout)
				{
					int32 itmSize = CustomEffectParameterType_GetSize(v.Type);

					if (idx < localStorageCount)
					{
						m_localItemInfo[idx] = ValueProxy(localDst, v.Type);
						m_localItemInfo[idx] = v;

						localDst += itmSize;
					}
					else
					{
						m_allocatedItemInfo[idx - localStorageCount] = ValueProxy(allocDst, v.Type);
						m_allocatedItemInfo[idx - localStorageCount] = v;

						allocDst += itmSize;
					}

					idx++;
				}
			}


			InstanceInfoBlob2::~InstanceInfoBlob2()
			{
				delete[] m_allocatedStorage;
				delete[] m_allocatedItemInfo;
			}

			InstanceInfoBlob2::ValueProxy& InstanceInfoBlob2::ValueProxy::operator = (const InstanceInfoBlobValue& o)
			{
				if (CustomEffectParameterType_IsReference(o.Type))
				{
					m_address = o.RefValue;
				}

				switch (o.Type)
				{
					case CEPT_Float: AsSingle() = o.AsSingle(); break;
					case CEPT_Vector2: AsVector2() = o.AsVector2(); break;
					case CEPT_Vector4: AsVector4() = o.AsVector4(); break;
					case CEPT_Matrix: AsMatrix() = o.AsMatrix(); break;
					case CEPT_Boolean: AsBoolean() = o.AsBoolean(); break;
					case CEPT_Integer: AsInteger() = o.AsInteger(); break;
					default:
						assert(0);
				}

				return *this;
			}

			InstanceInfoBlob2::InstanceInfoBlob2(const InstanceInfoBlob2& o)
				: m_count(o.m_count), m_locallyStoredCount(o.m_locallyStoredCount)
			{
				CopyArray(m_localStorage, o.m_localStorage);
				CopyArray(m_localItemInfo, o.m_localItemInfo);

				int32 allocatedStorageCount = m_count - m_locallyStoredCount;
				if (o.m_allocatedItemInfo)
				{
					m_allocatedItemInfo = new ValueProxy[allocatedStorageCount];

					int32 allocatedSize = 0;
					for (int32 i = 0; i < allocatedStorageCount; i++)
					{
						m_allocatedItemInfo[i] = o.m_allocatedItemInfo[i];
						allocatedSize += CustomEffectParameterType_GetSize(m_allocatedItemInfo[i].getType());
					}

					m_allocatedStorage = new char[allocatedSize];
					memcpy(m_allocatedStorage, o.m_allocatedStorage, allocatedSize);
				}

			}
			InstanceInfoBlob2::InstanceInfoBlob2(InstanceInfoBlob2&& o)
				: m_count(o.m_count), m_locallyStoredCount(o.m_locallyStoredCount), 
				m_allocatedStorage(o.m_allocatedStorage), m_allocatedItemInfo(o.m_allocatedItemInfo)
			{
				CopyArray(m_localStorage, o.m_localStorage);
				CopyArray(m_localItemInfo, o.m_localItemInfo);

				o.m_allocatedStorage = nullptr;
				o.m_allocatedItemInfo = nullptr;
			}

			InstanceInfoBlob2& InstanceInfoBlob2::operator=(const InstanceInfoBlob2& o)
			{
				if (this != &o)
				{
					this->~InstanceInfoBlob2();
					new (this)InstanceInfoBlob2(o); // no derived class allowed. This is OK.
				}
				return *this;
			}
			InstanceInfoBlob2& InstanceInfoBlob2::operator=(InstanceInfoBlob2&& o)
			{
				if (this != &o)
				{
					this->~InstanceInfoBlob2();
					new (this)InstanceInfoBlob2(std::move(o));
				}
				return *this;
			}

		}
	}
}