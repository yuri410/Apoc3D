#pragma once
#ifndef APOC3D_PLUGINMANAGER_H
#define APOC3D_PLUGINMANAGER_H

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

#include "apoc3d/Collections/List.h"
#include "apoc3d/Collections/HashMap.h"

using namespace Apoc3D::Platform;
using namespace Apoc3D::Collections;

namespace Apoc3D
{
	namespace Core
	{
		typedef fastdelegate::FastDelegate<void(const Plugin*, int32, int32)> PluginLoadCallBack;

		typedef fastdelegate::FastDelegate<void(const Plugin*)> PluginErrorBack;

		/** A 
		*/
		class APAPI PluginManager
		{
			SINGLETON_DECL(PluginManager);
		public:
			PluginManager();
			~PluginManager();

			PluginLoadCallBack& eventPluginLoad() { return m_eventPluginLoad; }
			PluginErrorBack& eventPluginError() { return m_eventPluginError; }

			void LoadPlugins(const List<Plugin*>& plugins);
			void LoadPlugins(const List<String>& plugins);
			void LoadPlugins();
			void UnloadPlugins();

			int32 getPluginCount() const { return m_plugins.getCount(); }
			
			const Plugin* getPlugin(const String& name) const;
			
		private:
			typedef HashMap<String, Plugin*> PluginTable;
			typedef List<Library*> LibraryList;

			void LoadPlugin(const String& name);

			void OnPluginLoad(const Plugin* plg, int32 index, int32 count);
			void OnPluginError(const Plugin* plg);
			
			PluginLoadCallBack m_eventPluginLoad;
			PluginErrorBack m_eventPluginError;

			PluginTable m_plugins;
			LibraryList m_libraries;

		};
	}
}

#endif