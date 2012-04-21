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
#include "EditorExtensionManager.h"
#include "Core/Logging.h"

SINGLETON_DECL(APDesigner::EditorExtensionManager);

namespace APDesigner
{
	void EditorExtensionManager::RegisterExtension(EditorExtension* ext)
	{
		String name = ext->GetName();
		if (m_extensions.Contains(name))
		{

		}
		else
		{
			LogManager::getSingleton().Write(LOG_System, L"" + name);
		}
	}

	void EditorExtensionManager::UnregisterExtension(EditorExtension* ext)
	{

	}
}
extern "C"
{
	void RegisterExtension(APDesigner::EditorExtension* ext)
	{
		APDesigner::EditorExtensionManager::getSingleton().RegisterExtension(ext);
	}
	void UnregisterExtension(APDesigner::EditorExtension* ext)
	{
		APDesigner::EditorExtensionManager::getSingleton().UnregisterExtension(ext);
	}
};