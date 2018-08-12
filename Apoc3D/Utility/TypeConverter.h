#pragma once

#ifndef APOC3D_ENUMCONVERTERHELPER_H
#define APOC3D_ENUMCONVERTERHELPER_H

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

#include "apoc3d/Collections/HashMap.h"
#include "apoc3d/Collections/List.h"
#include "StringUtils.h"

namespace Apoc3D
{
	namespace Utility
	{
		template <typename T, typename StrType, typename H>
		StrType FlagFieldsToString(T flags, H& helper, const StrType& delim)
		{
			StrType result;

			for (auto e : helper)
			{
				T fld = e.Key;
				if ((flags & fld) == fld)
				{
					result.append(e.Value);
					result.append(delim);
				}
			}

			if (result.size() > 1)
				return result.substr(0, result.size() - 2);
			return result;
		}

		template <typename T, typename StrType, typename H, T(H::*parse)(const StrType&) const>
		T ParseFlagFields(const StrType& combo, const H* obj, const StrType& delim)
		{
			List<StrType> vals;
			StringUtils::Split(combo, vals, delim);
			std::underlying_type<T>::type result = 0;

			for (const StrType& e : vals)
			{
				result |= (obj->*parse)(e);
			}

			return static_cast<T>(result);
		}


		template <typename T, bool NarrowString>
		class TypeDualConverter
		{
			struct EnumEqualityComparer
			{
				static bool Equals(const T& x, const T& y) { return x == y; }
				static int32 GetHashCode(const T& obj) { return static_cast<int32>(obj); }
			};

		public:
			using StringType = typename std::conditional<NarrowString, std::string, String>::type;
			using StringTypeComparer = typename std::conditional<NarrowString, 
				Apoc3D::Collections::NStringEqualityComparerNoCase, 
				Apoc3D::Collections::StringEqualityComparerNoCase>::type;

			using CastTable = Apoc3D::Collections::HashMap<StringType, T, StringTypeComparer>;
			using InverseCastTable = Apoc3D::Collections::HashMap<T, StringType, EnumEqualityComparer>;
			using NameList = Apoc3D::Collections::List<StringType>;
			using ValueList = Apoc3D::Collections::List<T>;

			using Iterator = typename InverseCastTable::Iterator;
			using NameAccessor = typename InverseCastTable::ValueAccessor;
			using ValueAccessor = typename CastTable::ValueAccessor;

			explicit TypeDualConverter(int32 capacity)
				: m_cast(capacity), m_invCast(capacity) { }

			TypeDualConverter(std::initializer_list<std::pair<T, StringType>> list)
				: m_cast((int32)list.size()), m_invCast((int32)list.size())
			{
				for (const auto& e : list)
					AddPair(e.first, e.second);
			}

			TypeDualConverter(std::initializer_list<std::pair<StringType, T>> list)
				: m_cast((int32)list.size()), m_invCast((int32)list.size())
			{
				for (const auto& e : list)
					AddPair(e.first, e.second);
			}


			bool SupportsName(const StringType& name) const { return m_cast.Contains(name); }

			T Parse(const StringType& name) const { return m_cast[name]; }
			bool TryParse(const StringType& name, T& r) const { return m_cast.TryGetValue(name, r); }
			
			T ParseFlags(const StringType& combo, const StringType& delim) const
			{
				return ParseFlagFields<T, StringType, TypeDualConverter<T, NarrowString>, &TypeDualConverter<T, NarrowString>::Parse>(combo, this, delim);
			}


			const StringType& ToString(T e) const { return m_invCast[e]; }
			bool TryToString(T e, StringType& r) const { return m_invCast.TryGetValue(e, r); }

			StringType ToStringFlags(T flags, const StringType& delim) const { return FlagFieldsToString(flags, *this, delim); }

			T operator[](const StringType& name) const { return m_cast[name]; }
			const StringType& operator[](T e) const { return m_invCast[e]; }

			void DumpNames(NameList& names) const { m_invCast.FillValues(names); }
			void DumpValues(ValueList& values) const { m_cast.FillValues(values); }
			NameList DumpNames() const { NameList names; DumpNames(names); return names; }
			ValueList DumpValues() const { ValueList values; DumpValues(values); return values; }

			NameAccessor getNameAccessor() const { return m_invCast.getValueAccessor(); }
			ValueAccessor getValueAccessor() const { return m_cast.getValueAccessor(); }

			Iterator begin() const { return m_invCast.begin(); }
			Iterator end() const { return m_invCast.end(); }

			int32 getEntryCount() const { return m_cast.getCount(); }

		protected:
			void AddPair(const StringType& name, T v)
			{
				m_cast.Add(name, v);
				m_invCast.Add(v, name);
			}

			void AddPair(T v, const StringType& name) { AddPair(name, v); }

		private:
			CastTable m_cast;
			InverseCastTable m_invCast;
		};

		template <typename T, bool NarrowString>
		class TypeToStringConverter
		{
			struct EnumEqualityComparer
			{
				static bool Equals(const T& x, const T& y) { return x == y; }
				static int32 GetHashCode(const T& obj) { return static_cast<int32>(obj); }
			};

		public:
			using StringType = typename std::conditional<NarrowString, std::string, String>::type;

			typedef Apoc3D::Collections::HashMap<T, StringType, EnumEqualityComparer> CastTable;
			typedef Apoc3D::Collections::List<StringType> NameList;
			typedef Apoc3D::Collections::List<T> ValueList;

			typedef typename CastTable::Iterator Iterator;
			typedef typename CastTable::KeyAccessor NameAccessor;
			typedef typename CastTable::ValueAccessor ValueAccessor;
			
			explicit TypeToStringConverter(int32 capacity)
				: m_cast(capacity) { }

			TypeToStringConverter(std::initializer_list<std::pair<T, StringType>> list)
				: m_cast(list) { }

			TypeToStringConverter(std::initializer_list<std::pair<StringType, T>> list)
				: m_cast((int32)list.size())
			{
				for (const auto& e : list)
					m_cast.Add(e.second, e.first);
			}

			const StringType& ToString(T e) const { return m_cast[e]; }
			const StringType& operator[](T e) const { return m_cast[e]; }

			bool TryToString(T e, StringType& r) const { return m_cast.TryGetValue(e, r); }
			StringType ToStringFlags(T flags, const StringType& delim) const { return FlagFieldsToString(flags, *this, delim); }

			void DumpNames(NameList& names) const { m_cast.FillValues(names); }
			void DumpValues(ValueList& values) const { m_cast.FillKeys(values); }
			NameList DumpNames() const { NameList names; DumpNames(names); return names; }
			ValueList DumpValues() const { ValueList values; DumpValues(values); return values; }

			NameAccessor getNameAccessor() const { return m_cast.getKeyAccessor(); }
			ValueAccessor getValueAccessor() const { return m_cast.getValueAccessor(); }

			Iterator begin() const { return m_cast.begin(); }
			Iterator end() const { return m_cast.end(); }

			int32 getEntryCount() const { return m_cast.getCount(); }
		private:
			CastTable m_cast;
		};

		template <typename T, bool NarrowString>
		class TypeParseConverter
		{
		public:
			using StringType = typename std::conditional<NarrowString, std::string, String>::type;
			using StringTypeComparer = typename std::conditional<NarrowString,
				Apoc3D::Collections::NStringEqualityComparerNoCase,
				Apoc3D::Collections::StringEqualityComparerNoCase>::type;

			typedef Apoc3D::Collections::HashMap<StringType, T, StringTypeComparer> CastTable;
			typedef Apoc3D::Collections::List<T> ValueList;

			typedef typename CastTable::Iterator Iterator;
			typedef typename CastTable::KeyAccessor NameAccessor;
			typedef typename CastTable::ValueAccessor ValueAccessor;

			explicit TypeParseConverter(int32 capacity)
				: m_cast(capacity) { }

			TypeParseConverter(std::initializer_list<std::pair<T, StringType>> list)
				: m_cast((int32)list.size())
			{
				for (const auto& e : list)
					m_cast.Add(e.second, e.first);
			}

			TypeParseConverter(std::initializer_list<std::pair<StringType, T>> list)
				: m_cast(list) { }

			bool SupportsName(const StringType& name) const { return m_cast.Contains(name); }

			T Parse(const StringType& name) const { return m_cast[name]; }
			bool TryParse(const StringType& name, T& r) const { return m_cast.TryGetValue(name, r); }

			T ParseFlags(const StringType& combo, const StringType& delim) const
			{
				return ParseFlagFields<T, StringType, TypeParseConverter<T, NarrowString>, &TypeParseConverter<T, NarrowString>::Parse>(combo, this, delim);
			}

			T operator[](const StringType& name) const { return m_cast[name]; }

			void DumpValues(ValueList& values) const { m_cast.FillValues(values); }
			ValueList DumpValues() const { ValueList values; DumpValues(values); return values; }

			ValueAccessor getValueAccessor() const { return m_cast.getValueAccessor(); }

			Iterator begin() const { return m_cast.begin(); }
			Iterator end() const { return m_cast.end(); }

			int32 getEntryCount() const { return m_cast.getCount(); }
		private:
			CastTable m_cast;
		};


	}
}

#endif