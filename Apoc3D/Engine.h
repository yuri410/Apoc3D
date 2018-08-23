#pragma once
#ifndef APOC3D_ENGINE_H
#define APOC3D_ENGINE_H

/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2014-2017 Tao Xin
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

#include "ApocCommon.h"
#include "Collections/List.h"

using namespace Apoc3D::Core;
using namespace Apoc3D::Collections;

namespace Apoc3D
{
	/**
	 *  The engine's initialization config. 
	 *  When this is specified, this will override the engine's default settings.
	 */
	struct APAPI ManualStartConfig
	{
		/**
		 *  A list of dir of config files which is used as basic configuration 
		 *  and loaded, putted into ConfigurationManager.
		 *  ConfigurationManager::getConfiguration can be used to retrive theses configs
		 *  later.
		 */
		List<String> ConfigSet;

		/**
		 *  A list of absolute directories to be used in the engine's file system.
		 */
		List<String> WorkingDirectories;
		/**
		 *  A list of file names of plugin dynamic libraries to be loaded.
		 */
		List<String> PluginDynLibList;
		/**
		 *  A list of user specified pre-loaded plugins.
		 */
		List<Plugin*> PluginList;

		/**
		 *  Specified whether the incoming new log entries should be forwarded to standard output.
		 *  Useful when debugging.
		 */
		bool WriteLogToStd;

		/** 
		 *  Specified whether the TextureManager will be actuated to use async processing
		 */
		bool TextureAsync;
		/**
		 *  If using async processing, specifies the total cache size in bytes.
		 */
		uint TextureCacheSize;

		/**
		 *  Specified whether the ModelManager will be actuated to use async processing
		 */
		bool ModelAsync;
		/**
		 *  If using async processing, specifies the total cache size in bytes.
		 */
		uint ModelCacheSize;

		ManualStartConfig()
			: TextureCacheSize(1024*1024*100), ModelCacheSize(1024*1024*50), WriteLogToStd(false),
			TextureAsync(true), ModelAsync(true)
		{

		}
	};

	/**
	 *  Includes elementary operation on the engine like Initialize and Showdown
	 */
	namespace Engine
	{
		/** 
		 *  Initializes all fundamental services like resource management, file system, plugins, graphics API..
		 *  to prepare the engine ready for use.
		 */
		APAPI void Initialize(const ManualStartConfig* mconf = 0);

		/**
		 *  Releases all services, freeing the hardware resources used.
		 */
		APAPI void Shutdown();
	};
}

#endif