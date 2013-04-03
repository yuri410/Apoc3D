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

#include "Configuration.h"

#include "ConfigurationSection.h"

namespace Apoc3D
{
	namespace Config
	{
		Configuration::Configuration(const Configuration& another)
			: m_name(another.m_name), m_sections(another.m_sections)
		{
			for (ChildTable::Enumerator e = m_sections.GetEnumerator(); e.MoveNext(); )
			{
				ConfigurationSection* newSect = new ConfigurationSection(**e.getCurrentValue());

				*e.getCurrentValue() = newSect;
			}
		}


		Configuration::~Configuration()
		{
			for (Configuration::ChildTable::Enumerator e = m_sections.GetEnumerator();
				e.MoveNext();)
			{
				ConfigurationSection* sect = *e.getCurrentValue();
				delete sect;
			}

		}

		void Configuration::Add(ConfigurationSection* sect)
		{
			Add(sect->getName(), sect);
		}

		void Configuration::Merge(Configuration* config)
		{

		}

	}
}