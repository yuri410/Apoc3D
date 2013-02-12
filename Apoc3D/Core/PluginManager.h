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

#ifndef APOC3D_PLUGINMANAGER_H
#define APOC3D_PLUGINMANAGER_H

#include "apoc3d/Common.h"
#include "Singleton.h"

using namespace Apoc3D::Platform;
using namespace Apoc3D::Collections;

using namespace std;
using namespace fastdelegate;

namespace Apoc3D
{
	namespace Core
	{
		typedef fastdelegate::FastDelegate3<const Plugin*, int32, int32, void> PluginLoadCallBack;

		typedef fastdelegate::FastDelegate1<const Plugin*, void> PluginErrorBack;

		/** A 
		*/
		class APAPI PluginManager : public Singleton<PluginManager>
		{
		private:
			typedef unordered_map<String, Plugin*> PluginTable;
			typedef vector<Library*> LibraryList;

			PluginLoadCallBack m_eventPluginLoad;
			PluginErrorBack m_eventPluginError;
			
			PluginTable m_plugins;
			LibraryList m_libraries;

			void LoadPlugin(const String& name);

			void OnPluginLoad(const Plugin* plg, int32 index, int32 count);
			void OnPluginError(const Plugin* plg);
		
		public:
			PluginManager() { }
			~PluginManager();

			PluginLoadCallBack& eventPluginLoad() { return m_eventPluginLoad; }
			PluginErrorBack& eventPluginError() { return m_eventPluginError; }

			void LoadPlugins(const FastList<Plugin*>& plugins);
			void LoadPlugins(const List<String>& plugins);
			void LoadPlugins();
			void UnloadPlugins();

			int32 getPluginCount() const { return static_cast<int32>(m_plugins.size()); }
			
			const Plugin* getPlugin(const String& name) const;
			
			SINGLETON_DECL_HEARDER(PluginManager);
		};
	}
}

#endif