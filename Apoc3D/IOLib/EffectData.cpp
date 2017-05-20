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

#include "EffectData.h"
#include "apoc3d/IOLib/BinaryReader.h"
#include "apoc3d/IOLib/BinaryWriter.h"
#include "apoc3d/IOLib/Streams.h"
#include "apoc3d/IOLib/TaggedData.h"
#include "apoc3d/Vfs/ResourceLocation.h"
#include "apoc3d/Core/Logging.h"
#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/Math/Math.h"

using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace IO
	{
		const char* EffectProfileData::Imp_HLSL = "hlsl";
		const char* EffectProfileData::Imp_GLSL = "glsl";

		EffectProfileData::EffectProfileData()
		{
			ZeroArray(ImplementationType);
		}
		EffectProfileData::~EffectProfileData()
		{
			{ delete[] VSCode; VSCode = nullptr; }
			{ delete[] PSCode; PSCode = nullptr; }
			{ delete[] GSCode; GSCode = nullptr; }
		}

		EffectProfileData::EffectProfileData(EffectProfileData&& other)
			: MajorVer(other.MajorVer), MinorVer(other.MinorVer), 
			VSLength(other.VSLength), PSLength(other.PSLength), GSLength(other.GSLength),
			VSCode(other.VSCode), PSCode(other.PSCode), GSCode(other.GSCode),
			Parameters(std::move(other.Parameters))
		{
			CopyArray(ImplementationType, other.ImplementationType);

			other.VSCode = nullptr;
			other.PSCode = nullptr;
			other.GSCode = nullptr;
		}
		EffectProfileData& EffectProfileData::operator=(EffectProfileData&& other)
		{
			if (this != &other)
			{
				delete[] VSCode;
				delete[] PSCode;
				delete[] GSCode;

				CopyArray(ImplementationType, other.ImplementationType);

				MajorVer = other.MajorVer;
				MinorVer = other.MinorVer;

				VSLength = other.VSLength;
				PSLength = other.PSLength;
				GSLength = other.GSLength;

				VSCode = other.VSCode;
				PSCode = other.PSCode;
				GSCode = other.GSCode;

				other.VSCode = nullptr;
				other.PSCode = nullptr;
				other.GSCode = nullptr;

				Parameters = std::move(other.Parameters);
			}
			return *this;
		}


		struct EffectProfileCodeField
		{
			int32 ID;
			int32 EffectProfileData::* CodeLength;
			char* EffectProfileData::* Code;
		};

		const EffectProfileCodeField EffectProfileCodeMetaData[] =
		{
			{ 'VS', &EffectProfileData::VSLength, &EffectProfileData::VSCode },
			{ 'PS', &EffectProfileData::PSLength, &EffectProfileData::PSCode },
			{ 'GS', &EffectProfileData::GSLength, &EffectProfileData::GSCode }
		};

		void EffectProfileData::LoadV5(const String& name, BinaryReader* br)
		{
			br->ReadBytes(ImplementationType, sizeof(ImplementationType));

			MajorVer = br->ReadInt32();
			MinorVer = br->ReadInt32();

			int32 codeCount = br->ReadInt32();
			for (int32 i = 0; i < codeCount; i++)
			{
				int32 id = br->ReadInt32();

				const EffectProfileCodeField* fld = nullptr;
				for (const EffectProfileCodeField& f : EffectProfileCodeMetaData)
				{
					if (f.ID == id)
					{
						fld = &f;
						break;
					}
				}

				if (fld)
				{
					br->ReadInt32(); // preserved

					int32& len = this->*fld->CodeLength;
					char*& codeData = this->*fld->Code;
					len = br->ReadInt32();

					codeData = new char[len];

					br->ReadBytes(codeData, len);
				}
				else
				{
					String codeName;

					for (int32 j = 0; j < 4;j++)
					{
						char c = (char)((id >> (24 - j * 8) & 0xff));
						if (c) codeName.append(1, c);
					}
					
					ApocLog(LOG_Graphics, L"[EffectData][" + name + L"] Unsupported shader code type: " + codeName, LOGLVL_Warning);
				}
			}

			int paramCount = br->ReadInt32();
			Parameters.ReserveDiscard(paramCount);
			for (EffectParameter& fxParam : Parameters)
			{
				fxParam.Read(br);
			}
		}
		void EffectProfileData::SaveV5(BinaryWriter* bw)
		{
			bw->WriteBytes(ImplementationType, sizeof(ImplementationType));

			bw->WriteInt32(MajorVer);
			bw->WriteInt32(MinorVer);

			int32 codeCount = 0;

			for (const EffectProfileCodeField& f : EffectProfileCodeMetaData)
			{
				char* codeData = this->*f.Code;

				if (codeData)
					codeCount++;
			}

			bw->WriteInt32(codeCount);

			for (const EffectProfileCodeField& f : EffectProfileCodeMetaData)
			{
				int32& len = this->*f.CodeLength;
				char*& codeData = this->*f.Code;

				if (codeData)
				{
					codeCount++;
					bw->WriteInt32(f.ID);
					bw->WriteInt32(0);

					bw->WriteInt32(len);
					bw->WriteBytes(codeData, len);
				}
			}


			bw->WriteInt32(Parameters.getCount());
			for (EffectParameter& fxParam : Parameters)
			{
				fxParam.Write(bw);
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

		const uint32 AfxId_V5 = FourCC("AFX2");
		const uint32 CfxID_V5 = FourCC("CFX2");

		constexpr TaggedDataKey TAG_3_ParameterCountTag = "ParameterCount";
		constexpr TaggedDataKey TAG_3_ParameterTag = "Parameter";
		constexpr TaggedDataKey TAG_3_ParameterSamplerStateTag = "SamplerState";
		constexpr TaggedDataKey TAG_3_ShaderCodeTag = "ShaderCode";
		
		constexpr TaggedDataKey TAG_3_ShaderCodeLengthTag = "ShaderCodeLength";
		
		EffectData::EffectData() 
		{
		}
		EffectData::~EffectData() 
		{
		}


		void EffectData::Load(const ResourceLocation& rl)
		{
			BinaryReader br(rl);

			uint32 id = br.ReadUInt32();

			if (id == AfxId_V5)
			{
				LoadFXV5(&br);
			}
			else if (id == CfxID_V5)
			{
				LoadFXV5(&br);
				IsCFX = true;
			}
			else
			{
				LogManager::getSingleton().Write(LOG_Graphics, L"Invalid effect file. " + rl.getName(), LOGLVL_Error);
			}
			
		}

		void EffectData::LoadFXV5(BinaryReader* br)
		{
			Name = br->ReadString();
			int32 profileCount = br->ReadInt32();
			Profiles.ReserveDiscard(profileCount);
			for (EffectProfileData& proData : Profiles)
			{
				proData.LoadV5(Name, br);
			}
		}

		void EffectData::Save(Stream& strm) const
		{
			BinaryWriter bw(&strm, false);

			bw.WriteUInt32(IsCFX ? CfxID_V5 : AfxId_V5);

			bw.WriteString(Name);
			bw.WriteInt32(Profiles.getCount());
			for (EffectProfileData& proData : Profiles)
			{
				proData.SaveV5(&bw);
			}
		}
		

		void EffectData::SortProfiles()
		{
			Profiles.SortWithSorter<int64>([](const EffectProfileData& o)->int64
			{
				return ((int64)o.MajorVer) << 32 | (int32)o.MinorVer;
			});
		}
	}
};