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

			String destPathBase = mdl->GetAbsoluteDestinationPathBase(false);

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