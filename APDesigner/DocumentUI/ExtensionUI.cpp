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

#include "ExtensionUI.h"

#include "UIDocument.h"

namespace APDesigner
{
	Document* ExtensionUI::OpenItem(const ProjectItem* item)
	{
		if (item->getType() == ProjectItemType::UILayout)
		{
			const Project* prj = item->getProject();
			ProjectResUILayout* mtrl = static_cast<ProjectResUILayout*>(item->getData());
			String path = mtrl->GetAbsoluteDestinationPath(mtrl->DestinationFile);

			if (File::FileExists(path))
			{
				UIDocument* md = new UIDocument(m_mainWindow, this, mtrl->DestinationFile);
				return md;
			}
		}
		return nullptr;
	}
	Document* ExtensionUI::DirectOpen(const String& filePath)
	{
		return new UIDocument(m_mainWindow, this, filePath);
	}

	bool ExtensionUI::SupportsItem(const ProjectItem* item)
	{
		if (item->getType() == ProjectItemType::UILayout)
			return true;
		return false;
	}

}