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

#include "PluginManager.h"

#include "Plugin.h"
#include "apoc3d/Exception.h"
#include "apoc3d/Platform/Library.h"
#include "apoc3d/Config/ConfigurationManager.h"
#include "apoc3d/Config/Configuration.h"
#include "apoc3d/Config/ConfigurationSection.h"
#include "Logging.h"

using namespace Apoc3D::Config;
using namespace Apoc3D::Platform;

namespace Apoc3D
{
	namespace Core
	{
		SINGLETON_IMPL(PluginManager);

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
			Plugin* plg;
			if (m_plugins.TryGetValue(name, plg))
			{
				return plg;
			}
			throw AP_EXCEPTION(ExceptID::KeyNotFound, name);
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
			Library* lib = nullptr;
			Plugin* plugin = nullptr;
			try
			{
				LogManager::getSingleton().Write(LOG_System, L"Loading plugin" + name, LOGLVL_Infomation);

				lib = new Library(name);
				lib->Load();

				void* ptr = lib->getSymbolAddress(L"Apoc3DGetPlugin");
				if (ptr == nullptr)
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

				plugin = ((LIB_GET_PLUGIN)ptr)();
				plugin->Load();

				m_libraries.Add(lib);
				m_plugins.Add(name, plugin);
			}
			catch (const Exception& e)
			{
				(void)e;
				OnPluginError(plugin);

				delete lib;
			}
		}

		void PluginManager::LoadPlugins(const List<Plugin*>& plugins)
		{
			for (int i = 0; i < plugins.getCount(); i++)
			{
				Plugin* plugin = plugins[i];
				try
				{
					LogManager::getSingleton().Write(LOG_System, L"Loading plugin " + plugin->GetName(), LOGLVL_Infomation);

					plugin->Load();

					m_plugins.Add(plugin->GetName(), plugin);
				}
				catch (const Exception& e)
				{
					(void)e;
					OnPluginError(plugin);
				}
			}
		}
		void PluginManager::LoadPlugins(const List<String>& plugins)
		{
			for (int i = 0; i < plugins.getCount(); i++)
			{
				LoadPlugin(plugins[i]);
			}
		}
		void PluginManager::LoadPlugins()
		{
			Configuration* conf = ConfigurationManager::getSingleton().getConfiguration(L"PLUGIN");
			
			for (ConfigurationSection* ss : conf->getSubSections())
			{
				String name = ss->getAttribute(L"Library");
				LoadPlugin(name);
			}
		}
		void PluginManager::UnloadPlugins()
		{
			for (Plugin* plg : m_plugins.getValueAccessor())
			{
				try
				{
					plg->Unload();
				}
				catch (const Exception& ex)
				{
					(void)ex;
					OnPluginError(plg);
				}
			}
			m_plugins.Clear();

			for (int32 i=0;i<m_libraries.getCount();i++)
			{
				m_libraries[i]->Unload();
				delete m_libraries[i];
			}
			m_libraries.Clear();
		}

	}
}
