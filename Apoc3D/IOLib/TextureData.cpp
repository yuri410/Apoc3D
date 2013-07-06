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
		const String Tag1_Width = L"Width";
		const String Tag_Height = L"Height";
		const String Tag_Depth = L"Depth";
		const String Tag_Content = L"Content";
		const String Tag_LevelSize = L"LevelSize";
		
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
			else
			{
				ContentData = nullptr;
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

		void TextureLevelData::LoadData(BinaryReader* br, bool doNotLoadContent, int32 flags)
		{
			Width = br->ReadInt32();
			Height = br->ReadInt32();
			Depth = br->ReadInt32();
			LevelSize = br->ReadInt32();

			if (!doNotLoadContent)
			{
				Stream* strm = br->getBaseStream();

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
			}
			else
			{
				ContentData = nullptr;
			}
		}
		void TextureLevelData::SaveData(BinaryWriter* bw, int32 flags) const
		{
			bw->WriteInt32(Width);
			bw->WriteInt32(Height);
			bw->WriteInt32(Depth);
			bw->WriteInt32(LevelSize);

			Stream* strm = bw->getBaseStream();
			if ((flags & TextureData::TDF_RLECompressed) == TextureData::TDF_RLECompressed)
			{
				rleCompress(ContentData, LevelSize, strm);
			}
			else
			{
				strm->Write(ContentData, LevelSize);
			}
		}

		
		const String Tag_Type = L"Type";
		const String Tag_Format = L"Format";
		const String Tag_ContentSize = L"ContentSize";
		const String Tag_LevelCount = L"LevelCount";
		const String Tag_Level = L"Level";
		const String Tag_Flags = L"Flags";

		void TextureData::Load(const ResourceLocation* rl, bool doNotLoadLevel, bool doNotLoadContent)
		{
			BinaryReader* br = new BinaryReader(rl);

			int32 id = br->ReadInt32();
			if (id == FileID1)
			{
				TaggedDataReader* data = br->ReadTaggedDataBlock();

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
						lvl.LoadData(data2, doNotLoadContent, Flags);
						Levels.Add(lvl);

						data2->Close();
						br2->Close();

						delete data2;
						delete br2;
					}
				}

				data->Close();
				delete data;
			}
			else if (id == FileID2)
			{
				Type = static_cast<TextureType>(br->ReadInt32());
				Format = static_cast<PixelFormat>(br->ReadInt32());

				ContentSize = br->ReadInt32();
				LevelCount = br->ReadInt32();

				Flags = static_cast<TextureDataFlags>(br->ReadUInt32());

				if (!doNotLoadLevel)
				{
					Levels.ReserveDiscard(LevelCount);
					for (int i = 0; i < LevelCount; i++)
					{
						Levels[i].LoadData(br, doNotLoadContent, Flags);
					}
				}
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
			bw->WriteInt32(FileID2);

			bw->WriteInt32(static_cast<int32>(Type));
			bw->WriteInt32(static_cast<int32>(Format));

			bw->WriteInt32(ContentSize);
			bw->WriteInt32(LevelCount);

			bw->WriteInt32(static_cast<int32>(Flags));

			for (int i = 0; i < LevelCount; i++)
			{
				Levels[i].SaveData(bw, Flags);
			}

			bw->Close();
			delete bw;
		}
		void TextureData::SaveAsTagged(Stream* strm) const
		{
			BinaryWriter* bw = new BinaryWriter(strm);
			bw->WriteInt32(FileID1);

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
	}
}
