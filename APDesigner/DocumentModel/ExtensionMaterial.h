/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Games

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

		virtual std::vector<String> GetFileExtensions() 
		{
			std::vector<String> r;
			r.push_back(L".mtrl");
			return r;
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