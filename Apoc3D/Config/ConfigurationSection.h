#pragma once
#ifndef APOC3D_CONFIGURATIONSECTION_H
#define APOC3D_CONFIGURATIONSECTION_H

/**
 * -----------------------------------------------------------------------------
 * This source file is part of Apoc3D Engine
 * 
 * Copyright (c) 2009+ Tao Games
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


#include "apoc3d/Common.h"
#include "apoc3d/Math/ColorValue.h"
#include "apoc3d/Collections/FastMap.h"

using namespace Apoc3D::Math;
using namespace Apoc3D::Core;
using namespace Apoc3D::Collections;
using namespace std;

namespace Apoc3D
{
	namespace Config
	{
		/** 
		 * Represents a section in a configuration.
		 *
		 * @remarks
		 *  As tree node in configuration, each section can have a name, a value
		 *  and several attributes. Particular value parsing functions are also provided.
		 *  The parsing will convert a string representation to the target data type.
		 *
		 *  The ConfigurationSection can get value in a key-value way, beside attributes,
		 *  by treating all the sub-sections as value with the name, which is the key.
		 *
		 *  Ideally, if the sub section does not have any children, its config file
		 *  representation should be well understandable. For instance, in a xml file:
		 *  <Section>
		 *    <SubSect1>Value1</SubSect1>
		 *    <Key2>Value2</Key2>
		 *  </Section>
		 *  At the "Section" level, SubSect1 and Key2 and their value(like Value1, Value2)
		 *  can be obtained by providing their name as the key, by using getXXX method.
		 */
		class APAPI ConfigurationSection
		{
		public:
			typedef FastMap<String, String> AttributeTable;
			typedef FastMap<String, ConfigurationSection*> SubSectionTable;

			typedef SubSectionTable::Enumerator SubSectionEnumerator;
			typedef AttributeTable::Enumerator AttributeEnumerator;

			ConfigurationSection(const String& name, int capacity)
				: m_name(name), m_subSection(capacity, Apoc3D::Collections::IBuiltInEqualityComparer<String>::Default)
			{ }
			ConfigurationSection(const String& name)
				: m_name(name) 
			{ }
			ConfigurationSection(const ConfigurationSection& another);
			~ConfigurationSection()
			{
				for (SubSectionTable::Enumerator e = m_subSection.GetEnumerator();e.MoveNext();)
				{
					ConfigurationSection* sect = *e.getCurrentValue();
					delete sect;
				}
			}

			AttributeTable::Enumerator GetAttributeEnumrator() const { return m_attributes.GetEnumerator(); }
			SubSectionTable::Enumerator GetSubSectionEnumrator() const { return m_subSection.GetEnumerator(); }

			void AddSection(ConfigurationSection* section);
			void SetValue( const String& value);

			const String& getName() const { return m_name; }
			/**
			 *  Gets the value of the sub-section with the given name.
			 */
			const String& getValue(const String& name) const;
			const String& getValue() const { return m_value; }
			const String& getAttribute(const String& name) const;

			bool hasAttribute(const String& name) const;

			bool tryGetValue(const String& name, String& result) const;
			bool tryGetAttribute(const String& name, String& result) const;

			ConfigurationSection* getSection(const String& name) const;


			void Get(const String& key, IParsable* value) const;

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

			int getAttributeCount() const { return m_attributes.getCount(); }
			int getSubSectionCount() const { return m_subSection.getCount(); }

			void AddStringValue(const String& name, const String& value);

			void AddBool(const String& key, bool value);
			void AddSingle(const String& key, float value);
			void AddPercentage(const String& key, float value);
			void AddInt(const String& key, int32 value);
			void AddUInt(const String& key, uint32 value);
			void AddColorValue(const String& key, ColorValue value);
			
			void AddStrings(const String& key, const String* v, int count);
			void AddSingles(const String& key, const float* v, int count);
			void AddPercentages(const String& key, const float* v, int count);
			void AddInts(const String& key, const int32* v, int count);
			void AddUInts(const String& key, const uint32* v, int count);
			
			void AddStrings(const String& key, const std::vector<String>& v) { AddStrings(key, &v[0], static_cast<int>(v.size())); }
			void AddSingles(const String& key, const std::vector<float>& v) { AddSingles(key, &v[0], static_cast<int>(v.size())); }
			void AddPercentages(const String& key, const std::vector<float>& v) { AddPercentages(key, &v[0], static_cast<int>(v.size())); }
			void AddInts(const String& key, const std::vector<int32>& v) { AddInts(key, &v[0], static_cast<int>(v.size())); }
			void AddUInts(const String& key, const std::vector<uint32>& v) { AddUInts(key, &v[0], static_cast<int>(v.size())); }


			void AddAttributeString(const String& name, const String& value);
			void AddAttributeBool(const String& name, bool val);
			void AddAttributeSingle(const String& name, float val);
			void AddAttributePercentage(const String& name, float val);
			void AddAttributeInt(const String& name, int32 val);
			void AddAttributeUInt(const String& name, uint32 val);
			void AddAttributeColorValue(const String& name, ColorValue val);

			void AddAttributeStrings(const String& name, const String* v, int count);
			void AddAttributeSingles(const String& name, const float* v, int count);
			void AddAttributePercentages(const String& name, const float* v, int count);
			void AddAttributeInts(const String& name, const int32* v, int count);
			void AddAttributeUInts(const String& name, const uint32* v, int count);

			void AddAttributeStrings(const String& name, const std::vector<String>& v) { AddAttributeStrings(name, &v[0], static_cast<int>(v.size())); }
			void AddAttributeSingles(const String& name, const std::vector<float>& v) { AddAttributeSingles(name, &v[0], static_cast<int>(v.size())); }
			void AddAttributePercentages(const String& name, const std::vector<float>& v) { AddAttributePercentages(name, &v[0], static_cast<int>(v.size())); }
			void AddAttributeInts(const String& name, const std::vector<int32>& v) { AddAttributeInts(name, &v[0], static_cast<int>(v.size())); }
			void AddAttributeUInts(const String& name, const std::vector<uint32>& v) { AddAttributeUInts(name, &v[0], static_cast<int>(v.size())); }

		protected:
			
			String m_name;
			String m_value;

			AttributeTable m_attributes;
			SubSectionTable m_subSection;

		};
	}
}
#endif