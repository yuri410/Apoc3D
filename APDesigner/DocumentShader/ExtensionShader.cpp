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

#include "ExtensionShader.h"

#include "docShaderNet.h"
#include "docShaderAtom.h"
#include "docEffectCoding.h"

namespace APDesigner
{
	Document* ExtensionShaderNetwork::OpenItem(const ProjectItem* item)
	{
		if (item->getType() == ProjectItemType::ShaderNetwork)
		{
			ProjectResShaderNetwork* shaderNet = static_cast<ProjectResShaderNetwork*>(item->getData());

			String filePath = shaderNet->GetAllOutputFiles()[0];

			if (File::FileExists(filePath))
			{
				return new ShaderNetDocument(m_mainWindow, this, filePath);
			}
		}
		return 0;
	}
	Document* ExtensionShaderNetwork::DirectOpen(const String& filePath)
	{
		return new ShaderNetDocument(m_mainWindow, this, filePath);
	}


	bool ExtensionShaderNetwork::SupportsItem(const ProjectItem* item)
	{
		if (item->getType() == ProjectItemType::ShaderNetwork)
			return true;
		return false;
	}

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/

	Document* ExtensionShaderAtom::OpenItem(const ProjectItem* item)
	{
		return 0;
	}
	Document* ExtensionShaderAtom::DirectOpen(const String& filePath)
	{
		return new ShaderAtomDocument(m_mainWindow, this, filePath);
	}

	bool ExtensionShaderAtom::SupportsItem(const ProjectItem* item)
	{
		return false;
	}

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/

	Document* ExtensionEffect::OpenItem(const ProjectItem* item)
	{
		if (item->getType() == ProjectItemType::Effect)
		{
			ProjectResEffect* data = static_cast<ProjectResEffect*>(item->getData());

			String srcBase = data->GetAbsoluteSourcePathBase(false);
			String filePath = PathUtils::Combine(srcBase, data->PListFile);
			String vsPath = PathUtils::Combine(srcBase, data->VS);
			String psPath = PathUtils::Combine(srcBase, data->PS);

			if (File::FileExists(filePath))
			{
				return new EffectDocument(m_mainWindow, this, filePath, vsPath, psPath);
			}
		}
		return 0;
	}
	Document* ExtensionEffect::DirectOpen(const String& filePath)
	{
		String temp;
		String base;

		PathUtils::SplitFileNameExtension(filePath, base, temp);

		return new EffectDocument(m_mainWindow, this, filePath, base + L".vs", base + L".ps");
	}


	bool ExtensionEffect::SupportsItem(const ProjectItem* item)
	{
		if (item->getType() == ProjectItemType::Effect)
			return true;
		return false;
	}
}