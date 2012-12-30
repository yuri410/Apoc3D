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
#ifndef APOC3D_CONFIGURATIONMANAGER_H
#define APOC3D_CONFIGURATIONMANAGER_H

#include "Common.h"
#include "Core/Singleton.h"
#include "Configuration.h"

using namespace Apoc3D::Core;
using namespace Apoc3D::VFS;

using namespace std;

namespace Apoc3D
{
	namespace Config
	{
		//template class APAPI unordered_map<String, Configuration*>;
			 
		class APAPI ConfigurationManager : public Singleton<ConfigurationManager>
		{
		public:
			ConfigurationManager();
			virtual ~ConfigurationManager();

			/** Loads a config from given location. Then stores it in the
			 *  manager with the given name as an identifier, which can be 
			 *  used to retrieve the config using the getConfiguration method.
			 */
			void LoadConfig(const String& name, const ResourceLocation* rl, ConfigurationFormat* fmt = nullptr);

			/** Gets a pre-loaded configuration with the given name
			*/
			Configuration* getConfiguration(const String& name) const
			{
				Configuration* result = nullptr;
				m_configs.TryGetValue(name, result);
				return result;
			}

			Configuration* CreateInstance(const ResourceLocation* rl, ConfigurationFormat* fmt = nullptr);

			void RegisterFormat(ConfigurationFormat* fmt);
			void UnregisterFormat(ConfigurationFormat* fmt);

			SINGLETON_DECL_HEARDER(ConfigurationManager);
		private:
			typedef FastMap<String, Configuration*> ConfigTable;
			typedef FastMap<String, ConfigurationFormat*> FormatTable;

			ConfigTable m_configs;
			FormatTable m_formats;
		};
	}
}
#endif