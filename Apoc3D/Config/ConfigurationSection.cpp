/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2011-2017 Tao Xin
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

#include "ConfigurationSection.h"

#include "ConfigurationManager.h"

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
		bool ConfigurationSection::FloatPointCustomStoringPrecision = false;

		uint64 GetCurrentFPFlags()
		{
			uint64 flags = ConfigurationSection::FloatPointCustomStoringPrecision ?
				StrFmt::FP(ConfigurationSection::FloatPointStoringPrecision) : StringUtils::SF_Default;
			return flags;
		}

		template <typename T>
		T SimpleReturn(const T& v) { return v; }

		String SimpleFloatToString(const float& v);

		/** Actual parsing/printing functions */
		float ParsePercentage(const String& val);
		String PercentageToString(const float& v);

		ColorValue ParseColorValue(const String& str);
		String ColorValueToString(ColorValue v);

		Vector3 ParseVector3(const String& str);
		String Vector3ToString(const Vector3& vec);

		Point ParsePoint(const String& str);
		String PointToString(const Point& vec);

		void CombineString(const String* v, int32 count, String& result) { StringUtils::Pack<String, SimpleReturn<String>>(v, count, result); }
		void SplitString(const String& str, List<String>& result);

		void SinglesToString(const float* v, int32 count, String& result);
		void PrecentagesToString(const float* v, int32 count, String& result);
		void IntsToString(const int32* v, int32 count, String& result);
		void UIntsToString(const uint32* v, int32 count, String& result);
		void Vector3sToString(const Vector3* v, int32 count, String& result);
		void PointsToString(const Point* v, int32 count, String& result);

		template <int32 N> void IntsToString(const int32(&v)[N], String& result) { IntsToString(v, N, result); }


		void SplitSingles(const String& str, List<float>& result);
		void SplitPercentages(const String& str, List<float>& result);
		void SplitInt(const String& str, List<int32>& result);
		void SplitUint(const String& str, List<uint32>& result);
		void SplitVector3s(const String& str, List<Vector3>& result);
		void SplitPoints(const String& str, List<Point>& result);

		int32 SplitSinglesArr(const String& str, float* result, int32 expectedCount);
		int32 SplitPercentagesArr(const String& str, float* result, int32 expectedCount);
		int32 SplitIntArr(const String& str, int32* result, int32 expectedCount);
		int32 SplitUintArr(const String& str, uint32* result, int32 expectedCount);
		int32 SplitVector3sArr(const String& str, Vector3* result, int32 expectedCount);
		int32 SplitPointsArr(const String& str, Point* result, int32 expectedCount);

		/************************************************************************/
		/*   ConfigurationSection                                               */
		/************************************************************************/

		ConfigurationSection::ConfigurationSection(const String& name)
			: m_name(name) { }

		ConfigurationSection::ConfigurationSection(const String& name, int32 capacity)
			: m_name(name), m_subSection(capacity) { }

		ConfigurationSection::ConfigurationSection(const String& name, const String& value)
			: m_name(name), m_value(value) { }

		ConfigurationSection::ConfigurationSection(const String& name,
			std::initializer_list<std::pair<String, String>> attribs,
			std::initializer_list<ConfigurationSection> values)
			: m_name(name), m_attributes(attribs), m_subSection((int32)values.size())
		{
			for (const auto& e : values)
			{
				m_subSection.Add(e.getName(), new ConfigurationSection(e));
			}
		}

		ConfigurationSection::ConfigurationSection(const ConfigurationSection& o)
			: m_name(o.m_name), m_value(o.m_value), m_attributes(o.m_attributes), m_subSection(o.m_subSection)
		{
			DeepCopySubsections();
		}

		ConfigurationSection::ConfigurationSection(ConfigurationSection&& o)
			: m_name(std::move(o.m_name)), m_value(std::move(o.m_value)), m_attributes(std::move(o.m_attributes)), m_subSection(std::move(o.m_subSection))
		{ }

		ConfigurationSection::~ConfigurationSection()
		{
			for (ConfigurationSection* s : m_subSection.getValueAccessor())
			{
				delete s;
			}
			m_subSection.Clear();
		}

		ConfigurationSection& ConfigurationSection::operator=(const ConfigurationSection& o)
		{
			if (this != &o)
			{
				m_name = o.m_name;
				m_value = o.m_value;

				m_attributes = o.m_attributes;
				m_subSection = o.m_subSection;

				DeepCopySubsections();
			}
			return *this;
		}
		ConfigurationSection& ConfigurationSection::operator=(ConfigurationSection&& o)
		{
			if (this != &o)
			{
				m_name = std::move(o.m_name);
				m_value = std::move(o.m_value);

				m_attributes = std::move(o.m_attributes);
				m_subSection = std::move(o.m_subSection);
			}
			return *this;
		}

		void ConfigurationSection::DeepCopySubsections()
		{
			for (ConfigurationSection*& s : m_subSection.getValueAccessor())
			{
				ConfigurationSection* newSect = new ConfigurationSection(*s);

				s = newSect;
			}
		}

		ConfigurationSection* ConfigurationSection::CreateSubSection(const String& name)
		{
			ConfigurationSection* sect;

			if (m_subSection.TryGetValue(name, sect))
			{
				LogManager::getSingleton().Write(LOG_System, L"Configuration Section with name '" + name + L"' already exists. Ignored.", LOGLVL_Warning);
			}
			else
			{
				sect = new ConfigurationSection(name);
				m_subSection.Add(name, sect);
			}
			return sect;
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
			if (m_value != value && 
				!m_value.empty())
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

		const String* ConfigurationSection::tryGetValue(const String& name) const
		{
			ConfigurationSection* sect;
			if (m_subSection.TryGetValue(name, sect))
				return &sect->getValue();
			return nullptr;
		}
		const String* ConfigurationSection::tryGetAttribute(const String& name) const
		{
			return m_attributes.TryGetValue(name);
		}


#define CONFIG_SECT_GETER_IMP(type, typeName, parser) \
		type ConfigurationSection::Get##typeName() const { return parser(m_value); } \
		type ConfigurationSection::Get##typeName(const String& key) const { return parser(getValue(key)); } \
		type ConfigurationSection::GetAttribute##typeName(const String& key) const { return parser(getAttribute(key)); } \
		bool ConfigurationSection::TryGet##typeName(const String& key, type& result) const \
		{ \
			const String* str = tryGetValue(key); \
			if (str) \
			{ \
				result = parser(*str); \
				return true; \
			} \
			return false; \
		} \
		bool ConfigurationSection::TryGetAttribute##typeName(const String& key, type& result) const \
		{ \
			const String* str = tryGetAttribute(key); \
			if (str) \
			{ \
				result = parser(*str); \
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
			const String* str = tryGetValue(key); \
			if (str) \
			{ \
				splitParser(*str, result); \
				return true; \
			} \
			return false; \
		} \
		bool ConfigurationSection::TryGetAttribute##typeName(const String& key, type& result) const \
		{ \
			const String* str = tryGetAttribute(key); \
			if (str) \
			{ \
				splitParser(*str, result); \
				return true; \
			} \
			return false; \
		}

		CONFIG_SECT_SPLITER_IMP(List<String>, Strings, SplitString);
		CONFIG_SECT_SPLITER_IMP(List<float>, Singles, SplitSingles);
		CONFIG_SECT_SPLITER_IMP(List<float>, Percentages, SplitPercentages);
		CONFIG_SECT_SPLITER_IMP(List<int32>, Ints, SplitInt);
		CONFIG_SECT_SPLITER_IMP(List<uint32>, UInts, SplitUint);
		CONFIG_SECT_SPLITER_IMP(List<Vector3>, Vector3s, SplitVector3s);
		CONFIG_SECT_SPLITER_IMP(List<Point>, Points, SplitPoints);



#define CONFIG_SECT_SPLITER_ARR_IMP(type, typeName, splitParser) \
		void ConfigurationSection::Get##typeName(type* v, int32 maxCount, int32* acutallCount) const \
		{ \
			int32 actual = splitParser(m_value, v, maxCount); \
			if (acutallCount) *acutallCount = actual; \
		} \
		void ConfigurationSection::Get##typeName(const String& key, type* v, int32 maxCount, int32* acutallCount) const \
		{ \
			int32 actual = splitParser(getValue(key), v, maxCount); \
			if (acutallCount) *acutallCount = actual; \
		} \
		void ConfigurationSection::GetAttribute##typeName(const String& key, type* v, int32 maxCount, int32* acutallCount) const \
		{ \
			int32 actual = splitParser(getAttribute(key), v, maxCount); \
			if (acutallCount) *acutallCount = actual; \
		} \
		bool ConfigurationSection::TryGet##typeName(const String& key, type* v, int32 maxCount, int32* acutallCount) const \
		{ \
			const String* str = tryGetValue(key); \
			if (str) \
			{ \
				int32 actual = splitParser(*str, v, maxCount); \
				if (acutallCount) *acutallCount = actual; \
				return true; \
			} \
			return false; \
		} \
		bool ConfigurationSection::TryGetAttribute##typeName(const String& key, type* v, int32 maxCount, int32* acutallCount) const \
		{ \
			const String* str = tryGetAttribute(key); \
			if (str) \
			{ \
				int32 actual = splitParser(*str, v, maxCount); \
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

		void ConfigurationSection::AddStringValue(const String& name, const String& value)
		{
			ConfigurationSection* ss = new ConfigurationSection(name);
			ss->SetValue(value);
			AddSection(ss);
		}
		void ConfigurationSection::AddAttributeString(const String& name, const String& value)
		{
			if (!m_attributes.TryAdd(name, value))
			{
				LogManager::getSingleton().Write(LOG_System, L"Attribute with name '" + name + L"' already exists. ", LOGLVL_Warning);
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
		void ConfigurationSection::SetVector3s(const Vector3* v, int32 count)				{ Vector3sToString(v, count, m_value); }
		
		void ConfigurationSection::SetStrings(const List<String>& v)					{ SetStrings(&v[0], v.getCount()); }
		void ConfigurationSection::SetSingles(const List<float>& v)						{ SetSingles(&v[0],  v.getCount()); }
		void ConfigurationSection::SetPercentages(const List<float>& v)					{ SetPercentages(&v[0],  v.getCount()); }
		void ConfigurationSection::SetInts(const List<int32>& v)						{ SetInts(&v[0],  v.getCount()); }
		void ConfigurationSection::SetUInts(const List<uint32>& v)						{ SetUInts(&v[0],  v.getCount()); }


		void ConfigurationSection::Merge(const ConfigurationSection* thatSect, bool noMessages)
		{
			// merge attributes
			for (auto e : thatSect->getAttributes())
			{
				if (!hasAttribute(e.Key))
					AddAttributeString(e.Key, e.Value);
				else if (!noMessages)
				{
					ApocLog(LOG_System, L"[Configuration] " + getName() +
						L": Ignoring duplicated attribute " + e.Key + L" in " + getName() + L".", LOGLVL_Warning);
				}
			}

			if (thatSect->getValue().size())
			{
				if (getValue().empty())
				{
					SetValue(thatSect->getValue());
				}
				else if (!noMessages && getValue() != thatSect->getValue())
				{
					ApocLog(LOG_System, L"[Configuration] " + getName() +
						L": Merging section " + getName() + L" has conflicting values.", LOGLVL_Warning);
				}
			}


			// merge sub sections
			for (const ConfigurationSection* thatSubSect : thatSect->getSubSections())
			{
				ConfigurationSection* thisSubSect = getSection(thatSubSect->getName());

				if (thisSubSect)
					thisSubSect->Merge(thatSubSect, noMessages);
				else
					AddSection(new ConfigurationSection(*thatSubSect));
			}
		}
		void ConfigurationSection::Merge(const ConfigurationSection* thatSect, bool noMessages, const String& newSectionName)
		{
			Merge(thatSect, noMessages);
			m_name = newSectionName;
		}
		void ConfigurationSection::RemoveIntersection(const ConfigurationSection* thatSect)
		{
			for (const ConfigurationSection* thatSubSect : thatSect->getSubSections())
			{
				ConfigurationSection* thisSubSect = getSection(thatSubSect->getName());

				if (thisSubSect)
				{
					thisSubSect->RemoveIntersection(thatSubSect);

					if (thisSubSect->getSubSectionCount() == 0 && thatSubSect->getSubSectionCount() == 0 &&
						thisSubSect->getAttributeCount() == 0 && thatSubSect->getAttributeCount() == 0 &&
						thisSubSect->getValue() == thatSubSect->getValue())
					{
						m_subSection.RemoveAndDelete(thisSubSect->getName());
						continue;
					}
				}
			}

			for (auto e : thatSect->getAttributes())
			{
				if (hasAttribute(e.Key) && e.Value == getAttribute(e.Key))
				{
					m_attributes.Remove(e.Key);
				}
			}
		}

		void ConfigurationSection::RemoveAttribute(const String& name)
		{
			m_attributes.Remove(name);
		}

		int32 ConfigurationSection::GetHashCode() const
		{
			FNVHash32 hasher;

			hasher.Accumulate(m_name.c_str(), sizeof(String::value_type) * m_name.size());
			hasher.Accumulate(m_value.c_str(), sizeof(String::value_type) * m_value.size());

			for (auto e : m_attributes)
			{
				hasher.Accumulate(e.Key.c_str(), sizeof(String::value_type) * e.Key.size());
				hasher.Accumulate(e.Value.c_str(), sizeof(String::value_type) * e.Value.size());
			}

			int32 h = static_cast<int32>(hasher.getResult());

			for (ConfigurationSection* s : m_subSection.getValueAccessor())
			{
				h ^= s->GetHashCode();
			}
			return h;
		}

		/************************************************************************/
		/* ParameterDictionary                                                  */
		/************************************************************************/

#define PDICT_GETER_IMP(type, typeName, parser) \
		type ParameterDictionary::Get##typeName(const String& key) const { return parser(this->operator[](key)); } \
		bool ParameterDictionary::TryGet##typeName(const String& key, type& result) const \
		{ \
			const String* str = TryGetValue(key); \
			if (str) \
			{ \
				result = parser(*str); \
				return true; \
			} \
			return false; \
		} \

#define PDICT_SPLITER_IMP(type, typeName, splitParser) \
		type ParameterDictionary::Get##typeName(const String& key) const \
		{ \
			type result; \
			splitParser(this->operator[](key), result); \
			return result; \
		} \
		void ParameterDictionary::Get##typeName(const String& key, type& result) const { splitParser(this->operator[](key), result); } \
		bool ParameterDictionary::TryGet##typeName(const String& key, type& result) const \
		{ \
			const String* str = TryGetValue(key); \
			if (str) \
			{ \
				splitParser(*str, result); \
				return true; \
			} \
			return false; \
		} 

#define PDICT_SPLITER_ARR_IMP(type, typeName, splitParser) \
		void ParameterDictionary::Get##typeName(const String& key, type* v, int32 maxCount, int32* acutallCount) const \
		{ \
			int32 actual = splitParser(this->operator[](key), v, maxCount); \
			if (acutallCount) *acutallCount = actual; \
		} \
		bool ParameterDictionary::TryGet##typeName(const String& key, type* v, int32 maxCount, int32* acutallCount) const \
		{ \
			const String* str = TryGetValue(key); \
			if (str) \
			{ \
				int32 actual = splitParser(*str, v, maxCount); \
				if (acutallCount) *acutallCount = actual; \
				return true; \
			} \
			return false; \
		} 

#define PDICT_ADDER_IMP(type, typeName, toStr) void ParameterDictionary::Add##typeName(const String& key, type value) { Add(key, toStr(value)); }

#define PDICT_COMBINER_IMP(type, typeName, combiner) \
		void ParameterDictionary::Add##typeName(const String& key, const type* v, int32 count) \
		{ \
			String result; \
			combiner(v, count, result); \
			Add(key, result); \
		}


		PDICT_GETER_IMP(bool, Bool, StringUtils::ParseBool);
		PDICT_GETER_IMP(float, Single, StringUtils::ParseSingle);
		PDICT_GETER_IMP(float, Percentage, ParsePercentage);
		PDICT_GETER_IMP(int32, Int, StringUtils::ParseInt32);
		PDICT_GETER_IMP(uint32, UInt, StringUtils::ParseUInt32);
		PDICT_GETER_IMP(ColorValue, ColorValue, ParseColorValue);
		PDICT_GETER_IMP(Vector3, Vector3, ParseVector3);
		PDICT_GETER_IMP(Point, Point, ParsePoint);
		
		PDICT_SPLITER_IMP(List<String>, Strings, SplitString);
		PDICT_SPLITER_IMP(List<float>, Singles, SplitSingles);
		PDICT_SPLITER_IMP(List<float>, Percentages, SplitPercentages);
		PDICT_SPLITER_IMP(List<int32>, Ints, SplitInt);
		PDICT_SPLITER_IMP(List<uint32>, UInts, SplitUint);
		PDICT_SPLITER_IMP(List<Vector3>, Vector3s, SplitVector3s);
		PDICT_SPLITER_IMP(List<Point>, Points, SplitPoints);
		
		PDICT_SPLITER_ARR_IMP(float, Singles, SplitSinglesArr);
		PDICT_SPLITER_ARR_IMP(float, Percentages, SplitPercentagesArr);
		PDICT_SPLITER_ARR_IMP(int32, Ints, SplitIntArr);
		PDICT_SPLITER_ARR_IMP(uint32, UInts, SplitUintArr);
		PDICT_SPLITER_ARR_IMP(Vector3, Vector3s, SplitVector3sArr);
		PDICT_SPLITER_ARR_IMP(Point, Points, SplitPointsArr);
		
		PDICT_ADDER_IMP(bool, Bool, StringUtils::BoolToString);
		PDICT_ADDER_IMP(float, Single, SimpleFloatToString);
		PDICT_ADDER_IMP(float, Percentage, PercentageToString);
		PDICT_ADDER_IMP(int32, Int, StringUtils::IntToString);
		PDICT_ADDER_IMP(uint32, UInt, StringUtils::UIntToString);
		PDICT_ADDER_IMP(ColorValue, ColorValue, ColorValueToString);
		PDICT_ADDER_IMP(const Vector3&, Vector3, Vector3ToString);
		PDICT_ADDER_IMP(const Point&, Point, PointToString);

		PDICT_COMBINER_IMP(String, Strings, CombineString);
		PDICT_COMBINER_IMP(float, Singles, SinglesToString);
		PDICT_COMBINER_IMP(float, Percentages, PrecentagesToString);
		PDICT_COMBINER_IMP(int32, Ints, IntsToString);
		PDICT_COMBINER_IMP(uint32, UInts, UIntsToString);
		PDICT_COMBINER_IMP(Vector3, Vector3s, Vector3sToString);
		PDICT_COMBINER_IMP(Point, Points, PointsToString);


		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////

		float ParsePercentage(const String& val)
		{
			size_t pos = val.find_last_of('%');
			if (pos != String::npos)
			{
				String ss = val.substr(0, pos);
				return StringUtils::ParseSingle(ss) / 100.0f;
			}
			AP_EXCEPTION(ErrorID::FormatException, val);
			return 0;
		}
		String PercentageToString(const float& v)
		{
			String result = StringUtils::SingleToString(v * 100.0f, GetCurrentFPFlags());
			result.append(L"%");
			return result;
		}

		ColorValue ParseColorValue(const String& str)
		{
			int32 val[4];
			int32 count = StringUtils::SplitParseInts(str, val, countof(val),  L",");
			if (count == 4)
			{
				const uint r = val[0];
				const uint g = val[1];
				const uint b = val[2];
				const uint a = val[3];

				return CV_PackColor(r, g, b, a);
			}
			else if (count == 3)
			{
				const uint r = val[0];
				const uint g = val[1];
				const uint b = val[2];
				const uint a = 0xff;

				return CV_PackColor(r, g, b, a);
			}
			AP_EXCEPTION(ErrorID::FormatException, L"Wrong number of channels: " + str);
			return 0;
		}
		String ColorValueToString(ColorValue v)
		{
			uint a = CV_GetColorA(v);
			uint r = CV_GetColorR(v);
			uint g = CV_GetColorG(v);
			uint b = CV_GetColorB(v);

			String result;
			if (a == 0xff)
			{
				int32 vals[3] = { (int32)r, (int32)g, (int32)b };
				IntsToString(vals, result);
			}
			else
			{
				int32 vals[4] = { (int32)r, (int32)g, (int32)b, (int32)a };
				IntsToString(vals, result);
			}
			return result;
		}

		Vector3 ParseVector3(const String& str)		{ Vector3 v; v.Parse(str); return v; }
		String Vector3ToString(const Vector3& vec)	{ return vec.ToParsableString(ConfigurationSection::FloatPointStoringPrecision); }

		Point ParsePoint(const String& str)
		{
			int32 vals[2];
			StringUtils::SplitParseIntsChecked(str, vals, 2, L", ");

			return Point(vals[0], vals[1]);
		}
		String PointToString(const Point& vec)
		{
			int32 vals[2] = { vec.X, vec.Y };
			String result;
			IntsToString(vals, result);
			return result;
		}



		void SplitString(const String& str, List<String>& result)
		{
			bool isIn = false;
			bool preventTriming = false;

			String buffer;
			for (size_t i = 0; i < str.size(); i++)
			{
				wchar_t ch = str[i];
				if (ch == '"')
				{
					isIn = !isIn;
					if (isIn)
						preventTriming = true;
				}
				else if (ch == ',' && !isIn)
				{
					if (!preventTriming)
						StringUtils::Trim(buffer);

					result.Add(buffer);
					buffer = String();
					preventTriming = false;
				}
				else
				{
					buffer.append(&ch, 1);
				}
			}
			if (buffer.size())
			{
				if (!preventTriming)
					StringUtils::Trim(buffer);

				if (buffer.size())
					result.Add(buffer);
			}
		}



		void SplitSingles(const String& text, List<float>& result)		{ StringUtils::SplitParseSingles(text, result, L", "); } 
		void SplitInt(const String& text, List<int32>& result)			{ StringUtils::SplitParseInts(text, result, L", "); } 
		void SplitPercentages(const String& text, List<float>& result)	{ StringUtils::SplitParse<String, List<float>, float, ParsePercentage>(text, result, L", "); }
		void SplitUint(const String& text, List<uint32>& result)		{ StringUtils::SplitParse<String, List<uint32>, uint32, StringUtils::ParseUInt32>(text, result, L", "); }
		void SplitVector3s(const String& str, List<Vector3>& result)
		{
			List<float> buffer;
			StringUtils::SplitParseSingles(str, buffer, L", ");

			assert((buffer.getCount() % 3) == 0);
			int32 vecCount = buffer.getCount() / 3;
			result.ReserveDiscard(vecCount);
			for (int32 i = 0; i < vecCount; i++)
			{
				Vector3& v = result[i];
				v.X = buffer[i * 3];
				v.Y = buffer[i * 3 + 1];
				v.Z = buffer[i * 3 + 2];
			}
		}
		void SplitPoints(const String& str, List<Point>& result)
		{
			List<int32> buffer;
			StringUtils::SplitParseInts(str, buffer, L", ");

			assert((buffer.getCount() % 2) == 0);
			int32 vecCount = buffer.getCount() / 2;
			result.ReserveDiscard(vecCount);
			for (int32 i = 0; i < vecCount; i++)
			{
				Point& v = result[i];
				v.X = buffer[i * 2];
				v.Y = buffer[i * 2 + 1];
			}
		}



		String SimpleInt32ToString(const int32& v)		{ return StringUtils::IntToString(v); }
		String SimpleUInt32ToString(const uint32& v)	{ return StringUtils::UIntToString(v); }
		String SimpleFloatToString(const float& v)		{ return StringUtils::SingleToString(v, GetCurrentFPFlags()); }
		
		void IntsToString(const int32* v, int32 count, String& result)		{ StringUtils::Pack<int32, SimpleInt32ToString>(v, count, result); }
		void UIntsToString(const uint32* v, int32 count, String& result)		{ StringUtils::Pack<uint32, SimpleUInt32ToString>(v, count, result); }
		void SinglesToString(const float* v, int32 count, String& result)		{ StringUtils::Pack<float, SimpleFloatToString>(v, count, result); }
		void PrecentagesToString(const float* v, int32 count, String& result) { StringUtils::Pack<float, PercentageToString>(v, count, result); }
		void Vector3sToString(const Vector3* v, int32 count, String& result)	{ StringUtils::Pack<Vector3, Vector3ToString>(v, count, result); }
		void PointsToString(const Point* v, int32 count, String& result)		{ StringUtils::Pack<Point, PointToString>(v, count, result); }




		int32 SplitSinglesArr(const String& str, float* result, int32 expectedCount) { return StringUtils::SplitParseSingles(str, result, expectedCount, L", "); }
		int32 SplitPercentagesArr(const String& str, float* result, int32 expectedCount)
		{
			WrappedList<float> lst(result, expectedCount);
			StringUtils::SplitParse<String, WrappedList<float>, float, ParsePercentage>(str, lst, L", ");
			return lst.getCount();
		}
		int32 SplitIntArr(const String& str, int32* result, int32 expectedCount) { return StringUtils::SplitParseInts(str, result, expectedCount, L", "); }
		int32 SplitUintArr(const String& str, uint32* result, int32 expectedCount)
		{
			WrappedList<uint32> lst(result, expectedCount);
			StringUtils::SplitParse<String, WrappedList<uint32>, uint32, StringUtils::ParseUInt32>(str, lst, L", ");
			return lst.getCount();
		}
		int32 SplitVector3sArr(const String& str, Vector3* result, int32 expectedCount)
		{
			int32 actual = StringUtils::SplitParseSingles(str, (float*)result, expectedCount * 3, L", ");

			assert((actual % 3) == 0);
			return actual / 3;
		}
		int32 SplitPointsArr(const String& str, Point* result, int32 expectedCount)
		{
			int32 actual = StringUtils::SplitParseInts(str, (int32*)result, expectedCount * 2, L", ");

			assert((actual % 2) == 0);
			return actual / 2;
		}


	}
}