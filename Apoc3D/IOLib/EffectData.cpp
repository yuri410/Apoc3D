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

using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace IO
	{
		static const int AfxId_V3 = ((byte)'A' << 24) | ((byte)'F' << 16) | ((byte)'X' << 8) | ((byte)' ');
		static const int AfxId_V31 = ((byte)'A' << 24) | ((byte)'F' << 16) | ((byte)'X' << 8) | ((byte)'0');

		static const int CfxID_V3 = ((byte)'C' << 24) | ((byte)'F' << 16) | ((byte)'X' << 8) | ((byte)' ');

		static const String TAG_3_ParameterCountTag = L"ParameterCount";
		static const String TAG_3_ParameterTag = L"Parameter";
		static const String TAG_3_ParameterSamplerStateTag = L"SamplerState";
		static const String TAG_3_ShaderCodeTag = L"ShaderCode";
		
		static const String TAG_3_ShaderCodeLengthTag = L"ShaderCodeLength";
		
		void EffectData::Load(const ResourceLocation* rl)
		{
			BinaryReader* br = new BinaryReader(rl->GetReadStream());

			int id = br->ReadInt32();

			if (id == AfxId_V3)
			{
				LoadAFXV3(br);
			}
			else if (id == AfxId_V31)
			{
				LoadAFXV3_1(br);
			}
			else if (id == CfxID_V3)
			{
				IsCFX = true;

				MajorVer = br->ReadInt32();
				MinorVer = br->ReadInt32();
				Name = br->ReadString();

				TaggedDataReader* data = br->ReadTaggedDataBlock();

				BinaryReader* br3 = data->GetData(TAG_3_ShaderCodeLengthTag);
				VSLength = br3->ReadInt32();
				PSLength = br3->ReadInt32();
				br3->Close();
				delete br3;

				VSCode = new char[VSLength];
				PSCode = new char[PSLength];
				br3 = data->GetData(TAG_3_ShaderCodeTag);
				br3->ReadBytes(VSCode, VSLength);
				br3->ReadBytes(PSCode, PSLength);
				br3->Close();
				delete br3;

				data->Close();
				delete data;
			}
			else
			{
				LogManager::getSingleton().Write(LOG_Graphics, L"Invalid effect file. " + rl->getName(), LOGLVL_Error);
			}
			

			br->Close();
			delete br;
		}

		void EffectData::LoadAFXV3(BinaryReader* br)
		{
			MajorVer = br->ReadInt32();
			MinorVer = br->ReadInt32();
			Name = br->ReadString();

			TaggedDataReader* data = br->ReadTaggedDataBlock();

			int count = data->GetDataInt32(TAG_3_ParameterCountTag);
			Parameters.ResizeDiscard(count);

			for (int i=0;i<count;i++)
			{
				String tag = StringUtils::ToString(i);

				tag = TAG_3_ParameterTag + tag;

				BinaryReader* br2 = data->GetData(tag);

				// load parameter
				String name = br2->ReadString();

				EffectParameter params(name);
				//params.TypicalUsage = static_cast<EffectParamUsage>(br2->ReadUInt32());
				//params.IsCustomUsage = br->ReadBoolean();
				params.CustomMaterialParamName = br->ReadString();
				params.ProgramType = static_cast<ShaderType>(br->ReadInt32());

				params.Usage = EffectParameter::ParseParamUsage(params.CustomMaterialParamName);
				if (params.Usage == EPUSAGE_Unknown)
				{
					params.Usage = EPUSAGE_CustomMaterialParam;
				}
				//params.IsCustomUsage = params.Usage == EPUSAGE_Unknown;

				br2->Close();
				delete br2;

				tag = StringUtils::ToString(i);
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
					params.SamplerState.MipMapLODBias = br2->ReadUInt32();

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

				Parameters.Add(params);

			}


			BinaryReader* br3 = data->GetData(TAG_3_ShaderCodeLengthTag);
			VSLength = br3->ReadInt32();
			PSLength = br3->ReadInt32();
			br3->Close();
			delete br3;

			VSCode = new char[VSLength];
			PSCode = new char[PSLength];
			br3 = data->GetData(TAG_3_ShaderCodeTag);
			br3->ReadBytes(VSCode, VSLength);
			br3->ReadBytes(PSCode, PSLength);
			br3->Close();
			delete br3;

			data->Close();
			delete data;

		}
		void EffectData::LoadAFXV3_1(BinaryReader* br)
		{
			MajorVer = br->ReadInt32();
			MinorVer = br->ReadInt32();
			Name = br->ReadString();

			TaggedDataReader* data = br->ReadTaggedDataBlock();

			int count = data->GetDataInt32(TAG_3_ParameterCountTag);
			Parameters.ResizeDiscard(count);

			for (int i=0;i<count;i++)
			{
				String tag = StringUtils::ToString(i);

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

				tag = StringUtils::ToString(i);
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
					params.SamplerState.MipMapLODBias = br2->ReadUInt32();

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

				Parameters.Add(params);

			}


			BinaryReader* br3 = data->GetData(TAG_3_ShaderCodeLengthTag);
			VSLength = br3->ReadInt32();
			PSLength = br3->ReadInt32();
			br3->Close();
			delete br3;

			VSCode = new char[VSLength];
			PSCode = new char[PSLength];
			br3 = data->GetData(TAG_3_ShaderCodeTag);
			br3->ReadBytes(VSCode, VSLength);
			br3->ReadBytes(PSCode, PSLength);
			br3->Close();
			delete br3;

			data->Close();
			delete data;

		}

		void EffectData::Save(Stream* strm) const
		{
			if (IsCFX)
			{
				BinaryWriter* bw = new BinaryWriter(strm);
				bw->WriteInt32((int32)CfxID_V3);
				bw->WriteInt32((int32)MajorVer);
				bw->WriteInt32((int32)MinorVer);
				bw->WriteString(Name);
				{
					TaggedDataWriter* data = new TaggedDataWriter(strm->IsWriteEndianDependent());

					BinaryWriter* bw3 = data->AddEntry(TAG_3_ShaderCodeLengthTag);
					bw3->WriteInt32((int32)VSLength);
					bw3->WriteInt32((int32)PSLength);
					bw3->Close();
					delete bw3;


					bw3 = data->AddEntry(TAG_3_ShaderCodeTag);
					bw3->Write(VSCode, VSLength);
					bw3->Write(PSCode, PSLength);
					bw3->Close();
					delete bw3;



					bw->WriteTaggedDataBlock(data);
					delete data;
				}

				bw->Close();
				delete bw;
			}
			else
			{
				BinaryWriter* bw = new BinaryWriter(strm);
				bw->WriteInt32((int32)AfxId_V31);
				bw->WriteInt32((int32)MajorVer);
				bw->WriteInt32((int32)MinorVer);
				bw->WriteString(Name);
				{
					TaggedDataWriter* data = new TaggedDataWriter(strm->IsWriteEndianDependent());

					data->AddEntryInt32(TAG_3_ParameterCountTag, (int32)Parameters.getCount());


					for (int i=0;i<Parameters.getCount();i++)
					{
						const EffectParameter& pm = Parameters[i];

						String tag = StringUtils::ToString(i);
						tag = TAG_3_ParameterTag + tag;

						BinaryWriter* bw2 = data->AddEntry(tag);

						bw2->WriteString(pm.Name);

						bw2->WriteString(EffectParameter::ToString(pm.Usage));
						bw2->WriteString(pm.CustomMaterialParamName);
						bw2->WriteInt32(pm.InstanceBlobIndex);
						bw2->WriteInt32(static_cast<int>(pm.ProgramType));

						bw2->Close();
						delete bw2;

						tag = StringUtils::ToString(i);
						tag = TAG_3_ParameterSamplerStateTag + tag;
						bw2 = data->AddEntry(tag);
						bw2->WriteUInt32((uint)pm.SamplerState.AddressU);
						bw2->WriteUInt32((uint)pm.SamplerState.AddressV);
						bw2->WriteUInt32((uint)pm.SamplerState.AddressW);
						bw2->WriteUInt32((uint)pm.SamplerState.BorderColor);
						bw2->WriteUInt32((uint)pm.SamplerState.MagFilter);
						bw2->WriteInt32((int)pm.SamplerState.MaxAnisotropy);
						bw2->WriteInt32((int)pm.SamplerState.MaxMipLevel);
						bw2->WriteUInt32((uint)pm.SamplerState.MinFilter);
						bw2->WriteUInt32((uint)pm.SamplerState.MipFilter);
						bw2->WriteUInt32((uint)pm.SamplerState.MipMapLODBias);

						bw2->Close();
						delete bw2;
					}

					BinaryWriter* bw3 = data->AddEntry(TAG_3_ShaderCodeLengthTag);
					bw3->WriteInt32((int32)VSLength);
					bw3->WriteInt32((int32)PSLength);
					bw3->Close();
					delete bw3;


					bw3 = data->AddEntry(TAG_3_ShaderCodeTag);
					bw3->Write(VSCode, VSLength);
					bw3->Write(PSCode, PSLength);
					bw3->Close();
					delete bw3;



					bw->WriteTaggedDataBlock(data);
					delete data;
				}

				bw->Close();
				delete bw;
			}
			
		}

	}
};