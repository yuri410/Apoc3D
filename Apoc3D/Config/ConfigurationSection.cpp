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
#include "ConfigurationSection.h"

#include "apoc3d/Apoc3DException.h"
#include "apoc3d/Core/IParsable.h"
#include "apoc3d/Core/Logging.h"
#include "apoc3d/Utility/StringUtils.h"

using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace Config
	{
		/** Actual parsing/printing functions
		*/
		float ParsePercentage(const String& val);
		String PercentageToString(float v);
		
		ColorValue ParseColorValue(const String& str);
		String ColorValueToString(ColorValue v);
		
		void CombineString(const String* v, int count, String& result);
		void SplitString(const String& str, List<String>& result);

		void SinglesToString(const float* v, int count, String& result);
		void PrecentagesToString(const float* v, int count, String& result);
		void IntsToString(const int32* v, int count, String& result);
		void UIntsToString(const uint32* v, int count, String& result);

		void SplitSingles(const List<String>& vals, List<float>& result);
		void SplitPercentages(const List<String>& vals, List<float>& result);
		void SplitInt(const List<String>& vals, List<int32>& result);
		void SplitUint(const List<String>& vals, List<uint32>& result);



		ConfigurationSection::ConfigurationSection(const ConfigurationSection& another)
			: m_attributes(another.m_attributes), m_subSection(another.m_subSection), m_name(another.m_name), m_value(another.m_value)
		{
			for (SubSectionTable::Enumerator e = another.GetSubSectionEnumrator(); e.MoveNext(); )
			{
				ConfigurationSection* newSect = new ConfigurationSection(**e.getCurrentValue());

				*e.getCurrentValue() = newSect;
			}
		}

		void ConfigurationSection::AddSection(ConfigurationSection* section)
		{
			assert(section != this);

			if (m_subSection.Contains(section->getName()))
			{
				LogManager::getSingleton().Write(LOG_System,  L"Configuration Section with name '" + section->getName() + L"' already exists. Ignored.", LOGLVL_Warning);
				delete section;
				return;
			}
			m_subSection.Add(section->getName(), section);
		}
		void ConfigurationSection::SetValue( const String& value)
		{
			if (m_value != value && !m_value.empty())
			{
				LogManager::getSingleton().Write(LOG_System,  L"Overwriting the value of configuration section '" + m_name + L"'. ", LOGLVL_Warning);
			}
			m_value = value;
		}

		bool ConfigurationSection::hasAttribute(const String& name) const
		{
			return m_attributes.Contains(name);
		}

		const String& ConfigurationSection::getValue(const String& name) const
		{
			assert(m_subSection.Contains(name));
			ConfigurationSection* sect = m_subSection[name];
			return sect->getValue();
		}
		const String& ConfigurationSection::getAttribute(const String& name) const
		{
			assert(m_attributes.Contains(name));
			return m_attributes[name];
		}
		ConfigurationSection* ConfigurationSection::getSection(const String& name) const
		{
			//assert(m_subSection.Contains(name));
			ConfigurationSection* sect = 0;
			m_subSection.TryGetValue(name, sect);
			return sect;
		}
		
		bool ConfigurationSection::tryGetValue(const String& name, String& result) const
		{
			ConfigurationSection* sect;
			if (m_subSection.TryGetValue(name, sect))
			{
				result = sect->getValue();
				return true;
			}
			return false;
		}
		bool ConfigurationSection::tryGetAttribute(const String& name, String& result) const
		{
			return (m_attributes.TryGetValue(name, result));
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


		List<String> ConfigurationSection::GetStrings(const String& key) const
		{			
			List<String> result;
			SplitString(getValue(key), result);
			return result;
		}
		List<String> ConfigurationSection::GetAttributeStrings(const String& key) const
		{
			List<String> result;
			SplitString(getAttribute(key), result);
			return result;
		}

		List<float> ConfigurationSection::GetSingles(const String& key) const
		{
			List<String> vals;
			StringUtils::Split(getValue(key), vals, L",");
			List<float> result(vals.getCount());
			SplitSingles(vals, result);
			return result;
		}
		List<float> ConfigurationSection::GetAttributeSingles(const String& key) const
		{
			List<String> vals;
			StringUtils::Split(getAttribute(key), vals, L",");
			List<float> result(vals.getCount());
			SplitSingles(vals, result);
			return result;
		}
		
		List<float> ConfigurationSection::GetPercentages(const String& key) const
		{
			List<String> vals;
			StringUtils::Split(getValue(key), vals, L",");
			List<float> result(vals.getCount());
			SplitPercentages(vals, result);
			return result;
		}
		List<float> ConfigurationSection::GetAttributePercentages(const String& key) const
		{
			List<String> vals;
			StringUtils::Split(getAttribute(key), vals, L",");
			List<float> result(vals.getCount());
			SplitPercentages(vals, result);
			return result;
		}

		List<int32> ConfigurationSection::GetInts(const String& name) const 
		{
			List<String> vals;
			StringUtils::Split(getValue(name), vals, L",");
			List<int32> result(vals.getCount());
			SplitInt(vals, result);
			return result;
		}
		List<int32> ConfigurationSection::GetAttributeInts(const String& name) const 
		{
			List<String> vals;
			StringUtils::Split(getAttribute(name), vals, L",");
			List<int32> result(vals.getCount());
			SplitInt(vals, result);
			return result;
		}

		List<uint32> ConfigurationSection::GetUInts(const String& name) const 
		{
			List<String> vals;
			StringUtils::Split(getValue(name), vals, L",");
			List<uint32> result(vals.getCount());
			SplitUint(vals, result);
			return result;
		}
		List<uint32> ConfigurationSection::GetAttributeUInts(const String& name) const 
		{
			List<String> vals;
			StringUtils::Split(getAttribute(name), vals, L",");
			List<uint32> result(vals.getCount());
			SplitUint(vals, result);
			return result;
		}



		void ConfigurationSection::AddStringValue(const String& name, const String& value)
		{
			ConfigurationSection* ss = new ConfigurationSection(name);
			ss->SetValue(value);
			AddSection(ss);
		}

		void ConfigurationSection::AddBool(const String& key, bool value)
		{
			AddStringValue(key, StringUtils::ToString(value));
		}
		void ConfigurationSection::AddSingle(const String& key, float value)
		{
			AddStringValue(key, StringUtils::ToString(value));
		}
		void ConfigurationSection::AddPercentage(const String& key, float value)
		{
			AddStringValue(key, StringUtils::ToString(value));
		}
		void ConfigurationSection::AddInt(const String& key, int32 value)
		{
			AddStringValue(key, StringUtils::ToString(value));
		}
		void ConfigurationSection::AddUInt(const String& key, uint32 value)
		{
			AddStringValue(key, StringUtils::ToString(value));
		}
		void ConfigurationSection::AddColorValue(const String& key, ColorValue value)
		{
			AddStringValue(key, ColorValueToString(value));
		}

		void ConfigurationSection::AddStrings(const String& key, const String* v, int count)
		{
			String result;
			CombineString(v, count, result);
			AddAttributeString(key, result);
		}
		void ConfigurationSection::AddSingles(const String& key, const float* v, int count)
		{
			String result;
			SinglesToString(v, count, result);
			AddStringValue(key, result);
		}
		void ConfigurationSection::AddPercentages(const String& key, const float* v, int count)
		{
			String result;
			SinglesToString(v, count, result);
			AddStringValue(key, result);
		}
		void ConfigurationSection::AddInts(const String& key, const int32* v, int count)
		{
			String result;
			IntsToString(v, count, result);
			AddStringValue(key, result);
		}
		void ConfigurationSection::AddUInts(const String& key, const uint32* v, int count)
		{
			String result;
			UIntsToString(v, count, result);
			AddStringValue(key, result);
		}



		void ConfigurationSection::AddAttributeString(const String& name, const String& value)
		{
			try
			{
				m_attributes.Add(name, value);
				//m_attributes.insert(make_pair(name, value));
			}
			catch (const Apoc3DException& e)
			{
				switch (e.getType())
				{
				case EX_Duplicate:
					LogManager::getSingleton().Write(LOG_System,  L"Attribute with name '" + name + L"' already exists. ", LOGLVL_Warning);
					break;
				}
			}
		}
		void ConfigurationSection::AddAttributeBool(const String& name, bool val) { AddAttributeString(name, StringUtils::ToString(val)); }
		void ConfigurationSection::AddAttributeSingle(const String& name, float val) { AddAttributeString(name, StringUtils::ToString(val)); }
		void ConfigurationSection::AddAttributePercentage(const String& name, float val) { AddAttributeString(name, PercentageToString(val)); }
		void ConfigurationSection::AddAttributeInt(const String& name, int32 val) { AddAttributeString(name, StringUtils::ToString(val)); }
		void ConfigurationSection::AddAttributeUInt(const String& name, uint32 val) { AddAttributeString(name, StringUtils::ToString(val)); }
		void ConfigurationSection::AddAttributeColorValue(const String& name, ColorValue val) { AddAttributeString(name, ColorValueToString(val)); }

		void ConfigurationSection::AddAttributeStrings(const String& name, const String* v, int count)
		{
			String result;
			CombineString(v, count, result);
			AddAttributeString(name, result);
		}
		void ConfigurationSection::AddAttributeSingles(const String& name, const float* v, int count)
		{
			String result;
			SinglesToString(v, count, result);
			AddAttributeString(name, result);
		}
		void ConfigurationSection::AddAttributePercentages(const String& name, const float* v, int count)
		{
			String result;
			PrecentagesToString(v, count, result);
			AddAttributeString(name, result);
		}
		void ConfigurationSection::AddAttributeInts(const String& name, const int32* v, int count)
		{
			String result;
			IntsToString(v, count, result);
			AddAttributeString(name, result);
		}
		void ConfigurationSection::AddAttributeUInts(const String& name, const uint32* v, int count)
		{
			String result;
			UIntsToString(v, count, result);
			AddAttributeString(name, result);
		}


		void CombineString(const String* v, int count, String& result)
		{
			for (int i=0;i<count;i++)
			{
				result.append(1, '"');
				result.append(v[i]);
				result.append(1, '"');
				if (i != count - 1)
					result.append(1, ',');
			}
		}
		void SplitString(const String& str, List<String>& result)
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
					result.Add(buffer);
					buffer = String();
				}
				else
				{
					buffer.append(&ch, 1);
				}
			}
			if (buffer.size())
			{
				result.Add(buffer);
			}
		}

		void SinglesToString(const float* v, int count, String& result)
		{
			for (int i=0;i<count;i++)
			{
				result.append(StringUtils::ToString(v[i]));
				if (i != count - 1)
					result.append(1, ',');
			}
		}
		void PrecentagesToString(const float* v, int count, String& result)
		{
			for (int i=0;i<count;i++)
			{
				result.append(PercentageToString(v[i]));
				if (i != count - 1)
					result.append(1, ',');
			}
		}
		void IntsToString(const int32* v, int count, String& result)
		{
			for (int i=0;i<count;i++)
			{
				result.append(StringUtils::ToString(v[i]));
				if (i != count - 1)
					result.append(1, ',');
			}
		}
		void UIntsToString(const uint32* v, int count, String& result)
		{
			for (int i=0;i<count;i++)
			{
				result.append(StringUtils::ToString(v[i]));
				if (i != count - 1)
					result.append(1, ',');
			}
		}

		void SplitSingles(const List<String>& vals, List<float>& result)
		{
			for (int32 i=0;i<vals.getCount();i++)
			{
				result.Add( StringUtils::ParseSingle(vals[i]) );
			}
		}
		void SplitPercentages(const List<String>& vals, List<float>& result)
		{
			for (int32 i=0;i<vals.getCount();i++)
			{
				result.Add( ParsePercentage(vals[i]) );
			}
		}
		void SplitInt(const List<String>& vals, List<int32>& result)
		{
			for (int32 i=0;i<vals.getCount();i++)
			{
				result.Add( StringUtils::ParseInt32(vals[i]) );
			}
		}
		void SplitUint(const List<String>& vals, List<uint32>& result)
		{
			for (int32 i=0;i<vals.getCount();i++)
			{
				result.Add( StringUtils::ParseUInt32(vals[i]) );
			}
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
		String PercentageToString(float v)
		{
			String result = StringUtils::ToString(v);
			result.append(L"%");
			return result;
		}

		ColorValue ParseColorValue(const String& str)
		{
			List<String> val;
			StringUtils::Split(str, val, L",");
			if (val.getCount() == 4)
			{				
				const uint r = StringUtils::ParseUInt32(val[0]);
				const uint g = StringUtils::ParseUInt32(val[1]);
				const uint b = StringUtils::ParseUInt32(val[2]);
				const uint a = StringUtils::ParseUInt32(val[3]);

				return PACK_COLOR(r,g,b,a);
			}
			else if (val.getCount() == 3)
			{
				const uint r = StringUtils::ParseUInt32(val[0]);
				const uint g = StringUtils::ParseUInt32(val[1]);
				const uint b = StringUtils::ParseUInt32(val[2]);
				const uint a = 0xff;

				return PACK_COLOR(r,g,b,a);
			}
			throw Apoc3DException::createException(EX_FormatException, L"Wrong number of channels");
		}
		String ColorValueToString(ColorValue v)
		{
			uint a = GetColorA(v);
			uint r = GetColorR(v);
			uint g = GetColorG(v);
			uint b = GetColorB(v);

			String result;
			if (a == 0xff)
			{
				result.append(StringUtils::ToString(r));
				result.append(StringUtils::ToString(g));
				result.append(StringUtils::ToString(b));
			}
			else
			{
				result.append(StringUtils::ToString(r));
				result.append(StringUtils::ToString(g));
				result.append(StringUtils::ToString(b));
				result.append(StringUtils::ToString(a));
			}
			return result;
		}
	}
}