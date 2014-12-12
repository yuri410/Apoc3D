#include "StringTable.h"

#include "apoc3d/Math/MathCommon.h"

#include "apoc3d/IOLib/BinaryReader.h"
#include "apoc3d/IOLib/BinaryWriter.h"
#include "apoc3d/IOLib/Streams.h"
#include "apoc3d/VFS/ResourceLocation.h"
#include "apoc3d/Utility/StringUtils.h"


namespace Apoc3D
{
	namespace Utility
	{
		StringTableEntry::StringTableEntry() 
		{
			ZeroArray(Extra);
		}

		StringTableEntry::StringTableEntry(const String& txt, const std::string& extra)
			: Text(txt)
		{
			ZeroArray(Extra);

			size_t c = Math::_Min<size_t>(extra.size(), countof(Extra));
			memcpy(Extra, extra.c_str(), c * sizeof(char));
		}

		//////////////////////////////////////////////////////////////////////////

		String StringTable::GetString(const std::string& name) const
		{
			const StringTableEntry* e = m_entryTable.TryGetValue(name);
			if (e)
			{
				return e->Text;
			}
			return L"MISSING: " + StringUtils::toASCIIWideString(name);
		}

		void StringTable::Load(StringTableFormat* fmt, const ResourceLocation& rl)
		{
			Stream* strm = rl.GetReadStream();
			fmt->Read(*strm, m_entryTable);
			delete strm;
		}
		void StringTable::Load(StringTableFormat* fmt, Stream& strm)
		{
			fmt->Read(strm, m_entryTable);
		}

		void StringTable::Save(StringTableFormat* fmt, Stream& strm)
		{
			fmt->Write(m_entryTable, strm);
		}

		//////////////////////////////////////////////////////////////////////////

		const int32 CSFFileID = 'CSF ';
		const int32 CSFLabelID = 'LBL ';
		const int32 CSFStringID = 'STR ';
		const int32 CSFWStringID = 'STRW';

		void CsfStringTableFormat::Read(Stream& strm, StringTableMap& map)
		{
			BinaryReader br(&strm, false);

			int32 id = br.ReadInt32();

			if (id == CSFFileID)
			{
				int32 version = br.ReadInt32();
				int32 count1 = br.ReadInt32();
				int32 count2 = br.ReadInt32();
				int32 preserved1 = br.ReadInt32();
				int32 preserved2 = br.ReadUInt32();

				map.Resize(count1);

				for (int32 i = 0; i < count1; i++)
				{
					id = br.ReadInt32();

					if (id == CSFLabelID)
					{
						int32 flag = br.ReadInt32();

						std::string lblName = br.ReadMBString();
						
						if ((flag & 1) != 0)
						{
							id = br.ReadInt32();

							int32 len = br.ReadInt32();
							String content(len, ' ');
							for (wchar_t& ch : content)
								ch = ~br.ReadInt16();

							std::string extra;
							if (id == CSFWStringID)
								extra = br.ReadMBString();

							map.Add(lblName, { content, extra });
						}
						else
						{
							map.Add(lblName, {});
						}
					}
				}
			}
		}
		void CsfStringTableFormat::Write(StringTableMap& map, Stream& strm)
		{
			BinaryWriter bw(&strm, false);

			bw.WriteInt32(CSFFileID);

			bw.WriteInt32(3);
			bw.WriteInt32(map.getCount());
			bw.WriteInt32(map.getCount());
			bw.WriteInt32(0);
			bw.WriteInt32(0);

			for (auto e : map)
			{
				const StringTableEntry& ent = e.Value;
				const std::string& key = e.Key;

				bw.WriteInt32(CSFLabelID);
				bw.WriteInt32(1);

				bw.WriteMBString(key);


				bool hasExtra = strlen(ent.Extra)>0;
				bw.WriteInt32(hasExtra ? CSFWStringID : CSFStringID);
				bw.WriteInt32((int32)ent.Text.size());
				for (wchar_t ch : ent.Text)
					bw.WriteUInt16(ch);

				if (hasExtra)
				{
					bw.WriteMBString(ent.Extra);
				}
			}
		}

		//////////////////////////////////////////////////////////////////////////


	}
}