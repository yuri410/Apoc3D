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
#include "apoc3d/Library/lz4.h"
#include "apoc3d/Library/lz4hc.h"

using namespace Apoc3D::Core;
using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace IO
	{
		const char Tag1_Width[] = "Width";
		const char Tag_Height[] = "Height";
		const char Tag_Depth[] = "Depth";
		const char Tag_Content[] = "Content";
		const char Tag_LevelSize[] = "LevelSize";
		
		// v1
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
					BufferedStreamReader bsr(strm);
					int32 ret = rleDecompress(ContentData, LevelSize, &bsr);
					assert(ret == LevelSize);
				}
				else
				{
					int64 ret = strm->Read(ContentData, LevelSize);
					assert(ret == LevelSize);
				}

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
			delete strm;
		}

		// v2
		void TextureLevelData::LoadData(BinaryReader& br, bool doNotLoadContent, int32 flags)
		{
			Width = br.ReadInt32();
			Height = br.ReadInt32();
			Depth = br.ReadInt32();
			LevelSize = br.ReadInt32();

			if (!doNotLoadContent)
			{
				Stream* strm = br.getBaseStream();

				ContentData = new char[LevelSize];
				if ((flags & TextureData::TDF_RLECompressed) == TextureData::TDF_RLECompressed)
				{
					BufferedStreamReader bsr(strm);
					int32 ret = rleDecompress(ContentData, LevelSize, &bsr);
					assert(ret == LevelSize);
				}
				else if ((flags & TextureData::TDF_LZ4Compressed) == TextureData::TDF_LZ4Compressed)
				{
					int32 comprsesedSize = br.ReadInt32();
					char* compressedData = new char[comprsesedSize];
					strm->Read(compressedData, comprsesedSize);
					int32 ret = LZ4_decompress_safe(compressedData, ContentData, comprsesedSize, LevelSize);
					assert(ret == LevelSize);
					delete[] compressedData;
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
		void TextureLevelData::SaveData(BinaryWriter& bw, int32 flags) const
		{
			bw.WriteInt32(Width);
			bw.WriteInt32(Height);
			bw.WriteInt32(Depth);
			bw.WriteInt32(LevelSize);

			Stream* strm = bw.getBaseStream();
			if ((flags & TextureData::TDF_RLECompressed) == TextureData::TDF_RLECompressed)
			{
				rleCompress(ContentData, LevelSize, strm);
			}
			else if ((flags & TextureData::TDF_LZ4Compressed) == TextureData::TDF_LZ4Compressed)
			{
				char* compressedData = new char[LZ4_COMPRESSBOUND(LevelSize)];
				int32 compressedSize = LZ4_compressHC2(ContentData, compressedData, LevelSize, 8);
				assert(compressedSize>0);
				bw.WriteInt32(compressedSize);
				strm->Write(compressedData, compressedSize);
				delete[] compressedData;
			}
			else
			{
				strm->Write(ContentData, LevelSize);
			}
		}

		
		const char Tag_Type[] = "Type";
		const char Tag_Format[] = "Format";
		const char Tag_ContentSize[] = "ContentSize";
		const char Tag_LevelCount[] = "LevelCount";
		const char Tag_Level[] = "Level";
		const char Tag_Flags[] = "Flags";

		void TextureData::Load(const ResourceLocation& rl, bool doNotLoadLevel, bool doNotLoadContent)
		{
			BinaryReader br(rl);

			int32 id = br.ReadInt32();
			if (id == FileID1)
			{
				TaggedDataReader* data = br.ReadTaggedDataBlock();

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
					for (int32 i = 0; i < LevelCount; i++)
					{
						std::string levelName = Tag_Level;
						levelName.append(StringUtils::IntToNarrowString(i));

						BinaryReader* br2 = data->GetData(levelName);
						TaggedDataReader* data2 = br2->ReadTaggedDataBlock();

						TextureLevelData lvl;
						lvl.LoadData(data2, doNotLoadContent, Flags);
						Levels.Add(lvl);

						data2->Close();

						delete data2;
						delete br2;
					}
				}

				data->Close();
				delete data;
			}
			else if (id == FileID2)
			{
				Type = static_cast<TextureType>(br.ReadInt32());
				Format = static_cast<PixelFormat>(br.ReadInt32());

				ContentSize = br.ReadInt32();
				LevelCount = br.ReadInt32();

				Flags = static_cast<TextureDataFlags>(br.ReadUInt32());

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
				LogManager::getSingleton().Write(LOG_Graphics, L"Invalid texture file. " + rl.getName(), LOGLVL_Error);
			}
		}
		void TextureData::Save(Stream& strm) const
		{
			BinaryWriter bw(&strm, false);
			bw.WriteInt32(FileID2);

			bw.WriteInt32(static_cast<int32>(Type));
			bw.WriteInt32(static_cast<int32>(Format));

			bw.WriteInt32(ContentSize);
			bw.WriteInt32(LevelCount);

			bw.WriteInt32(static_cast<int32>(Flags));

			for (int i = 0; i < LevelCount; i++)
			{
				Levels[i].SaveData(bw, Flags);
			}

		}
		void TextureData::SaveAsTagged(Stream& strm) const
		{
			BinaryWriter bw(&strm, false);

			bw.WriteInt32(FileID1);

			TaggedDataWriter* data = new TaggedDataWriter(strm.IsWriteEndianIndependent());

			data->AddEntryInt32(Tag_Type, (int32)Type);
			data->AddEntryInt32(Tag_Format, (int32)Format);
			data->AddEntryInt32(Tag_ContentSize, ContentSize);
			data->AddEntryInt32(Tag_LevelCount, LevelCount);
			data->AddEntryInt32(Tag_Flags, Flags);

			for (int32 i = 0; i < LevelCount; i++)
			{
				std::string levelName = Tag_Level;
				levelName.append(StringUtils::IntToNarrowString(i));

				BinaryWriter* bw2 = data->AddEntry(levelName);
				TaggedDataWriter* data2 = new TaggedDataWriter(strm.IsWriteEndianIndependent());
				Levels[i].SaveData(data2, Flags);

				bw2->WriteTaggedDataBlock(data2);

				delete data2;
				delete bw2;
			}

			bw.WriteTaggedDataBlock(data);
			delete data;

		}

		void TextureData::Free()
		{
			for (int i = 0; i < LevelCount; i++)
			{
				delete[] Levels[i].ContentData;
			}
			Levels.Clear();
		}
	}
}
