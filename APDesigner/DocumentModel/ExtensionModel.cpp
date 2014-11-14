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
#include "ExtensionModel.h"

#include "ModelDocument.h"

namespace APDesigner
{
	Document* ExtensionModel::OpenItem(const ProjectItem* item)
	{
		if (item->getType() == ProjectItemType::Model)
		{
			const Project* prj = item->getProject();
			ProjectResModel* mdl = static_cast<ProjectResModel*>(item->getData());

			String destPathBase = mdl->GetAbsoluteDestinationPathBase();

			String path = PathUtils::Combine(destPathBase, mdl->DstFile);
			String pathAnim = PathUtils::Combine(destPathBase, mdl->DstAnimationFile);
			if (File::FileExists(path))
			{
				ModelDocument* md = new ModelDocument(m_mainWindow, this, mdl->DstFile, path, File::FileExists(pathAnim) ? pathAnim : L"");
				return md;
			}
		}
		return 0;
	}
	Document* ExtensionModel::DirectOpen(const String& filePath)
	{
		String name = PathUtils::GetDirectory(filePath);
		String animFile = PathUtils::GetFileNameNoExt(filePath);
		animFile.append(L".anim");

		bool haveAnim = File::FileExists(animFile);

		ModelDocument* md = new ModelDocument(m_mainWindow, this, name, filePath, haveAnim ? animFile : L"");
		return md;
	}


	bool ExtensionModel::SupportsItem(const ProjectItem* item)
	{
		if (item->getType() == ProjectItemType::Model)
			return true;
		return false;
	}

}