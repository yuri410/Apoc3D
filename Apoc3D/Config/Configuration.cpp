/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2011-2017 Tao Xin
 * 
 * This content of this file is subject to the terms of the Mozilla Public 
 * License v2.0. If a copy of the MPL was not distributed with this file, 
 * you can obtain one at http://mozilla.org/MPL/2.0/.
 * 
 * This program is distributed in the hope that it will be useful, 
 * WITHOUT WARRANTY OF ANY KIND; either express or implied. See the 
 * Mozilla Public License for more details.
 * 
 * ------------------------------------------------------------------------
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