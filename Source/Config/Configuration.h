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
#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "Common.h"
#include "Collections/FastMap.h"

using namespace Apoc3D::Collections;
using namespace std;

namespace Apoc3D
{
	namespace Config
	{
		//template class APAPI unordered_map<String, ConfigurationSection*>;

		class APAPI Configuration
		{
		public:
			typedef FastMap<String, ConfigurationSection*> ChildTable;
		private:			
			String m_name;			
		protected:
			ChildTable m_sections;

			Configuration(const String& name)
				: m_name(name)
			{
				
			}			
		public:
			virtual ~Configuration();

			const String& getName() const { return m_name; }

			ConfigurationSection* get(const String& name) const
			{
				ConfigurationSection* result;
				m_sections.TryGetValue(name, result);
				return result;
			}

			ChildTable::Enumerator GetEnumerator() { return m_sections.GetEnumerator(); }

			virtual Configuration* Clone() const = 0;
			virtual void Merge(Configuration* config) = 0;

			
		};
	}
}
#endif