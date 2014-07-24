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
		public:
			typedef HashMap<String, uint64> CastTable;
			typedef HashMap<uint64, String> InverseCastTable;

			EnumDualConversionHelper(int32 capacity)
				: m_cast(capacity), 
				m_invCast(capacity)
			{ }

			bool SupportsName(const String& name) { String n = name; Apoc3D::Utility::StringUtils::ToLowerCase(n); return m_cast.Contains(n); }
			T Parse(const String& name) const { String n = name; Apoc3D::Utility::StringUtils::ToLowerCase(n); return static_cast<T>(m_cast[n]); }
			

			String ToString(T e) const { return m_invCast[static_cast<uint64>(e)]; }

			void DumpNames(List<String>& names) const
			{
				m_cast.FillKeys(names);
			}
			void DumpValues(List<T>& values) const
			{
				for (CastTable::Enumerator e = m_cast.GetEnumerator();e.MoveNext();)
				{
					values.Add(e.getCurrentValue());
				}
			}

			CastTable::Enumerator GetCastTableEnumerator() { return m_cast.GetEnumerator(); }
			InverseCastTable::Enumerator GetInverseTableEnumerator() const { return m_invCast.GetEnumerator(); }

		protected:
			void AddPair(const String& name, T v)
			{
				String cpy = name;
				Apoc3D::Utility::StringUtils::ToLowerCase(cpy);
				m_cast.Add(cpy, static_cast<uint64>(v));
				m_invCast.Add(static_cast<uint64>(v), name);
			}

		private:
			CastTable m_cast;
			InverseCastTable m_invCast;
		};
	}
}

#endif