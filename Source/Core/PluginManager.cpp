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

#include "PluginManager.h"

#include "Plugin.h"
#include "Apoc3DException.h"
#include "Platform/Library.h"
#include "Config/ConfigurationManager.h"
#include "Config/Configuration.h"
#include "Config/ConfigurationSection.h"
#include "Logging.h"
#include "Collections/FastList.h"

using namespace Apoc3D::Config;
using namespace Apoc3D::Platform;

SINGLETON_DECL(Apoc3D::Core::PluginManager);

namespace Apoc3D
{
	namespace Core
	{
		typedef Plugin* (*LIB_GET_PLUGIN)(void);
		//void PluginManager::Initialize()
		//{
		//	
		//}
		PluginManager::~PluginManager()
		{
			UnloadPlugins();
			
		}

		const Plugin* PluginManager::getPlugin(const String& name) const
		{
			PluginTable::const_iterator iter = m_plugins.find(name);

			if (iter != m_plugins.end())
			{
				return iter->second;
			}
			throw Apoc3DException::createException(EX_KeyNotFound, name.c_str());
		}

		void PluginManager::OnPluginLoad(const Plugin* plg, int32 index, int32 count)
		{
			if (!m_eventPluginLoad.empty())
			{
				m_eventPluginLoad(plg, index, count);
			}
		}
		void PluginManager::OnPluginError(const Plugin* plg)
		{
			if (!m_eventPluginError.empty())
			{
				m_eventPluginError(plg);
			}
		}

		void PluginManager::LoadPlugin(const String& name)
		{
			try
			{
				LogManager::getSingleton().Write(LOG_System, L"Loading plugin" + name, LOGLVL_Infomation);

				Library* lib = new Library(name);

				lib->Load();

				void* ptr = lib->getSymbolAddress(L"Apoc3DGetPlugin");
				if (!ptr)
				{
					String msg = L"Entry point 'Apoc3DGetPlugin' not found on ";
					msg.append(name);
					msg.append(L" . Cannot load");
					LogManager::getSingleton().Write(LOG_System, L"Unable to load" + name + L".\n " + msg, LOGLVL_Infomation);
					
					lib->Unload();
					delete lib;
					OnPluginError(0);
					return;
				}

				Plugin* plugin = ((LIB_GET_PLUGIN)ptr)();
				
				plugin->Load();

				m_libraries.push_back(lib);
				m_plugins.insert(make_pair(name, plugin));
			}
			catch (Apoc3DException& e)
			{
				OnPluginError(0);
			}
		}

		void PluginManager::LoadPlugins(const FastList<Plugin*>& plugins)
		{
			for (int i=0;i<plugins.getCount();i++)
			{
				try
				{
					Plugin* plugin = plugins[i];
					LogManager::getSingleton().Write(LOG_System, L"Loading plugin" + plugin->GetName(), LOGLVL_Infomation);

					plugin->Load();

					m_plugins.insert(make_pair(plugin->GetName(), plugin));
				}
				catch (Apoc3DException& e)
				{
					OnPluginError(0);
				}
			}
		}
		void PluginManager::LoadPlugins(const FastList<String>& plugins)
		{
			for (int i=0;i<plugins.getCount();i++)
			{
				LoadPlugin(plugins[i]);
			}
		}
		void PluginManager::LoadPlugins()
		{
			Configuration* conf = ConfigurationManager::getSingleton().getConfiguration(L"PLUGIN");
			
			for (Configuration::Iterator iter = conf->begin(); iter != conf->end(); iter++)
			{
				String name = iter->second->getAttribute(L"Library");
				LoadPlugin(name);
			}
		}
		void PluginManager::UnloadPlugins()
		{
			for (PluginTable::iterator iter = m_plugins.begin(); iter != m_plugins.end(); iter++)
			{
				try
				{
					iter->second->Unload();
				}
				catch (Apoc3DException& e)
				{
					OnPluginError(iter->second);
				}
			}
			m_plugins.clear();

			for (LibraryList::iterator iter = m_libraries.begin(); iter != m_libraries.end(); iter++)
			{
				Library* lib = *iter;
				lib->Unload();
				delete lib;
			}
		}

	}
}
