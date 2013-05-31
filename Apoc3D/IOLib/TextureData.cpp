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

#include "TextureData.h"

#include "BinaryReader.h"
#include "BinaryWriter.h"
#include "TaggedData.h"
#include "Streams.h"
#include "apoc3d/Core/Logging.h"
#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/Utility/Compression.h"
#include "apoc3d/Vfs/ResourceLocation.h"

using namespace Apoc3D::Core;
using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace IO
	{
		static const String Tag1_Width = L"Width";
		static const String Tag_Height = L"Height";
		static const String Tag_Depth = L"Depth";
		static const String Tag_Content = L"Content";
		static const String Tag_LevelSize = L"LevelSize";
		
		void TextureLevelData::LoadContentTo(void* dest, TaggedDataReader* data)
		{
			BinaryReader* br = data->GetData(Tag_Content);

			int64 ret = br->ReadBytes(reinterpret_cast<char*>(dest), LevelSize);
			assert(ret == LevelSize);

			br->Close();
			delete br;
		}
		void TextureLevelData::LoadData(TaggedDataReader* data, bool doNotLoadContent, int32 flags)
		{
			Width = data->GetDataInt32(Tag1_Width);
			Height = data->GetDataInt32(Tag_Height);
			Depth = data->GetDataInt32(Tag_Depth);
			LevelSize = data->GetDataInt32(Tag_LevelSize);

			if (!doNotLoadContent)
			{
				Stream* strm = data->GetDataStream(Tag_Content);

				ContentData = new char[LevelSize];

				if ((flags & TextureData::TDF_RLECompressed) == TextureData::TDF_RLECompressed)
				{
					int32 ret = rleDecompress(ContentData, LevelSize, strm);
					assert(ret == LevelSize);
				}
				else
				{
					int64 ret = strm->Read(ContentData, LevelSize);
					assert(ret == LevelSize);
				}

				strm->Close();
				delete strm;
			}
		}
		void TextureLevelData::SaveData(TaggedDataWriter* data, int32 flags) const
		{
			data->AddEntryInt32(Tag1_Width, Width);
			data->AddEntryInt32(Tag_Height, Height);
			data->AddEntryInt32(Tag_Depth, Depth);
			data->AddEntryInt32(Tag_LevelSize, LevelSize);

			Stream* strm = data->AddEntryStream(Tag_Content);
			if ((flags & TextureData::TDF_RLECompressed) == TextureData::TDF_RLECompressed)
			{
				rleCompress(ContentData, LevelSize, strm);
			}
			else
			{
				strm->Write(ContentData, LevelSize);
			}
			strm->Close();
			delete strm;
		}


		
		static const String Tag_Type = L"Type";
		static const String Tag_Format = L"Format";
		static const String Tag_ContentSize = L"ContentSize";
		static const String Tag_LevelCount = L"LevelCount";
		static const String Tag_Level = L"Level";
		static const String Tag_Flags = L"Flags";

		void TextureData::Load(const ResourceLocation* rl)
		{
			BinaryReader* br = new BinaryReader(rl);

			if (br->ReadInt32() == FileID)
			{
				TaggedDataReader* data = br->ReadTaggedDataBlock();

				LoadFromData(data, false, false);

				data->Close();
				delete data;
			}
			else
			{
				LogManager::getSingleton().Write(LOG_Graphics, L"Invalid texture file. " + rl->getName(), LOGLVL_Error);
			}
			br->Close();

			delete br;
		}
		void TextureData::Save(Stream* strm) const
		{
			BinaryWriter* bw = new BinaryWriter(strm);

			bw->WriteInt32(FileID);

			TaggedDataWriter* data = new TaggedDataWriter(strm->IsWriteEndianDependent());

			data->AddEntryInt32(Tag_Type, (int32)Type);
			data->AddEntryInt32(Tag_Format, (int32)Format);
			data->AddEntryInt32(Tag_ContentSize, ContentSize);
			data->AddEntryInt32(Tag_LevelCount, LevelCount);
			data->AddEntryInt32(Tag_Flags, Flags);

			for (int i = 0; i < LevelCount; i++)
			{
				String levelName = Tag_Level;
				const String temp = StringUtils::ToString(i);
				levelName.append(temp);

				BinaryWriter* bw2 = data->AddEntry(levelName);
				TaggedDataWriter* data2 = new TaggedDataWriter(strm->IsWriteEndianDependent());
				Levels[i].SaveData(data2, Flags);

				bw2->WriteTaggedDataBlock(data2);

				delete data2;
				bw2->Close();
				delete bw2;
			}

			bw->WriteTaggedDataBlock(data);
			delete data;

			bw->Close();
			delete bw;
		}

		void TextureData::LoadFromData(TaggedDataReader* data, bool doNotLoadLevel, bool doNotLoadLevelContent)
		{
			Type = static_cast<TextureType>(data->GetDataInt32(Tag_Type));

			Format = static_cast<PixelFormat>(data->GetDataInt32(Tag_Format));
			ContentSize = data->GetDataInt32(Tag_ContentSize);
			LevelCount = data->GetDataInt32(Tag_LevelCount);

			if (!data->TryGetDataUInt32(Tag_Flags, Flags))
			{
				Flags = TDF_None;
			}

			if (!doNotLoadLevel)
			{
				for (int i = 0; i < LevelCount; i++)
				{
					String levelName = Tag_Level;
					const String temp = StringUtils::ToString(i);
					levelName.append(temp);

					BinaryReader* br2 = data->GetData(levelName);
					TaggedDataReader* data2 = br2->ReadTaggedDataBlock();

					TextureLevelData lvl;
					lvl.LoadData(data2, doNotLoadLevelContent, Flags);
					Levels.Add(lvl);

					data2->Close();
					br2->Close();

					delete data2;
					delete br2;
				}
			}
		}

	}
}
