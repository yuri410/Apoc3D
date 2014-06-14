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

#include "apoc3d/ApocException.h"
#include "apoc3d/Core/Logging.h"
#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/Math/Vector.h"
#include "apoc3d/Math/Point.h"

using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace Config
	{
		int32 ConfigurationSection::FloatPointStoringPrecision = 3;

		/** Actual parsing/printing functions
		*/
		float ParsePercentage(const String& val);
		String PercentageToString(const float& v);
		
		ColorValue ParseColorValue(const String& str);
		String ColorValueToString(ColorValue v);
		
		Vector3 ParseVector3(const String& str);
		String Vector3ToString(const Vector3& vec);

		Point ParsePoint(const String& str);
		String PointToString(const Point& vec);


		void CombineString(const String* v, int count, String& result);
		void SplitString(const String& str, List<String>& result);

		void SinglesToString(const float* v, int count, String& result);
		void PrecentagesToString(const float* v, int count, String& result);
		void IntsToString(const int32* v, int count, String& result);
		void UIntsToString(const uint32* v, int count, String& result);
		void Vector3sToString(const Vector3* v, int count, String& result);
		void PointsToString(const Point* v, int count, String& result);

		void SplitSingles(const String& str, FastList<float>& result);
		void SplitPercentages(const String& str, FastList<float>& result);
		void SplitInt(const String& str, FastList<int32>& result);
		void SplitUint(const String& str, FastList<uint32>& result);
		void SplitVector3s(const String& str, FastList<Vector3>& result);
		void SplitPoints(const String& str, FastList<Point>& result);

		int32 SplitSinglesArr(const String& str, float* result, int32 expectedCount);
		int32 SplitPercentagesArr(const String& str, float* result, int32 expectedCount);
		int32 SplitIntArr(const String& str, int32* result, int32 expectedCount);
		int32 SplitUintArr(const String& str, uint32* result, int32 expectedCount);
		int32 SplitVector3sArr(const String& str, Vector3* result, int32 expectedCount);
		int32 SplitPointsArr(const String& str, Point* result, int32 expectedCount);


		ConfigurationSection::ConfigurationSection(const String& name, int capacity)
			: m_name(name), m_subSection(capacity, Apoc3D::Collections::IBuiltInEqualityComparer<String>::Default)
		{ }
		ConfigurationSection::ConfigurationSection(const String& name)
			: m_name(name)
		{ }
		ConfigurationSection::ConfigurationSection(const ConfigurationSection& another)
			: m_attributes(another.m_attributes), m_subSection(another.m_subSection), m_name(another.m_name), m_value(another.m_value)
		{
			for (SubSectionTable::Enumerator e = another.GetSubSectionEnumrator(); e.MoveNext(); )
			{
				ConfigurationSection* newSect = new ConfigurationSection(**e.getCurrentValue());

				*e.getCurrentValue() = newSect;
			}
		}
		ConfigurationSection::~ConfigurationSection()
		{
			for (SubSectionTable::Enumerator e = m_subSection.GetEnumerator();e.MoveNext();)
			{
				ConfigurationSection* sect = *e.getCurrentValue();
				delete sect;
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

		bool ConfigurationSection::hasAttribute(const String& name) const { return m_attributes.Contains(name); }

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
			ConfigurationSection* sect = nullptr;
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
			return m_attributes.TryGetValue(name, result);
		}


#define CONFIG_SECT_GETER_IMP(type, typeName, parser) \
		type ConfigurationSection::Get##typeName() const { return parser(m_value); } \
		type ConfigurationSection::Get##typeName(const String& key) const { return parser(getValue(key)); } \
		type ConfigurationSection::GetAttribute##typeName(const String& key) const { return parser(getAttribute(key)); } \
		bool ConfigurationSection::TryGet##typeName(const String& key, type& result) const \
		{ \
			String str; \
			if (tryGetValue(key, str)) \
			{ \
				result = parser(str); \
				return true; \
			} \
			return false; \
		} \
		bool ConfigurationSection::TryGetAttribute##typeName(const String& key, type& result) const \
		{ \
			String str; \
			if (tryGetAttribute(key, str)) \
			{ \
				result = parser(str); \
				return true; \
			} \
			return false; \
		}

		CONFIG_SECT_GETER_IMP(bool, Bool, StringUtils::ParseBool);
		CONFIG_SECT_GETER_IMP(float, Single, StringUtils::ParseSingle);
		CONFIG_SECT_GETER_IMP(float, Percentage, ParsePercentage);
		CONFIG_SECT_GETER_IMP(int32, Int, StringUtils::ParseInt32);
		CONFIG_SECT_GETER_IMP(uint32, UInt, StringUtils::ParseUInt32);
		CONFIG_SECT_GETER_IMP(ColorValue, ColorValue, ParseColorValue);
		CONFIG_SECT_GETER_IMP(Vector3, Vector3, ParseVector3);
		CONFIG_SECT_GETER_IMP(Point, Point, ParsePoint);


#define CONFIG_SECT_SPLITER_IMP(type, typeName, splitParser) \
		type ConfigurationSection::Get##typeName() const \
		{ \
			type result; \
			splitParser(m_value, result); \
			return result; \
		} \
		type ConfigurationSection::Get##typeName(const String& key) const \
		{ \
			type result; \
			splitParser(getValue(key), result); \
			return result; \
		} \
		type ConfigurationSection::GetAttribute##typeName(const String& key) const \
		{ \
			type result; \
			splitParser(getAttribute(key), result); \
			return result; \
		} \
		void ConfigurationSection::Get##typeName(type& result) const { splitParser(m_value, result); } \
		void ConfigurationSection::Get##typeName(const String& key, type& result) const { splitParser(getValue(key), result); } \
		void ConfigurationSection::GetAttribute##typeName(const String& key, type& result) const { splitParser(getAttribute(key), result); } \
		bool ConfigurationSection::TryGet##typeName(const String& key, type& result) const \
		{ \
			String str; \
			if (tryGetValue(key, str)) \
			{ \
				splitParser(str, result); \
				return true; \
			} \
			return false; \
		} \
		bool ConfigurationSection::TryGetAttribute##typeName(const String& key, type& result) const \
		{ \
			String str; \
			if (tryGetAttribute(key, str)) \
			{ \
				splitParser(str, result); \
				return true; \
			} \
			return false; \
		}

		CONFIG_SECT_SPLITER_IMP(List<String>, Strings, SplitString);
		CONFIG_SECT_SPLITER_IMP(FastList<float>, Singles, SplitSingles);
		CONFIG_SECT_SPLITER_IMP(FastList<float>, Percentages, SplitPercentages);
		CONFIG_SECT_SPLITER_IMP(FastList<int32>, Ints, SplitInt);
		CONFIG_SECT_SPLITER_IMP(FastList<uint32>, UInts, SplitUint);
		CONFIG_SECT_SPLITER_IMP(FastList<Vector3>, Vector3s, SplitVector3s);
		CONFIG_SECT_SPLITER_IMP(FastList<Point>, Points, SplitPoints);



#define CONFIG_SECT_SPLITER_ARR_IMP(type, typeName, splitParser) \
		void ConfigurationSection::Get##typeName(type* v, int32 expectedCount, int32* acutallCount) const \
		{ \
			int32 actual = splitParser(m_value, v, expectedCount); \
			if (acutallCount) *acutallCount = actual; \
		} \
		void ConfigurationSection::Get##typeName(const String& key, type* v, int32 expectedCount, int32* acutallCount) const \
		{ \
			int32 actual = splitParser(getValue(key), v, expectedCount); \
			if (acutallCount) *acutallCount = actual; \
		} \
		void ConfigurationSection::GetAttribute##typeName(const String& key, type* v, int32 expectedCount, int32* acutallCount) const \
		{ \
			int32 actual = splitParser(getAttribute(key), v, expectedCount); \
			if (acutallCount) *acutallCount = actual; \
		} \
		bool ConfigurationSection::TryGet##typeName(const String& key, type* v, int32 expectedCount, int32* acutallCount) const \
		{ \
			String str; \
			if (tryGetValue(key, str)) \
			{ \
				int32 actual = splitParser(str, v, expectedCount); \
				if (acutallCount) *acutallCount = actual; \
				return true; \
			} \
			return false; \
		} \
		bool ConfigurationSection::TryGetAttribute##typeName(const String& key, type* v, int32 expectedCount, int32* acutallCount) const \
		{ \
			String str; \
			if (tryGetAttribute(key, str)) \
			{ \
				int32 actual = splitParser(str, v, expectedCount); \
				if (acutallCount) *acutallCount = actual; \
				return true; \
			} \
			return false; \
		}

		CONFIG_SECT_SPLITER_ARR_IMP(float, Singles, SplitSinglesArr);
		CONFIG_SECT_SPLITER_ARR_IMP(float, Percentages, SplitPercentagesArr);
		CONFIG_SECT_SPLITER_ARR_IMP(int32, Ints, SplitIntArr);
		CONFIG_SECT_SPLITER_ARR_IMP(uint32, UInts, SplitUintArr);
		CONFIG_SECT_SPLITER_ARR_IMP(Vector3, Vector3s, SplitVector3sArr);
		CONFIG_SECT_SPLITER_ARR_IMP(Point, Points, SplitPointsArr);

		//////////////////////////////////////////////////////////////////////////
		String SimpleFloatToString(const float& v);

		void ConfigurationSection::AddStringValue(const String& name, const String& value)
		{
			ConfigurationSection* ss = new ConfigurationSection(name);
			ss->SetValue(value);
			AddSection(ss);
		}
		void ConfigurationSection::AddAttributeString(const String& name, const String& value)
		{
			try
			{
				m_attributes.Add(name, value);
				//m_attributes.insert(make_pair(name, value));
			}
			catch (const ApocException& e)
			{
				switch (e.getType())
				{
				case EX_Duplicate:
					LogManager::getSingleton().Write(LOG_System,  L"Attribute with name '" + name + L"' already exists. ", LOGLVL_Warning);
					break;
				}
			}
		}

#define CONFIG_SECT_ADDER_IMP(type, typeName, toStr) \
		void ConfigurationSection::Add##typeName(const String& key, type value) { AddStringValue(key, toStr(value)); } \
		void ConfigurationSection::AddAttribute##typeName(const String& key, type value) { AddAttributeString(key, toStr(value)); }

		CONFIG_SECT_ADDER_IMP(bool, Bool, StringUtils::BoolToString);
		CONFIG_SECT_ADDER_IMP(float, Single, SimpleFloatToString);
		CONFIG_SECT_ADDER_IMP(float, Percentage, PercentageToString);
		CONFIG_SECT_ADDER_IMP(int32, Int, StringUtils::IntToString);
		CONFIG_SECT_ADDER_IMP(uint32, UInt, StringUtils::UIntToString);
		CONFIG_SECT_ADDER_IMP(ColorValue, ColorValue, ColorValueToString);
		CONFIG_SECT_ADDER_IMP(const Vector3&, Vector3, Vector3ToString);
		CONFIG_SECT_ADDER_IMP(const Point&, Point, PointToString);


#define CONFIG_SECT_COMBINER_IMP(type, typeName, combiner) \
		void ConfigurationSection::Add##typeName(const String& key, const type* v, int32 count) \
		{ \
			String result; \
			combiner(v, count, result); \
			AddStringValue(key, result); \
		} \
		void ConfigurationSection::AddAttribute##typeName(const String& name, const type* v, int32 count) \
		{ \
			String result; \
			combiner(v, count, result); \
			AddAttributeString(name, result); \
		}
		
		CONFIG_SECT_COMBINER_IMP(String, Strings, CombineString);
		CONFIG_SECT_COMBINER_IMP(float, Singles, SinglesToString);
		CONFIG_SECT_COMBINER_IMP(float, Percentages, PrecentagesToString);
		CONFIG_SECT_COMBINER_IMP(int32, Ints, IntsToString);
		CONFIG_SECT_COMBINER_IMP(uint32, UInts, UIntsToString);
		CONFIG_SECT_COMBINER_IMP(Vector3, Vector3s, Vector3sToString);
		CONFIG_SECT_COMBINER_IMP(Point, Points, PointsToString);



		


		void ConfigurationSection::SetStringValue(const String& name, const String& value)
		{
			ConfigurationSection* ss = getSection(name); assert(ss);
			ss->SetValue(value);
		}

		void ConfigurationSection::SetBool(bool value)									{ m_value = StringUtils::BoolToString(value); }
		void ConfigurationSection::SetBool(const String& name, bool value)				{ SetStringValue(name, StringUtils::BoolToString(value)); }
		void ConfigurationSection::SetSingle(float value)								{ m_value = StringUtils::SingleToString(value); }
		void ConfigurationSection::SetSingle(const String& name, float value)			{ SetStringValue(name, StringUtils::SingleToString(value)); }
		void ConfigurationSection::SetPercentage(float value)							{ m_value = PercentageToString(value); }
		void ConfigurationSection::SetPercentage(const String& name, float value)		{ SetStringValue(name, PercentageToString(value)); }
		void ConfigurationSection::SetInt(int32 value)									{ m_value = StringUtils::IntToString(value); }
		void ConfigurationSection::SetInt(const String& name, int32 value)				{ SetStringValue(name, StringUtils::IntToString(value)); }
		void ConfigurationSection::SetUInt(uint32 value)								{ m_value = StringUtils::UIntToString(value); }
		void ConfigurationSection::SetUInt(const String& name, uint32 value)			{ SetStringValue(name, StringUtils::UIntToString(value)); }
		void ConfigurationSection::SetColorValue(ColorValue value)						{ m_value = ColorValueToString(value); }
		void ConfigurationSection::SetColorValue(const String& name, ColorValue value)	{ SetStringValue(name, ColorValueToString(value)); }
		void ConfigurationSection::SetVector3(const Vector3& v)							{ m_value = Vector3ToString(v); }
		void ConfigurationSection::SetVector3(const String& name, const Vector3& v)		{ SetStringValue(name, Vector3ToString(v)); }

		void ConfigurationSection::SetStrings(const String* v, int32 count)				{ CombineString(v, count, m_value); }
		void ConfigurationSection::SetSingles(const float* v, int32 count)				{ SinglesToString(v, count, m_value); }
		void ConfigurationSection::SetPercentages(const float* v, int32 count)			{ PrecentagesToString(v, count, m_value); }
		void ConfigurationSection::SetInts(const int32* v, int32 count)					{ IntsToString(v, count, m_value); }
		void ConfigurationSection::SetUInts(const uint32* v, int32 count)				{ UIntsToString(v, count, m_value); }
		void ConfigurationSection::SetVector3s(const Vector3* v, int count)				{ Vector3sToString(v, count, m_value); }

		


		float ParsePercentage(const String& val)
		{
			size_t pos = val.find_last_of('%');
			if (pos != String::npos)
			{
				String ss = val.substr(0, pos);
				return StringUtils::ParseSingle(ss) / 100.0f;
			}
			throw AP_EXCEPTION(EX_FormatException, val);
		}
		String PercentageToString(const float& v)
		{
			String result = StringUtils::SingleToString(v * 100.0f, ConfigurationSection::FloatPointStoringPrecision);
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
			throw AP_EXCEPTION(EX_FormatException, L"Wrong number of channels: " + str);
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
				result.append(StringUtils::UIntToString(r));
				result.append(StringUtils::UIntToString(g));
				result.append(StringUtils::UIntToString(b));
			}
			else
			{
				result.append(StringUtils::UIntToString(r));
				result.append(StringUtils::UIntToString(g));
				result.append(StringUtils::UIntToString(b));
				result.append(StringUtils::UIntToString(a));
			}
			return result;
		}

		Vector3 ParseVector3(const String& str)
		{
			Vector3 v;
			v.Parse(str);
			return v;
		}
		String Vector3ToString(const Vector3& vec) { return vec.ToParsableString(ConfigurationSection::FloatPointStoringPrecision); }

		Point ParsePoint(const String& str)
		{
			int32 vals[2];
			StringUtils::SplitParseIntsChecked(str, vals, 2, L", ");
			
			return Point(vals[0], vals[1]);
		}
		String PointToString(const Point& vec)
		{
			int32 vals[2] = {vec.X, vec.Y};
			String result;
			IntsToString(vals, 2, result);
			return result;
		}


		void CombineString(const String* v, int count, String& result)
		{
			for (int i=0;i<count;i++)
			{
				if (v[i].find(',') != String::npos)
				{
					result.append(1, '"');
					result.append(v[i]);
					result.append(1, '"');
				}
				else
				{
					result.append(v[i]);
				}
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

		template <typename ListElementT, typename ElementT, ElementT (*TConverter)(const String&) >
		void SplitT(const String& str, ListElementT& result, const String& delims)
		{
			assert(result.getCount() == 0);

			// Use STL methods 
			size_t start, pos;
			start = 0;
			do 
			{
				pos = str.find_first_of(delims, start);
				if (pos == start)
				{
					// Do nothing
					start = pos + 1;
				}
				else if (pos == String::npos)
				{
					// Copy the rest of the string
					result.Add(TConverter(str.substr(start) ));
					break;
				}
				else
				{
					// Copy up to delimiter
					result.Add(TConverter(str.substr(start, pos - start) ));
					start = pos + 1;
				}
				// parse up to next real data
				start = str.find_first_not_of(delims, start);

			} while (pos != String::npos);
		}

		template <typename ElementT, String (*ToStringConverter)(const ElementT&) >
		void GenericToString(const ElementT* array, int32 count, String& result)
		{
			for (int32 i=0;i<count;i++)
			{
				result.append(ToStringConverter(array[i]));
				if (i != count -1)
					result.append(L", ");
			}
		}


		void ColorValuesToString(const ColorValue* v, int count, String& result);
		void Vector3sToString(const Vector3* v, int count, String& result);
		void PointsToString(const Point* v, int count, String& result);

		void SplitSingles(const String& str, FastList<float>& result);
		void SplitPercentages(const String& str, FastList<float>& result);
		void SplitInt(const String& str, FastList<int32>& result);
		void SplitUint(const String& str, FastList<uint32>& result);
		



		void SplitSingles(const String& text, FastList<float>& result) { StringUtils::SplitParseSingles(text, result, L", "); } 
		void SplitInt(const String& text, FastList<int32>& result) { StringUtils::SplitParseInts(text, result, L", "); } 
		void SplitPercentages(const String& text, FastList<float>& result) { SplitT<FastList<float>, float, ParsePercentage>(text, result, L", "); }
		void SplitUint(const String& text, FastList<uint32>& result) { SplitT<FastList<uint32>, uint32, StringUtils::ParseUInt32>(text, result, L", "); }
		void SplitVector3s(const String& str, FastList<Vector3>& result) 
		{
			FastList<float> buffer;
			StringUtils::SplitParseSingles(str, buffer, L", ");

			assert((buffer.getCount()%3) == 0);
			int32 vecCount = buffer.getCount()/3;
			result.ReserveDiscard(vecCount);
			for (int32 i=0;i<vecCount;i++)
			{
				Vector3& v = result[i];
				v.X = buffer[i*3];
				v.Y = buffer[i*3+1];
				v.Z = buffer[i*3+2];
			}
		}
		void SplitPoints(const String& str, FastList<Point>& result)
		{
			FastList<int32> buffer;
			StringUtils::SplitParseInts(str, buffer, L", ");

			assert((buffer.getCount()%2) == 0);
			int32 vecCount = buffer.getCount()/2;
			result.ReserveDiscard(vecCount);
			for (int32 i=0;i<vecCount;i++)
			{
				Point& v = result[i];
				v.X = buffer[i*2];
				v.Y = buffer[i*2+1];
			}
		}



		String SimpleInt32ToString(const int32& v) { return StringUtils::IntToString(v); }
		String SimpleUInt32ToString(const uint32& v) { return StringUtils::UIntToString(v); }
		String SimpleFloatToString(const float& v) { return StringUtils::SingleToString(v, ConfigurationSection::FloatPointStoringPrecision); }
		

		void IntsToString(const int32* v, int count, String& result) { GenericToString<int32, SimpleInt32ToString>(v, count, result); }
		void UIntsToString(const uint32* v, int count, String& result) { GenericToString<uint32, SimpleUInt32ToString>(v, count, result); }
		void SinglesToString(const float* v, int count, String& result) { GenericToString<float, SimpleFloatToString>(v, count, result); }
		void PrecentagesToString(const float* v, int count, String& result) { GenericToString<float, PercentageToString>(v, count, result); }
		void Vector3sToString(const Vector3* v, int count, String& result) { GenericToString<Vector3, Vector3ToString>(v, count, result); }
		void PointsToString(const Point* v, int count, String& result) { GenericToString<Point, PointToString>(v, count, result); }



		template <typename T>
		class CappedBufferList
		{
		public:
			CappedBufferList(T* dataBuf, int32 sizeCap)
				: m_elements(dataBuf), m_sizeCap(sizeCap), m_internalPointer(0)
			{
			}

			void Add(const T& item)
			{
				assert(m_internalPointer<m_sizeCap);
				m_elements[m_internalPointer++] = item;
			}

			int32 getCount() const { return m_internalPointer; }
		private:
			T* m_elements;
			int32 m_sizeCap;

			int32 m_internalPointer;
		};

		int32 SplitSinglesArr(const String& str, float* result, int32 expectedCount) { return StringUtils::SplitParseSingles(str, result, expectedCount, L", "); }
		int32 SplitPercentagesArr(const String& str, float* result, int32 expectedCount)
		{
			CappedBufferList<float> lst(result, expectedCount);
			SplitT<CappedBufferList<float>, float, StringUtils::ParseSingle>(str, lst, L", ");
			return lst.getCount();
		}
		int32 SplitIntArr(const String& str, int32* result, int32 expectedCount) { return StringUtils::SplitParseInts(str, result, expectedCount, L", "); }
		int32 SplitUintArr(const String& str, uint32* result, int32 expectedCount)
		{
			CappedBufferList<uint32> lst(result, expectedCount);
			SplitT<CappedBufferList<uint32>, uint32, StringUtils::ParseUInt32>(str, lst, L", ");
			return lst.getCount();
		}
		int32 SplitVector3sArr(const String& str, Vector3* result, int32 expectedCount)
		{
			int32 actual = StringUtils::SplitParseSingles(str, (float*)result, expectedCount*3, L", ");

			assert((actual%3) == 0);

			return actual/3;
		}
		int32 SplitPointsArr(const String& str, Point* result, int32 expectedCount)
		{
			int32 actual = StringUtils::SplitParseInts(str, (int32*)result, expectedCount*2, L", ");
			
			assert((actual%2) == 0);

			return actual/2;
		}


	}
}