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

			String filePath = PathUtils::Combine(item->getProject()->getBasePath(), data->PListFile);
			String vsPath = PathUtils::Combine(item->getProject()->getBasePath(), data->VS);
			String psPath = PathUtils::Combine(item->getProject()->getBasePath(), data->PS);

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