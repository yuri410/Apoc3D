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
			static int32 FloatPointStoringPrecision;

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


#define CONFIG_SECT_GETER_DECL(type, typeName) \
			type Get##typeName() const; \
			type Get##typeName(const String& key) const; \
			type GetAttribute##typeName(const String& key) const; \
			bool TryGet##typeName(const String& key, type& result) const; \
			bool TryGetAttribute##typeName(const String& key, type& result) const;

			CONFIG_SECT_GETER_DECL(bool, Bool);
			CONFIG_SECT_GETER_DECL(float, Single);
			CONFIG_SECT_GETER_DECL(float, Percentage);
			CONFIG_SECT_GETER_DECL(int32, Int);
			CONFIG_SECT_GETER_DECL(uint32, UInt);
			CONFIG_SECT_GETER_DECL(ColorValue, ColorValue);
			CONFIG_SECT_GETER_DECL(Vector3, Vector3);
			CONFIG_SECT_GETER_DECL(Point, Point);

#undef CONFIG_SECT_GETER_DECL

#define CONFIG_SECT_SPLITER_DECL(type, typeName) \
			type Get##typeName() const; \
			type Get##typeName(const String& key) const; \
			type GetAttribute##typeName(const String& key) const; \
			void Get##typeName(type& result) const; \
			void Get##typeName(const String& key, type& result) const; \
			void GetAttribute##typeName(const String& key, type& result) const; \
			bool TryGet##typeName(const String& key, type& result) const; \
			bool TryGetAttribute##typeName(const String& key, type& result) const;

			CONFIG_SECT_SPLITER_DECL(List<String>, Strings);
			CONFIG_SECT_SPLITER_DECL(FastList<float>, Singles);
			CONFIG_SECT_SPLITER_DECL(FastList<float>, Percentages);
			CONFIG_SECT_SPLITER_DECL(FastList<int32>, Ints);
			CONFIG_SECT_SPLITER_DECL(FastList<uint32>, UInts);
			CONFIG_SECT_SPLITER_DECL(FastList<Vector3>, Vector3s);
			CONFIG_SECT_SPLITER_DECL(FastList<Point>, Points);

#undef CONFIG_SECT_SPLITER_DECL


#define CONFIG_SECT_SPLITER_ARR_DECL(type, typeName) \
			void Get##typeName(type* v, int32 maxCount, int32* acutallCount = nullptr) const; \
			void Get##typeName(const String& key, type* v, int32 maxCount, int32* acutallCount = nullptr) const; \
			void GetAttribute##typeName(const String& key, type* v, int32 maxCount, int32* acutallCount = nullptr) const; \
			bool TryGet##typeName(const String& key, type* v, int32 maxCount, int32* acutallCount = nullptr) const; \
			bool TryGetAttribute##typeName(const String& key, type* v, int32 maxCount, int32* acutallCount = nullptr) const; \
			void Get##typeName##Checked(type* v, int32 expectedCount) const { int32 actuallCount; Get##typeName(v, expectedCount, &actuallCount); assert(actuallCount == expectedCount); } \
			void Get##typeName##Checked(const String& key, type* v, int32 expectedCount) const { int32 actuallCount; Get##typeName(key, v, expectedCount, &actuallCount); assert(actuallCount == expectedCount); } \
			void GetAttribute##typeName##Checked(const String& key, type* v, int32 expectedCount) const { int32 actuallCount; GetAttribute##typeName(key, v, expectedCount, &actuallCount); assert(actuallCount == expectedCount); } \
			bool TryGet##typeName##Checked(const String& key, type* v, int32 expectedCount) const { int32 actuallCount; bool r = TryGet##typeName(key, v, expectedCount, &actuallCount); assert(actuallCount == expectedCount); return r; } \
			bool TryGetAttribute##typeName##Checked(const String& key, type* v, int32 expectedCount) const { int32 actuallCount; bool r = TryGetAttribute##typeName(key, v, expectedCount, &actuallCount); assert(actuallCount == expectedCount); return r; } 

			CONFIG_SECT_SPLITER_ARR_DECL(float, Singles);
			CONFIG_SECT_SPLITER_ARR_DECL(float, Percentages);
			CONFIG_SECT_SPLITER_ARR_DECL(int32, Ints);
			CONFIG_SECT_SPLITER_ARR_DECL(uint32, UInts);
			CONFIG_SECT_SPLITER_ARR_DECL(Vector3, Vector3s);
			CONFIG_SECT_SPLITER_ARR_DECL(Point, Points);

#undef CONFIG_SECT_SPLITER_ARR_DECL


			void AddStringValue(const String& name, const String& value);
			void AddAttributeString(const String& name, const String& value);


#define CONFIG_SECT_ADDER_DECL(type, typeName) \
			void Add##typeName(const String& key, type value); \
			void AddAttribute##typeName(const String& key, type value); 

			CONFIG_SECT_ADDER_DECL(bool, Bool);
			CONFIG_SECT_ADDER_DECL(float, Single);
			CONFIG_SECT_ADDER_DECL(float, Percentage);
			CONFIG_SECT_ADDER_DECL(int32, Int);
			CONFIG_SECT_ADDER_DECL(uint32, UInt);
			CONFIG_SECT_ADDER_DECL(ColorValue, ColorValue);
			CONFIG_SECT_ADDER_DECL(const Vector3&, Vector3);
			CONFIG_SECT_ADDER_DECL(const Point&, Point);

#undef CONFIG_SECT_ADDER_DECL


#define CONFIG_SECT_COMBINER_DECL_NOLIST(type, typeName) \
			void Add##typeName(const String& key, const type* v, int32 count); \
			void AddAttribute##typeName(const String& name, const type* v, int32 count);
			
#define CONFIG_SECT_COMBINER_DECL(type, typeName, listType) \
			CONFIG_SECT_COMBINER_DECL_NOLIST(type, typeName); \
			void Add##typeName(const String& key, const listType<type>& v)			{ Add##typeName(key, &v[0], v.getCount()); }  \
			void AddAttribute##typeName(const String& name, const listType<type>& v)			{ AddAttribute##typeName(name, &v[0], v.getCount()); } 

			CONFIG_SECT_COMBINER_DECL(String, Strings, List);
			CONFIG_SECT_COMBINER_DECL(float, Singles, FastList);
			CONFIG_SECT_COMBINER_DECL(float, Percentages, FastList);
			CONFIG_SECT_COMBINER_DECL(int32, Ints, FastList);
			CONFIG_SECT_COMBINER_DECL(uint32, UInts, FastList);
			CONFIG_SECT_COMBINER_DECL_NOLIST(Vector3, Vector3s);
			CONFIG_SECT_COMBINER_DECL_NOLIST(Point, Points);

#undef CONFIG_SECT_COMBINER_DECL
#undef CONFIG_SECT_COMBINER_DECL_NOLIST


			void SetStringValue(const String& name, const String& value);

			void SetBool(bool value);
			void SetSingle(float value);
			void SetPercentage(float value);
			void SetInt(int32 value);
			void SetUInt(uint32 value);
			void SetColorValue(ColorValue value);
			void SetVector3(const Vector3& v);

			void SetStrings(const String* v, int count);
			void SetSingles(const float* v, int count);
			void SetPercentages(const float* v, int count);
			void SetInts(const int32* v, int count);
			void SetUInts(const uint32* v, int count);
			void SetVector3s(const Vector3* v, int count);

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
			void SetVector3(const String& name, const Vector3& v);

		protected:
			
			String m_name;
			String m_value;

			AttributeTable m_attributes;
			SubSectionTable m_subSection;

		};
	}
}
#endif