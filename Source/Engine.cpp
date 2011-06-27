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

#include "Engine.h"

#include <sys/types.h> 
#include <sys/stat.h> 
#include <io.h>
#include <direct.h>

#include "Vfs/FileSystem.h"
#include "Vfs/FileLocateRule.h"
#include "Config/ConfigurationManager.h"
#include "Graphics/RenderSystem/GraphicsAPI.h"
#include "Input/InputAPI.h"
#include "Core/PluginManager.h"
#include "Core/Logging.h"
#include "Utility/StringUtils.h"

using namespace Apoc3D::Core;
using namespace Apoc3D::VFS;
using namespace Apoc3D::Config;
using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::Input;
using namespace Apoc3D::Utility;

namespace Apoc3D
{
	void Engine::Initialize(const ManualStartConfig* mconf)
	{
		LogManager::Initialize();

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
			char currentDir[260];

			if( _getcwd( currentDir, sizeof(currentDir)-1 ) != NULL )
			{
				FileSystem::getSingleton().AddWrokingDirectory(StringUtils::toWString(currentDir));
			}
		}
		FileLocateRule::Initialize();

		ConfigurationManager::Initialize();
		

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
		
		

	}
	void Engine::Shutdown()
	{

	}
}