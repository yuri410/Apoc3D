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

			// rules
			// %[flags][width][.precision][:fill]type

			// flags:
			//  - left align
			//  + show pos sign
			//  # show base
			//  ^ upper case

			// type
			//  [any]: item
			//  e: scientific
			//  h: hex
			//  p: percentage

			String buf;
			for (size_t i = 0; i < txt.size(); i++)
			{
				wchar_t ch = txt[i];
				if (ch == '%')
				{
					bool isNewPart = false;
					bool onFlags = true;
					bool onWidth = false;
					bool onPrecision = false;
					bool onFill = false;

					wchar_t fillChar = ' ';
					int32 width = 0;
					int32 precision = 0;

					uint64 formatFlag = 0;

					bool hasWidth = false;
					bool hasFill = false;
					bool hasPrecision = false;

					wchar_t typeChar = 0;

					// read format sequence
					for (size_t j = i + 1; j < txt.size(); j++)
					{
						wchar_t fch = txt[j];

						if (isspace(fch) || fch == '%')
						{
							if (j == i + 1)
								buf.append(1, '%');

							i = j - 1;
							break;
						}
						else
						{
							isNewPart = true;

							bool isDig = isdigit(fch) != 0;

							if (onFlags)
							{
								if (isDig)
								{
									onFlags = false;
									onWidth = true;
								}
								else if (fch == '-')
									formatFlag |= StringUtils::SF_Left;
								else if (fch == '+')
									formatFlag |= StringUtils::SF_ShowPositiveSign;
								else if (fch == '#')
									formatFlag |= StringUtils::SF_ShowHexBase;
								else if (fch == '^')
									formatFlag |= StringUtils::SF_UpperCase;
							}
							else if (onWidth)
							{
								if (isDig)
								{
									width *= 10;
									width += fch - '0';

									hasWidth = true;
								}
								else
								{
									onWidth = false;
								}
							}
							else if (onPrecision)
							{
								if (isDig)
								{
									precision *= 10;
									precision += fch - '0';

									hasPrecision = true;
								}
								else
								{
									onPrecision = false;
								}
							}
							else if (onFill)
							{
								fillChar = fch;
								// only one char needed
								onFill = false;
								hasFill = true;
							}
							else
							{
								typeChar = fch;
							}

							if (fch == ':')
							{
								onFlags = false;
								onFill = true;
								onPrecision = false;
								onWidth = false;
							}
							else if (fch == '.')
							{
								onPrecision = true;
								onFlags = false;
								onFill = false;
								onWidth = false;
							}
						}
					}
					
					if (isNewPart)
					{
						Parts.Add(PreprocessedParts(buf));

						if (hasWidth && hasFill)
							formatFlag |= StrFmt::A(width, fillChar);
						else if (hasWidth)
							formatFlag |= StrFmt::A(width);
						else if (hasFill)
							formatFlag |= StrFmt::A(fillChar);

						if (hasPrecision)
							formatFlag |= StrFmt::FP(precision);

						if (typeChar == 'e')
							formatFlag |= StringUtils::SF_FPScientific;
						else if (hasPrecision)
							formatFlag |= StringUtils::SF_FPDecimal;

						Parts.Add(PreprocessedParts(formatFlag, typeChar == 'p', typeChar == 'h'));

						buf.clear();
					}
				}
				else
				{
					buf.append(1, ch);
				}
			}

			if (buf.size())
			{
				Parts.Add(PreprocessedParts(buf));
			}
		}

		StringTableEntry::StringTableEntry(StringTableEntry&& rhs)
			: Text(std::move(rhs.Text)), Parts(std::move(rhs.Parts))
		{
			CopyArray(Extra, rhs.Extra);
		}

		StringTableEntry& StringTableEntry::operator=(StringTableEntry&& rhs)
		{
			if (this != &rhs)
			{
				Text = std::move(rhs.Text);
				Parts = std::move(rhs.Parts);

				CopyArray(Extra, rhs.Extra);
			}
			return *this;
		}

		String StringTableEntry::PreprocessedParts::Format(const StringTableFormatArgument arg) const
		{
			if (arg.Type == StringTableFormatArgument::AT_Int)
				return StringUtils::IntToString(arg.IntValue, FormatFlags);
			else if (arg.Type == StringTableFormatArgument::AT_UInt)
				return IsHex ? StringUtils::UIntToStringHex(arg.UIntValue, FormatFlags) : StringUtils::UIntToString(arg.UIntValue, FormatFlags);
			else if (arg.Type == StringTableFormatArgument::AT_Single)
				return StringUtils::SingleToString(arg.SingleValue, FormatFlags);
			else if (arg.Type == StringTableFormatArgument::AT_Double)
				return StringUtils::DoubleToString(arg.DoubleValue, FormatFlags);

			return Text;
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

		String StringTable::GetMissingText(const std::string& name) const
		{
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

				bool hasExtra = strlen(ent.Extra) > 0;
				bw.WriteInt32(hasExtra ? CSFWStringID : CSFStringID);
				bw.WriteInt32((int32)ent.Text.size());
				for (wchar_t ch : ent.Text)
					bw.WriteUInt16(~(uint16)ch);

				if (hasExtra)
				{
					bw.WriteMBString(ent.Extra);
				}
			}
		}

		//////////////////////////////////////////////////////////////////////////


	}
}