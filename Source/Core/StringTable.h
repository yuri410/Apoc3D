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
#ifndef STRINGTABLE_H
#define STRINGTABLE_H
#pragma once

#include "Common.h"

using namespace std;

namespace Apoc3D
{
	namespace Core
	{
		class APAPI StringTable
		{
		private:
			unordered_map<String, pair<String, String>> m_entryTable;

		public:
			StringTable(void) {}
			~StringTable(void) {}
		};

		class APAPI StringTableLoader
		{
		public:
			StringTable* Load(const ResourceLocation const &rl)
			{
				StringTable* st = new StringTable();
				Read(st, rl.GetStream);
				return st;
			}
			StringTable* Load(const String const &file)
			{
				return Load(new FileLocation(file));
			}

			void Read(unordered_map<String, pair<string, string>> &data, Stream stm) = 0;
			void Write(unordered_map<string, pair<string, string>> &data, Stream stm) = 0;

			vector<String> getFilers() = 0;
		};
	}
}
#endif