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
#ifndef APOC3D_CONFIGURATION_H
#define APOC3D_CONFIGURATION_H

#include "Common.h"
#include "Collections/FastMap.h"

using namespace Apoc3D::Collections;
using namespace Apoc3D::IO;
using namespace Apoc3D::VFS;
using namespace std;

namespace Apoc3D
{
	namespace Config
	{
		//template class APAPI unordered_map<String, ConfigurationSection*>;
		class ConfigurationFormat
		{
		public:
			virtual Configuration* Load(const ResourceLocation* rl) = 0;
			virtual void Save(Configuration* config, Stream* strm) = 0;

			virtual std::vector<String> GetSupportedFileSystemExtensions() = 0;
		};

		/** Represents a configuration
		 *
		 * @remarks
		 *  A configuration is a tree structure, containing sections as portions, which
		 *  themselves have sections as sub sections. Each section can have a name, a value
		 *  and several attributes.
		 *
		 *  In each level, every sections has a unique name. When loading from file, a valid
		 *  config file should not have 2 or more sections at the same level with the same name.
		 */
		class APAPI Configuration
		{
		public:
			typedef FastMap<String, ConfigurationSection*> ChildTable;
			

			Configuration(const String& name)
				: m_name(name)
			{

			}
			Configuration(const Configuration& another);
			virtual ~Configuration();

			const String& getName() const { return m_name; }

			/** Gets the top level sections with a name.
			*/
			ConfigurationSection* get(const String& name) const
			{
				ConfigurationSection* result;
				if (m_sections.TryGetValue(name, result))
				{
					return result;
				}
				return 0;
			}

			/** Gets the top section enumerator
			*/
			ChildTable::Enumerator GetEnumerator() { return m_sections.GetEnumerator(); }

			void Merge(Configuration* config);
			
			void Add(const String& name, ConfigurationSection* sect)
			{
				m_sections.Add(name, sect);
			}
			void Add(ConfigurationSection* sect);
		private:
			String m_name;
			ChildTable m_sections;

			friend class ConfigurationFormat;
		};
	}
}
#endif