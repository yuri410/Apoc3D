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

using namespace std;

namespace Apoc3D
{
	namespace Config
	{
		template class APAPI unordered_map<String, ConfigurationSection*>;

		class APAPI Configuration
		{
		private:
			typedef unordered_map<String, ConfigurationSection*> ChildTable;

			String m_name;
			
			ChildTable m_sections;

		protected:
			Configuration(const String& name)
				: m_name(name)
			{
			}			
		public:
			virtual ~Configuration();

			const String& getName() const { return m_name; }

			ConfigurationSection* get(const String& name) const
			{
				ChildTable::const_iterator iter = m_sections.find(name);

				if (iter != m_sections.end())
				{
					return iter->second;
				}
				return 0;
			}

			virtual Configuration* Clone() const = 0;
			virtual void Merge(Configuration* config) = 0;
		};
	}
}
#endif