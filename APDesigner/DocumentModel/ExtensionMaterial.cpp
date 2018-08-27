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

#include "ExtensionMaterial.h"

#include "MaterialDocument.h"

namespace APDesigner
{
	Document* ExtensionMaterial::OpenItem(const ProjectItem* item)
	{
		if (item->getType() == ProjectItemType::Material)
		{
			const Project* prj = item->getProject();
			ProjectResMaterial* mtrl = static_cast<ProjectResMaterial*>(item->getData());
			String path = mtrl->GetAbsoluteDestinationPath(mtrl->DestinationFile);
			
			if (File::FileExists(path))
			{
				MaterialDocument* md = new MaterialDocument(m_mainWindow, this, mtrl->DestinationFile, path);
				return md;
			}
		}
		return 0;
	}
	Document* ExtensionMaterial::DirectOpen(const String& filePath)
	{
		String name = PathUtils::GetDirectory(filePath);
		
		MaterialDocument* md = new MaterialDocument(m_mainWindow, this, name, filePath);
		return md;
	}


	bool ExtensionMaterial::SupportsItem(const ProjectItem* item)
	{
		if (item->getType() == ProjectItemType::Material)
			return true;
		return false;
	}

}