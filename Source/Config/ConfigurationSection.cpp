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
#include "Core/IParsable.h"
#include "Utility/StringUtils.h"
#include "Core/Logging.h"

using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace Config
	{

		void ConfigurationSection::AddSection(ConfigurationSection* section)
		{
			pair<SubSectionTable::iterator,bool> ret = m_subSection.insert(SubSectionTable::value_type(section->getName(), section));
			if (!ret.second)
			{
				LogManager::getSingleton().Write(LOG_System,  L"Configuration Section with name '" + section->getName() + L"' already exists. Ignored.", LOGLVL_Warning);
				delete section;
			}
		}
		void ConfigurationSection::AddAttribute(const String& name, const String& value)
		{
			try
			{
				m_attributes.insert(make_pair(name, value));
			}
			catch (const Apoc3DException& e)
			{
				LogManager::getSingleton().Write(LOG_System,  L"Attribute with name '" + name + L"' already exists. ", LOGLVL_Warning);
			}
		}
		void ConfigurationSection::SetValue( const String& value)
		{
			if (!value.empty())
			{
				LogManager::getSingleton().Write(LOG_System,  L"Overwriting the value of configuration section '" + m_name + L"'. ", LOGLVL_Warning);
			}
			m_value = value;
		}


		const String& ConfigurationSection::getValue(const String& name) const
		{
			SubSectionTable::const_iterator iter = m_subSection.find(name);

			if (iter != m_subSection.end())
			{
				return iter->second->getValue();
			}
			
			throw Apoc3DException::createException(EX_KeyNotFound, name.c_str());
		}
		const String& ConfigurationSection::getAttribute(const String& name) const
		{
			AttributeTable::const_iterator iter = m_attributes.find(name);

			if (iter != m_attributes.end())
			{
				return iter->second;
			}

			throw Apoc3DException::createException(EX_KeyNotFound, name.c_str());
		}
		ConfigurationSection* ConfigurationSection::getSection(const String& name) const
		{
			SubSectionTable::const_iterator iter = m_subSection.find(name);

			if (iter != m_subSection.end())
			{
				return iter->second;
			}
			throw Apoc3DException::createException(EX_KeyNotFound, name.c_str());
		}
		
		bool ConfigurationSection::tryGetValue(const String& name, String& result) const
		{
			SubSectionTable::const_iterator iter = m_subSection.find(name);

			if (iter != m_subSection.end())
			{
				result = iter->second->getValue();
				return true;
			}
			return false;
		}
		bool ConfigurationSection::tryGetAttribute(const String& name, String& result) const
		{
			SubSectionTable::const_iterator iter = m_subSection.find(name);

			if (iter != m_subSection.end())
			{
				result = iter->second->getValue();
				return true;
			}
			return false;
		}

		void ConfigurationSection::Get(const String& key, IParsable* value) const
		{
			assert(value);
			const String& str = getValue(key);

			value->Parse(str);
		}

		bool ConfigurationSection::GetBool(const String& key) const
		{
			return StringUtils::ParseBool(getValue(key));
		}
		bool ConfigurationSection::GetAttributeBool(const String& key) const
		{
			return StringUtils::ParseBool(getAttribute(key));
		}
		bool ConfigurationSection::TryGetBool(const String& key, bool& result) const
		{
			String str;
			if (tryGetValue(key, str))
			{
				result = StringUtils::ParseBool(str);
				return true;
			}
			return false;
		}
		bool ConfigurationSection::TryGetAttributeBool(const String& key, bool& result) const
		{
			String str;
			if (tryGetAttribute(key, str))
			{
				result = StringUtils::ParseBool(str);
				return true;
			}
			return false;
		}

		float ConfigurationSection::GetSingle(const String& name) const
		{
			return StringUtils::ParseSingle(getValue(name));
		}
		float ConfigurationSection::GetAttributeSingle(const String& key) const
		{
			return StringUtils::ParseSingle(getAttribute(key));
		}
		bool ConfigurationSection::TryGetSingle(const String& key, float& result) const
		{
			String str;
			if (tryGetValue(key, str))
			{
				result = StringUtils::ParseSingle(str);
				return true;
			}
			return false;
		}
		bool ConfigurationSection::TryGetAttributeSingle(const String& key, float& result) const
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
		

		float ConfigurationSection::GetPercentage(const String& key) const
		{
			return ParsePercentage(getValue(key));
		}
		float ConfigurationSection::GetAttributePercentage(const String& key) const
		{
			return ParsePercentage(getAttribute(key));
		}
		bool ConfigurationSection::TryGetPercentage(const String& key, float& result) const
		{
			String str;
			if (tryGetValue(key, str))
			{
				result = ParsePercentage(str);
				return true;
			}
			return false;
		}
		bool ConfigurationSection::TryGetAttributePercentage(const String& key, float& result) const
		{
			String str;
			if (tryGetAttribute(key, str))
			{
				result = ParsePercentage(str);
				return true;
			}
			return false;
		}

		int32 ConfigurationSection::GetInt(const String& key) const
		{
			return StringUtils::ParseInt32(getValue(key));
		}
		int32 ConfigurationSection::GetAttributeInt(const String& key) const
		{
			return StringUtils::ParseInt32(getAttribute(key));
		}
		bool ConfigurationSection::TryGetInt(const String& key, int32& result) const
		{
			String str;
			if (tryGetValue(key, str))
			{
				result = StringUtils::ParseInt32(str);
				return true;
			}
			return false;
		}
		bool ConfigurationSection::TryGetAttributeInt(const String& key, int32& result) const
		{
			String str;
			if (tryGetAttribute(key, str))
			{
				result = StringUtils::ParseInt32(str);
				return true;
			}
			return false;
		}

		uint32 ConfigurationSection::GetUInt(const String& key) const
		{
			return StringUtils::ParseUInt32(getValue(key));
		}
		uint32 ConfigurationSection::GetAttributeUInt(const String& key) const
		{
			return StringUtils::ParseUInt32(getAttribute(key));
		}
		bool ConfigurationSection::TryGetUInt(const String& key, uint32& result) const
		{
			String str;
			if (tryGetValue(key, str))
			{
				result = StringUtils::ParseUInt32(str);
				return true;
			}
			return false;
		}
		bool ConfigurationSection::TryGetAttributeUInt(const String& key, uint32& result) const
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

		ColorValue ConfigurationSection::GetColorValue(const String& key) const
		{
			return ParseColorValue(getValue(key));
		}
		ColorValue ConfigurationSection::GetAttributeColorValue(const String& key) const
		{
			return ParseColorValue(getAttribute(key));
		}
		bool ConfigurationSection::TryGetColorValue(const String& key, ColorValue& result) const
		{
			String str;
			if (tryGetValue(key, str))
			{
				result = ParseColorValue(str);
				return true;
			}
			return false;
		}
		bool ConfigurationSection::TryGetAttributeColorValue(const String& key, ColorValue& result) const
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

		vector<String> ConfigurationSection::GetStrings(const String& key) const
		{			
			vector<String> result;
			SplitString(getValue(key), result);
			
			return result;
		}
		vector<String> ConfigurationSection::GetAttributeStrings(const String& key) const
		{
			vector<String> result;
			SplitString(getAttribute(key), result);
			
			return result;
		}

		vector<float> ConfigurationSection::GetSingles(const String& key) const
		{
			String val = getValue(key);
			vector<String> vals = StringUtils::Split(val, L",");
			vector<float> result(val.size());
			for (size_t i=0;i<vals.size();i++)
			{
				result[i] = StringUtils::ParseSingle(vals[i]);
			}
			return result;
		}
		vector<float> ConfigurationSection::GetAttributeSingles(const String& key) const
		{
			String val = getAttribute(key);
			vector<String> vals = StringUtils::Split(val, L",");
			vector<float> result(val.size());
			for (size_t i=0;i<vals.size();i++)
			{
				result[i] = StringUtils::ParseSingle(vals[i]);
			}
			return result;
		}
		
		vector<float> ConfigurationSection::GetPercentages(const String& key) const
		{
			String val = getValue(key);
			vector<String> vals = StringUtils::Split(val, L",");
			vector<float> result(val.size());
			for (size_t i=0;i<vals.size();i++)
			{
				result[i] = ParsePercentage(vals[i]);
			}
			return result;
		}
		vector<float> ConfigurationSection::GetAttributePercentages(const String& key) const
		{
			String val = getAttribute(key);
			vector<String> vals = StringUtils::Split(val, L",");
			vector<float> result(val.size());
			for (size_t i=0;i<vals.size();i++)
			{
				result[i] = ParsePercentage(vals[i]);
			}
			return result;
		}

		vector<int32> ConfigurationSection::GetInts(const String& name) const 
		{
			String val = getValue(name);
			vector<String> vals = StringUtils::Split(val, L",");
			vector<int32> result(val.size());
			for (size_t i=0;i<vals.size();i++)
			{
				result[i] = StringUtils::ParseInt32(vals[i]);
			}
			return result;
		}
		vector<int32> ConfigurationSection::GetAttributeInts(const String& name) const 
		{
			String val = getAttribute(name);
			vector<String> vals = StringUtils::Split(val, L",");
			vector<int32> result(val.size());
			for (size_t i=0;i<vals.size();i++)
			{
				result[i] = StringUtils::ParseInt32(vals[i]);
			}
			return result;
		}

		vector<uint32> ConfigurationSection::GetUInts(const String& name) const 
		{
			String val = getValue(name);
			vector<String> vals = StringUtils::Split(val, L",");
			vector<uint32> result(val.size());
			for (size_t i=0;i<vals.size();i++)
			{
				result[i] = StringUtils::ParseInt32(vals[i]);
			}
			return result;
		}
		vector<uint32> ConfigurationSection::GetAttributeUInts(const String& name) const 
		{
			String val = getAttribute(name);
			vector<String> vals = StringUtils::Split(val, L",");
			vector<uint32> result(val.size());
			for (size_t i=0;i<vals.size();i++)
			{
				result[i] = StringUtils::ParseInt32(vals[i]);
			}
			return result;
		}

	}
}