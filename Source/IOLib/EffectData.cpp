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

#include "EffectData.h"
#include "IOLib/BinaryReader.h"
#include "IOLib/BinaryWriter.h"
#include "IOLib/Streams.h"
#include "IOLib/TaggedData.h"
#include "Vfs/ResourceLocation.h"
#include "Core/Logging.h"
#include "Utility/StringUtils.h"

using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace IO
	{
		static const int AfxId_V3 = ((byte)'A' << 24) | ((byte)'F' << 16) | ((byte)'X' << 8) | ((byte)' ');

		static const String TAG_3_ParameterCountTag = L"ParameterCount";
		static const String TAG_3_ParameterTag = L"Parameter";
		static const String TAG_3_ShaderCodeTag = L"ShaderCode";

		static const String TAG_3_ShaderCodeLengthTag = L"ShaderCodeLength";

		void EffectData::Load(const ResourceLocation* rl)
		{
			BinaryReader* br = new BinaryReader(rl->GetReadStream());

			int id = br->ReadInt32();

			if (id == AfxId_V3)
			{
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
					params.TypicalUsage = static_cast<EffectParamUsage>(br2->ReadUInt32());
					params.IsCustomUsage = br->ReadBoolean();
					params.CustomUsage = br->ReadString();
					params.ProgramType = static_cast<ShaderType>(br->ReadInt32());
					Parameters.Add(name);

					br2->Close();
					delete br2;
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
			else
			{
				LogManager::getSingleton().Write(LOG_Graphics, L"Invalid model file. " + rl->getName(), LOGLVL_Error);
			}
			

			br->Close();
			delete br;
		}
		void EffectData::Save(Stream* strm) const
		{
			BinaryWriter* bw = new BinaryWriter(strm);
			bw->Write(AfxId_V3);

			{
				TaggedDataWriter* data = new TaggedDataWriter(strm->IsWriteEndianDependent());

				data->AddEntry(TAG_3_ParameterCountTag, Parameters.getCount());


				for (int i=0;i<Parameters.getCount();i++)
				{
					String tag = StringUtils::ToString(i);

					tag = TAG_3_ParameterTag + tag;

					BinaryWriter* bw2 = data->AddEntry(tag);

					bw2->Write(Parameters[i].Name);
					bw2->Write(static_cast<uint>(Parameters[i].TypicalUsage));
					bw2->Write(Parameters[i].IsCustomUsage);
					bw2->Write(Parameters[i].CustomUsage);
					bw2->Write(static_cast<int>(Parameters[i].ProgramType));

					bw2->Close();
					delete bw2;
				}

				BinaryWriter* bw3 = data->AddEntry(TAG_3_ShaderCodeLengthTag);
				bw3->Write((int32)VSLength);
				bw3->Write((int32)PSLength);
				bw3->Close();
				delete bw3;


				bw3 = data->AddEntry(TAG_3_ShaderCodeTag);
				bw->Write(VSCode, VSLength);
				bw->Write(PSCode, PSLength);
				bw3->Close();
				delete bw3;



				bw->Write(data);
				delete data;
			}

			bw->Close();
			delete bw;
		}

	}
};