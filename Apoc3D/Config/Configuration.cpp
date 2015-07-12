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

#include "apoc3d/Core/Logging.h"
#include "apoc3d/Config/ConfigurationManager.h"

namespace Apoc3D
{
	namespace Config
	{
		Configuration::Configuration(const Configuration& another)
			: m_name(another.m_name), m_sections(another.m_sections)
		{
			for (ConfigurationSection*& newSect : m_sections.getValueAccessor())
			{
				newSect = new ConfigurationSection(*newSect);
			}
		}

		Configuration::~Configuration()
		{
			for (ConfigurationSection* s : m_sections.getValueAccessor())
			{
				delete s;
			}
			m_sections.Clear();
		}

		void Configuration::Add(ConfigurationSection* sect)
		{
			Add(sect->getName(), sect);
		}
		void Configuration::Add(const String& name, ConfigurationSection* sect)
		{
			m_sections.Add(name, sect);
		}

		ConfigurationSection* Configuration::get(const String& name) const
		{
			ConfigurationSection* result;
			if (m_sections.TryGetValue(name, result))
				return result;
			return nullptr;
		}


		void Configuration::Merge(Configuration* other, bool noMessages)
		{
			for (ConfigurationSection* sect : other->getSubSections())
			{
				ConfigurationSection* thisSect = get(sect->getName());

				if (thisSect)
					thisSect->Merge(sect, noMessages);
				else
					Add(new ConfigurationSection(*sect));

			}
		}

		
	}
}