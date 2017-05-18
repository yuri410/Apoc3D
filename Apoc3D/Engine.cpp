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

#include "Engine.h"

#include <sys/types.h> 
#include <sys/stat.h> 
#include <io.h>
#include <direct.h>

#include "Core/PluginManager.h"
#include "Core/Logging.h"
#include "Core/CommandInterpreter.h"
#include "Config/ConfigurationManager.h"
#include "Graphics/Animation/AnimationManager.h"
#include "Graphics/EffectSystem/EffectManager.h"
#include "Graphics/RenderSystem/GraphicsAPI.h"
#include "Graphics/TextureManager.h"
#include "Graphics/ModelManager.h"
#include "Input/InputAPI.h"
#include "Utility/StringUtils.h"
#include "UILib/FontManager.h"
#include "Vfs/PathUtils.h"
#include "Vfs/FileSystem.h"
#include "Vfs/FileLocateRule.h"
#include "Vfs/ResourceLocation.h"

using namespace Apoc3D::Core;
using namespace Apoc3D::VFS;
using namespace Apoc3D::Config;
using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::Animation;
using namespace Apoc3D::Graphics::EffectSystem;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::Input;
using namespace Apoc3D::Utility;
using namespace Apoc3D::UI;

namespace Apoc3D
{
	void Engine::Initialize(const ManualStartConfig* mconf)
	{
		setlocale(LC_CTYPE, ".ACP");
		

		LogManager::Initialize();
		if (mconf)
		{
			LogManager::getSingleton().WriteLogToStd = mconf->WriteLogToStd;
		}
		CommandInterpreter::Initialize();

		FileSystem::Initialize();
		if (mconf && mconf->WorkingDirectories.getCount())
		{
			for (int i=0;i<mconf->WorkingDirectories.getCount();i++)
			{
				FileSystem::getSingleton().AddWrokingDirectory(mconf->WorkingDirectories[i]);
			}
		}
		else
		{
			// uses the app's working dir as one of the FileSystem's working dir.
			char currentDir[260];

			if( _getcwd( currentDir, sizeof(currentDir)-1 ) != NULL )
			{
				FileSystem::getSingleton().AddWrokingDirectory(StringUtils::toPlatformWideString(currentDir));
			}
		}
		FileLocateRule::Initialize();

		ConfigurationManager::Initialize();
		if (mconf)
		{
			for (int i=0;i<mconf->ConfigSet.getCount();i++)
			{
				FileLocation fl = FileSystem::getSingleton().Locate(mconf->ConfigSet[i], FileLocateRule::Default);

				ConfigurationManager::getSingleton().LoadConfig(PathUtils::GetFileNameNoExt(mconf->ConfigSet[i]), fl);
			}
		}

		GraphicsAPIManager::Initialize();
		InputAPIManager::Initialize();

		PluginManager::Initialize();
		if (mconf)
		{
			PluginManager::getSingleton().LoadPlugins(mconf->PluginDynLibList);
			PluginManager::getSingleton().LoadPlugins(mconf->PluginList);
		}
		else
		{
			PluginManager::getSingleton().LoadPlugins();
		}

		if (mconf)
		{
			TextureManager::CacheSize = mconf->TextureCacheSize;
			TextureManager::UseCache = mconf->TextureAsync;

			ModelManager::CacheSize = mconf->ModelCacheSize;
			ModelManager::UseCache = mconf->ModelAsync;
		}
		
		TextureManager::Initialize();
		EffectManager::Initialize();
		AnimationManager::Initialize();
		ModelManager::Initialize();
		
		FontManager::Initialize();
	}
	void Engine::Shutdown()
	{
		FontManager::Finalize();
		
		ModelManager::getSingleton().Shutdown();
		ModelManager::Finalize();
		AnimationManager::Finalize();
		EffectManager::Finalize();
		TextureManager::getSingleton().Shutdown();
		TextureManager::Finalize();

		PluginManager::getSingleton().UnloadPlugins();
		PluginManager::Finalize();

		InputAPIManager::Finalize();
		GraphicsAPIManager::Finalize();
		ConfigurationManager::Finalize();
		FileSystem::Finalize();
		
		CommandInterpreter::Finalize();
		LogManager::Finalize();
	}
}