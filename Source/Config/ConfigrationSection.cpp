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
		
		bool ConfigrationSection::tryGetValue(const String& name, String& result) const
		{
			ValueTable::const_iterator iter = m_values.find(name);

			if (iter != m_values.end())
			{
				result = iter->second;
				return true;
			}
			return false;
		}
		bool ConfigrationSection::tryGetAttribute(const String& name, String& result) const
		{
			ValueTable::const_iterator iter = m_attributes.find(name);

			if (iter != m_attributes.end())
			{
				result = iter->second;
				return true;
			}
			return false;
		}

		void ConfigrationSection::Get(const String& key, Parsable* value) const
		{
			assert(value);
			const String& str = getValue(key);

			value->Parse(str);
		}

		bool ConfigrationSection::GetBool(const String& key) const
		{
			return StringUtils::ParseBool(getValue(key));
		}
		bool ConfigrationSection::GetAttributeBool(const String& key) const
		{
			return StringUtils::ParseBool(getAttribute(key));
		}
		bool ConfigrationSection::TryGetBool(const String& key, bool& result) const
		{
			String str;
			if (tryGetValue(key, str))
			{
				result = StringUtils::ParseBool(str);
				return true;
			}
			return false;
		}
		bool ConfigrationSection::TryGetAttributeBool(const String& key, bool& result) const
		{
			String str;
			if (tryGetAttribute(key, str))
			{
				result = StringUtils::ParseBool(str);
				return true;
			}
			return false;
		}

		float ConfigrationSection::GetSingle(const String& name) const
		{
			return StringUtils::ParseSingle(getValue(name));
		}
		float ConfigrationSection::GetAttributeSingle(const String& key) const
		{
			return StringUtils::ParseSingle(getAttribute(key));
		}
		bool ConfigrationSection::TryGetSingle(const String& key, float& result) const
		{
			String str;
			if (tryGetValue(key, str))
			{
				result = StringUtils::ParseSingle(str);
				return true;
			}
			return false;
		}
		bool ConfigrationSection::TryGetAttributeSingle(const String& key, float& result) const
		{
			String str;
			if (tryGetAttribute(key, str))
			{
				result = StringUtils::ParseSingle(str);
				return true;
			}
			return false;
		}

		float ParsePercentage(const String& val)
		{
			size_t pos = val.find_last_of('%');
			if (pos != String::npos)
			{
				String ss = val.substr(0, pos);
				return StringUtils::ParseSingle(ss);
			}
			throw Apoc3DException::createException(EX_FormatException, val.c_str());
		}
		

		float ConfigrationSection::GetPercentage(const String& key) const
		{
			return ParsePercentage(getValue(key));
		}
		float ConfigrationSection::GetAttributePercentage(const String& key) const
		{
			return ParsePercentage(getAttribute(key));
		}
		bool ConfigrationSection::TryGetPercentage(const String& key, float& result) const
		{
			String str;
			if (tryGetValue(key, str))
			{
				result = ParsePercentage(str);
				return true;
			}
			return false;
		}
		bool ConfigrationSection::TryGetAttributePercentage(const String& key, float& result) const
		{
			String str;
			if (tryGetAttribute(key, str))
			{
				result = ParsePercentage(str);
				return true;
			}
			return false;
		}

		int32 ConfigrationSection::GetInt(const String& key) const
		{
			return StringUtils::ParseInt32(getValue(key));
		}
		int32 ConfigrationSection::GetAttributeInt(const String& key) const
		{
			return StringUtils::ParseInt32(getAttribute(key));
		}
		bool ConfigrationSection::TryGetInt(const String& key, int32& result) const
		{
			String str;
			if (tryGetValue(key, str))
			{
				result = StringUtils::ParseInt32(str);
				return true;
			}
			return false;
		}
		bool ConfigrationSection::TryGetAttributeInt(const String& key, int32& result) const
		{
			String str;
			if (tryGetAttribute(key, str))
			{
				result = StringUtils::ParseInt32(str);
				return true;
			}
			return false;
		}

		uint32 ConfigrationSection::GetUInt(const String& key) const
		{
			return StringUtils::ParseUInt32(getValue(key));
		}
		uint32 ConfigrationSection::GetAttributeUInt(const String& key) const
		{
			return StringUtils::ParseUInt32(getAttribute(key));
		}
		bool ConfigrationSection::TryGetUInt(const String& key, uint32& result) const
		{
			String str;
			if (tryGetValue(key, str))
			{
				result = StringUtils::ParseUInt32(str);
				return true;
			}
			return false;
		}
		bool ConfigrationSection::TryGetAttributeUInt(const String& key, uint32& result) const
		{
			String str;
			if (tryGetAttribute(key, str))
			{
				result = StringUtils::ParseUInt32(str);
				return true;
			}
			return false;
		}

		ColorValue ParseColorValue(const String& str)
		{
			const vector<String>& val = StringUtils::Split(str, L",");
			if (val.size() == 4)
			{				
				const uint r = StringUtils::ParseUInt32(val[0]);
				const uint g = StringUtils::ParseUInt32(val[1]);
				const uint b = StringUtils::ParseUInt32(val[2]);
				const uint a = StringUtils::ParseUInt32(val[3]);

				return PACK_COLOR(r,g,b,a);
			}
			else if (val.size() == 3)
			{
				const uint r = StringUtils::ParseUInt32(val[0]);
				const uint g = StringUtils::ParseUInt32(val[1]);
				const uint b = StringUtils::ParseUInt32(val[2]);
				const uint a = 0xff;

				return PACK_COLOR(r,g,b,a);
			}
			throw Apoc3DException::createException(EX_FormatException, L"Wrong number of channels");
		}

		ColorValue ConfigrationSection::GetColorValue(const String& key) const
		{
			return ParseColorValue(getValue(key));
		}
		ColorValue ConfigrationSection::GetAttributeColorValue(const String& key) const
		{
			return ParseColorValue(getAttribute(key));
		}
		bool ConfigrationSection::TryGetColorValue(const String& key, ColorValue& result) const
		{
			String str;
			if (tryGetValue(key, str))
			{
				result = ParseColorValue(str);
				return true;
			}
			return false;
		}
		bool ConfigrationSection::TryGetAttributeColorValue(const String& key, ColorValue& result) const
		{
			String str;
			if (tryGetAttribute(key, str))
			{
				result = ParseColorValue(str);
				return true;
			}
			return false;
		}

		void SplitString(const String& str, vector<String>& result)
		{
			bool isIn = false;
			String buffer;
			for (size_t i=0;i<str.size();i++)
			{
				wchar_t ch = str[i];
				if (ch == '"')
				{
					isIn = !isIn;
				}
				else if (ch == ',' && !isIn)
				{
					result.push_back(buffer);
					buffer = String();
				}
				else
				{
					buffer.append(&ch, 1);
				}
			}
			if (buffer.size())
			{
				result.push_back(buffer);
			}
		}

		vector<String> ConfigrationSection::GetStrings(const String& key) const
		{			
			vector<String> result;
			SplitString(getValue(key), result);
			
			return result;
		}
		vector<String> ConfigrationSection::GetAttributeStrings(const String& key) const
		{
			vector<String> result;
			SplitString(getAttribute(key), result);
			
			return result;
		}

		vector<float> ConfigrationSection::GetSingles(const String& key) const
		{
			String val = getValue(key);
			vector<String> vals = StringUtils::Split(val, L",");
			vector<float> result(val.size());
			for (size_t i=0;i<vals.size();i++)
			{
				result.push_back(StringUtils::ParseSingle(vals[i]));
			}
			return result;
		}
		vector<float> ConfigrationSection::GetAttributeSingles(const String& key) const
		{
			String val = getAttribute(key);
			vector<String> vals = StringUtils::Split(val, L",");
			vector<float> result(val.size());
			for (size_t i=0;i<vals.size();i++)
			{
				result.push_back(StringUtils::ParseSingle(vals[i]));
			}
			return result;
		}
		
		vector<float> ConfigrationSection::GetPercentages(const String& key) const
		{
			String val = getValue(key);
			vector<String> vals = StringUtils::Split(val, L",");
			vector<float> result(val.size());
			for (size_t i=0;i<vals.size();i++)
			{
				result.push_back(ParsePercentage(vals[i]));
			}
			return result;
		}
		vector<float> ConfigrationSection::GetAttributePercentages(const String& key) const
		{
			String val = getAttribute(key);
			vector<String> vals = StringUtils::Split(val, L",");
			vector<float> result(val.size());
			for (size_t i=0;i<vals.size();i++)
			{
				result.push_back(ParsePercentage(vals[i]));
			}
			return result;
		}

		vector<int32> ConfigrationSection::GetInts(const String& name) const 
		{
			String val = getValue(name);
			vector<String> vals = StringUtils::Split(val, L",");
			vector<int32> result(val.size());
			for (size_t i=0;i<vals.size();i++)
			{
				result.push_back(StringUtils::ParseInt32(vals[i]));
			}
			return result;
		}
		vector<int32> ConfigrationSection::GetAttributeInts(const String& name) const 
		{
			String val = getAttribute(name);
			vector<String> vals = StringUtils::Split(val, L",");
			vector<int32> result(val.size());
			for (size_t i=0;i<vals.size();i++)
			{
				result.push_back(StringUtils::ParseInt32(vals[i]));
			}
			return result;
		}

		vector<uint32> ConfigrationSection::GetUInts(const String& name) const 
		{
			String val = getValue(name);
			vector<String> vals = StringUtils::Split(val, L",");
			vector<uint32> result(val.size());
			for (size_t i=0;i<vals.size();i++)
			{
				result.push_back(StringUtils::ParseInt32(vals[i]));
			}
			return result;
		}
		vector<uint32> ConfigrationSection::GetAttributeUInts(const String& name) const 
		{
			String val = getAttribute(name);
			vector<String> vals = StringUtils::Split(val, L",");
			vector<uint32> result(val.size());
			for (size_t i=0;i<vals.size();i++)
			{
				result.push_back(StringUtils::ParseInt32(vals[i]));
			}
			return result;
		}

	}
}