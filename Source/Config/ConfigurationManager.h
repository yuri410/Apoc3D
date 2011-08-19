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
		private:
			typedef unordered_map<String, Configuration*> ConfigTable;
			ConfigTable m_configs;
		public:
			ConfigurationManager();
		
			virtual ~ConfigurationManager()
			{
				for(ConfigTable::iterator iter = m_configs.begin(); 
					iter != m_configs.end(); iter++)
				{
					Configuration* config = iter->second;
					delete config;
				}
			}
			/** 
			*/
			void LoadConfig(const String& name, const ResourceLocation* rl);


			Configuration* getConfiguration(const String& name) const
			{
				ConfigTable::const_iterator iter = m_configs.find(name);
				if (iter != m_configs.end())
				{
					return iter->second;
				}
				return 0;
			}

			SINGLETON_DECL_HEARDER(ConfigurationManager);
		};
	}
}