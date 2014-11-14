/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Xin

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