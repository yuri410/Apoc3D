#pragma once
#ifndef APOC3D_STRINGTABLE_H
#define APOC3D_STRINGTABLE_H

/**
 * -----------------------------------------------------------------------------
 * This source file is part of Apoc3D Engine
 * 
 * Copyright (c) 2009+ Tao Xin
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  if not, write to the Free Software Foundation, 
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA, or go to
 * http://www.gnu.org/copyleft/gpl.txt.
 * 
 * -----------------------------------------------------------------------------
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
		struct APAPI StringTableEntry
		{
			String Text;
			char Extra[32];
			
			StringTableEntry();
			StringTableEntry(const String& txt, const std::string& extra);
		};

		typedef HashMap<std::string, StringTableEntry, NStringEqualityComparerNoCase> StringTableMap;
		class StringTableFormat;

		class APAPI StringTable
		{
			friend class StringTableFormat;
		public:
			StringTable() { }
			~StringTable() { }

			String GetString(const std::string& name) const;

			String operator[](const std::string& name) const { return GetString(name); }

			void Load(StringTableFormat* fmt, const ResourceLocation& rl);
			void Load(StringTableFormat* fmt, Stream& strm);

			void Save(StringTableFormat* fmt, Stream& strm);
		private:
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
	}
}
#endif