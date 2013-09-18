#pragma once
#ifndef APOC3D_CONFIGURATIONSECTION_H
#define APOC3D_CONFIGURATIONSECTION_H

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

#include "apoc3d/Math/ColorValue.h"
#include "apoc3d/Collections/HashMap.h"
#include "apoc3d/Collections/List.h"

using namespace Apoc3D::Math;
using namespace Apoc3D::Core;
using namespace Apoc3D::Collections;

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
			typedef HashMap<String, String> AttributeTable;
			typedef HashMap<String, ConfigurationSection*> SubSectionTable;

			typedef SubSectionTable::Enumerator SubSectionEnumerator;
			typedef AttributeTable::Enumerator AttributeEnumerator;

			ConfigurationSection(const String& name, int capacity);
			ConfigurationSection(const String& name);
			ConfigurationSection(const ConfigurationSection& another);
			~ConfigurationSection();

			AttributeTable::Enumerator GetAttributeEnumrator() const { return m_attributes.GetEnumerator(); }
			SubSectionTable::Enumerator GetSubSectionEnumrator() const { return m_subSection.GetEnumerator(); }

			void AddSection(ConfigurationSection* section);
			void SetValue( const String& value);

			const String& getName() const { return m_name; }

			int getAttributeCount() const { return m_attributes.getCount(); }
			int getSubSectionCount() const { return m_subSection.getCount(); }


			/**
			 *  Gets the value of the sub-section with the given name.
			 */
			const String& getValue() const { return m_value; }
			const String& getValue(const String& name) const;
			const String& getAttribute(const String& name) const;
			ConfigurationSection* getSection(const String& name) const;

			bool hasAttribute(const String& name) const;

			bool tryGetValue(const String& name, String& result) const;
			bool tryGetAttribute(const String& name, String& result) const;



			bool GetBool() const;
			float GetSingle() const;
			float GetPercentage() const;
			int32 GetInt() const;
			uint32 GetUInt() const;
			ColorValue GetColorValue() const;
			List<String> GetStrings() const;
			FastList<float> GetSingles() const;
			FastList<float> GetPercentages() const;
			FastList<int32> GetInts() const;
			FastList<uint32> GetUInts() const;

			bool GetBool(const String& key) const;
			float GetSingle(const String& key) const;
			float GetPercentage(const String& key) const;
			int32 GetInt(const String& key) const;
			uint32 GetUInt(const String& key) const;
			ColorValue GetColorValue(const String& key) const;
			List<String> GetStrings(const String& key) const;
			FastList<float> GetSingles(const String& key) const;
			FastList<float> GetPercentages(const String& key) const;
			FastList<int32> GetInts(const String& key) const;
			FastList<uint32> GetUInts(const String& key) const;

			
			bool GetAttributeBool(const String& key) const;
			float GetAttributeSingle(const String& key) const;
			float GetAttributePercentage(const String& key) const;
			int32 GetAttributeInt(const String& key) const;
			uint32 GetAttributeUInt(const String& key) const;
			ColorValue GetAttributeColorValue(const String& key) const;
			List<String> GetAttributeStrings(const String& key) const;
			FastList<float> GetAttributeSingles(const String& key) const;
			FastList<float> GetAttributePercentages(const String& key) const;
			FastList<int32> GetAttributeInts(const String& key) const;
			void GetAttributeInts(const String& key, FastList<int32>& values) const;
			FastList<uint32> GetAttributeUInts(const String& key) const;




			bool TryGetBool(const String& key, bool& result) const;
			bool TryGetSingle(const String& key, float& result) const;
			bool TryGetPercentage(const String& key, float& result) const;
			bool TryGetInt(const String& key, int32& result) const;
			bool TryGetUInt(const String& key, uint32& result) const;
			bool TryGetColorValue(const String& key, ColorValue& result) const;

			bool TryGetAttributeInts(const String& key, FastList<int32>& values) const;


			bool TryGetAttributeBool(const String& key, bool& result) const;
			bool TryGetAttributeSingle(const String& key, float& result) const;
			bool TryGetAttributePercentage(const String& key, float& result) const;
			bool TryGetAttributeInt(const String& key, int32& result) const;
			bool TryGetAttributeUInt(const String& key, uint32& result) const;
			bool TryGetAttributeColorValue(const String& key, ColorValue& result) const;


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
			
			void AddStrings(const String& key, const List<String>& v)			{ AddStrings(key, &v[0], v.getCount()); }
			void AddSingles(const String& key, const FastList<float>& v)		{ AddSingles(key, &v[0],  v.getCount()); }
			void AddPercentages(const String& key, const FastList<float>& v)	{ AddPercentages(key, &v[0],  v.getCount()); }
			void AddInts(const String& key, const FastList<int32>& v)			{ AddInts(key, &v[0],  v.getCount()); }
			void AddUInts(const String& key, const FastList<uint32>& v)			{ AddUInts(key, &v[0],  v.getCount()); }


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

			void AddAttributeStrings(const String& name, const List<String>& v)			{ AddAttributeStrings(name, &v[0], v.getCount()); }
			void AddAttributeSingles(const String& name, const FastList<float>& v)		{ AddAttributeSingles(name, &v[0], v.getCount()); }
			void AddAttributePercentages(const String& name, const FastList<float>& v)	{ AddAttributePercentages(name, &v[0], v.getCount()); }
			void AddAttributeInts(const String& name, const FastList<int32>& v)			{ AddAttributeInts(name, &v[0], v.getCount()); }
			void AddAttributeUInts(const String& name, const FastList<uint32>& v)		{ AddAttributeUInts(name, &v[0], v.getCount()); }


			void SetStringValue(const String& name, const String& value);

			void SetBool(bool value);
			void SetSingle(float value);
			void SetPercentage(float value);
			void SetInt(int32 value);
			void SetUInt(uint32 value);
			void SetColorValue(ColorValue value);


			void SetStrings(const String* v, int count);
			void SetSingles(const float* v, int count);
			void SetPercentages(const float* v, int count);
			void SetInts(const int32* v, int count);
			void SetUInts(const uint32* v, int count);

			void SetStrings(const List<String>& v)			{ SetStrings(&v[0], v.getCount()); }
			void SetSingles(const FastList<float>& v)		{ SetSingles(&v[0],  v.getCount()); }
			void SetPercentages(const FastList<float>& v)	{ SetPercentages(&v[0],  v.getCount()); }
			void SetInts(const FastList<int32>& v)			{ SetInts(&v[0],  v.getCount()); }
			void SetUInts(const FastList<uint32>& v)		{ SetUInts(&v[0],  v.getCount()); }


			void SetBool(const String& name, bool value);
			void SetSingle(const String& name, float value);
			void SetPercentage(const String& name, float value);
			void SetInt(const String& name, int32 value);
			void SetUInt(const String& name, uint32 value);
			void SetColorValue(const String& name, ColorValue value);

		protected:
			
			String m_name;
			String m_value;

			AttributeTable m_attributes;
			SubSectionTable m_subSection;

		};
	}
}
#endif