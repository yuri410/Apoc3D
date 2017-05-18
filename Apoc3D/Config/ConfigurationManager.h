#pragma once
#ifndef APOC3D_CONFIGURATIONMANAGER_H
#define APOC3D_CONFIGURATIONMANAGER_H

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

#include "apoc3d/Common.h"
#include "apoc3d/Collections/HashMap.h"
#include "apoc3d/Collections/Queue.h"
#include "apoc3d/Library/tinythread.h"

using namespace Apoc3D::Core;
using namespace Apoc3D::Collections;
using namespace Apoc3D::VFS;

namespace Apoc3D
{
	namespace Config
	{
		class APAPI ConfigurationManager
		{
			SINGLETON_DECL(ConfigurationManager);
		public:
			ConfigurationManager();
			virtual ~ConfigurationManager();

			/**
			 *  Loads a config from given location. Then stores it in the
			 *  manager with the given name as an identifier, which can be 
			 *  used to retrieve the config using the getConfiguration method.
			 */
			void LoadConfig(const String& name, const ResourceLocation& rl, ConfigurationFormat* fmt = nullptr);

			/**  Gets a pre-loaded configuration with the given name */
			Configuration* getConfiguration(const String& name) const;

			Configuration* CreateInstance(const ResourceLocation& rl, ConfigurationFormat* fmt = nullptr);

			void RegisterFormat(ConfigurationFormat* fmt);
			void UnregisterFormat(ConfigurationFormat* fmt);

		private:
			typedef HashMap<String, Configuration*> ConfigTable;
			typedef HashMap<String, ConfigurationFormat*> FormatTable;
			

			ConfigTable m_configs;
			FormatTable m_formats;

		};
	}
}
#endif