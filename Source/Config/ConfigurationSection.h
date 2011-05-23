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
#ifndef CONFIGURATIONSECTION_H
#define CONFIGURATIONSECTION_H

#include "Common.h"
#include "Math/ColorValue.h"

using namespace Apoc3D::Math;
using namespace Apoc3D::Core;

using namespace std;

namespace Apoc3D
{
	namespace Config
	{
		class APAPI ConfigrationSection
		{
		protected:
			typedef unordered_map<String, String> AttributeTable;
			typedef unordered_map<String, String> ValueTable;
			typedef unordered_map<String, ConfigrationSection*> SubSectionTable;

			String m_name;

			AttributeTable m_attributes;
			ValueTable m_values;
			SubSectionTable m_subSection;

		public:
			ConfigrationSection(const String& name)
				: m_name(name) 
			{ }

			const String& getName() const { return m_name; }
			const String& getValue(const String& name) const;
			const String& getAttribute(const String& name) const;
			ConfigrationSection* getSection(const String& name) const;

			void Get(const String& key, Parsable* value) const;

			bool GetBool(const String& key) const;
			float GetSingle(const String& key) const;
			float GetPercentage(const String& key) const;
			int32 GetInt(const String& key) const;
			uint32 GetUInt(const String& key) const;
			ColorValue GetColorValue(const String& key) const;
			vector<String> GetStrings(const String& key) const;
			vector<float> GetSingles(const String& key) const;			
			vector<float> GetPercentages(const String& key) const;
			vector<int32> GetInts(const String& key) const;
			vector<uint32> GetUInts(const String& key) const;
			vector<ColorValue> GetColorValues(const String& key) const;

			bool GetAttributeBool(const String& key) const;
			float GetAttributeSingle(const String& key) const;
			float GetAttributePercentage(const String& key) const;
			int32 GetAttributeInt(const String& key) const;
			uint32 GetAttributeUInt(const String& key) const;
			ColorValue GetAttributeColorValue(const String& key) const;
			vector<String> GetAttributeStrings(const String& key) const;
			vector<float> GetAttributeSingles(const String& key) const;
			vector<float> GetAttributePercentages(const String& key) const;
			vector<int32> GetAttributeInts(const String& key) const;
			vector<uint32> GetAttributeUInts(const String& key) const;
			vector<ColorValue> GetAttributeColorValues(const String& key) const;

			bool TryGetBool(const String& key, bool& result) const;
			bool TryGetSingle(const String& key, float& result) const;
			bool TryGetPercentage(const String& key, float& result) const;
			bool TryGetInt(const String& key, int32& result) const;
			bool TryGetUInt(const String& key, uint32& result) const;
			bool TryGetColorValue(const String& key, ColorValue& result) const;

			bool TryGetAttributeBool(const String& key, bool& result) const;
			bool TryGetAttributeSingle(const String& key, float& result) const;
			bool TryGetAttributePercentage(const String& key, float& result) const;
			bool TryGetAttributeInt(const String& key, int32& result) const;
			bool TryGetAttributeUInt(const String& key, uint32& result) const;
			bool TryGetAttributeColorValue(const String& key, ColorValue& result) const;

		};
	}
}
#endif