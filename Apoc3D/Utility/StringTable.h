#pragma once
#ifndef APOC3D_STRINGTABLE_H
#define APOC3D_STRINGTABLE_H

/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2009-2018 Tao Xin
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

#include "apoc3d/Collections/List.h"
#include "apoc3d/Collections/HashMap.h"

using namespace Apoc3D::Collections;
using namespace Apoc3D::VFS;
using namespace Apoc3D::IO;

namespace Apoc3D
{
	namespace Utility
	{
		struct StringTableFormatArgument
		{
			union
			{
				int32 IntValue;
				uint32 UIntValue;

				float SingleValue;
				double DoubleValue;
			};

			String Text;
			
			enum 
			{
				AT_String,
				AT_Int,
				AT_UInt,
				AT_Single,
				AT_Double
			} Type;

			StringTableFormatArgument(int32 v) : Type(AT_Int), IntValue(v) { }
			StringTableFormatArgument(uint32 v) : Type(AT_UInt), UIntValue(v) { }

			StringTableFormatArgument(float v) : Type(AT_Single), SingleValue(v) { }
			StringTableFormatArgument(double v) : Type(AT_Double), DoubleValue(v) { }

			StringTableFormatArgument(const String& txt) : Type(AT_String), Text(txt) { }
		};

		struct APAPI StringTableEntry
		{
			struct APAPI PreprocessedParts
			{
				String Text;
				uint64 FormatFlags = 0;
				bool IsPercentage = false;
				bool IsHex = false;

				PreprocessedParts() { }
				PreprocessedParts(const String& txt) : Text(txt) { }
				PreprocessedParts(uint64 flags, bool isPercentage, bool isHex) 
					: FormatFlags(flags), IsPercentage(isPercentage), IsHex(isHex) { }

				PreprocessedParts(PreprocessedParts&& rhs)
					: Text(std::move(rhs.Text)), FormatFlags(rhs.FormatFlags) { }

				PreprocessedParts& operator=(PreprocessedParts&& rhs)
				{
					if (this != &rhs)
					{
						Text = std::move(rhs.Text);
						FormatFlags = rhs.FormatFlags;
					}
					return *this;
				}

				PreprocessedParts(const PreprocessedParts&) = delete;
				PreprocessedParts& operator=(const PreprocessedParts&) = delete;

				String Format(const StringTableFormatArgument arg) const;
			};

			String Text;
			char Extra[32];

			List<PreprocessedParts> Parts;

			StringTableEntry();
			StringTableEntry(const String& txt, const std::string& extra);

			StringTableEntry(StringTableEntry&& rhs);
			StringTableEntry& operator=(StringTableEntry&& rhs);

			StringTableEntry(const StringTableEntry&) = delete;
			StringTableEntry& operator=(const StringTableEntry&) = delete;
		};


		typedef HashMap<std::string, StringTableEntry, NStringEqualityComparerNoCase> StringTableMap;
		class StringTableFormat;

		class APAPI StringTable
		{
			friend class StringTableFormat;
		public:
			StringTable() { }
			~StringTable() { }

			StringTable(const StringTable&) = delete;
			StringTable& operator=(const StringTable&) = delete;


			String GetString(const std::string& name) const;

			template <typename ... Args>
			String GetString(const std::string& name, const Args& ... params) const
			{
				const StringTableEntry* e = m_entryTable.TryGetValue(name);
				if (e)
				{
					if (e->Parts.getCount() > 0)
					{
						String r;
						MakeString(e, 0, r, params...);
						return r;
					}
					return e->Text;
				}
				return GetMissingText(name);
			}

			String operator[](const std::string& name) const { return GetString(name); }


			void Load(StringTableFormat* fmt, const ResourceLocation& rl);
			void Load(StringTableFormat* fmt, Stream& strm);

			void Save(StringTableFormat* fmt, Stream& strm);

		private:

			String GetMissingText(const std::string& name) const;

			template <typename ... Args>
			void MakeString(const StringTableEntry* e, int32 idx, String& result, const StringTableFormatArgument p0, const Args& ... rest) const
			{
				auto& p = e->Parts[idx];
				
				if (p.Text.size())
					result += p.Text;
				else
					result += p.Format(p0);

				if (idx + 1 < e->Parts.getCount())
					MakeString(e, idx + 1, result, rest...);
			}

			StringTableMap m_entryTable;
		};

		class StringTableFormat
		{
		public:
			virtual void Read(Stream& stm, StringTableMap& map) = 0;
			virtual void Write(StringTableMap& map, Stream& stm) = 0;

			virtual List<String> getFilters() = 0;
		};

		class APAPI CsfStringTableFormat : public StringTableFormat
		{
		public:
			virtual void Read(Stream& stm, StringTableMap& map) override;
			virtual void Write(StringTableMap& map, Stream& stm) override;

			List<String> getFilters() override 
			{
				return{ L"csf" }; 
			}
		};

		class APAPI ExcelXmlStringTableFormat : public StringTableFormat
		{
		public:
			virtual void Read(Stream& stm, StringTableMap& map) override;
			virtual void Write(StringTableMap& map, Stream& stm) override;

			List<String> getFilters() override
			{
				return{ L"xm" };
			}
		};
	}
}
#endif