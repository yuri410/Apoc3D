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
			String path = PathUtils::Combine(prj->getOutputPath(), mtrl->DestinationFile);
			
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