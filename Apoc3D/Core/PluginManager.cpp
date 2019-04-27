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

#include "PluginManager.h"

#include "Plugin.h"
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
		

		PluginManager::PluginManager() { }

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
			AP_EXCEPTION(ErrorID::KeyNotFound, name);
			return nullptr;
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
			LogManager::getSingleton().Write(LOG_System, L"Loading plugin" + name, LOGLVL_Infomation);

			Library* lib = new Library(name);
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

			Plugin* plugin = ((LIB_GET_PLUGIN)ptr)();
			if (plugin->Load())
			{
				m_libraries.Add(lib);
				m_plugins.Add(name, plugin);
			}
			else
			{
				OnPluginError(plugin);
				delete lib;
			}	
		}

		void PluginManager::LoadPlugins(const List<Plugin*>& plugins)
		{
			for (int i = 0; i < plugins.getCount(); i++)
			{
				Plugin* plugin = plugins[i];
				
				LogManager::getSingleton().Write(LOG_System, L"Loading plugin " + plugin->GetName(), LOGLVL_Infomation);

				plugin->Load();

				m_plugins.Add(plugin->GetName(), plugin);
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
				if (!plg->Unload())
				{
					OnPluginError(plg);
				}
			}
			m_plugins.Clear();
			m_libraries.DeleteAndClear();
		}

	}
}
