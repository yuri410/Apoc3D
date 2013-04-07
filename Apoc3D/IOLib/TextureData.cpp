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
#include "apoc3d/Vfs/ResourceLocation.h"

using namespace Apoc3D::Core;
using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace IO
	{
		static const String WidthTag = L"Width";
		static const String HeightTag = L"Height";
		static const String DepthTag = L"Depth";
		static const String ContentTag = L"Content";
		static const String LevelSizeTag = L"LevelSize";

		void TextureLevelData::LoadContentTo(void* dest, TaggedDataReader* data)
		{
			BinaryReader* br = data->GetData(ContentTag);

			int64 ret = br->ReadBytes(reinterpret_cast<char*>(dest), LevelSize);
			assert(ret == LevelSize);

			br->Close();
			delete br;
		}
		void TextureLevelData::LoadData(TaggedDataReader* data, bool doNotLoadContent)
		{
			Width = data->GetDataInt32(WidthTag);
			Height = data->GetDataInt32(HeightTag);
			Depth = data->GetDataInt32(DepthTag);
			LevelSize = data->GetDataInt32(LevelSizeTag);

			if (!doNotLoadContent)
			{
				BinaryReader* br = data->GetData(ContentTag);

				ContentData = new char[LevelSize];

				int64 ret = br->ReadBytes(ContentData, LevelSize);
				assert(ret == LevelSize);


				br->Close();
				delete br;
			}
		}
		void TextureLevelData::SaveData(TaggedDataWriter* data) const
		{
			data->AddEntryInt32(WidthTag, Width);
			data->AddEntryInt32(HeightTag, Height);
			data->AddEntryInt32(DepthTag, Depth);
			data->AddEntryInt32(LevelSizeTag, LevelSize);

			BinaryWriter* bw = data->AddEntry(ContentTag);
			bw->Write(ContentData, LevelSize);
			bw->Close();
			delete bw;
		}


		
		static const String TypeTag = L"Type";
		static const String FormatTag = L"Format";
		static const String ContentSizeTag = L"ContentSize";
		static const String LevelCountTag = L"LevelCount";
		static const String LevelTag = L"Level";


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

			bw->Write(FileID);

			TaggedDataWriter* data = new TaggedDataWriter(strm->IsWriteEndianDependent());

			data->AddEntryInt32(TypeTag, (int32)Type);
			data->AddEntryInt32(FormatTag, (int32)Format);
			data->AddEntryInt32(ContentSizeTag, ContentSize);
			data->AddEntryInt32(LevelCountTag, LevelCount);

			for (int i = 0; i < LevelCount; i++)
			{
				String levelName = LevelTag;
				const String temp = StringUtils::ToString(i);
				levelName.append(temp);

				BinaryWriter* bw2 = data->AddEntry(levelName);
				TaggedDataWriter* data2 = new TaggedDataWriter(strm->IsWriteEndianDependent());
				Levels[i].SaveData(data2);

				bw2->Write(data2);

				delete data2;
				bw2->Close();
				delete bw2;
			}

			bw->Write(data);
			delete data;

			bw->Close();
			delete bw;
		}

		void TextureData::LoadFromData(TaggedDataReader* data, bool doNotLoadLevel, bool doNotLoadLevelContent)
		{
			Type = static_cast<TextureType>(data->GetDataInt32(TypeTag));

			Format = static_cast<PixelFormat>(data->GetDataInt32(FormatTag));
			ContentSize = data->GetDataInt32(ContentSizeTag);
			LevelCount = data->GetDataInt32(LevelCountTag);

			if (!doNotLoadLevel)
			{
				for (int i = 0; i < LevelCount; i++)
				{
					String levelName = LevelTag;
					const String temp = StringUtils::ToString(i);
					levelName.append(temp);

					BinaryReader* br2 = data->GetData(levelName);
					TaggedDataReader* data2 = br2->ReadTaggedDataBlock();

					TextureLevelData lvl;
					lvl.LoadData(data2, doNotLoadLevelContent);
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
