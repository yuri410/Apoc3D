/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2009-2018 Tao Xin
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

#include "EditorExtensionManager.h"


namespace APDesigner
{
	SINGLETON_IMPL(EditorExtensionManager);

	void EditorExtensionManager::RegisterExtension(EditorExtension* ext)
	{
		String name = ext->GetName();
		if (!m_extensions.Contains(name))
		{
			m_extensions.Add(name, ext);
			LogManager::getSingleton().Write(LOG_System, L"[APD]Editor extension added: " + name);
		}
		else
		{
			LogManager::getSingleton().Write(LOG_System, L"[APD]Editor extension already registered: " + name, LOGLVL_Warning);
		}
	}

	void EditorExtensionManager::UnregisterExtension(EditorExtension* ext)
	{
		String name = ext->GetName();
		if (m_extensions.Contains(name))
		{
			m_extensions.Remove(name);
			LogManager::getSingleton().Write(LOG_System, L"[APD]Editor extension removed: " + name);
		}
		else
		{
			LogManager::getSingleton().Write(LOG_System, L"[APD]Editor extension not registered: " + name, LOGLVL_Warning);
		}
	}

	EditorExtension* EditorExtensionManager::FindSuitableExtension(const ProjectItem* item)
	{
		EditorExtension* result = nullptr;
		for (EditorExtension* ee : m_extensions.getValueAccessor())
		{
			if (ee->SupportsItem(item))
			{
				result = ee;
				break;
			}
		}
		return result;
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