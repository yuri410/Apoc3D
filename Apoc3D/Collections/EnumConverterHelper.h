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

#include "CollectionsCommon.h"
#include "HashMap.h"
#include "List.h"
#include "apoc3d/Utility/StringUtils.h"

namespace Apoc3D
{
	namespace Collections
	{
		template <typename T>
		class EnumDualConversionHelper
		{
			struct EnumEqualityComparer
			{
				static bool Equals(const T& x, const T& y) { return x == y; }
				static int32 GetHashCode(const T& obj) { return static_cast<int32>(obj); }
			};

		public:
			typedef HashMap<String, T> CastTable;
			typedef HashMap<T, String, EnumEqualityComparer> InverseCastTable;

			typedef typename InverseCastTable::Iterator Iterator;
			typedef typename InverseCastTable::ValueAccessor NameAccessor;
			typedef typename CastTable::ValueAccessor ValueAccessor;

			explicit EnumDualConversionHelper(int32 capacity)
				: m_cast(capacity), m_invCast(capacity) { }

			EnumDualConversionHelper(std::initializer_list<std::pair<T, String>> list)
				: m_cast(list.size()), m_invCast(list.size())
			{
				for (const auto& e : list)
					AddPair(e.first, e.second);
			}

			EnumDualConversionHelper(std::initializer_list<std::pair<String, T>> list)
				: m_cast(list.size()), m_invCast(list.size())
			{
				for (const auto& e : list)
					AddPair(e.first, e.second);
			}


			bool SupportsName(const String& name) { String n = name; Apoc3D::Utility::StringUtils::ToLowerCase(n); return m_cast.Contains(n); }

			T Parse(const String& name) const { String n = name; Apoc3D::Utility::StringUtils::ToLowerCase(n); return m_cast[n]; }
			T ParseFlags(const String& combo, const String& delim) const;

			bool TryParse(const String& name, T& r) const { String n = name; Apoc3D::Utility::StringUtils::ToLowerCase(n); return m_cast.TryGetValue(n, r); }

			const String& ToString(T e) const { return m_invCast[e]; }
			String ToStringFlags(T flags, const String& delim) const;

			void DumpNames(List<String>& names) const { m_invCast.FillValues(names); }
			void DumpValues(List<T>& values) const { m_cast.FillValues(values); }


			NameAccessor getNameAccessor() const { return m_invCast.getValueAccessor(); }
			ValueAccessor getValueAccessor() const { return m_cast.getValueAccessor(); }

			Iterator begin() const { return m_invCast.begin(); }
			Iterator end() const { return m_invCast.end(); }

			int32 getEntryCount() const { return m_cast.getCount(); }

		protected:
			void AddPair(const String& name, T v)
			{
				String cpy = name;
				Apoc3D::Utility::StringUtils::ToLowerCase(cpy);
				m_cast.Add(cpy, v);
				m_invCast.Add(v, name);
			}

			void AddPair(T v, const String& name) { AddPair(name, v); }

		private:
			CastTable m_cast;
			InverseCastTable m_invCast;
		};

		template <typename T>
		class EnumToStringConversionHelper
		{
			struct EnumEqualityComparer
			{
				static bool Equals(const T& x, const T& y) { return x == y; }
				static int32 GetHashCode(const T& obj) { return static_cast<int32>(obj); }
			};

		public:
			typedef HashMap<T, String, EnumEqualityComparer> CastTable;
			typedef typename CastTable::Iterator Iterator;
			typedef typename CastTable::KeyAccessor NameAccessor;
			typedef typename CastTable::ValueAccessor ValueAccessor;

			explicit EnumToStringConversionHelper(int32 capacity)
				: m_cast(capacity) { }

			EnumToStringConversionHelper(std::initializer_list<std::pair<T, String>> list)
				: m_cast(list.size())
			{
				for (const auto& e : list)
					m_cast.Add(e.first, e.second);
			}
			EnumToStringConversionHelper(std::initializer_list<std::pair<String, T>> list)
				: m_cast(list.size())
			{
				for (const auto& e : list)
					m_cast.Add(e.first, e.second);
			}

			const String& ToString(T e) const { return m_cast[e]; }
			const String& operator[](T e) const { return m_cast[e]; }
			String ToStringFlags(T flags, const String& delim) const;

			void DumpNames(List<String>& names) const { m_cast.FillValues(names); }
			void DumpValues(List<T>& values) const { m_cast.FillKeys(values); }

			NameAccessor getNameAccessor() const { return m_cast.getKeyAccessor(); }
			ValueAccessor getValueAccessor() const { return m_cast.getValueAccessor(); }

			Iterator begin() const { return m_cast.begin(); }
			Iterator end() const { return m_cast.end(); }

			int32 getEntryCount() const { return m_cast.getCount(); }
		private:
			CastTable m_cast;
		};

		template <typename T>
		class TypeParseConverter
		{
		public:
			typedef HashMap<String, T> CastTable;
			typedef typename CastTable::Iterator Iterator;
			typedef typename CastTable::KeyAccessor NameAccessor;
			typedef typename CastTable::ValueAccessor ValueAccessor;

			explicit TypeParseConverter(int32 capacity)
				: m_cast(capacity) { }

			TypeParseConverter(std::initializer_list<std::pair<T, String>> list)
				: m_cast(list.size())
			{
				for (const auto& e : list)
					m_cast.Add(e.second, e.first);
			}

			bool SupportsName(const String& name) { String n = name; Apoc3D::Utility::StringUtils::ToLowerCase(n); return m_cast.Contains(n); }

			T Parse(const String& name) const { String n = name; Apoc3D::Utility::StringUtils::ToLowerCase(n); return m_cast[n]; }
			T ParseFlags(const String& combo, const String& delim) const;
			bool TryParse(const String& name, T& r) const { String n = name; Apoc3D::Utility::StringUtils::ToLowerCase(n); return m_cast.TryGetValue(name, r); }

			//void DumpNames(List<String>& names) const { m_cast.FillValues(names); }
			void DumpValues(List<T>& values) const { m_cast.FillKeys(values); }

			//NameAccessor getNameAccessor() const { return m_cast.getKeyAccessor(); }
			ValueAccessor getValueAccessor() const { return m_cast.getValueAccessor(); }

			Iterator begin() const { return m_cast.begin(); }
			Iterator end() const { return m_cast.end(); }

			int32 getEntryCount() const { return m_cast.getCount(); }
		private:
			CastTable m_cast;
		};

		template <typename T, typename H>
		String FlagFieldsToString(T flags, H& helper, const String& delim)
		{
			String result;

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

		template <typename T, typename H, T (H::*parse)(const String&) const>
		T ParseFlagFields(const String& combo, const H* obj, const String& delim) 
		{
			String v = combo;
			StringUtils::ToLowerCase(v);

			List<String> vals;
			StringUtils::Split(v, vals, delim);
			std::underlying_type<T>::type result = 0;

			for (const String& e : vals)
			{
				result |= (obj->*parse)(e);
			}

			return static_cast<T>(result);
		}



		template <typename T>
		T EnumDualConversionHelper<T>::ParseFlags(const String& combo, const String& delim) const 
		{
			return ParseFlagFields<T, EnumDualConversionHelper<T>, &EnumDualConversionHelper<T>::Parse>(combo, this, delim);
		}

		template <typename T>
		T TypeParseConverter<T>::ParseFlags(const String& combo, const String& delim) const
		{
			return ParseFlagFields<T, TypeParseConverter<T>, &TypeParseConverter<T>::Parse>(combo, this, delim);
		}

		template <typename T>
		String EnumDualConversionHelper<T>::ToStringFlags(T flags, const String& delim) const { return FlagFieldsToString(flags, *this, delim); }

		template <typename T>
		String EnumToStringConversionHelper<T>::ToStringFlags(T flags, const String& delim) const { return FlagFieldsToString(flags, *this, delim); }

	}
}

#endif