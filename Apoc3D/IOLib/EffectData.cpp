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

#include "EffectData.h"
#include "apoc3d/IOLib/BinaryReader.h"
#include "apoc3d/IOLib/BinaryWriter.h"
#include "apoc3d/IOLib/Streams.h"
#include "apoc3d/IOLib/TaggedData.h"
#include "apoc3d/Vfs/ResourceLocation.h"
#include "apoc3d/Core/Logging.h"
#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/Math/MathCommon.h"

using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace IO
	{
		const char* EffectProfileData::Imp_HLSL = "hlsl";
		const char* EffectProfileData::Imp_GLSL = "glsl";

		EffectProfileData::EffectProfileData()
			: VSCode(nullptr), PSCode(nullptr), GSCode(nullptr),
			VSLength(0), PSLength(0), GSLength(0)
		{
			memset(ImplementationType, 0, sizeof(ImplementationType));
		}
		EffectProfileData::~EffectProfileData()
		{
			if (VSCode) { delete[] VSCode; VSCode = nullptr; }
			if (PSCode) { delete[] PSCode; PSCode = nullptr; }
			if (GSCode) { delete[] GSCode; GSCode = nullptr; }
		}

		void EffectProfileData::Load(BinaryReader* br)
		{
			br->ReadBytes(ImplementationType, sizeof(ImplementationType));

			MajorVer = br->ReadInt32();
			MinorVer = br->ReadInt32();
			VSLength = br->ReadInt32();
			PSLength = br->ReadInt32();
			GSLength = br->ReadInt32();
			br->ReadInt32(); // HS
			br->ReadInt32(); // CS
			br->ReadInt32(); // DS

			VSCode = new char[VSLength];
			br->ReadBytes(VSCode, VSLength);

			PSCode = new char[PSLength];
			br->ReadBytes(PSCode, PSLength);

			if (GSLength>0)
			{
				GSCode = new char[GSLength];
				br->ReadBytes(GSCode, GSLength);
			}

			int paramCount = br->ReadInt32();
			Parameters.ReserveDiscard(paramCount);
			for (int32 i=0;i<paramCount;i++)
			{
				EffectParameter& fxParam = Parameters[i];
				fxParam.Name = br->ReadString();
				fxParam.Usage = EffectParameter::ParseParamUsage(br->ReadString());
				fxParam.CustomMaterialParamName = br->ReadString();
				fxParam.InstanceBlobIndex = br->ReadInt32();
				fxParam.ProgramType = static_cast<ShaderType>(br->ReadInt32());

				fxParam.RegisterIndex = br->ReadInt32();
				fxParam.SamplerIndex = br->ReadInt32();

				fxParam.SamplerState.AddressU = static_cast<TextureAddressMode>(br->ReadUInt32());
				fxParam.SamplerState.AddressV = static_cast<TextureAddressMode>(br->ReadUInt32());
				fxParam.SamplerState.AddressW = static_cast<TextureAddressMode>(br->ReadUInt32());
				fxParam.SamplerState.BorderColor = br->ReadUInt32();
				fxParam.SamplerState.MagFilter = static_cast<TextureFilter>(br->ReadUInt32());
				fxParam.SamplerState.MaxAnisotropy = br->ReadInt32();
				fxParam.SamplerState.MaxMipLevel = br->ReadInt32();
				fxParam.SamplerState.MinFilter = static_cast<TextureFilter>(br->ReadUInt32());
				fxParam.SamplerState.MipFilter = static_cast<TextureFilter>(br->ReadUInt32());
				fxParam.SamplerState.MipMapLODBias = br->ReadUInt32();
			}
		}
		void EffectProfileData::Save(BinaryWriter* bw)
		{
			bw->Write(ImplementationType, sizeof(ImplementationType));

			bw->WriteInt32(MajorVer);
			bw->WriteInt32(MinorVer);
			bw->WriteInt32(VSLength);
			bw->WriteInt32(PSLength);
			bw->WriteInt32(GSLength);
			bw->WriteInt32(0);
			bw->WriteInt32(0);
			bw->WriteInt32(0);

			bw->Write(VSCode, VSLength);
			bw->Write(PSCode, PSLength);
			
			if (GSLength>0)
			{
				bw->Write(GSCode, GSLength);
			}

			bw->WriteInt32(Parameters.getCount());
			for (int32 i=0;i<Parameters.getCount();i++)
			{
				const EffectParameter& fxParam = Parameters[i];
				bw->WriteString(fxParam.Name);
				bw->WriteString(EffectParameter::ToString(fxParam.Usage));
				bw->WriteString(fxParam.CustomMaterialParamName);
				bw->WriteInt32(fxParam.InstanceBlobIndex);
				bw->WriteInt32((int32)fxParam.ProgramType);

				bw->WriteInt32(fxParam.RegisterIndex);
				bw->WriteInt32(fxParam.SamplerIndex);

				bw->WriteUInt32((uint32)fxParam.SamplerState.AddressU);
				bw->WriteUInt32((uint32)fxParam.SamplerState.AddressV);
				bw->WriteUInt32((uint32)fxParam.SamplerState.AddressW);
				bw->WriteUInt32((uint32)fxParam.SamplerState.BorderColor);
				bw->WriteUInt32((uint32)fxParam.SamplerState.MagFilter);
				bw->WriteUInt32((uint32)fxParam.SamplerState.MaxAnisotropy);
				bw->WriteUInt32((uint32)fxParam.SamplerState.MaxMipLevel);
				bw->WriteUInt32((uint32)fxParam.SamplerState.MinFilter);
				bw->WriteUInt32((uint32)fxParam.SamplerState.MipFilter);
				bw->WriteUInt32((uint32)fxParam.SamplerState.MipMapLODBias);
			}
		}
		void EffectProfileData::SaveLite(BinaryWriter* bw)
		{
			bw->Write(ImplementationType, sizeof(ImplementationType));

			bw->WriteInt32(MajorVer);
			bw->WriteInt32(MinorVer);
			bw->WriteInt32(VSLength);
			bw->WriteInt32(PSLength);
			bw->WriteInt32(GSLength);

			bw->Write(VSCode, VSLength);
			bw->Write(PSCode, PSLength);

			if (GSLength>0)
			{
				bw->Write(GSCode, GSLength);
			}
		}

		bool EffectProfileData::MatchImplType(const char* str) const
		{
			return strcmp(ImplementationType, str) == 0;
		}

		void EffectProfileData::SetImplType(const char* str)
		{
			memcpy(ImplementationType, str, Math::Min((int32)sizeof(ImplementationType)-1, (int32)strlen(str)));
		}
		void EffectProfileData::SetImplType(const std::string& str)
		{
			memcpy(ImplementationType, str.c_str(), Math::Min((int32)sizeof(ImplementationType)-1, (int32)str.size()));
		}

		//////////////////////////////////////////////////////////////////////////

		const int LfxID_V1 = 'LFX1';

		const int AfxId_V3 = ((byte)'A' << 24) | ((byte)'F' << 16) | ((byte)'X' << 8) | ((byte)' ');
		const int AfxId_V31 = ((byte)'A' << 24) | ((byte)'F' << 16) | ((byte)'X' << 8) | ((byte)'0');
		const int AfxId_V4 = 'AFX1';

		const int CfxID_V3 = ((byte)'C' << 24) | ((byte)'F' << 16) | ((byte)'X' << 8) | ((byte)' ');
		const int CfxID_V4 = 'CFX1';

		const String TAG_3_ParameterCountTag = L"ParameterCount";
		const String TAG_3_ParameterTag = L"Parameter";
		const String TAG_3_ParameterSamplerStateTag = L"SamplerState";
		const String TAG_3_ShaderCodeTag = L"ShaderCode";
		
		const String TAG_3_ShaderCodeLengthTag = L"ShaderCodeLength";
		
		EffectData::EffectData() 
			: Profiles(nullptr), ProfileCount(0), IsCFX(false)
		{
		}
		EffectData::~EffectData() 
		{
			delete[] Profiles; 
		}


		void EffectData::Load(const ResourceLocation& rl)
		{
			BinaryReader _br(rl.GetReadStream());
			BinaryReader* br = &_br;

			int id = br->ReadInt32();

			if (id == AfxId_V3)
			{
				LoadAFXV3(br);
			}
			else if (id == AfxId_V31)
			{
				LoadAFXV3_1(br);
			}
			else if (id == AfxId_V4)
			{
				LoadFXV4(br);
			}
			else if (id == CfxID_V3)
			{
				IsCFX = true;

				ProfileCount = 1;
				Profiles = new EffectProfileData[ProfileCount];
				Profiles->SetImplType(EffectProfileData::Imp_HLSL);
				Profiles->MajorVer = br->ReadInt32();
				Profiles->MinorVer = br->ReadInt32();
				Name = br->ReadString();

				TaggedDataReader* data = br->ReadTaggedDataBlock();

				BinaryReader* br3 = data->GetData(TAG_3_ShaderCodeLengthTag);
				Profiles->VSLength = br3->ReadInt32();
				Profiles->PSLength = br3->ReadInt32();
				br3->Close();
				delete br3;

				Profiles->VSCode = new char[Profiles->VSLength];
				Profiles->PSCode = new char[Profiles->PSLength];
				br3 = data->GetData(TAG_3_ShaderCodeTag);
				br3->ReadBytes(Profiles->VSCode, Profiles->VSLength);
				br3->ReadBytes(Profiles->PSCode, Profiles->PSLength);
				br3->Close();
				delete br3;

				data->Close();
				delete data;
			}
			else if (id == CfxID_V4)
			{
				LoadFXV4(br);
				IsCFX = true;
			}
			else
			{
				LogManager::getSingleton().Write(LOG_Graphics, L"Invalid effect file. " + rl.getName(), LOGLVL_Error);
			}
			
			br->Close();
		}

		void EffectData::LoadAFXV3(BinaryReader* br)
		{
			ProfileCount = 1;
			Profiles = new EffectProfileData[ProfileCount];
			Profiles->SetImplType(EffectProfileData::Imp_HLSL);
			Profiles->MajorVer = br->ReadInt32();
			Profiles->MinorVer = br->ReadInt32();
			Name = br->ReadString();

			TaggedDataReader* data = br->ReadTaggedDataBlock();

			int32 count = data->GetDataInt32(TAG_3_ParameterCountTag);
			Profiles->Parameters.ResizeDiscard(count);

			for (int32 i=0;i<count;i++)
			{
				String tag = StringUtils::IntToString(i);
				tag = TAG_3_ParameterTag + tag;

				BinaryReader* br2 = data->GetData(tag);

				// load parameter
				String name = br2->ReadString();

				EffectParameter params(name);
				params.CustomMaterialParamName = br->ReadString();
				params.ProgramType = static_cast<ShaderType>(br->ReadInt32());

				params.Usage = EffectParameter::ParseParamUsage(params.CustomMaterialParamName);
				if (params.Usage == EPUSAGE_Unknown)
				{
					params.Usage = EPUSAGE_CustomMaterialParam;
				}

				br2->Close();
				delete br2;

				tag = StringUtils::IntToString(i);
				tag = TAG_3_ParameterSamplerStateTag + tag;

				if (data->Contains(tag))
				{
					br2 = data->GetData(tag);

					params.SamplerState.AddressU = static_cast<TextureAddressMode>(br2->ReadUInt32());
					params.SamplerState.AddressV = static_cast<TextureAddressMode>(br2->ReadUInt32());
					params.SamplerState.AddressW = static_cast<TextureAddressMode>(br2->ReadUInt32());
					params.SamplerState.BorderColor = br2->ReadUInt32();
					params.SamplerState.MagFilter = static_cast<TextureFilter>(br2->ReadUInt32());
					params.SamplerState.MaxAnisotropy = br2->ReadInt32();
					params.SamplerState.MaxMipLevel = br2->ReadInt32();
					params.SamplerState.MinFilter = static_cast<TextureFilter>(br2->ReadUInt32());
					params.SamplerState.MipFilter = static_cast<TextureFilter>(br2->ReadUInt32());
					params.SamplerState.MipMapLODBias = br2->ReadInt32();

					br2->Close();
					delete br2;
				}
				else
				{
					params.SamplerState.AddressU = TA_Wrap;
					params.SamplerState.AddressV = TA_Wrap;
					params.SamplerState.AddressW = TA_Wrap;
					params.SamplerState.BorderColor = 0;
					params.SamplerState.MagFilter = TFLT_Linear;
					params.SamplerState.MaxAnisotropy = 1;
					params.SamplerState.MaxMipLevel = 0;
					params.SamplerState.MinFilter = TFLT_Linear;
					params.SamplerState.MipFilter = TFLT_Linear;
					params.SamplerState.MipMapLODBias = 0;

				}

				Profiles->Parameters.Add(params);

			}


			BinaryReader* br3 = data->GetData(TAG_3_ShaderCodeLengthTag);
			Profiles->VSLength = br3->ReadInt32();
			Profiles->PSLength = br3->ReadInt32();
			br3->Close();
			delete br3;

			Profiles->VSCode = new char[Profiles->VSLength];
			Profiles->PSCode = new char[Profiles->PSLength];
			br3 = data->GetData(TAG_3_ShaderCodeTag);
			br3->ReadBytes(Profiles->VSCode, Profiles->VSLength);
			br3->ReadBytes(Profiles->PSCode, Profiles->PSLength);
			br3->Close();
			delete br3;

			data->Close();
			delete data;

		}
		void EffectData::LoadAFXV3_1(BinaryReader* br)
		{
			ProfileCount = 1;
			Profiles = new EffectProfileData[ProfileCount];
			Profiles->SetImplType(EffectProfileData::Imp_HLSL);
			Profiles->MajorVer = br->ReadInt32();
			Profiles->MinorVer = br->ReadInt32();
			Name = br->ReadString();

			TaggedDataReader* data = br->ReadTaggedDataBlock();

			int32 count = data->GetDataInt32(TAG_3_ParameterCountTag);
			Profiles->Parameters.ResizeDiscard(count);

			for (int32 i=0;i<count;i++)
			{
				String tag = StringUtils::IntToString(i);
				tag = TAG_3_ParameterTag + tag;
				BinaryReader* br2 = data->GetData(tag);

				// load parameter
				String name = br2->ReadString();

				EffectParameter params(name);
				
				params.Usage = EffectParameter::ParseParamUsage(br->ReadString());
				params.CustomMaterialParamName = br->ReadString();
				params.InstanceBlobIndex = br->ReadInt32();
				params.ProgramType = static_cast<ShaderType>(br->ReadInt32());

				br2->Close();
				delete br2;

				tag = StringUtils::IntToString(i);
				tag = TAG_3_ParameterSamplerStateTag + tag;

				if (data->Contains(tag))
				{
					br2 = data->GetData(tag);

					params.SamplerState.AddressU = static_cast<TextureAddressMode>(br2->ReadUInt32());
					params.SamplerState.AddressV = static_cast<TextureAddressMode>(br2->ReadUInt32());
					params.SamplerState.AddressW = static_cast<TextureAddressMode>(br2->ReadUInt32());
					params.SamplerState.BorderColor = br2->ReadUInt32();
					params.SamplerState.MagFilter = static_cast<TextureFilter>(br2->ReadUInt32());
					params.SamplerState.MaxAnisotropy = br2->ReadInt32();
					params.SamplerState.MaxMipLevel = br2->ReadInt32();
					params.SamplerState.MinFilter = static_cast<TextureFilter>(br2->ReadUInt32());
					params.SamplerState.MipFilter = static_cast<TextureFilter>(br2->ReadUInt32());
					params.SamplerState.MipMapLODBias = br2->ReadInt32();

					br2->Close();
					delete br2;
				}
				else
				{
					params.SamplerState.AddressU = TA_Wrap;
					params.SamplerState.AddressV = TA_Wrap;
					params.SamplerState.AddressW = TA_Wrap;
					params.SamplerState.BorderColor = 0;
					params.SamplerState.MagFilter = TFLT_Linear;
					params.SamplerState.MaxAnisotropy = 0;
					params.SamplerState.MaxMipLevel = 0;
					params.SamplerState.MinFilter = TFLT_Linear;
					params.SamplerState.MipFilter = TFLT_Linear;
					params.SamplerState.MipMapLODBias = 0;

				}

				Profiles->Parameters.Add(params);
			}


			BinaryReader* br3 = data->GetData(TAG_3_ShaderCodeLengthTag);
			Profiles->VSLength = br3->ReadInt32();
			Profiles->PSLength = br3->ReadInt32();
			br3->Close();
			delete br3;

			Profiles->VSCode = new char[Profiles->VSLength];
			Profiles->PSCode = new char[Profiles->PSLength];
			br3 = data->GetData(TAG_3_ShaderCodeTag);
			br3->ReadBytes(Profiles->VSCode, Profiles->VSLength);
			br3->ReadBytes(Profiles->PSCode, Profiles->PSLength);
			br3->Close();
			delete br3;

			data->Close();
			delete data;

		}
		void EffectData::LoadFXV4(BinaryReader* br)
		{
			Name = br->ReadString();
			ProfileCount = br->ReadInt32();
			Profiles = new EffectProfileData[ProfileCount];
			for (int32 i=0;i<ProfileCount;i++)
			{
				Profiles[i].Load(br);
			}
		}

		void EffectData::Save(Stream* strm) const
		{
			BinaryWriter _bw(strm);
			BinaryWriter* bw = &_bw;

			if (IsCFX)
			{
				bw->WriteInt32((int32)CfxID_V4);
			}
			else
			{
				bw->WriteInt32((int32)AfxId_V4);
			}

			bw->WriteString(Name);
			bw->WriteInt32(ProfileCount);
			for (int32 i=0;i<ProfileCount;i++)
			{
				Profiles[i].Save(bw);
			}
			bw->Close();
		}
		void EffectData::SaveLite(Stream* strm) const
		{
			BinaryWriter _bw(strm);
			BinaryWriter* bw = &_bw;

			bw->WriteInt32(LfxID_V1);
			bw->WriteInt32(ProfileCount);
			for (int32 i=0;i<ProfileCount;i++)
			{
				Profiles[i].SaveLite(bw);
			}
			bw->Close();
		}

		void EffectData::SortProfiles()
		{
			for (int i=0;i<ProfileCount;i++)
			{
				for (int j=i+1;j<ProfileCount;j++)
				{
					uint64 verI = ((uint64)Profiles[i].MajorVer) << 32 | (uint32)Profiles[i].MinorVer;
					uint64 verJ = ((uint64)Profiles[j].MajorVer) << 32 | (uint32)Profiles[j].MinorVer;

					if (verI < verJ)
					{
						EffectProfileData temp = Profiles[i];
						Profiles[i] = Profiles[j];
						Profiles[j] = temp;

						temp.VSCode = temp.PSCode = temp.GSCode = nullptr;
					}
				}
			}
		}
	}
};