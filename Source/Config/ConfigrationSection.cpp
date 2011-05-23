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
#include "ConfigurationSection.h"

#include "Apoc3DException.h"
#include "Core/Parsable.h"
#include "Utility/StringUtils.h"

using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace Config
	{
		const String& ConfigrationSection::getValue(const String& name) const
		{
			ValueTable::const_iterator iter = m_values.find(name);

			if (iter != m_values.end())
			{
				return iter->second;
			}
			
			throw Apoc3DException::createException(EX_KeyNotFound, name.c_str());
		}
		const String& ConfigrationSection::getAttribute(const String& name) const
		{
			ValueTable::const_iterator iter = m_attributes.find(name);

			if (iter != m_attributes.end())
			{
				return iter->second;
			}

			throw Apoc3DException::createException(EX_KeyNotFound, name.c_str());
		}
		ConfigrationSection* ConfigrationSection::getSection(const String& name) const
		{
			SubSectionTable::const_iterator iter = m_subSection.find(name);

			if (iter != m_subSection.end())
			{
				return iter->second;
			}
			throw Apoc3DException::createException(EX_KeyNotFound, name.c_str());
		}


		void ConfigrationSection::Get(const String& key, Parsable* value) const
		{
			assert(value);
			const String& str = getValue(key);

			value->Parse(str);
		}

		bool ConfigrationSection::GetBool(const String& key) const
		{

		}
		bool ConfigrationSection::GetAttributeBool(const String& key) const
		{

		}
		bool ConfigrationSection::TryGetBool(const String& key, bool& result) const
		{

		}
		bool ConfigrationSection::TryGetAttributeBool(const String& key, bool& result) const
		{

		}

		float ConfigrationSection::GetSingle(const String& name) const
		{

		}
		float ConfigrationSection::GetAttributeSingle(const String& key) const
		{

		}
		bool ConfigrationSection::TryGetSingle(const String& key, float& result) const
		{

		}
		bool ConfigrationSection::TryGetAttributeSingle(const String& key, float& result) const
		{

		}

		float ConfigrationSection::GetPercentage(const String& key) const
		{

		}
		float ConfigrationSection::GetAttributePercentage(const String& key) const
		{

		}
		bool ConfigrationSection::TryGetPercentage(const String& key, float& result) const
		{

		}
		bool ConfigrationSection::TryGetAttributePercentage(const String& key, float& result) const
		{

		}

		int32 ConfigrationSection::GetInt(const String& key) const
		{

		}
		int32 ConfigrationSection::GetAttributeInt(const String& key) const
		{

		}
		bool ConfigrationSection::TryGetInt(const String& key, int32& result) const
		{

		}
		bool ConfigrationSection::TryGetAttributeInt(const String& key, int32& result) const
		{

		}

		uint32 ConfigrationSection::GetUInt(const String& key) const
		{

		}
		uint32 ConfigrationSection::GetAttributeUInt(const String& key) const
		{

		}
		bool ConfigrationSection::TryGetUInt(const String& key, uint32& result) const
		{

		}
		bool ConfigrationSection::TryGetAttributeUInt(const String& key, uint32& result) const
		{

		}

		ColorValue ConfigrationSection::GetColorValue(const String& key) const
		{

		}
		ColorValue ConfigrationSection::GetAttributeColorValue(const String& key) const
		{

		}
		bool ConfigrationSection::TryGetColorValue(const String& key, ColorValue& result) const
		{

		}
		bool ConfigrationSection::TryGetAttributeColorValue(const String& key, ColorValue& result) const
		{

		}

		vector<String> ConfigrationSection::GetStrings(const String& key) const
		{

		}
		vector<String> ConfigrationSection::GetAttributeStrings(const String& key) const
		{

		}

		vector<float> ConfigrationSection::GetSingles(const String& key) const
		{
		}
		vector<float> ConfigrationSection::GetAttributeSingles(const String& key) const
		{

		}
		
		vector<float> ConfigrationSection::GetPercentages(const String& key) const
		{

		}
		vector<float> ConfigrationSection::GetAttributePercentages(const String& key) const
		{

		}

		vector<int32> ConfigurationSection::GetInts(const String& name) const 
		{

		}
		vector<int32> ConfigurationSection::GetAttributeInts(const String& name) const 
		{

		}

		vector<uint32> ConfigurationSection::GetUInts(const String& name) const 
		{

		}
		vector<uint32> ConfigurationSection::GetAttributeUInts(const String& name) const 
		{

		}


		vector<ColorValue> ConfigurationSection::GetColorValues(const String& name) const 
		{

		}
		vector<ColorValue> ConfigurationSection::GetAttributeColorValues(const String& name) const 
		{

		}
	}
}