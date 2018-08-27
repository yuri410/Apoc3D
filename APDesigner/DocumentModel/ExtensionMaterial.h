#pragma once

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

#ifndef EXTENSIONMATERAIL_H
#define EXTENSIONMATERAIL_H

#include "APDesigner/EditorExtensionManager.h"

namespace APDesigner
{
	/** Model editor extension
	*/
	class ExtensionMaterial : public EditorExtension
	{
	public:
		virtual String GetName() { return L"Material Editor"; }
		virtual Document* OpenItem(const ProjectItem* item);
		virtual Document* DirectOpen(const String& filePath);

		virtual bool SupportsItem(const ProjectItem* item);

		virtual bool SupportsIndependentEditing() { return true; };

		virtual List<String> GetFileExtensions() 
		{
			return{ L".mtrl" };
		}

		ExtensionMaterial(MainWindow* wnd)
			: m_mainWindow(wnd)
		{

		}
	private:
		MainWindow* m_mainWindow;
	};
}

#endif